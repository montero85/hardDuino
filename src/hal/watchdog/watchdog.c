/*!\file watchdog.c
** \author 
** \copyright
** \brief Implementation for watchdog timer API
** \details
**/
/****************************************************************/

#include "watchdog.h"
#include "watchdog_config.h"
#include <stddef.h>
#include <avr/wdt.h>
/****************************************************************/

/* AVR libc provides a wdt_enable but it doesn't seem to fully support
 * all the features and timeout values. Better to DIY.
 */
void watchdog_init(void)
{
	/* Enable watchdog by programming the timeout */
	_PROTECTED_WRITE(WDT.CTRLA, WATCHDOG_TIMEOUT);
	/* Wait for the setting to be written (just in case) */
	while(WDT.STATUS & WDT_SYNCBUSY_bm);
	/* Lock the watchdog to protect it from accidental writing */
	_PROTECTED_WRITE(WDT.STATUS, WDT_LOCK_bm);
}

/* This can happily wrap avr macro to call asm("WDT") */
void watchdog_kick(void)
{
	wdt_reset();
}

/****************************************************************/
