/*!\file timers.h
** \author 
** \copyright TODO
** \brief System timer API
** \details System timer module configures one of the hardware timers in the chip
**          to perform timed, user-defined operations.
**          Users of this module can set a callback that will be executed after a
**          delta time from the current point in time.
**/
/****************************************************************/
#ifndef __TIMER_H
#define __TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdbool.h>
/****************************************************************/

/*!    \brief Type for system timer callbacks.
**
** System timer callback gets called when the timer expires.
**/
typedef void (*timer_callback_t)(void);

/*!    \brief Initialise system timer.
**
**    Call at boot to perform hardware setup.
**
**    \return None
**/
void timer_init(void);

/*!    \Check if system timer is free
**
**    \return true if system timer is available to use.
**/
bool timer_is_free(void);

/*!    \brief Start a millisecond timer in one-shot mode
**
** Set system timer to execute a callback after "ms" milliseconds
** from now. In one-shot mode, timer is stopped after first expiration.
**
**    \param [in] ms   - expiration time from now in milliseconds.
**    \param [in] clbk - Callback to execute when timer expires.
**
**    \return Nothing. Use timer_is_free to avoid silent errors.
**/
void timer_start_one_shot_ms(uint16_t ms, timer_callback_t clbk);

/*!    \brief Start a millisecond timer in one-shot mode
**
** Set system timer to execute a callback after "ms" milliseconds
** from now. In continuous mode, timer is restarted each time it expires.
** Use timer_stop, to stop it.
**
**    \param [in] ms   - expiration time from now in milliseconds.
**    \param [in] clbk - Callback to execute when timer expires.
**
**    \return Nothing. Use timer_is_free to avoid silent errors.
**/
void timer_start_continuous_ms(uint16_t ms, timer_callback_t clbk);

/*!    \brief Start timer in seconds in one-shot mode
**
** Set system timer to execute a callback after "sec" seconds from now.
** In one-shot mode, timer is stopped after first expiration.
**
**    \param [in] sec  - expiration time from now in seconds.
**    \param [in] clbk - Callback to execute when timer expires.
**
**    \return Nothing. Use timer_is_free to avoid silent errors.
**/
void timer_start_one_shot_sec(uint16_t sec, timer_callback_t clbk);

/*!    \brief Start a timer in seconds in continuous mode
**
** Set system timer to execute the callback after "sec" seconds from now.
** In continuous mode, timer is restarted each time it expires.
** Use timer_stop, to stop it.
**
**    \param [in] sec  - expiration time from now in seconds.
**    \param [in] clbk - Callback to execute when timer expires.
**
**    \return Nothing. Use timer_is_free to avoid silent errors.
**/
void timer_start_continuous_sec(uint16_t sec, timer_callback_t clbk);

/*!    \brief Start a minutes timer in one-shot mode
**
** Set system timer to execute the callback after "min" minutes from now.
** In one-shot mode, timer is stopped after first expiration.
**
**    \param [in] min  - expiration time from now in minutes.
**    \param [in] clbk - Callback to execute when timer expires.
**
**    \return Nothing. Use timer_is_free to avoid silent errors.
**/
void timer_start_one_shot_min(uint16_t min, timer_callback_t clbk);

/*!    \brief Start a minutes timer in continuous mode
**
** Set a timer to execute the callback after "min" minutes from now.
** In continuous mode, timer is restarted each time it expires.
** Use timer_stop, to stop it.
**
**    \param [in] min  - expiration time from now in minutes.
**    \param [in] clbk - Callback to execute when timer expires.
**
**    \return Nothing. Use timer_is_free to avoid silent errors.
**/
void timer_start_continuous_min(uint16_t min, timer_callback_t clbk);

/*!    \brief Stop system timer
**
** Stop system timer if running.
**
**    \return Nothing.
**/
void timer_stop(void);

/*!    \brief Get system timer value.
**
** Read the value of the hardware counter used to implement
** the system timer.
**
** TODO: For the dispatcher interface to work, this probably
** needs to be a number in ms.
**
**    \return Number of ticks in the hardware counter.
**/
uint16_t timer_get_tick(void);

/****************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* __TIMER_H */
/****************************************************************/
