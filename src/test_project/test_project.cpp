/*
  Test_project entry point: derived from core Arduino main.cpp

  Copyright (c) 2005-2013 Arduino Team.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <Arduino.h>
#include "timer.h"
#include "sleep.h"
#include "watchdog.h"
#include "reset.h"
#include <avr/io.h>

// Declared weak in Arduino.h to allow user redefinitions.
int atexit(void (* /*func*/ )()) { return 0; }


void setupUSB() __attribute__((weak));
void setupUSB() { }

void timer_clbk(void)
{
    /* Basically do nothing. Just wake the chip up. */
    Serial.println("Interrupt");
}

int main(void)
{
    reset_cause_t hw;
    sw_reset_t sw;

    reset_init();
    timer_init();
    init();

    initVariant();

#if defined(USBCON)
    USBDevice.attach();
#endif

    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(9600);
    hw = reset_read_last(&sw);
    delay(10000);
    watchdog_init();
    if((hw != reset_power_on) && (hw != reset_external))
    {
        /* Abnormal reset: spit it out. */
        Serial.print("Reset: ");
        Serial.print(hw);
        Serial.print(" Software code: ");
        Serial.println(sw);
    }

    for (;;) 
    {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(1000);
        digitalWrite(LED_BUILTIN, LOW);
        delay(1000);
        if (serialEventRun) serialEventRun();
        watchdog_kick();
    }

    return 0;
}

