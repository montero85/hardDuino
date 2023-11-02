/*!\file event.c
** \author
** \copyright TODO
** \brief Implementation file for inter-module communication events API.
**
**/
/****************************************************************/

#include "event.h"

Event::Event(eventId id)
{
    this->id = id;
}

eventId Event::getId(void)
{
    return id;
}

void EventQueue::pushEvent(baseEventPtr &&e)
{
    eventQ.push(std::move(e));
}

void EventQueue::processQ(void)
{
    while(!eventQ.empty())
    {
        /* Call virtual method on each queued event */
        auto e = std::move(eventQ.front());
        handleEvent(std::move(e));
        eventQ.pop();
    }
}

