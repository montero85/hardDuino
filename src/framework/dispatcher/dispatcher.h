/*!\file dispatcher.h
** \author 
** \copyright TODO
** \brief High level time keeper of the system.
** \details Dispatcher module allows the execution of tasks (implementing iTask) at a specific
** time recurrently or in a one shot fashion.
** It achieves that by providing a wrapper around the C hal module.
**/
/****************************************************************/
#ifndef __DISPATCHER_H
#define __DISPATCHER_H

#include "timestamp.h"
#include <cstdint>
#include <memory>
#include <map>
/****************************************************************/

/*!    \brief Type for timestamps used by dispatcher module.
 *
 * TODO: Needs to find the appropriate value for the timer type.
 *
**/
using dispatchTimestamp = timestamp<uint32_t>;

/*!    \brief Virtual interface for dispatcher tasks.
**
** Defines the virtual type for tasks run through the dispatcher.
** Each class code that requires timed execution through the dispatcher
** needs to provide an implementation for this interface.
**/
class iTask
{
public:
    virtual ~iTask() {};

    /*!    \brief Virtual method for iTasks interface.
    **
    ** The method that gets executed when the dispatch timer expires.
    **/
    virtual void run() = 0;
};

/*!    \brief Pointer type for iTasks.
**
** For simplification, Dispatcher doesn't have interest in
** participating in lifetime management of the iTasks it stores.
**/
using iTaskPtr = std::weak_ptr<iTask>;

/*!    \brief Dispatcher for tasks to run at specific time.
**
** This module wraps the C HAL timer to provide timed execution
** of tasks. Tasks must implement the iTask interface.
** Users can request the execution of tasks in a one-shot or
** as in a periodic fashion.
**
** Dispatcher is a global facility: different modules will requests
** execution of tasks always to the same dispatcher. Also, Dispatcher
** requires exclusive access to the HAL timer.
**
** For this reason, it implements a Singleton. To avoid the inconvenience
** of the pattern, user code can rely on ClockEvent module instead.
** This keeps use of Dispatcher confined within its internal implementation
** and provides timer facilities in a "per-instance" way.
**/
class Dispatcher
{
public:
    ~Dispatcher() = default;
    /** Singleton pattern: disable copy constructor/operator.
     **/
    Dispatcher(const Dispatcher &) = delete;
    Dispatcher& operator=(const Dispatcher &) = delete;

    /*!    \brief Get singleton instance of Dispatcher.
    **
    ** \return Reference to the unique Dispatcher object.
    **
    ** Lazily instantiate Dispatcher and return a reference to the
    ** singleton instance.
    **
    ** Blocks interrupts for thread safety.
    **/
    static Dispatcher& get(void);

    /*!    \brief Add a periodic task to the time dispatcher.
    **
    ** \param [in] task - Pointer to the task to be added.
    ** \param [in] ms - Period in ms.
    **
    ** Adds a task to the dispatcher so that it is run
    ** periodically every "ms" milliseconds.
    **/
    void addTaskPeriodic(iTaskPtr task, dispatchTimestamp ms);

    /*!    \brief Add a "one-shot" task to the time dispatcher.
    **
    ** \param [in] task - Pointer to the task to be added.
    ** \param [in] ms - Delay in ms since the time of the call.
    **
    ** Adds a task to the dispatcher so that it is run
    ** once with a delay of "ms" since the call to this
    ** function.
    **/
    void addTaskOneShot(iTaskPtr task, dispatchTimestamp ms);

    /*!    \brief Remove an active task from the time dispatcher.
    **
    ** Remove a one-shot or periodic task scheduled for run in the
    ** time dispatcher.
    **/
    bool removeTask(iTaskPtr task);

private:
    static Dispatcher *instance;

    struct dispatchRecord
    {
        iTaskPtr task;
        dispatchTimestamp period;
    };
    dispatchTimestamp timestamp;
    dispatchTimestamp headTimestamp;
    bool timerActive;
    std::map<dispatchTimestamp, dispatchRecord> timetable;
    /** Constructor is private.
     **/
    Dispatcher(): timestamp{0}, headTimestamp{0}, timerActive{false} {};
    void addTask(iTaskPtr task, dispatchTimestamp ms, bool periodic);
    void refreshTimestamp(void);
    void updateHeadAndTimer(void);
    void processTimetable(void);
    /** Unit test class to access private properties from separate
     ** unit test file.
     **/
    friend class DispatcherUnitTest;
    friend void on_hal_timer_callback(void);
};

/****************************************************************/
#endif /* __DISPATCHER_H */
/****************************************************************/
