/*!\file timers_config.h
** \author 
** \copyright TODO
** \brief Static configuration for system timer.
** \details This is a private header that can be used to statically configure
**          the megavr implementation of the driver.
**/
/****************************************************************/
#ifndef __TIMER_CONFIG_H
#define __TIMER_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdbool.h>
/****************************************************************/

/****************************************************************/
#ifdef __cplusplus
}
#endif

/*!	\brief Oscillator frequency.
**
** Frequency of the oscillator driving the RTC.
** In this implementation we use the external 32kHz cristal: XOSC32K.
**/
#define TIMER_OSC_FREQ_HZ  32768

/*!	\brief Frequency divider for milliseconds timer.
**
** RTC will be run at a frequency equal to TIMER_OSC_FREQ/TIMER_FREQ_DIV_MS
** when it is used to count milliseconds.
**/
#define TIMER_MS_FREQ_DIV  1 //30.5 us resolution, 1.999 seconds max range

/*!	\brief Frequency divider for seconds timer.
**
** RTC will be run at a frequency equal to TIMER_OSC_FREQ/TIMER_FREQ_DIV_SEC
** when it is used to count seconds.
**/
#define TIMER_SEC_FREQ_DIV 32 //976us resolution, 1min:4sec max range

/*!	\brief Frequency divider for minutes timer.
**
** RTC will be run at a frequency equal to TIMER_OSC_FREQ/TIMER_FREQ_DIV_MIN
** when it is used to count seconds.
**/
#define TIMER_MIN_FREQ_DIV 16384  // 499,7 ms, ~9h max range

/*!	\brief Keep the timer running during sleep mode.
**
** Define this flag to have the timer hardware running when the device
** is in sleep mode. Timer hardware can be used as a wake-up source in
** this way.
**/
#define TIMER_ENABLED_IN_SLEEP

/*!	\brief Use a low power clock source for timer in sleep mode.
**
** Define to use a low power clock source to run the timer during sleep mode.
** Time tracking will be less accurate but will require less power.
**/
// #define TIMER_USE_LP_CLOCK_IN_SLEEP

#endif /* __TIMER_CONFIG_H */
/****************************************************************/
