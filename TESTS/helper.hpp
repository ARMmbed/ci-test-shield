/*
 * Copyright (c) 2016 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cmsis.h"
#include "pinmap.h"
#include "PeripheralPins.h"
#include "mbed.h"

template <const PinMap pinmap[], void (*func)(PinName), bool iterateOnce> 
void FindAndRunPins() {
	int i = 0;
	while(pinmap[i].pin != (PinName)NC) {
		func(pinmap[i].pin);
		i++;
		if (iterateOnce) break;
	}
    TEST_ASSERT(true);
}

void AnalogIn_PinTest(PinName pin) {
    DEBUG_PRINTF("Testing Analog Input library on pin %d\n", pin);
    TEST_ASSERT_MESSAGE(pin != NC, "Pin is NC");

    AnalogIn ain(pin);
    DigitalOut dout(MBED_CONF_APP_DIO_0);
    dout=0;
    ain.read();
    dout=1;
    ain.read();

    DigitalOut dout_a(pin);
    dout_a=0;
    dout_a=1;
}
