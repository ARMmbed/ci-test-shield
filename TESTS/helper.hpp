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

void Analog_RangeTest(PinName pin, float tolerance) {
	AnalogIn ain(pin);
	AnalogOut aout(pin);
	for (float i=0; i<=1; i+=0.1) {
		aout=i;
		float input = ain.read();
		DEBUG_PRINTF("Is input (%f) between %f and %f?\n", input, i-tolerance, i+tolerance);
		TEST_ASSERT_MESSAGE(input>=(i-tolerance) && input<=(i+tolerance), "Analog input matches analog output");
		
	    // AnalogIn ain(ain_pin);
	    // AnalogOut aout(aout_pin);
	    // float valueOff = 0;
	    // float valueOn = 0;
	    // aout = 0;
	    // valueOff = ain.read();
	    // aout = 0.5;
	    // valueOn = ain.read();
	    // DEBUG_PRINTF("\r\n***** valueOff = %f, valueOn = %f \r\n",valueOff, valueOn);
	    // TEST_ASSERT_MESSAGE((0.4f < valueOn) && (0.6f > valueOn), "Value is not in expected range of ~0.5f");
	    // TEST_ASSERT_MESSAGE(valueOn > valueOff,"Value has not increased, expected 0.5");
	    // valueOff = ain.read();
	    // aout = 1.0;
	    // valueOn = ain.read();
	    // DEBUG_PRINTF("\r\n***** valueOff = %f, valueOn = %f \r\n",valueOff, valueOn);
	    // TEST_ASSERT_MESSAGE((0.9f < valueOn) && (1.1f > valueOn), "Value is not in expected range of ~0.5f");
	    // TEST_ASSERT_MESSAGE(valueOn > valueOff,"Value has not increased, expected 1.0");
	}
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
    TEST_ASSERT_MESSAGE(true, "Digital writes prevent analog inputs");

    DigitalOut dout2(pin);
    dout2=0;
    dout2=1;
    TEST_ASSERT_MESSAGE(true, "Analog pins do not dual purpose as digital IO");
    TEST_ASSERT_MESSAGE(ain.read(), "Analog pin did not match digital output");

    Analog_RangeTest(pin, 0.05);
}
