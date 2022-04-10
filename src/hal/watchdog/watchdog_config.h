/*!\file watchdog_config.h
** \author 
** \copyright TODO
** \brief Static configuration for watchdog timer hardware
** \detail This is a private header that can be used to statically configure
**          the megavr implementation of the driver.
**/
/****************************************************************/
#ifndef __WATCHDOG_CONFIG_H
#define __WATCHDOG_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdbool.h>
/****************************************************************/

/*!    \brief Watchdog timeout value.
**
** Can be any of the following labels (see WDT_PERIOD_t in avr/io.h):
**
** WDT_PERIOD_OFF_gc => Off
** WDT_PERIOD_8CLK_gc => 8 cycles (8ms)
** WDT_PERIOD_16CLK_gc => 16 cycles (16ms)
** WDT_PERIOD_32CLK_gc => 32 cycles (32ms)
** WDT_PERIOD_64CLK_gc => 64 cycles (64ms)
** WDT_PERIOD_128CLK_gc => 128 cycles (0.128s)
** WDT_PERIOD_256CLK_gc => 256 cycles (0.256s)
** WDT_PERIOD_512CLK_gc => 512 cycles (0.512s)
** WDT_PERIOD_1KCLK_gc => 1K cycles (1.0s)
** WDT_PERIOD_2KCLK_gc => 2K cycles (2.0s)
** WDT_PERIOD_4KCLK_gc =>4K cycles (4.1s)
** WDT_PERIOD_8KCLK_gc => 8K cycles (8.2s
**/
#define WATCHDOG_TIMEOUT WDT_PERIOD_8KCLK_gc

/****************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* __WATCHDOG_CONFIG_H */
/****************************************************************/
