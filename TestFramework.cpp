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
	if (pin_iterators[pintype] < pinout[pintype].size())
		return utest::v1::CaseRepeatAll;
	else {
		pin_iterators[pintype] = 0;
		return utest::v1::CaseNext;
	}
}

utest::v1::control_t TestFramework::test_l0_analogin(const size_t call_count) {
	PinName pin = pinout[AnalogInput][pin_iterators[AnalogInput]++];
	DEBUG_PRINTF("Running analog input constructor on pin %d\n", pin);
    TEST_ASSERT_MESSAGE(pin != NC, "Pin is NC");

	AnalogIn ain(pin);

	TEST_ASSERT(true);
	return reset_iterator(AnalogInput);
}

utest::v1::control_t TestFramework::test_l0_analogout(const size_t call_count) {
	PinName pin = pinout[AnalogOutput][pin_iterators[AnalogOutput]++];
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
		test_analogin_execute(pin, 0.10, 10);
		pin_iterators[AnalogInput] = 0;
		return utest::v1::CaseNext;
	}
	while (pin_iterators[AnalogInput] < pinout[AnalogInput].size()) {
		// State: Increment iterator
		pin_iterators[AnalogInput]++;
		pin = pinout[AnalogInput][pin_iterators[AnalogInput]];
		index = find_pin(pin, CITS_AnalogInput);

		if (index != -1) {
			// State: Execute
			test_analogin_execute(pin, 0.10, 10);
			pin_iterators[AnalogInput] = 0;
			return utest::v1::CaseNext;
		}
	}
	// State: End test
	return reset_iterator(AnalogInput);
}

utest::v1::control_t TestFramework::test_l2_analogin(const size_t call_count) {
	PinName pin = pinout[AnalogInput][pin_iterators[AnalogInput]];
	int index = find_pin(pin, CITS_AnalogInput);
	int tag = 0;
	// State: Execute
	if (index != -1) {
		test_analogin_execute(pin, 0.05, 100);
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
				test_analogin_execute(pin, 0.05, 100);
				tag = 1;
			}
		}
	}
	// State: End test
	return reset_iterator(AnalogInput);
}

void TestFramework::test_analogin_execute(PinName pin, float tolerance, int iterations) {
	DEBUG_PRINTF("Running analog input range test on pin %d\n", pin);
    TEST_ASSERT_MESSAGE(pin != NC, "Pin is NC");

    for (int i=0; i<iterations; i++) {

	    // Case 1: Digital and Analog non-conflicting
		AnalogIn ain(pin);
	    DigitalOut dout(MBED_CONF_APP_DIO_0);
	    dout=0;
	    float compare1 = ain.read();
	    dout=1;
	    float compare2 = ain.read();
	    TEST_ASSERT_MESSAGE(true, "Digital writes prevent analog inputs");
	    TEST_ASSERT_MESSAGE(abs(compare1-compare2)<tolerance, "Digital write changed the analog input");

	    // Case 2: Dual purpose analog/digital
	    // DigitalOut dout2(pin);
	    // dout2=0;
	    // dout2=1;
	    // TEST_ASSERT_MESSAGE(true, "Analog pins do not dual purpose as digital IO");
	    // float input = ain.read();
	    // dout2=0;
	    // TEST_ASSERT_MESSAGE(input > (1-tolerance) && input < (1+tolerance), "Analog pin did not match digital output");

	    // Case 3: Range test
	    float input = 0.0f;
		AnalogOut aout(MBED_CONF_APP_AOUT);
		aout = 0.5;
	    TEST_ASSERT_MESSAGE(true, "Analog input pins do not dual purpose as analog output pins");
		for (float i=0.0f; i<=1.0f; i+=0.1f) {
			aout=i;
			input = ain.read();
			TEST_ASSERT_MESSAGE(input>=(i-tolerance) && input<=(i+tolerance), "Analog input matches analog output");
		}
	}
}