/*!\file timers.c
** \author 
** \copyright
** \brief 
** \details 
**/
/****************************************************************/

#include "timer.h"
#include "timer_config.h"
#include "interrupts.h"
#include "sleep.h"
#include <stddef.h>
#include <avr/interrupt.h>
#include <avr/io.h>
/****************************************************************/

/* C-preprocessor's hacks: convert human-friendly frequency divider
 * values from timer_config.h into RTC_PRESCALER_t masks provided by avr.
 * Prescaler's mask are enum values in the form: RTC_PRESCALER_DIV<div>_gc
 * The double expansion (TIMER_PRESCALER_MASK, _TIMER_PRESCALER_MASK) is
 * required to force the preprocessor to expand the actual numerical value
 * into the concatenation (RTC_PRESCALER_DIV##div##_gc).
 */
#define TIMER_PRESCALER_MASK(div)  _TIMER_PRESCALER_MASK(div)
#define _TIMER_PRESCALER_MASK(div)  RTC_PRESCALER_DIV##div##_gc
#define TIMER_MS_PRESCALER_MASK    TIMER_PRESCALER_MASK(TIMER_MS_FREQ_DIV)
#define TIMER_SEC_PRESCALER_MASK   TIMER_PRESCALER_MASK(TIMER_SEC_FREQ_DIV)
#define TIMER_MIN_PRESCALER_MASK   TIMER_PRESCALER_MASK(TIMER_MIN_FREQ_DIV)

/* Resolution of the RTC in microseconds based on the prescaler setting
 * of each timer type (ms, sec, min).
 *
 * The aim is to let the compiler perform floating point operations,
 * and switch to uint_16 in the last minute.
 *
 * timer_type can be on of TIMER_MS, TIMER_SEC, TIMER_MIN
 */
#define TICK_US(timer_type) ((1000000.0 * timer_type##_FREQ_DIV)/TIMER_OSC_FREQ_HZ)

/* Multiplier applied when casting the conversion factor to integer.
 * This is to limit the lost of decimal precision.
 * It must be divided out after conversion is performed.
 */
#define INT_CAST_MULTIPLIER  1024

/* Conversion macros for the milliseconds timer.
 */
#define MS_TO_TICKS (1000.0/TICK_US(TIMER_MS))
#define MS_TO_TICKS_INT ((uint32_t)(MS_TO_TICKS * INT_CAST_MULTIPLIER))
#define rtcTicksFromMilliseconds(ms) (((ms) * MS_TO_TICKS_INT)/INT_CAST_MULTIPLIER)

/* Conversion macros for the seconds timer.
 */
#define SEC_TO_TICKS (1000000.0/TICK_US(TIMER_SEC))
#define SEC_TO_TICKS_INT ((uint32_t)(SEC_TO_TICKS * INT_CAST_MULTIPLIER))
#define rtcTicksFromSeconds(sec) (((sec) * SEC_TO_TICKS_INT)/INT_CAST_MULTIPLIER)

/* Conversion macros for the minutes timer.
 */
#define MIN_TO_TICKS (60000000.0/TICK_US(TIMER_MIN))
#define MIN_TO_TICKS_INT ((uint32_t)(MIN_TO_TICKS * INT_CAST_MULTIPLIER))
#define rtcTicksFromMinutes(min) (((min) * MIN_TO_TICKS_INT)/INT_CAST_MULTIPLIER)

/*!    \brief Timer private control structure
**/
struct timer_ctrl
{
    bool continuous; /** Continuous mode (true) or one-shot (false) */
    uint16_t ticks;  /** Number of ticks to load into RTC.PER */
    RTC_PRESCALER_t prescaler; /** Prescaler mask as define by avr/io.h */
    timer_callback_t callback; /** Callback to executed during expiration. */
};

volatile struct timer_ctrl sys_timer;

/*!    \brief Common function to start the various timers.
**
** Groups the common code to actually set the hardware and
** make the timer run.
**
**    \param [in] ticks  - Ticks to set into .
**    \param [in] presc  - Prescaler mask to use as define by avr/io.
**    \param [in] clbk   - Callback to execute when timer expires.
**    \param [in] continuos - Run timer in continuous or one-shot
**
**  \return Nothing.
**/
static void timer_start_common(uint16_t ticks, RTC_PRESCALER_t presc,
                               timer_callback_t clbk, bool continuous)
{
    interrupts_off();
    /* Atomically check timer is free and make it busy
     * Can't use timer_is_free due to interrupts_off/on nesting.
     */
    if(sys_timer.callback != NULL)
    {
        return;
    }
    sys_timer.callback = clbk;
    interrupts_on();

    /* Once we obtained the timer, we can set the rest of the structure
     * safely.
     */
    sys_timer.ticks = ticks;
    sys_timer.prescaler = presc;
    sys_timer.continuous = continuous;


    while(RTC.STATUS != 0);
    /* We only use RTC overflow feature. Set the clock overflow value (RTC.PER) and
     * enable overflow interrupt */
    RTC.PER = sys_timer.ticks;
    RTC.INTCTRL = RTC_OVF_bm;
    /* Set the prescaler and enable the RTC.
     */
    RTC.CTRLA |= (sys_timer.prescaler | RTC_RTCEN_bm);
}

#ifdef TIMER_ENABLED_IN_SLEEP
/*!    \brief Timer one-off initialisation for sleep mode
**
** Function to register with the sleep subsystem for sleep
** mode.
**
** Configure the timer hardware to enter sleep mode.
** Configure timer hardware to remain active during sleep.
**
**  \return Nothing.
**/
static void timer_init_for_sleep(void)
{
    while(RTC.STATUS & RTC_CTRLABUSY_bm);
    /* Enable device during deep sleep. */
    RTC.CTRLA |= RTC_RUNSTDBY_bm;
}

/*!    \brief Configure the timer hardware for sleep mode.
**
** Function to register with the sleep subsystem for sleep
** mode.
**
** Configure the timer hardware to enter sleep mode.
** Mainly used to determine which clock source should be used
** to run the timer hardware during sleep.
**
**    \return Nothing.
**/
static void timer_on_enter_sleep(void)
{
#ifdef TIMER_USE_LP_CLOCK_IN_SLEEP
    /* Use internal ULP oscillator @32kHz as low power clock source.
     *
     * Note that hardware also supports ULP oscillator @1kHz. We don't
     * use it for now as it will require using two different frequencies for
     * LP and non LP timer clocks.
     */
    RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;
#endif /* TIMER_USE_LP_CLOCK_IN_SLEEP */
}

/*!    \brief Configure the timer hardware for active mode.
**
** Function to register with the sleep subsystem for sleep
** mode.
**
** Configure the timer hardware for active mode.
** Restore active settings that have been changed
** by timer_on_enter_sleep.
**
**    \return Nothing.
**/
static void timer_on_exit_sleep(void)
{
#ifdef TIMER_USE_LP_CLOCK_IN_SLEEP
    /* Restore external crystal as power source.
     */
    RTC.CLKSEL = RTC_CLKSEL_TOSC32K_gc;
#endif /* TIMER_USE_LP_CLOCK_IN_SLEEP */
}
#endif /* TIMER_ENABLED_IN_SLEEP */

ISR(RTC_CNT_vect)
{
    /* Acknowledge interrupt */
    RTC.INTFLAGS &= RTC_OVF_bm;
    /* Execute the callback */
    (*sys_timer.callback)();
    if(!sys_timer.continuous)
    {
        /* Disable timer if in one-shot mode. */
        RTC.CTRLA = 0;
        sys_timer.callback = NULL;
    }
}

void timer_init(void)
{
    uint32_t wait = 1000000; //50ms very approximate

    /* Init control structure. No protection: assumes interrupts are disabled. */
    sys_timer.callback = NULL;

    /* Enable external 32KHz Xtal */
    _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, CLKCTRL_ENABLE_bm);

    /* Wait for it to stabilize. */
    while(wait--);

    /* From data-sheet: The RTC peripheral is used internally during device start-up.
     * Always check the Busy bits in the RTC.STATUS and RTC.PITSTATUS registers,
     * also on initial configuration*/
    while(RTC.STATUS != 0);
    /* Select clock source to 32.768 kHz external oscillator */
    RTC.CLKSEL = RTC_CLKSEL_TOSC32K_gc;

#ifdef TIMER_ENABLED_IN_SLEEP
    sleep_register_peripheral( timer_init_for_sleep,
                               timer_on_enter_sleep,
                               timer_on_exit_sleep);
#endif
}

bool timer_is_free(void)
{
    bool res;

    interrupts_off();
    res = sys_timer.callback == NULL;
    interrupts_on();
    return res;
}

void timer_start_one_shot_ms(uint16_t ms, timer_callback_t clbk)
{
    uint16_t ticks = rtcTicksFromMilliseconds(ms);
    timer_start_common(ticks, TIMER_MS_PRESCALER_MASK, clbk, false);
}

void timer_start_continuous_ms(uint16_t ms, timer_callback_t clbk)
{
    uint16_t ticks = rtcTicksFromMilliseconds(ms);
    timer_start_common(ticks, TIMER_MS_PRESCALER_MASK, clbk, true);
}

void timer_start_one_shot_sec(uint16_t sec, timer_callback_t clbk)
{
    uint16_t ticks = rtcTicksFromSeconds(sec);
    timer_start_common(ticks, TIMER_SEC_PRESCALER_MASK, clbk, false);
}

void timer_start_continuous_sec(uint16_t sec, timer_callback_t clbk)
{
    uint16_t ticks = rtcTicksFromSeconds(sec);
    timer_start_common(ticks, TIMER_SEC_PRESCALER_MASK, clbk, true);
}

void timer_start_one_shot_min(uint16_t min, timer_callback_t clbk)
{
    uint16_t ticks = rtcTicksFromMinutes(min);
    timer_start_common(ticks, TIMER_MIN_PRESCALER_MASK, clbk, false);
}

void timer_start_continuous_min(uint16_t min, timer_callback_t clbk)
{
    uint16_t ticks = rtcTicksFromMinutes(min);
    timer_start_common(ticks, TIMER_MIN_PRESCALER_MASK, clbk, true);
}

void timer_stop(void)
{
    while(RTC.STATUS);
    RTC.CTRLA = 0;
    interrupts_off();
    sys_timer.callback = NULL;
    interrupts_on();
}
/****************************************************************/
