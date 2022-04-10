/*!\file reset.c
** \author 
** \copyright
** \brief Implementation for reset module.
** \details
**/
/****************************************************************/

#include "reset.h"
#include <stddef.h>
#include <avr/wdt.h>
/****************************************************************/

struct reset_dump
{
    reset_cause_t cause;
    sw_reset_t sw_code;
};

/** RAM location to store the cause of the last reset. It is preserved on reboot
 ** to ensure that the sw_code eventually stored by the reset() function, persists
 ** after the software reset.
 **/
static struct reset_dump last_reset __attribute__ ((section (".noinit")));

/** Read the reset cause from hardware, store it in the preserved RAM and
 ** clear the register.
 **/
static void dump_last_reset(void)
{
    /* Determine hardware reset cause.
     * If-ladder: establishes a priority order in the evaluation.
     * More than 1 bits should only be seen at the first start-up or after
     * pressing the reset button.
     * Power on and external would be set in that case.
     * Ensure they are prioritised to capture the fact that it's a normal
     * reboot. If we are outside that case, only the bit of interest is set
     * (i.e. watchdog).
     *
     * If one of the bits is set performing a "set bit action" on it from software will clear
     * the whole register (see the mcu datasheet).
     */
    if(RSTCTRL.RSTFR & RSTCTRL_PORF_bm)
    {
        last_reset.cause = reset_power_on;
        RSTCTRL.RSTFR |= RSTCTRL_PORF_bm;
    }
    else if(RSTCTRL.RSTFR & RSTCTRL_EXTRF_bm)
    {
        last_reset.cause = reset_external;
        RSTCTRL.RSTFR |= RSTCTRL_EXTRF_bm;
    }
    else if(RSTCTRL.RSTFR & RSTCTRL_BORF_bm)
    {
        last_reset.cause = reset_brown_out;
        RSTCTRL.RSTFR |= RSTCTRL_BORF_bm;
    }
    else if(RSTCTRL.RSTFR & RSTCTRL_WDRF_bm)
    {
        last_reset.cause = reset_watchdog;
        RSTCTRL.RSTFR |= RSTCTRL_WDRF_bm;
    }
    else if(RSTCTRL.RSTFR & RSTCTRL_UPDIRF_bm)
    {
        last_reset.cause = reset_debugger;
        RSTCTRL.RSTFR |= RSTCTRL_UPDIRF_bm;
    }
    else if(RSTCTRL.RSTFR & RSTCTRL_SWRF_bm)
    {
        last_reset.cause = reset_software;
        RSTCTRL.RSTFR |= RSTCTRL_SWRF_bm;
    }
    else
    {
        /* This shouldn't happen. */
        last_reset.cause = reset_undefined;
    }

    if(last_reset.cause != reset_software)
    {
        /* No software reset: clear software reset cause */
        last_reset.sw_code = SW_RESET_UNSPECIFIED;
    }
}

void reset_init(void)
{
    dump_last_reset();
}

reset_cause_t reset_read_last(sw_reset_t *sw_code)
{
    /* Read data out from last_reset store.
     * Assumes operation is thread-safe.
     */
    if(sw_code != NULL)
    {
        *sw_code = last_reset.sw_code;
    }
    return last_reset.cause;
}

void reset(sw_reset_t sw_reset_code)
{
    /* Store the reset code. This will be preserved
     * after reset and it will be possible to read it.
     */
    last_reset.sw_code = sw_reset_code;
    /* Issue software reset. */
    _PROTECTED_WRITE(RSTCTRL.SWRR, RSTCTRL_SWRE_bm);
}
/****************************************************************/
