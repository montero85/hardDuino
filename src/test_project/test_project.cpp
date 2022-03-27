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

// Declared weak in Arduino.h to allow user redefinitions.
int atexit(void (* /*func*/ )()) { return 0; }


void setupUSB() __attribute__((weak));
void setupUSB() { }

unsigned int timer_tick = 0;
void timer_clbk(void)
{
	/* Basically do nothing. Just wake the chip up. */
	Serial.println("Interrupt");
}

int main(void)
{
    init();

    initVariant();

#if defined(USBCON)
    USBDevice.attach();
#endif

    pinMode(LED_BUILTIN, OUTPUT);

    timer_init();
    //watchdog_init();
    sleep_init();
    Serial.begin(9600);
    timer_start_continuous_sec(5, timer_clbk);
    for (;;) 
    {
    	digitalWrite(LED_BUILTIN, HIGH);
    	delay(1000);
    	digitalWrite(LED_BUILTIN, LOW);
    	delay(1000);
    	digitalWrite(LED_BUILTIN, HIGH);
     	Serial.println("Enter Sleep");
    	sleep_on_the_bed();
    	//watchdog_kick();
    	Serial.println("Exit Sleep");
    	if (serialEventRun) serialEventRun();
    }

    return 0;
}

