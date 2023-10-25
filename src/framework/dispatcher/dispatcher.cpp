/*!\file dispatcher.cpp
** \author 
** \copyright
** \brief Implementation of dispatcher module.
** \details TODO
**/
/****************************************************************/

#include "dispatcher.h"
#include <map>
#include <iostream>
#include <algorithm>
#include <utility>

#define STUBS
#ifdef STUBS
extern uint32_t ut_timer;
typedef void (*timer_callback_t)(void);
extern void timer_stop(void);
extern void timer_start_one_shot_ms(uint16_t ms, timer_callback_t clbk);
#define interrupts_on()
#define interrupts_off()
#endif /* STUBS */

/** Maximum value for the ms hal timer.
 **/
#define HAL_TIMER_MAX_RANGE_MS UINT16_MAX

/** Minimum value used to reload the HAL timer.
 **/
#define HAL_TIMER_MIN_RELOAD_MS 1

/** Period value for non periodic tasks.
 **/
#define NO_PERIOD              0

Dispatcher *registered_dispatcher;

/****************************************************************/
void on_hal_timer_callback(void)
{
    if(registered_dispatcher != nullptr)
    {
        registered_dispatcher->process_timetable();
    }
}

Dispatcher::Dispatcher(void): timestamp{0}, head_timestamp{0}, timer_active{false}
{
    if(registered_dispatcher != nullptr)
    {
        throw HALTimerNotAvailable();
    }
    registered_dispatcher = this;
}

Dispatcher::~Dispatcher(void)
{
    registered_dispatcher = nullptr;
}

void Dispatcher::refreshTimestamp(void)
{
    timestamp = ut_timer;
}

void Dispatcher::process_timetable(void)
{
    if(timetable.empty())
    {
        /* Shouldn't happen but just in case. */
        return;
    }

    refreshTimestamp();
    auto it = timetable.begin();
    while(it != timetable.end())
    {
        auto task = it->second.task;

        if(it->first > timestamp)
        {
            /* Reached new head when tasks are not yet expired. */
            break;
        }
        task->run();
        refreshTimestamp();
        it++;
    }
    for(auto it2 = timetable.begin(); it2 != it; it2++)
    {
        auto task = it2->second.task;
        auto period = it2->second.period;
        /* Run the task*/
        /* TODO: measure this time and check if this operation
         * needs to be postponed (i.e. run all the tasks first,
         * then update the timetable).
         */
        /* Update the timetable. */
        timetable.erase(it2);
        if(period!= NO_PERIOD)
        {
            auto deadline = timestamp + period;
            timetable[deadline] = {task , period};
        }
        else if(timetable.empty())
        {
            break;
        }
    }
    /* FInally update the new head and the hal timer to match it. */
    update_head_and_timer();
}

void Dispatcher::update_head_and_timer(void)
{
    auto head = timetable.cbegin();

    if(timetable.empty())
    {
        /* "Remove" operations can dry out the timetable. */
        timer_stop();
        timer_active = false;
    }
    else if((head->first != head_timestamp) || !timer_active)
    {
        timer_active = true;
        /* Head element changed (after "add" or "remove").
         * Refresh the hal timer.
         */
        timer_stop();
        refreshTimestamp();
        /* Ensure first deadline is not past the timestamp, otherwise reload
         * a minimum value into the hal timer. In this case, first deadline
         * will be further postponed. It should be rare enough not to worry about.
         * Also design constraints don't consider hard real timing.
         */
        auto timer_value = head->first > timestamp + HAL_TIMER_MIN_RELOAD_MS ?
                head->first - timestamp : (d_timestamp)HAL_TIMER_MIN_RELOAD_MS;

        /* Ensure also that we don't exceed the max range. If we do, we'd need to
         * progressively hop towards the deadline.
         */
        timer_value = std::min(timer_value, (d_timestamp)HAL_TIMER_MAX_RANGE_MS);
        timer_start_one_shot_ms(timer_value, on_hal_timer_callback);
        head_timestamp = head->first;
    }
}

bool Dispatcher::removeTask(iTaskPtr task)
{
    bool res = false;

    interrupts_off();
    for(auto it = timetable.begin(); it != timetable.end(); it++)
    {
        if(it->second.task == task)
        {
            res = true;
            timetable.erase(it);
            update_head_and_timer();
            break;
        }
    }
    interrupts_on();
    return res;
}

void Dispatcher::addTask(iTaskPtr task, d_timestamp ms, bool periodic)
{
    refreshTimestamp();
    auto period = periodic ? ms : NO_PERIOD;
    interrupts_off();
    auto deadline = timestamp + ms;
    timetable[deadline] = {task , period};
    update_head_and_timer();
    interrupts_on();
}

void Dispatcher::addTaskPeriodic(iTaskPtr task, d_timestamp ms)
{
    addTask(task, ms, true);
}

void Dispatcher::addTaskOneShot(iTaskPtr task, d_timestamp ms)
{
    addTask(task, ms, false);
}
/****************************************************************/
