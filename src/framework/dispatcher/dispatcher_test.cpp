/*!\file dispatcher_test.cpp
** \author 
** \copyright
** \brief Unit test for dispatcher.cpp
** \details This file contains simple unit tests to verify basic functionalities
**          of the "Dispatcher" class.
**/
/****************************************************************/

#include "dispatcher.h"
#include "timer_host_stubs.h"
#include<iostream>
#include<map>
#include<vector>
#include<memory>
#include<utility>
#include<stdexcept>
/****************************************************************/

/*!    \brief Concrete dispatcher task for test.
**
** Implementor of iTask interface for unit tests.
**
** Constructor takes a reference to a "runCount" variable
** that gets incremented by TestTask::run() method.
**
** This is used to verify if the method gets called or not
** at a specific time.
**
** "runCount" is not part of the internal state of TestTask
** because, for some tests, it needs to survive the existence
** of the TestTask itself.
**/
class TestTask : public iTask
{
public:
    TestTask(unsigned int i, unsigned int &rc): id{i}, runCount {rc} {};
    ~TestTask() override {};
    void run(void) override{ runCount++; std::cout << "Running TestTask{" << id << "} - runCount: " << runCount << std::endl; };
private:
    unsigned int id;
    unsigned int &runCount;
};

/*!    \brief Dispatcher friend class used for verification.
**
** This class has access to the full state of a Dispatcher instance and therefore
** can be used to verify execution in details during unit tests.
**/
class DispatcherUnitTest
{
public:
    DispatcherUnitTest(Dispatcher &d): dispatcher{d} {};
    void printTimetable(void);
    void verifyTimerState(bool active);
    void verifyTimetable(std::vector< std::shared_ptr<iTask> > &expectedTasks);
    void destroyDispatcher(void);
private:
    Dispatcher &dispatcher;
};

/*!    \brief Destroys the Dispatcher singleton instance.
**
** This is normally used as part of test tidy-up to reset the internal
** state of the Dispatcher.
**/
void DispatcherUnitTest::destroyDispatcher(void)
{
    delete Dispatcher::instance;
    Dispatcher::instance = nullptr;
}

/*!    \brief Prints the Dispatcher timetable.
**
** Prints the content of the Dispatcher timetable at the current time.
**/
void DispatcherUnitTest::printTimetable(void)
{
    std::cout << "   Timetable @time=" << timer_get_tick() << ":" << std::endl;
    if(dispatcher.timetable.empty())
    {
        std::cout << "Empty" << std::endl;
        return;
    }
    for(auto t : dispatcher.timetable)
    {
        std::cout << "   " << t.first << " - task: " << t.second.task.lock() << ", period: " << t.second.period << std::endl;
    }
}

/*!    \brief Verify consistency of Dispatcher timetable.
**
** \param[in] expectedTasks - tasks that should be present in the timetable.
**
** \throws runtime_error in case of negative outcome.
**
** Check that the tasks in the timetable match the expectations.
** Check also coherence of HAL timer state and headTimestamp.
**/
void DispatcherUnitTest::verifyTimetable(std::vector< std::shared_ptr<iTask> > &expectedTasks)
{
    std::vector< std::shared_ptr<iTask> > actualTasks;
    auto headT = dispatcher.timetable.cbegin()->first;

    std::cout <<" Verifying timetable..." << std::endl;
    printTimetable();
    for(auto t : dispatcher.timetable)
    {
        actualTasks.push_back(t.second.task.lock());
    }
    if(expectedTasks != actualTasks)
    {
        throw std::runtime_error("FAIL: dispatcher timetable!!");
    }
    if(expectedTasks.empty() && dispatcher.timerActive)
    {
        throw std::runtime_error("FAIL: inconsistent timer active flag!!");
    }

    if(dispatcher.timerActive && dispatcher.headTimestamp != headT)
    {
        throw std::runtime_error("FAIL: inconsistent timer head timestamp!!");
    }
    std::cout << " OK!" << std::endl;
}

/*!    \brief Verify coherence of HAL timer state.
**
** \param[in] active - whether expectation is to have timer
**                     active or not.
**
** \throws runtime_error in case of negative outcome.
**
** Check that the HAL timer state matches expectations.
**/
void DispatcherUnitTest::verifyTimerState(bool active)
{
    if(active)
    {
        std::cout << " Check that timer is running";
    }
    else
    {
        std::cout << " Check that timer is not running";
    }
    if(((!active) && timer_host_is_timer_active()) ||
       ((active) && !timer_host_is_timer_active()))
    {
        throw std::runtime_error("FAIL: Timer state verification!!");
    }
    std::cout << " - OK!" <<std::endl;
}

/*!    \brief Verify run counter for test task.
**
** \param[in] actual - actual value of the run counter.
** \param[in] expected - expected value of the run counter.
**
** \throws runtime_error if they differ.
**
** Run counter is a counter that gets incremented by TestTask.run()
** method each time it gets called. This function verifies that TestTask.run()
** has been called when it needs to be called by checking the value
** of the counter.
**/
void verifyRunCount(unsigned int actual, unsigned int expected)
{
    if(expected == 0)
    {
        std::cout << " Check that task has not run.";
    }
    else if(expected == 1)
    {
        std::cout << " Check that task has run once.";
    }
    else
    {
        std::cout << " Check that task has run " << expected << " times.";
    }
    std::cout << " Actual runs: " << actual;
    if(actual != expected)
    {
        throw std::runtime_error("FAIL: runCount verification failed!!");
    }
    std::cout << " - OK!" << std::endl;
}

/*!    \brief Verify one-shot dispatches.
**/
void testSimpleOneShot(void)
{
    std::cout << "  <<testSimpleOneShot>>" << std::endl;
    timer_init();
    timer_host_reset_time();

    unsigned int runCount = 0;
    auto &dispatcher { Dispatcher::get() };
    auto testTask1 { std::make_shared<TestTask>(1, runCount) };
    DispatcherUnitTest dispUT {dispatcher};

    std::vector< std::shared_ptr<iTask> > expectedTasks{testTask1};
    std::vector< std::shared_ptr<iTask> > expectedTasks2{};
    std::cout <<"Adding testTask1 (one-shot) @ time=25" <<std::endl;
    dispatcher.addTaskOneShot(testTask1, 25);
    std::cout <<" Wait 24 ms" <<std::endl;
    timer_host_elapse_time(24);
    dispUT.verifyTimetable(expectedTasks);
    verifyRunCount(runCount, 0);
    std::cout <<" Wait 1 ms" <<std::endl;
    timer_host_elapse_time(1);
    verifyRunCount(runCount, 1);
    dispUT.verifyTimerState(false);
    dispUT.verifyTimetable(expectedTasks2);
    dispUT.destroyDispatcher();
    std::cout << std::endl;
    std::cout << std::endl;
}

/*!    \brief Verify periodic dispatches.
**/
void testSimplePeriodic(size_t testIterations)
{
    std::cout << "  <<testSimplePeriodic>>" << std::endl;
    timer_init();
    timer_host_reset_time();

    unsigned int runCount = 0;
    auto &dispatcher { Dispatcher::get() };
    auto testTask1 { std::make_shared<TestTask>(1, runCount) };
    DispatcherUnitTest dispUT {dispatcher};

    std::vector< std::shared_ptr<iTask> > expectedTasks{testTask1};
    std::cout <<"Adding testTask1 (periodic) @ time=25" <<std::endl;
    dispatcher.addTaskPeriodic(testTask1, 25);

    for(auto i = 0; i < testIterations; i++)
    {
        std::cout << "Iteration " << i << " out of " << testIterations << ":" <<std::endl;
        std::cout <<" Wait 24 ms" <<std::endl;
        timer_host_elapse_time(24);
        dispUT.verifyTimetable(expectedTasks);
        verifyRunCount(runCount, i);
        std::cout <<" Wait 1 ms" <<std::endl;
        timer_host_elapse_time(1);
        verifyRunCount(runCount, i + 1);
        dispUT.verifyTimerState(true);
        dispUT.verifyTimetable(expectedTasks);
    }
    dispUT.destroyDispatcher();
    std::cout << std::endl;
    std::cout << std::endl;
}

/*!    \brief Verify dispatch removal.
**/
void testRemove(void)
{
    std::cout << "  <<testRemove>>" << std::endl;
    timer_init();
    timer_host_reset_time();

    unsigned int runCount[3] = {0, 0, 0};
    auto &dispatcher { Dispatcher::get() };
    auto testTask1 { std::make_shared<TestTask>(1, runCount[0]) };
    auto testTask2 { std::make_shared<TestTask>(2, runCount[1]) };
    auto testTask3 { std::make_shared<TestTask>(3, runCount[2]) };
    DispatcherUnitTest dispUT {dispatcher};

    std::vector< std::shared_ptr<iTask> > expectedTasks1{testTask1};
    std::vector< std::shared_ptr<iTask> > expectedTasks2{testTask1, testTask2};
    std::vector< std::shared_ptr<iTask> > expectedTasks3{testTask3, testTask1, testTask2};
    std::vector< std::shared_ptr<iTask> > empty{};

    dispUT.verifyTimerState(false);

    std::cout <<"Adding task1" <<std::endl;
    dispatcher.addTaskPeriodic(testTask1, 25);
    dispUT.verifyTimerState(true);

    std::cout <<"Adding task2" <<std::endl;
    dispatcher.addTaskPeriodic(testTask2, 50);
    dispUT.verifyTimerState(true);

    std::cout <<"Adding task3" <<std::endl;
    dispatcher.addTaskPeriodic(testTask3, 15);
    dispUT.verifyTimerState(true);

    std::cout <<"Removing task3" <<std::endl;
    dispatcher.removeTask(testTask3);
    dispUT.verifyTimerState(true);
    dispUT.verifyTimetable(expectedTasks2);

    std::cout <<"Removing task2" <<std::endl;
    dispatcher.removeTask(testTask2);
    dispUT.verifyTimerState(true);
    dispUT.verifyTimetable(expectedTasks1);

    std::cout <<"Removing task1" <<std::endl;
    dispatcher.removeTask(testTask1);
    dispUT.verifyTimerState(false);
    dispUT.verifyTimetable(empty);
    dispUT.destroyDispatcher();
    std::cout << std::endl;
    std::cout << std::endl;
}

/*!    \brief Verify singleton pattern is enforced.
**/
void testSingleton(void)
{
    std::cout << "  <<testSingleton>>" << std::endl;
    std::cout << "Getting first dispatcher" << std::endl;
    auto &dispatcher1 { Dispatcher::get() };
    std::cout << "Getting second dispatcher" << std::endl;
    auto &dispatcher2 { Dispatcher::get() };
    std::cout << "Check they refer to the same object.";
    if(&dispatcher1 != &dispatcher2)
    {
        throw std::runtime_error("TWo dispatcher instances didn't throw exception!!");
    }
    std::cout << " - OK!" << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
}

void testDanglingTaskOneShot(void)
{
    timer_init();
    timer_host_reset_time();

    unsigned int runCount = 0;
    std::cout << "  <<testDanglingTaskOneShot>>" << std::endl;
    auto &dispatcher { Dispatcher::get() };
    auto testTask1 { std::make_shared<TestTask>(1, runCount) };
    DispatcherUnitTest dispUT {dispatcher};

    std::vector< std::shared_ptr<iTask> > expectedTasks{testTask1};
    std::cout <<"Adding testTask1 (one-shot) @ time=25" <<std::endl;
    dispatcher.addTaskOneShot(testTask1, 25);
    std::cout <<" Wait 24 ms" <<std::endl;
    timer_host_elapse_time(24);
    verifyRunCount(runCount, 0);
    dispUT.verifyTimetable(expectedTasks);

    std::cout <<"Destroy testTask1" << std::endl;
    testTask1.reset();
    /* expectedTasks shall also be empty now. */
    expectedTasks.clear();
    dispUT.printTimetable();

    std::cout <<" Wait 1 ms" <<std::endl;
    timer_host_elapse_time(1);
    verifyRunCount(runCount, 0);
    dispUT.verifyTimerState(false);
    dispUT.verifyTimetable(expectedTasks);
    dispUT.destroyDispatcher();
    std::cout << std::endl;
    std::cout << std::endl;
}

void testDanglingTaskPeriodic(void)
{
    timer_init();
    timer_host_reset_time();

    unsigned int runCount = 0;
    std::cout << "  <<testDanglingTaskPeriodic>>" << std::endl;
    auto &dispatcher { Dispatcher::get() };
    auto testTask1 { std::make_shared<TestTask>(1, runCount) };
    DispatcherUnitTest dispUT {dispatcher};

    std::vector< std::shared_ptr<iTask> > expectedTasks{testTask1};
    std::cout <<"Adding testTask1 (periodic) @ time=25" <<std::endl;
    dispatcher.addTaskPeriodic(testTask1, 25);
    std::cout <<" Wait 24 ms" <<std::endl;
    timer_host_elapse_time(24);
    verifyRunCount(runCount, 0);
    dispUT.verifyTimetable(expectedTasks);

    std::cout <<" Wait 1 ms" <<std::endl;
    timer_host_elapse_time(1);
    verifyRunCount(runCount, 1);
    dispUT.verifyTimetable(expectedTasks);
    dispUT.verifyTimerState(true);

    std::cout <<"Destroy testTask1" << std::endl;
    testTask1.reset();
    /* expectedTasks shall also be empty now. */
    expectedTasks.clear();
    dispUT.printTimetable();

    std::cout <<" Wait 1 ms" <<std::endl;
    timer_host_elapse_time(26);
    verifyRunCount(runCount, 1);
    dispUT.verifyTimetable(expectedTasks);
    dispUT.verifyTimerState(false);

    dispUT.destroyDispatcher();
    std::cout << std::endl;
    std::cout << std::endl;
}

int main(void)
{
    testSimpleOneShot();
    testSimplePeriodic(3);
    testRemove();
    testSingleton();
    testDanglingTaskOneShot();
    testDanglingTaskPeriodic();
}
/****************************************************************/
