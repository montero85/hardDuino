/*!\file event_test.cpp
** \author
** \copyright
** \brief Unit test for event.cpp, event.h
** \details TODO
**/
/****************************************************************/


#include "event.h"

#include<iostream>
#include <string>
#include<cstdint>

/*!    \brief Template event 1.
**
** Test event that can be used as template.
**/
class TemplateEvent1 : public Event
{
public:
    TemplateEvent1(uint32_t d) :  Event(eventId::template_1), data {d} {};
    decltype(auto) getData(void) { return data; };
private:
    uint32_t data;
};

/*!    \brief Template event 2.
**
** Test event that can be used as template.
**/
class TemplateEvent2 : public Event
{
public:
    TemplateEvent2(std::string str) :  Event(eventId::template_2), data {str} {};
    decltype(auto) getData(void) { return data; };
private:
    std::string data;
};

/*!    \brief EventQueue for positive testing.
**
** Queue to check that events are sent and received correctly.
**/
class QueueValidBehaviour : public EventQueue
{
public:
    QueueValidBehaviour(std::queue<eventId> ids,
                        std::queue<uint32_t> data1,
                        std::queue<std::string> data2):

                            expectedIds (ids),
                            expectedEvent1Data (data1),
                            expectedEvent2Data (data2) {};

    void handleEvent(baseEventPtr &&e) override;
private:
    std::queue<eventId> expectedIds;
    std::queue<uint32_t> expectedEvent1Data;
    std::queue<std::string> expectedEvent2Data;
    void verifyEvent(std::unique_ptr<TemplateEvent1> &&ev);
    void verifyEvent(std::unique_ptr<TemplateEvent2> &&ev);
};

/*!    \brief EventQueue to check invalid reconstructions.
**
** This queue will perform a wrong downcast via reconstructEvent.
** Used to check that exception is correctly sent.
**/
class QueueInvalidBehaviour : public EventQueue
{
public:
    void handleEvent(baseEventPtr &&e) override;
};

void QueueValidBehaviour::handleEvent(baseEventPtr &&e)
{
    switch(e->getId())
    {
        case eventId::template_1:
        {
            auto t = reconstructEvent<TemplateEvent1>(std::move(e));
            verifyEvent(std::move(t));
        }
            break;
        case eventId::template_2:
        {
            auto t = reconstructEvent<TemplateEvent2>(std::move(e));
            verifyEvent(std::move(t));
        }
            break;
        default:
            std::cout << "Event Not found" <<std::endl;
            break;
    }
}

void QueueInvalidBehaviour::handleEvent(baseEventPtr &&e)
{
    switch(e->getId())
    {
        case eventId::template_1:
        {
            auto t = reconstructEvent<TemplateEvent2>(std::move(e));
        }
        break;

        default:
            std::cout << "Event Not found" <<std::endl;
            break;
    }
}

std::string idToStr(eventId id)
{
    if(id == eventId::template_1)
    {
        return std::string("TemplateEvent1");
    }
    else if(id == eventId::template_2)
    {
        return std::string("TemplateEvent2");
    }
    else
    {
        throw std::runtime_error("FAIL: Attempt to convert unknown event to string!");
    }
}
void QueueValidBehaviour::verifyEvent(std::unique_ptr<TemplateEvent1> &&ev)
{
    std::cout << "  Received: " << idToStr(ev->getId()) << " {" << ev->getData() << "}" << std::endl;
    auto exp_id = expectedIds.front();
    expectedIds.pop();
    auto exp_ev1_data = expectedEvent1Data.front();
    expectedEvent1Data.pop();

    std::cout << "  Expected: " << idToStr(exp_id);
    if((ev->getId() != exp_id) || (ev->getData() != exp_ev1_data))
    {
        std::cout << " - FAIL!!!" << std::endl;
        throw std::runtime_error("FAIL: Failure to validate event!");
    }
    std::cout << " - OK!" << std::endl;
}

void QueueValidBehaviour::verifyEvent(std::unique_ptr<TemplateEvent2> &&ev)
{
    std::cout << "  Received: " << idToStr(ev->getId()) << " {\"" << ev->getData() << "\"}" << std::endl;
    auto exp_id = expectedIds.front();
    expectedIds.pop();
    auto exp_ev2_data = expectedEvent2Data.front();
    expectedEvent2Data.pop();

    std::cout << "  Expected: " << idToStr(exp_id);
    if((ev->getId() != exp_id) || (ev->getData() != exp_ev2_data))
    {
        std::cout << " - FAIL!!!" << std::endl;
        throw std::runtime_error("FAIL: Failure to validate event!");
    }
    std::cout << " - OK!" << std::endl;
}

/*!    \brief Valid behaviour testing.
**
** Sends a sequence of events with data and ensure they are processed
** in the right order.
**/
void testValidSendReceive(void)
{
    std::cout << "  <<testValidSendReceive>>" << std::endl;
    /* Initialize sequence of expected event IDs */
    std::queue<eventId> initQId;
    initQId.push(eventId::template_1);
    initQId.push(eventId::template_2);
    initQId.push(eventId::template_1);
    initQId.push(eventId::template_2);
    initQId.push(eventId::template_2);
    initQId.push(eventId::template_1);

    /* Initialize sequence of expected data from template_event1 */
    std::queue<uint32_t> initQData1;
    initQData1.push(1);
    initQData1.push(2);
    initQData1.push(3);

    /* Initialize sequence of expected data from template_event2 */
    std::queue<std::string> initQData2;
    initQData2.push("Hello");
    initQData2.push("World");
    initQData2.push("!");

    QueueValidBehaviour q = { initQId, initQData1, initQData2};

    std::cout << "Send following sequence of events:" << std::endl;
    std::cout << "  TemplateEvent1 {1}" << std::endl;
    std::cout << "  TemplateEvent2 {\"Hello\"}" << std::endl;
    std::cout << "  TemplateEvent1 {2}" << std::endl;
    std::cout << "  TemplateEvent2 {\"World\"}" << std::endl;
    std::cout << "  TemplateEvent2 {\"!\"}" << std::endl;
    std::cout << "  TemplateEvent1 {3}" << std::endl;
    /* Send event in the same order as in the expected sequences. */
    sendEvent<TemplateEvent1>(q, 1);
    sendEvent<TemplateEvent2>(q, "Hello");
    sendEvent<TemplateEvent1>(q, 2);
    sendEvent<TemplateEvent2>(q, "World");
    sendEvent<TemplateEvent2>(q, "!");
    sendEvent<TemplateEvent1>(q, 3);

    std::cout << "Check reception in the EventQueue:" << std::endl;
    /* Process queue and verify. */
    q.processQ();
    std::cout << std::endl;
    std::cout << std::endl;
}

/*!    \brief Invalid behaviour testing.
**
** Attempt to perform a wrong downcast using reconstructEvent and
** check that exception is correctly thrown.
**/
void testInvalidReconstruction(void)
{
    bool result = false;
    QueueInvalidBehaviour q;
    std::cout << "  <<testInvalidReconstruction>>" << std::endl;
    std::cout << "Sending TemplateEvent1 {1}" << std::endl;
    sendEvent<TemplateEvent1>(q, 1);
    try
    {
        std::cout << "Attempt invalid reconstruction to \"TemplateEvent2\"" << std::endl;
        q.processQ();
    }
    catch(BadReconstruction &e)
    {
        std::cout << "Raised \"BadReconstruction\" exception - OK!" << std::endl;
        result = true;
    }
    if(!result)
    {
        throw std::runtime_error("FAIL: Expected exception of type \"BadReconstruction\"");
    }

}
int main(void)
{
    testValidSendReceive();
    testInvalidReconstruction();
}
