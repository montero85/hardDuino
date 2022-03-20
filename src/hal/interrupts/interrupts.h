/*!\file interrupts.h
** \author 
** \copyright TODO
** \brief Provide interrupts functionality that needs to be exported to other subsystems
** \details 
**/
/****************************************************************/
#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdbool.h>
/****************************************************************/

/*!\brief Disable interrupts
**
**	Note: interrupts_off, interrupts_on implementation is not safe
**	      to nesting. Calls shall not be nested.
**/
#define interrupts_off() cli()

/*!\brief Enable interrupts
**
**	Note: interrupts_off, interrupts_on implementation is not safe
**	      to nesting. Calls shall not be nested.
**/
#define interrupts_on() sei()

#ifdef __cplusplus
}
#endif

#endif /* __INTERRUPTS_H */
/****************************************************************/
