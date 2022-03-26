/*!\file watchdog.h
** \author 
** \copyright TODO
** \brief Watchdog timer API
** \detail Public API to initialise and service the hardware watchdog timer.
**         Watchdog timers are protection mechanisms that allow the system to
**         recover from erroneous software conditions such as deadlocks or
**         runaway code. It does so by issuing a reset at expiration.
**/
/****************************************************************/
#ifndef __WATCHDOG_H
#define __WATCHDOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdbool.h>
/****************************************************************/

/*!	\brief Initialise watchdog timer.
**
**	Call at boot to perform hardware setup.
**
**	\return None
**/
void watchdog_init(void);

/*!	\brief Service the watchdog timer.
**
**	Call this function regularly to clear the watchdog counter and
**	prevent system reset.
**
**	\return None
**/
void watchdog_kick(void);
/****************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* __WATCHDOG_H */
/****************************************************************/
