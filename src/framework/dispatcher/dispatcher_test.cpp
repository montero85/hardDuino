/*!\file dispatcher_test.cpp
** \author 
** \copyright
** \brief Unit test for dispatcher.cpp
** \details TODO
**/
/****************************************************************/

#include "dispatcher.h"
#include<iostream>
#include<map>
#include<vector>
#include<memory>
#include<utility>
#include<stdexcept>
/****************************************************************/

class TestTask : public iTask
{
public:
    TestTask(unsigned int i): id{i} {};
    ~TestTask() override {};
    void run(void) const override{ std::cout << "Running TestTask{" << id << "}" << std::endl; };
private:
    unsigned int id;
};

class DispatcherUnitTest
{
public:
    DispatcherUnitTest(std::shared_ptr<Dispatcher> d): dispatcher{d} {};
    void printTimetable(void);
    void verify_timer_state(bool active);
    void verify_timetable(std::vector<iTaskPtr> &expected_tasks);
private:
    std::shared_ptr<Dispatcher> dispatcher;
};

#define STUBS
#ifdef STUBS
typedef void (*timer_callback_t)(void);
uint32_t ut_timer = 0;
struct timer_control
{
    bool active;
    uint32_t next_expiry;
    timer_callback_t clbk;
};
struct timer_control timerCtrl;
void timer_init(void)
{
    timerCtrl.active = false;
    timerCtrl.next_expiry = 0;
    timerCtrl.clbk = nullptr;
}

void timer_stop(void)
{
    timerCtrl.active = false;
}
void timer_start_one_shot_ms(uint16_t ms, timer_callback_t clbk)
{
    timerCtrl.active = true;
    timerCtrl.next_expiry = ut_timer + ms;
    timerCtrl.clbk = clbk;
}
void reset_time(void)
{
    ut_timer = 0;
}

void elapseTime(uint32_t time)
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
#endif /* STUBS */

void DispatcherUnitTest::printTimetable(void)
{
    std::cout << "Timetable @time: " << ut_timer << ":" << std::endl;
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

void DispatcherUnitTest::verify_timetable(std::vector<iTaskPtr> &expected_tasks)
{
    std::vector<iTaskPtr> actual_tasks;
    auto t = dispatcher->timetable.cbegin()->first;

    for(auto t : dispatcher->timetable)
    {
        actual_tasks.push_back(t.second.task);
    }
    if(expected_tasks != actual_tasks)
    {
        throw std::runtime_error("FAIL: dispatcher timetable!!");
    }
    if(expected_tasks.empty() && dispatcher->timer_active)
    {
        throw std::runtime_error("FAIL: inconsistent timer active flag!!");
    }

    if(dispatcher->timer_active && dispatcher->head_timestamp != t)
    {
        throw std::runtime_error("FAIL: inconsistent timer head timestamp!!");
    }
}

void DispatcherUnitTest::verify_timer_state(bool active)
{
    if(timerCtrl.active != active)
    {
        throw std::runtime_error("FAIL: Timer state verification!!");
    }
}

void test_simple_one_shot(void)
{
    timer_init();
    reset_time();

    auto dispatcher { std::make_shared<Dispatcher>()};
    auto testTask1 { std::make_shared<TestTask>(1) };
    DispatcherUnitTest d_ut {dispatcher};

    std::vector<iTaskPtr> expectedTasks{testTask1};
    std::vector<iTaskPtr> expectedTasks2{};
    std::cout <<" Wait 24 ms" <<std::endl;
    dispatcher->addTaskOneShot(testTask1, 25);
    d_ut.printTimetable();
    elapseTime(24);
    d_ut.verify_timetable(expectedTasks);
    std::cout <<" Wait 1 ms" <<std::endl;
    elapseTime(1);
    d_ut.printTimetable();
    d_ut.verify_timer_state(false);
    d_ut.verify_timetable(expectedTasks2);
}

void test_simple_periodic(void)
{
    timer_init();
    reset_time();

    auto dispatcher { std::make_shared<Dispatcher>()};
    auto testTask1 { std::make_shared<TestTask>(1) };
    DispatcherUnitTest d_ut {dispatcher};

    std::vector<iTaskPtr> expectedTasks{testTask1};
    std::cout <<" Wait 24 ms" <<std::endl;
    dispatcher->addTaskPeriodic(testTask1, 25);
    d_ut.printTimetable();
    elapseTime(24);
    d_ut.verify_timetable(expectedTasks);
    std::cout <<" Wait 1 ms" <<std::endl;
    elapseTime(1);
    d_ut.printTimetable();
    d_ut.verify_timer_state(true);
    d_ut.verify_timetable(expectedTasks);
}

void test_remove(void)
{
    timer_init();
    reset_time();

    auto dispatcher { std::make_shared<Dispatcher>()};
    auto testTask1 { std::make_shared<TestTask>(1) };
    auto testTask2 { std::make_shared<TestTask>(2) };
    auto testTask3 { std::make_shared<TestTask>(3) };
    DispatcherUnitTest d_ut {dispatcher};

    std::vector<iTaskPtr> expectedTasks1{testTask1};
    std::vector<iTaskPtr> expectedTasks2{testTask1, testTask2};
    std::vector<iTaskPtr> expectedTasks3{testTask3, testTask1, testTask2};
    std::vector<iTaskPtr> empty{};

    d_ut.verify_timer_state(false);

    std::cout <<" Adding task1" <<std::endl;
    dispatcher->addTaskPeriodic(testTask1, 25);
    d_ut.printTimetable();
    d_ut.verify_timer_state(true);
    d_ut.verify_timetable(expectedTasks1);

    std::cout <<" Adding task2" <<std::endl;
    dispatcher->addTaskPeriodic(testTask2, 50);
    d_ut.printTimetable();
    d_ut.verify_timer_state(true);
    d_ut.verify_timetable(expectedTasks2);

    std::cout <<" Adding task3" <<std::endl;
    dispatcher->addTaskPeriodic(testTask3, 15);
    d_ut.printTimetable();
    d_ut.verify_timer_state(true);
    d_ut.verify_timetable(expectedTasks3);

    std::cout <<" Remvoing task3" <<std::endl;
    dispatcher->removeTask(testTask3);
    d_ut.printTimetable();
    d_ut.verify_timer_state(true);
    d_ut.verify_timetable(expectedTasks2);

    std::cout <<" Remvoing task2" <<std::endl;
    dispatcher->removeTask(testTask2);
    d_ut.printTimetable();
    d_ut.verify_timer_state(true);
    d_ut.verify_timetable(expectedTasks1);

    std::cout <<" Remvoing task1" <<std::endl;
    dispatcher->removeTask(testTask1);
    d_ut.printTimetable();
    d_ut.verify_timer_state(false);
    d_ut.verify_timetable(empty);
}

void test_two_dispatchers(void)
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
    test_simple_one_shot();
    test_simple_periodic();
    test_remove();
    test_two_dispatchers();
}
/****************************************************************/
