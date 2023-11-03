/*!\file timer_host_stubs.h
** \author
** \copyright
** \brief Stubs to "simulate" the timer HAL on the host.
** \details This is a stub module to "simulate" the timer HAL
**          on unit tests run on the host pc.
**
**          There is no intention to support timer stubbing in a
**          "clever way" and exploit host timed execution
**          capabilities. Instead, this module provides a very
**          basic implementation based around a software counter
**          that can be synchronously incremented using an API.
**          When incrementing the timer counter hits on of the
**          deadlines set to the timer module, the appropriate
**          callback (registered via stub to the timer API)
**          is called.
**
**          This header only contains declaration for "helper"
**          functions to run the stub infrastructure.
**          IT NEEDS ONLY TO BE INCLUDED IN HOST TESTS.
**          Timer stubbed API is still declared in timer.h.
**/
/****************************************************************/

#ifndef __TIMER_HOST_STUBS_H
#define __TIMER_HOST_STUBS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "timer.h"

/*!    \brief Reset the timer counter.
**
** Shall be called at the start of each test to reset,
** the software variable simulating the hardware counter.
**/
void timer_host_reset_time(void);

/*!    \brief Increment the timer counter.
**
** \param [in] time - Amount of time to elapse.
**
** Simulate passing of time by incrementing the software
** counter. If the stubbed HAL timer is active and a callback
** is registered, callback will be called when software counter
** matches its deadline.
**/
void timer_host_elapse_time(uint32_t time);

/*!    \brief Check HAL timer is curently active.
**
** \return true if stubbed HAL timer is active.
**
** TODO: Consider moving into the actual "HAL" timer
** API.
**/
bool timer_host_is_timer_active(void);

#ifdef __cplusplus
}
#endif

#endif /* __TIMER_HOST_STUBS_H */
