/*!\file sleep.c
** \author 
** \copyright
** \brief AVR implementation of low power modes for device.
** \details
**/
/****************************************************************/

#include "sleep.h"
#include "sleep_config.h"
#include <stddef.h>
#include <avr/sleep.h>
/****************************************************************/

struct sleep_config_handlers
{
    sleep_on_init on_init;
    sleep_on_enter on_enter;
    sleep_on_exit on_exit;
};

/** Library doesn't allow for peripherals de-registration so, we can
 ** just use an array and an index to the first invalid element to store
 ** the handlers of registered devices.
 **/
static struct sleep_config_handlers peripheral_handlers[SLEEP_MAX_REGISTERED_PERIPHERALS];
static uint8_t peripheral_handlers_first_invalid = 0;

/** Run "on_init" handlers of all the registered peripherals
 **/
static void run_init_handlers(void)
{
    uint8_t i;
    struct sleep_config_handlers *ph;

    for(i = 0; i < peripheral_handlers_first_invalid; i++)
    {
        ph = &peripheral_handlers[i];
        if(ph->on_init != NULL)
        {
            (*ph->on_init)();
        }
    }
}

/** Run "on_enter" handlers of all the registered peripherals
 **/
static void run_enter_handlers(void)
{
    uint8_t i;
    struct sleep_config_handlers *ph;

    for(i = 0; i < peripheral_handlers_first_invalid; i++)
    {
        ph = &peripheral_handlers[i];
        if(ph->on_enter != NULL)
        {
            (*ph->on_enter)();
        }
    }
}
/** Run "on_exit" handlers of all the registered peripherals
 **/
static void run_exit_handlers(void)
{
    uint8_t i;
    struct sleep_config_handlers *ph;

    for(i = 0; i < peripheral_handlers_first_invalid; i++)
    {
        ph = &peripheral_handlers[i];
        if(ph->on_exit != NULL)
        {
            (*ph->on_exit)();
        }
    }
}

void sleep_init(void)
{
    run_init_handlers();
}

bool sleep_register_peripheral(sleep_on_init on_init_hlr,
                               sleep_on_enter on_enter_hlr,
                               sleep_on_exit on_exit_hlr)
{
    struct sleep_config_handlers *ph;

    if(peripheral_handlers_first_invalid >= SLEEP_MAX_REGISTERED_PERIPHERALS)
    {
        /* No more slots to register a peripheral */
        return false;
    }

    /* Select first available slot */
    ph = &peripheral_handlers[peripheral_handlers_first_invalid];
    /* Store the handlers (not NULL is allowed) */
    ph->on_init = on_init_hlr;
    ph->on_enter = on_enter_hlr;
    ph->on_exit = on_exit_hlr;
    /* Move to the next slot. */
    peripheral_handlers_first_invalid++;

    return true;
}

/** Implementation will mainly wrap macros
 ** that AVR libc provides us to make our life easier
 **/
void sleep_on_the_couch(void)
{
    /* Set sleep mode register */
    set_sleep_mode(SLEEP_MODE_IDLE);
    /* Set sleep enable bit. */
    sleep_enable();
    /* Call asm("SLEEP") to actually enter sleep. zzzz!*/
    sleep_cpu();
    /* Here we are out of sleep!! Reset sleep enable bit. */
    sleep_disable();
}

/** Implementation will mainly wrap macros
 ** that AVR libc provides us to make our life easier
 **/
void sleep_on_the_bed(void)
{
    /* Set sleep mode register */
    set_sleep_mode(SLEEP_MODE_STANDBY);
    /* Set sleep enable bit and run enter handlers. */
    run_enter_handlers();
    sleep_enable();
    /* Call asm("SLEEP") to actually enter sleep. ZZZZ!!*/
    sleep_cpu();
    /* Here we are out of sleep!!
     * Reset sleep enable bit and call exit handlers. */
    sleep_disable();
    run_exit_handlers();
}
/****************************************************************/
