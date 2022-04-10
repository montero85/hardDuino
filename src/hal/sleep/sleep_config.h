/*!\file sleep_config.h
** \author 
** \copyright TODO
** \brief Static configuration for sleep module
** \detail This is a private header that can be used to statically configure
**          the megavr implementation of the driver.
**/
/****************************************************************/
#ifndef __SLEEP_CONFIG_H
#define __SLEEP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdbool.h>
/****************************************************************/

/*!    \brief Maximum number of registered peripherals
**
** This is the maximum number of peripherals that can be registered
** using sleep_register_peripheral(...) API.
**/
#define SLEEP_MAX_REGISTERED_PERIPHERALS 4

/****************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* __SLEEP_CONFIG_H */
/****************************************************************/
