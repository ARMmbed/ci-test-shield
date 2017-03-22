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
 *
 * @author Michael Ray
 * @since 3/22/2017
 * @version 1.0.0
 * 
 */

#if !DEVICE_ANALOGOUT
  #error [NOT_SUPPORTED] AnalogOut not supported on this platform, add 'DEVICE_ANALOGOUT' definition to your platform.
#endif

#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
#include "ci_test_config.h"

#include "TestFramework.hpp"
#include <vector>

using namespace utest::v1;

// Static variables for managing the dynamic list of pins
std::vector< vector <PinMap> > TestFramework::pinout(TS_NC);
std::vector<unsigned int> TestFramework::pin_iterators(TS_NC);

// Initialize a test framework object
TestFramework test_framework;

void test_analogout_execute(PinName pin, float tolerance, int iterations) {
	DEBUG_PRINTF("Running analog output range test on pin %d\n", pin);
    TEST_ASSERT_MESSAGE(pin != NC, "Pin is NC");

    // Find all pins on the resistor ladder that are not the current pin
	std::vector<PinName> resistor_ladder_pins = TestFramework::find_resistor_ladder_pins(pin);
	if (resistor_ladder_pins.size() < 5)
		TEST_ASSERT_MESSAGE(false, "Error finding the resistor ladder pins");

	// Float the remaining pins so that they don't drive the input wire to a specific voltage
    BusInOut inputs(resistor_ladder_pins[0],resistor_ladder_pins[1],resistor_ladder_pins[2],resistor_ladder_pins[3],resistor_ladder_pins[4]);
	inputs.input();

	AnalogIn ain(MBED_CONF_APP_AIN_0);

	// Repeat to guarentee consistency
    for (unsigned int i=0; i<iterations; i++) {

	    float input = 0.0f;
		AnalogOut aout(pin);

		// Itereate at 100mV increments
		for (float i=0.0f; i<=1.0f; i+=0.1f) {
			aout = i;
			input = ain.read();
			DEBUG_PRINTF("Input voltage: %f, Output voltage: %f\n", input, i);
			// Verify input matches expected output within a certain tolerance
			// TEST_ASSERT_MESSAGE(input>=(i-tolerance) && input<=(i+tolerance), "Analog input does not match analog output");
		}
	}
}

utest::v1::control_t test_level1_analogout(const size_t call_count) {
	return TestFramework::test_level1_framework(TestFramework::AnalogOutput, TestFramework::CITS_AnalogOutput, &test_analogout_execute, 0.05, 10);
}

Case cases[] = {
	Case("Level 1 - Analog Output Range test (single pin)", test_level1_analogout, TestFramework::greentea_failure_handler),
};

int main() {
	// Formulate a specification and run the tests based on the Case array
	Specification specification(TestFramework::test_setup<30>, cases);
    return !Harness::run(specification);
}
