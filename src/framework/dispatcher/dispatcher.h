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
    virtual void run() const = 0;
};

/*!    \brief Pointer type for iTasks.
**
** Shared pointers allows ownership of the task lifetime to be
** shared among the Dispatcher and any creator/user of the concrete task
** instance.
**/
using iTaskPtr = std::shared_ptr<const iTask>;

class Dispatcher
{
public:
    Dispatcher();
    ~Dispatcher();

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
    struct dispatchRecord
    {
        iTaskPtr task;
        dispatchTimestamp period;
    };
    dispatchTimestamp timestamp;
    dispatchTimestamp headTimestamp;
    bool timerActive;
    std::map<dispatchTimestamp, dispatchRecord> timetable;
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

/*!    \brief HAL timer has already been grabbed.
**
** Exception raised when the HAL timer has already been grabbed.
**/
class HALTimerNotAvailable : public std::exception
{
public:
    const char * what ()
    {
        return "All available HAL timers have been allocated to a dispatcher.";
    }
};

/****************************************************************/
#endif /* __DISPATCHER_H */
/****************************************************************/
