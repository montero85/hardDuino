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

// Declared weak in Arduino.h to allow user redefinitions.
int atexit(void (* /*func*/ )()) { return 0; }


void setupUSB() __attribute__((weak));
void setupUSB() { }

void timer_clbk(void)
{
	digitalWrite(LED_BUILTIN, CHANGE);
}

int main(void)
{
    init();

    initVariant();

#if defined(USBCON)
    USBDevice.attach();
#endif

    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(9600);

    timer_init();

    timer_start_continuous_ms(1000, timer_clbk);

    for (;;) 
    {
    	if (serialEventRun) serialEventRun();
    }

    return 0;
}

