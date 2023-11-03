/*!\file timer_host_stubs.c
** \author
** \copyright
** \brief Stubs to "simulate" the timer HAL on the host.
** \details This is a stub module to "simulate" the timer HAL
**          unit tests run on host pc. There is
**/
/****************************************************************/
#include"timer_host_stubs.h"
#include<stdint.h>

typedef void (*timer_callback_t)(void);

/*!    \brief Software counter to simulate hardware timer.
**
** Unit test host stub used a software variable to simulate
** the actual hardware counter. This software counter has to
** be manually (and synchronously) incremented by the unit test
** using provided API.
**/
uint32_t ut_timer = 0;

struct timer_control
{
    bool active;
    uint32_t next_expiry;
    timer_callback_t clbk;
};

struct timer_control timerCtrl;

/*!    \brief Stub to timer_init HAL function.
**/
void timer_init(void)
{
    timerCtrl.active = false;
    timerCtrl.next_expiry = 0;
    timerCtrl.clbk = nullptr;
}

/*!    \brief Stub to timer_stop HAL function.
**/
void timer_stop(void)
{
    timerCtrl.active = false;
}

/*!    \brief Stub to timer_start_one_shot_ms HAL function.
**
** Sets callback to be executed when ut_timer equals its current value + ms.
**/
void timer_start_one_shot_ms(uint16_t ms, timer_callback_t clbk)
{
    timerCtrl.active = true;
    timerCtrl.next_expiry = ut_timer + ms;
    timerCtrl.clbk = clbk;
}

/*!    \brief Stub to timer_get_tick HAL function.
**/
uint16_t timer_get_tick(void)
{
    return ut_timer;
}

/** This is not a stub but an helper to run the host timer
 ** infrastructure.
 **/
bool timer_host_is_timer_active(void)
{
    return timerCtrl.active;
}

/** This is not a stub but an helper to run the host timer
 ** infrastructure.
 **/
void timer_host_reset_time(void)
{
    ut_timer = 0;
}

/** This is not a stub but an helper to run the host timer
 ** infrastructure.
 **/
void timer_host_elapse_time(uint32_t time)
{
    if(!timerCtrl.active)
    {
        ut_timer +=time;
        return;
    }
    while(time > 0)
    {
        time--;
        ut_timer++;
        if(ut_timer >= timerCtrl.next_expiry)
        {
            /* De-activate timer. Callback may reactivate it. */
            timerCtrl.active = false;
            timerCtrl.clbk();
            if(!timerCtrl.active)
            {
                ut_timer += time;
                return;
            }
        }
    }
}

