/*!\file reset.h
** \author 
** \copyright TODO
** \brief System reset module
** \detail This module initialise system reset of the MCU, provides
**         software reset and allows to read out last reset cause.
**/
/****************************************************************/
#ifndef __RESET_H
#define __RESET_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdbool.h>
/****************************************************************/

/*!	\brief Software reset code is not specified
**
** It is typically the sw_code value returned by reset_read_last
** when no reset_software took place.
**/
#define SW_RESET_UNSPECIFIED 0

/*!	\brief Possible causes of reset.
**/
typedef enum
{
	reset_undefined, /** Unknown reset cause. Shouldn't happen. */
	reset_power_on,  /** Not a true reset. Device was powered on. */
	reset_brown_out, /** Reset issued by brown-out detection. */
	reset_watchdog,  /** Reset issued by the watchdog timer. */
	reset_external,  /** Reset caused by an external trigger. */
	reset_debugger,  /** Reset issued by on-chip debugger. */
	reset_software,  /** Reset issued by software. */
} reset_cause_t;

/*!	\brief Type used for software reset codes.
**/
typedef uint8_t sw_reset_t;

/*!	\brief Initialise reset module.
**
** This will also dump the reset cause data that is read by
** reset_read_last.
**
** Ensure it is called early in the boot process.
**
**	\return None
**/
void reset_init(void);

/*!	\brief Return the cause of the last reset.
**
** This function returns the source of the last reset that took
** place in the system.
**
** Shall be called after reset_init() or behaviour will be undefined.
**
**	\param [out] sw_code - sw_reset_code passed to reset()
**	                       if a software reset took place.
**	                       SW_RESET_UNSPECIFIED otherwise.
**	                       Ignored if set to NULL.
**
**	\return Cause of the last reset that took place
**/
reset_cause_t reset_read_last(sw_reset_t *sw_code);

/*!	\brief Trigger a reset from software.
**
**	\param [in] sw_reset_code - status code that will be preserved on boot
**                              and can be used for debugging (i.e. reset
**                              can be used on errors).
**
**	\return None
**/
void reset(sw_reset_t sw_reset_code);
/****************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* __RESET_H */
/****************************************************************/
