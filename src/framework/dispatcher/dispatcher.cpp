/*!\file dispatcher.cpp
** \author 
** \copyright
** \brief Implementation of dispatcher module.
** \details Dispatcher module wraps the HAL timer API to schedule
** its tasks. For doing so, it registers an on_hal_timer_callback
** with the HAL timer.
**/
/****************************************************************/

#include "dispatcher.h"
#include "timer.h"
#include <map>
#include <iostream>
#include <algorithm>
#include <utility>

#define STUBS
#ifdef STUBS
// TODO: Remove!!!!!!!!!!!!!!!!!!!!!!!!
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

/** Static instance of the dispatcher singleton.
 **
 ** Lazily initialised by Dispatcher::get()
 **/
Dispatcher *Dispatcher::instance = nullptr;

/****************************************************************/

/*! Call back to register with the timer HAL.
 **
 ** Callback called on timer interrupts. It will
 ** process the dispatcher timetable.
 **/
void on_hal_timer_callback(void)
{
    /* If this function is called, instance is initialised. */
    Dispatcher::instance->processTimetable();
}

/** Updates dispatcher timestamp to the current time.
 **/
void Dispatcher::refreshTimestamp(void)
{
    timestamp = timer_get_tick();
}

/** Goes through the backlog of expired iTasks and calls
 ** their "run" method.
 **/
void Dispatcher::processTimetable(void)
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
        if(it->first > timestamp)
        {
            /* Reached new head when tasks are not yet expired. */
            break;
        }

        /* Convert weak pointer to shared ptr */
        auto task = it->second.task.lock();

        if(task != nullptr)
        {
            task->run();
        }
        else
        {
            /* Dangled pointer: Invalidate task for deletion. */
            it->second.period = NO_PERIOD;
        }
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
    updateHeadAndTimer();
}

/** Ensures that the HAL timer is kept in synch with the "head" task in the
 ** timetable (earliest to expire).
 **
 ** It needs to be called every time an iTask is added/removed to the
 ** timetable.
 **/
void Dispatcher::updateHeadAndTimer(void)
{
    auto head = timetable.cbegin();

    if(timetable.empty())
    {
        /* "Remove" operations can dry out the timetable. */
        timer_stop();
        timerActive = false;
    }
    else if((head->first != headTimestamp) || !timerActive)
    {
        timerActive = true;
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
                head->first - timestamp : (dispatchTimestamp)HAL_TIMER_MIN_RELOAD_MS;

        /* Ensure also that we don't exceed the max range. If we do, we'd need to
         * progressively hop towards the deadline.
         */
        timer_value = std::min(timer_value, (dispatchTimestamp)HAL_TIMER_MAX_RANGE_MS);
        timer_start_one_shot_ms(timer_value, on_hal_timer_callback);
        headTimestamp = head->first;
    }
}

/** Helper function to add iTasks to the timetable.
 **/
void Dispatcher::addTask(iTaskPtr task, dispatchTimestamp ms, bool periodic)
{
    refreshTimestamp();
    auto period = periodic ? ms : NO_PERIOD;
    interrupts_off();
    auto deadline = timestamp + ms;
    timetable[deadline] = {task , period};
    updateHeadAndTimer();
    interrupts_on();
}

Dispatcher& Dispatcher::get(void)
{
    interrupts_off();
    if(instance == nullptr)
    {
        instance = new Dispatcher();
    }
    interrupts_on();
    return *instance;
}

void Dispatcher::addTaskPeriodic(iTaskPtr task, dispatchTimestamp ms)
{
    addTask(task, ms, true);
}

void Dispatcher::addTaskOneShot(iTaskPtr task, dispatchTimestamp ms)
{
    addTask(task, ms, false);
}

bool Dispatcher::removeTask(iTaskPtr task)
{
    bool res = false;

    interrupts_off();
    for(auto it = timetable.begin(); it != timetable.end(); it++)
    {
        if(it->second.task.lock() == task.lock())
        {
            res = true;
            timetable.erase(it);
            updateHeadAndTimer();
            break;
        }
    }
    interrupts_on();
    return res;
}
/****************************************************************/
