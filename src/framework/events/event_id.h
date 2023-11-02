/*!\file event_id.h
** \author
** \copyright TODO
** \brief Global header containing the ids of all the events managed by the system.
** \details Each event that is to be handled by any module in the system needs to have
**          a unique ID within this header. This is fairly broadly scoped but it is simple
**          enough and works well for project of small scale.
**/
/****************************************************************/
#ifndef __EVENT_ID_H
#define __EVENT_ID_H

/*!    \brief IDs of all the events understood by the system.
**
** IDs for the event based messaging infrastructure. Events can be used to
** perform inter-module communication. Each module can send and receive
** events to/from another module (see events.h).
**
** Despite each module can define its own set of events as part of their
** public API, each event within the system must have a unique ID within
** this global header. This is quite broad in scope but it allows to keep
** a simple design for event handling. It shouldn't be a problem for small
** projects.
**/
enum class eventId
{
    template_1, /* Template event for test 1. */
    template_2, /* Template event for test 2. */
};
#endif /* __EVENT_ID_H */
/****************************************************************/
