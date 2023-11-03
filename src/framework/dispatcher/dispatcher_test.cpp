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
**/
class TestTask : public iTask
{
public:
    TestTask(unsigned int i): id{i} {};
    ~TestTask() override {};
    void run(void) const override{ std::cout << "Running TestTask{" << id << "}" << std::endl; };
private:
    unsigned int id;
};

/*!    \brief Dispatcher friend class used for verification.
**
** This class has access to the full state of a Dispatcher instance and therefore
** can be used to verify execution in details during unit tests.
**/
class DispatcherUnitTest
{
public:
    DispatcherUnitTest(std::shared_ptr<Dispatcher> d): dispatcher{d} {};
    void printTimetable(void);
    void verifyTimerState(bool active);
    void verifyTimetable(std::vector<iTaskPtr> &expectedTasks);
private:
    std::shared_ptr<Dispatcher> dispatcher;
};

/*!    \brief Prints the Dispatcher timetable.
**
** Prints the content of the Dispatcher timetable at the current time.
**/
void DispatcherUnitTest::printTimetable(void)
{
    std::cout << "Timetable @time: " << timer_get_tick() << ":" << std::endl;
    if(dispatcher->timetable.empty())
    {
        std::cout << "Empty" << std::endl;
        return;
    }
    for(auto t : dispatcher->timetable)
    {
        std::cout << t.first << " - task: " << t.second.task << ", period: " << t.second.period << std::endl;
    }
}

/*!    \brief Verify consistency of Dispatcher timetable.
**
** \param[in] expectedTasks - tasks that should be present in the timetable.
**
** \throws runtime_error in case of negative outcome.
**
** Check that the tasks in the timetable match the expectations.
** Check also coherence of HAL timer state and headTimestamp.ß
**/
void DispatcherUnitTest::verifyTimetable(std::vector<iTaskPtr> &expectedTasks)
{
    std::vector<iTaskPtr> actualTasks;
    auto t = dispatcher->timetable.cbegin()->first;

    for(auto t : dispatcher->timetable)
    {
        actualTasks.push_back(t.second.task);
    }
    if(expectedTasks != actualTasks)
    {
        throw std::runtime_error("FAIL: dispatcher timetable!!");
    }
    if(expectedTasks.empty() && dispatcher->timerActive)
    {
        throw std::runtime_error("FAIL: inconsistent timer active flag!!");
    }

    if(dispatcher->timerActive && dispatcher->headTimestamp != t)
    {
        throw std::runtime_error("FAIL: inconsistent timer head timestamp!!");
    }
}

/*!    \brief Verify coherence of HAL timer state.
**
** \param[in] active - whether expectation is to have timer
**                     active or not.
**
** \throws runtime_error in case of negative outcome.
**
** Check that the HAL timer state matche expectations.
**/
void DispatcherUnitTest::verifyTimerState(bool active)
{
    if(((!active) && timer_host_is_timer_active()) ||
       ((active) && !timer_host_is_timer_active()))
    {
        throw std::runtime_error("FAIL: Timer state verification!!");
    }
}

/*!    \brief Verify one-shot dispatches.
**/
void testSimpleOneShot(void)
{
    timer_init();
    timer_host_reset_time();

    auto dispatcher { std::make_shared<Dispatcher>()};
    auto testTask1 { std::make_shared<TestTask>(1) };
    DispatcherUnitTest dispUT {dispatcher};

    std::vector<iTaskPtr> expectedTasks{testTask1};
    std::vector<iTaskPtr> expectedTasks2{};
    std::cout <<" Wait 24 ms" <<std::endl;
    dispatcher->addTaskOneShot(testTask1, 25);
    dispUT.printTimetable();
    timer_host_elapse_time(24);
    dispUT.verifyTimetable(expectedTasks);
    std::cout <<" Wait 1 ms" <<std::endl;
    timer_host_elapse_time(1);
    dispUT.printTimetable();
    dispUT.verifyTimerState(false);
    dispUT.verifyTimetable(expectedTasks2);
}

/*!    \brief Verify periodic dispatches.
**/
void testSimplePeriodic(void)
{
    timer_init();
    timer_host_reset_time();

    auto dispatcher { std::make_shared<Dispatcher>()};
    auto testTask1 { std::make_shared<TestTask>(1) };
    DispatcherUnitTest dispUT {dispatcher};

    std::vector<iTaskPtr> expectedTasks{testTask1};
    std::cout <<" Wait 24 ms" <<std::endl;
    dispatcher->addTaskPeriodic(testTask1, 25);
    dispUT.printTimetable();
    timer_host_elapse_time(24);
    dispUT.verifyTimetable(expectedTasks);
    std::cout <<" Wait 1 ms" <<std::endl;
    timer_host_elapse_time(1);
    dispUT.printTimetable();
    dispUT.verifyTimerState(true);
    dispUT.verifyTimetable(expectedTasks);
}

/*!    \brief Verify dispatch removal.
**/
void testRemove(void)
{
    timer_init();
    timer_host_reset_time();

    auto dispatcher { std::make_shared<Dispatcher>()};
    auto testTask1 { std::make_shared<TestTask>(1) };
    auto testTask2 { std::make_shared<TestTask>(2) };
    auto testTask3 { std::make_shared<TestTask>(3) };
    DispatcherUnitTest dispUT {dispatcher};

    std::vector<iTaskPtr> expectedTasks1{testTask1};
    std::vector<iTaskPtr> expectedTasks2{testTask1, testTask2};
    std::vector<iTaskPtr> expectedTasks3{testTask3, testTask1, testTask2};
    std::vector<iTaskPtr> empty{};

    dispUT.verifyTimerState(false);

    std::cout <<" Adding task1" <<std::endl;
    dispatcher->addTaskPeriodic(testTask1, 25);
    dispUT.printTimetable();
    dispUT.verifyTimerState(true);
    dispUT.verifyTimetable(expectedTasks1);

    std::cout <<" Adding task2" <<std::endl;
    dispatcher->addTaskPeriodic(testTask2, 50);
    dispUT.printTimetable();
    dispUT.verifyTimerState(true);
    dispUT.verifyTimetable(expectedTasks2);

    std::cout <<" Adding task3" <<std::endl;
    dispatcher->addTaskPeriodic(testTask3, 15);
    dispUT.printTimetable();
    dispUT.verifyTimerState(true);
    dispUT.verifyTimetable(expectedTasks3);

    std::cout <<" Remvoing task3" <<std::endl;
    dispatcher->removeTask(testTask3);
    dispUT.printTimetable();
    dispUT.verifyTimerState(true);
    dispUT.verifyTimetable(expectedTasks2);

    std::cout <<" Remvoing task2" <<std::endl;
    dispatcher->removeTask(testTask2);
    dispUT.printTimetable();
    dispUT.verifyTimerState(true);
    dispUT.verifyTimetable(expectedTasks1);

    std::cout <<" Remvoing task1" <<std::endl;
    dispatcher->removeTask(testTask1);
    dispUT.printTimetable();
    dispUT.verifyTimerState(false);
    dispUT.verifyTimetable(empty);
}

/*!    \brief Verify that we cannot create two dispatchers.
**/
void testTwoDispatchers(void)
{
    std::cout <<"Instantiating first dispatcher" << std::endl;
    auto dispatcher1 { std::make_shared<Dispatcher>()};
    try{
        std::cout <<"Instantiating second dispatcher" << std::endl;
        auto dispatcher2 { std::make_shared<Dispatcher>()};
    }
    catch (HALTimerNotAvailable& e)
    {
        std::cout <<"HALTimerNotAvailable thrown. OK!" << std::endl;
        return;
    }
    throw std::runtime_error("TWo dispatcher instances didn't throw exception!!");
}

int main(void)
{
    testSimpleOneShot();
    testSimplePeriodic();
    testRemove();
    testTwoDispatchers();
}
/****************************************************************/
