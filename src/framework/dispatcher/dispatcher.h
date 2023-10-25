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
using d_timestamp = timestamp<uint32_t>;

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
    virtual void run() const = 0;
};

using iTaskPtr = std::shared_ptr<const iTask>;

class Dispatcher
{
public:
    Dispatcher();
    ~Dispatcher();
    void addTaskPeriodic(iTaskPtr task, d_timestamp ms);
    void addTaskOneShot(iTaskPtr task, d_timestamp ms);
    bool removeTask(iTaskPtr task);
    void process_timetable(void);
private:
    struct d_record
    {
        iTaskPtr task;
        d_timestamp period;
    };
    d_timestamp timestamp;
    d_timestamp head_timestamp;
    bool timer_active;
    std::map<d_timestamp, d_record> timetable;
    void addTask(iTaskPtr task, d_timestamp ms, bool periodic);
    void refreshTimestamp(void);
    void update_head_and_timer(void);

    /** Unit test class to access private properties from separate
     ** unit test file.
     **/
    friend class DispatcherUnitTest;
};

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
