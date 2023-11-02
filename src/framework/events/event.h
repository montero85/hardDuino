/*!\file event.h
** \author
** \copyright TODO
** \brief API for event based inter-module communication.
** \details This is a simple event based messaging system to allow inter-module
**          communication.
**          The API provides two main facilities: an "Event" virtual base class
**          and an EventQueue virtual base class.
**
**          A module sending events as part of its external interface shall provide
**          a concrete (meaningful) implementation for these events, deriving from
**          "Event" base type (and add a unique ID for the concrete event type
**          to event_id.h). Concrete events can encapsulate data.
**
**          A module that requires to receive specific events shall derive from
**          EventQueue and provide an implementation to the "handle_event" method.
**
**          In order to minimize coupling, event "senders" can allow registration of
**          EventQueues as part of a publish-subsribe scheme.
**/
/****************************************************************/
#ifndef __EVENT_H
#define __EVENT_H

#include <cstdint>
#include <cstddef>
#include <queue>
#include <memory>
#include "event_id.h"

/*!    \brief Base class for system events.
**
** System events allow to send messages (events) among system components.
** The destination component shall be owner of an EventQueue
** (where such events are posted).
**
** All concrete events travelling the system shall be derived from this
** base class and shall be assigned a unique ID defined in event_id.h.
** Each eventId shall match one and only one specific concrete event type.
** (That is to say that "class FooEvent : public Event" shall only correspond
** to an "eventId:foo" and a eventId:foo shall only correspond to a "class FooEvent"
** type).
**
** Concrete event types can have data associated with them. Concrete EventQueues can
** retrieve them by matching the unique eventId and then down-casting to the appropriate
** type (see reconstructEvent).
**
** Events are meant to move from one "point" to another of the system. This
** is why they are designed to use unique ownership (unique_ptrs) and move semantics.
** At any time, only one module shall be responsible for the destruction of the
** event.
** Copy semantics have been deleted from the base class.
**/
class Event
{
public:
    /*!    \brief Base class constructor.
    **
    ** \param [in] id - id that uniquely identifies the derived event type.
    **/
    Event(eventId id);

    /*!    \brief Copy constructor is deleted.
    **/
    Event(const Event &e) = delete;

    /*!    \brief Copy operation is deleted.
     **/
    const Event& operator=(const Event &e) = delete;

    /*!    \brief Base class destructor.
    **
    ** Virtual destructor allow concrete event "disposal"
    ** through a reference to the base type. (i.e. from an EventQueue).
    **/
    virtual ~Event() = default;

    /*!    \brief Get event id.
    **
    ** \return Unique id for the derived event type.
    **/
    eventId getId(void);

private:
    eventId id;
};

/*!    \brief Pointer type to base "Event".
**
** Events are designed to use unique ownership and move semantics
** (see class event).
**/
using baseEventPtr = std::unique_ptr<Event>;

/*!    \brief Template Pointer type for concrete events.
**
** This template will provide the pointer type to a concrete
** event, where T is the event type.
**
** Similarly to base events, unique pointers are the way to go.
**/
template<typename T>
using eventPtr = std::unique_ptr<T>;

/*!    \brief Base class for event queues.
**
** Event queues are the receivers of system "events". Modules intended to
** receive events shall extend this base class and provide an implementation
** to the "handleEvent" method. In this way, they can define custom handling
** in response to specific events.
**
** In order to retrieve data from the original (concrete) event, implementations
** of EventQueues can match on the unique eventId and down-cast using the
** "reconstructEvent" helper function.
**
** Note: EventQueue base API is not thread safe. Users shall provide thread
** safety, in the cases it is needed.
**/
class EventQueue
{
public:
    /*!    \brief Base class destructor.
    **
    ** Standard default virtual destructor.
    **/
    virtual ~EventQueue() = default;

    /*!    \brief Post "Event" to the EventQueue.
    **
    ** \param [in] e - event to be posted to the queue.
    **
    ** Allows users to send "Events" to the EventQueue.
    **
    ** It is recommended to call this API via the
    ** "sendEvent" helper function which takes also
    ** care of the concrete event instantiation.
    **/
    void pushEvent(baseEventPtr &&e);

    /*!    \brief Process the EventQueue.
    **
    ** Process EventQueue by calling "handleEvent"
    ** on each "Event" present in the queue at the time
    ** of call. Order of event processing is
    ** (unsurprisingly) FIFO.
    **
    ** EventQueue is left empty after calls to this function.
    **/
    void processQ(void);

    /*!    \brief Handle a specific event.
    **
    ** \param [in] e - event to be handled.
    **
    ** This function is called by processQ for each
    ** event present in the EventQueue at the time of call.
    **
    ** Concrete class shall provide an implementation to
    ** provide custom handling for specific events.
    **/
    virtual void handleEvent(baseEventPtr &&e) = 0;
private:
    std::queue<baseEventPtr> eventQ;
};

/*!    \brief Bad event reconstruction.
**
** reconstructEvent throws this exception when it
** fails to dynamic cast a pointer to a base event type
** to the requested concrete event type.
**/
class BadReconstruction: public std::exception
{
public:
    const char *what(void)
    {
        return "Failed to reconstruct concrete event from Event pointer.";
    }
};

/*!    \brief Reconstruct a concrete event from a base event reference.
**
** \param [in] e - Pointer reference to the base event to reconstruct.
**
** \throws BadReconstruction - if downcast fails.
**
** This helper API can be used to reconstruct (dynamically downcast) a
** concrete event type "T" from a pointer reference to a base event type
** (i.e. the ones stored in EventQueues).
**
** Event ownership is transferred to the returned pointer.
**
** Example:
** void ConcreteQueue::handleEvent(eventPtr &&e)
** {
**     switch(e->getId())
**     {
**         case eventId::foo:
**             // Reconstruct event of type
**             // "class foo_event : public event"
**             auto ce = reconstructEvent<FooEvent>(e);
**             doSomething( ce->getData() );
**             break;
**....
**/
template<typename T>
eventPtr<T> reconstructEvent(baseEventPtr &&e)
{
    T *ptr = dynamic_cast<T *>(e.release());
    if(ptr == nullptr)
    {
        /* Small gap in which smart pointer is released.
         * We need to manually de-allocate it.
         */
        delete ptr;
        throw BadReconstruction();
    }
    return eventPtr<T>(ptr);
}

/*!    \brief Create event and post it to and event queue.
**
** \param [in] q - EventQueue where to post the event.
** \param [in] args - parameters passed to the concrete event
**                    constructor.
**
** Create an event of concrete type T, assign its ownership to a
** unique_ptr and post it to the target EventQueue.
** Variadic parameters are forwarded (as in std::forward) to the
** concrete event constructor.
**/
template<typename T, typename... Args>
void sendEvent(EventQueue &q, Args&& ...args)
{
    auto ev = std::make_unique<T>( std::forward<Args>(args)...);
    q.pushEvent(std::move(ev));
}

#endif /* __EVENT_H */
/****************************************************************/
