/*!\file sleep.h
** \author 
** \copyright TODO
** \brief API for MCU low power modes.
** \detail Sleep module supports two levels of low power modes whose implementation
**         details may vary depending on the underlining hardware platform.
**         They are:
**        - sleep_on_the_couch: soft low power mode. Typically, only the CPU is halted.
**                              Peripherals (or most of them) are still active.
**        - sleep_on_the_bed: hard low power mode. Both CPU and (most of)
**                            peripherals are switched off. Peripherals meant
**                            to be active during sleep, need to register themselves
**                            to the sleep module to allow their self-configuration.
**                            (i.e. enable them selves during sleep, switch to a
**                            lower power clock source).
**/
/****************************************************************/
#ifndef __SLEEP_H
#define __SLEEP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdbool.h>
/****************************************************************/

/*!	\brief Type for on-init handler for peripherals.
**
** Self-configuration handler that peripherals should provide during
** registration.
**
** It will be called during sleep_init().
**
** It can be used for one-off configurations, i.e. to allow
** the device to be always active during sleep.
**/
typedef void (*sleep_on_init)(void);

/*!	\brief Type for on-enter handler for peripherals.
**
** Self-configuration handler that peripherals should provide during
** registration.
**
** It will be called before entering sleep_on_bed mode.
**
** It can be used i.e. to switch clock source before going into
** hard sleep states.
**/
typedef void (*sleep_on_enter)(void);

/*!	\brief Type for on-exit handler for peripherals.
**
** Self-configuration handler that peripherals should provide during
** registration.
**
** It will be called just after leaving sleep_on_bed mode.
**
** It can be used to restore the settings altered by sleep_on_enter.
**/
typedef void (*sleep_on_exit)(void);

/*!	\brief One-off initialisation for sleep module.
**
** Initialise the sleep hardware and call sleep_on_init function for
** all the registered peripherals.
**
** NOTE: This function shall be called after all the required peripherals
** have been initialised and registered.
**
**	\return None
**/
void sleep_init(void);

/*!	\brief Register a device for sleep self-configuration.
**
** This function decouples the sleep module from the configuration
** of each peripherals for sleep_on_bed() mode. Each peripheral,
** intended to be active during sleep (i.e. as a wake-up source),
** will need to register a set of handlers to allow its in/out of
** sleep configuration.
** Even though sleep is typically a system (global) level feature,
** this should make it easier to configure individual hardware
** modules with a discrete level of independence.
**
**	\param [in] on_init_hlr   - handler to call during sleep_init()
**	\param [in] sleep_on_enter - handler to call before entering sleep_to_bed() mode.
**	\param [in] sleep_on_exit - handler to call just after exit from sleep_to_bed()
**	                            mode.
**
**	\return true if registration succeeded.
**/
bool sleep_register_peripheral(sleep_on_init on_init_hlr,
							   sleep_on_enter on_enter_hlr,
		                       sleep_on_exit on_exit_hlr);

/*!	\brief Enter soft low power mode where only CPU is off.
**
** After calling this function, software execution will halt until
** an interrupt occurs. At least one interrupt shall be configured
** as a wake up source or device will never be able to exit sleep.
**
** When wake-up interrupt is fired, interrupt handler will be executed
** first and, after that, background software will resume from after
** sleep_on_couch() function.
**
**	\return None.
**/
void sleep_on_the_couch(void);

/*!	\brief Enter hard low power mode where CPU and most of peripherals are off.
**
** This function will call on-enter handlers first and will then halt hardware/software
** execution. Hardware/software is restarted when one of the still active interrupts
** is fired. At least, one interrupt shall be configured as a wake-up source or device
** will never be able to exit sleep.
**
** When wake-up interrupt is fired, interrupt handler will be executed first. After that
** on-exit handlers are executed and background software can resume from after
** sleep_on_bed() function.
**
**	\return None.
**/
void sleep_on_the_bed(void);
/****************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* __SLEEP_H */
/****************************************************************/
