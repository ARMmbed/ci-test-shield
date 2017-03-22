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

#if !DEVICE_ANALOGIN
  #error [NOT_SUPPORTED] AnalogIn not supported on this platform, add 'DEVICE_ANALOGIN' definition to your platform.
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

void test_analogin_execute(PinName pin, float tolerance, int iterations) {
	DEBUG_PRINTF("Running analog input range test on pin %d\n", pin);
    TEST_ASSERT_MESSAGE(pin != NC, "Pin is NC");

    // Find all pins on the resistor ladder that are not the current pin
	std::vector<PinName> resistor_ladder_pins = TestFramework::find_resistor_ladder_pins(pin);
	if (resistor_ladder_pins.size() < 5)
		TEST_ASSERT_MESSAGE(false, "Error finding the resistor ladder pins");

	// Create a bus with all of these pins
    BusInOut outputs(resistor_ladder_pins[0],resistor_ladder_pins[1],resistor_ladder_pins[2],resistor_ladder_pins[3],resistor_ladder_pins[4]);
	outputs.output();

	AnalogIn ain(pin);

    for (unsigned int i=0; i<iterations; i++) {

    	// Initialization
	    int x = 0;
	    int y= 0;
	    outputs = 0;
	    float prev_value = 0;
	    float new_value = 0;
	    float test_value = 0;

	    // Iterate through each of the bus pins
	    for(x = 0; x<5; x++) {

	    	// Read the previous value, and turn another pin on in the resistor ladder
	        prev_value = ain.read();
	        y = (y<<1) + 1;
	        outputs = y;

	        // Read the new value to make sure the voltage increased
	        new_value = ain.read();
	        TEST_ASSERT_MESSAGE(new_value > prev_value,"Analog Input did not increment. Check that you have assigned valid pins in mbed_app.json file")

	        // Repeat the read multiple times to verify the output is not fluctuating
	        for (unsigned int j = 0; j<iterations; j++) {
	        	test_value = ain.read();
	        	TEST_ASSERT_MESSAGE(abs(test_value - new_value) < tolerance, "Analog Input fluctuated past the tolerance");
	        }
	    }
	}
}

utest::v1::control_t test_level1_analogin(const size_t call_count) {
	return TestFramework::test_level1_framework(TestFramework::AnalogInput, TestFramework::CITS_AnalogInput, &test_analogin_execute, 0.05, 10);
}

Case cases[] = {
	Case("Level 1 - Analog Input Range test (single pin)", test_level1_analogin, TestFramework::greentea_failure_handler),
};

int main() {
	// Formulate a specification and run the tests based on the Case array
	Specification specification(TestFramework::test_setup<30>, cases);
    return !Harness::run(specification);
}
