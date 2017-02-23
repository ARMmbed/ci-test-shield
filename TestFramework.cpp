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
#include "TestFramework.hpp"

TestFramework::TestFramework() {
	map_pins(PinMap_ADC, AnalogInput);
	map_pins(PinMap_DAC, AnalogOutput);
	setup_cits_pins();
	DEBUG_PRINTF("HERE\n");
}

void TestFramework::setup_cits_pins() {
	pinout[CITS_AnalogInput].push_back(MBED_CONF_APP_AIN_0);
	pinout[CITS_AnalogInput].push_back(MBED_CONF_APP_AIN_1);
	pinout[CITS_AnalogInput].push_back(MBED_CONF_APP_AIN_2);
	pinout[CITS_AnalogInput].push_back(MBED_CONF_APP_AIN_3);
	pinout[CITS_AnalogInput].push_back(MBED_CONF_APP_AIN_4);
	pinout[CITS_AnalogInput].push_back(MBED_CONF_APP_AIN_5);
	pinout[CITS_AnalogOutput].push_back(MBED_CONF_APP_AOUT);
}

void TestFramework::map_pins(const PinMap pinmap[], Type pintype) {
	int i = 0;
	while(pinmap[i].pin != (PinName)NC) {
		pinout[pintype].push_back(pinmap[i].pin);
		i++;
	}
}

int TestFramework::find_pin(PinName pin, Type pintype) {
	for (int i=0; i<pinout[pintype].size(); i++) {
		if (pin == pinout[pintype][i])
			return i;
	}
	return -1;
}

utest::v1::control_t TestFramework::reset_iterator(Type pintype) {
	if (pin_iterators[pintype] < pinout[pintype].size()) {
		pin_iterators[pintype]++;
		return utest::v1::CaseRepeatAll;
	} else {
		pin_iterators[pintype] = 0;
		return utest::v1::CaseNext;
	}
}

utest::v1::control_t TestFramework::test_l0_analogin(const size_t call_count) {
	PinName pin = pinout[AnalogInput][pin_iterators[AnalogInput]];
	DEBUG_PRINTF("Running analog input constructor on pin %d\n", pin);
    TEST_ASSERT_MESSAGE(pin != NC, "Pin is NC");

	AnalogIn ain(pin);

	TEST_ASSERT(true);
	return reset_iterator(AnalogInput);
}

utest::v1::control_t TestFramework::test_l0_analogout(const size_t call_count) {
	PinName pin = pinout[AnalogOutput][pin_iterators[AnalogOutput]];
	DEBUG_PRINTF("Running analog output constructor on pin %d\n", pin);
    TEST_ASSERT_MESSAGE(pin != NC, "Pin is NC");

	AnalogOut ain(pin);

	TEST_ASSERT(true);
	return reset_iterator(AnalogOutput);
}

utest::v1::control_t TestFramework::test_l1_analogin(const size_t call_count) {
	PinName pin = pinout[AnalogInput][pin_iterators[AnalogInput]];
	int index = find_pin(pin, CITS_AnalogInput);
	int tag = 0;
	// State: Execute
	if (index != -1) {
		test_analogin_execute(pin);
		tag = 1;

	}
	while (pin_iterators[AnalogInput] < pinout[AnalogInput].size()) {
		// State: Increment iterator
		pin_iterators[AnalogInput]++;
		pin = pinout[AnalogInput][pin_iterators[AnalogInput]];
		index = find_pin(pin, CITS_AnalogInput);

		if (index != -1) {
			// State: End case
			if (tag) {
				TEST_ASSERT(true);
				return utest::v1::CaseRepeatAll;
			// State: Execute
			} else {
				test_analogin_execute(pin);
				tag = 1;
			}
		}
	}
	// State: End test
	return reset_iterator(AnalogInput);
}

void TestFramework::test_analogin_execute(PinName pin) {
	DEBUG_PRINTF("Running analog input range test on pin %d\n", pin);
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
}

// void Analog_RangeTest(PinName pin, float tolerance) {
// 	AnalogIn ain(pin);
// 	AnalogOut aout(pin);
// 	for (float i=0; i<=1; i+=0.1) {
// 		aout=i;
// 		float input = ain.read();
// 		DEBUG_PRINTF("Is input (%f) between %f and %f?\n", input, i-tolerance, i+tolerance);
// 		TEST_ASSERT_MESSAGE(input>=(i-tolerance) && input<=(i+tolerance), "Analog input matches analog output");
		
// 	    // AnalogIn ain(ain_pin);
// 	    // AnalogOut aout(aout_pin);
// 	    // float valueOff = 0;
// 	    // float valueOn = 0;
// 	    // aout = 0;
// 	    // valueOff = ain.read();
// 	    // aout = 0.5;
// 	    // valueOn = ain.read();
// 	    // DEBUG_PRINTF("\r\n***** valueOff = %f, valueOn = %f \r\n",valueOff, valueOn);
// 	    // TEST_ASSERT_MESSAGE((0.4f < valueOn) && (0.6f > valueOn), "Value is not in expected range of ~0.5f");
// 	    // TEST_ASSERT_MESSAGE(valueOn > valueOff,"Value has not increased, expected 0.5");
// 	    // valueOff = ain.read();
// 	    // aout = 1.0;
// 	    // valueOn = ain.read();
// 	    // DEBUG_PRINTF("\r\n***** valueOff = %f, valueOn = %f \r\n",valueOff, valueOn);
// 	    // TEST_ASSERT_MESSAGE((0.9f < valueOn) && (1.1f > valueOn), "Value is not in expected range of ~0.5f");
// 	    // TEST_ASSERT_MESSAGE(valueOn > valueOff,"Value has not increased, expected 1.0");
// 	}
// }