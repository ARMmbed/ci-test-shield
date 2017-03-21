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
#if !DEVICE_ANALOGOUT
  #error [NOT_SUPPORTED] AnalogOut not supported on this platform, add 'DEVICE_ANALOGOUT' definition to your platform.
#endif

#include "cmsis.h"
#include "pinmap.h"
#include "PeripheralPins.h"

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
std::vector<int> TestFramework::pin_iterators(TS_NC);

// Initialize a test framework object
TestFramework test_framework;

utest::v1::status_t test_setup(const size_t number_of_cases) {
    GREENTEA_SETUP(30, "default_auto");
    return verbose_test_setup_handler(number_of_cases);
}

utest::v1::status_t greentea_failure_handler(const Case *const source, const failure_t reason) {
    greentea_case_failure_abort_handler(source, reason);
    return STATUS_ABORT;
}

void test_analogout_execute(PinName pin, float tolerance, int iterations) {
	DEBUG_PRINTF("Running analog output range test on pin %d\n", pin);
    TEST_ASSERT_MESSAGE(pin != NC, "Pin is NC");
	AnalogIn ain(TestFramework::find_resistor_ladder_pins(pin)[0]);

    for (unsigned int i=0; i<iterations; i++) {
	    float input = 0.0f;
		AnalogOut aout(pin);
		aout = 0.5;
		for (float i=0.0f; i<=1.0f; i+=0.1f) {
			aout=i;
			input = ain.read();
			TEST_ASSERT_MESSAGE(input>=(i-tolerance) && input<=(i+tolerance), "Analog input matches analog output");
		}
	}
}

utest::v1::control_t test_level1_analogout(const size_t call_count) {
	return TestFramework::test_level1_framework(TestFramework::AnalogOutput, TestFramework::CITS_AnalogOutput, &test_analogout_execute, 0.05, 10);
}

Case cases[] = {
	Case("Level 1 - Analog Output Range test (single pin)", test_level1_analogout, greentea_failure_handler),
};

int main() {
	// Formulate a specification and run the tests based on the Case array
	Specification specification(test_setup, cases);
    return !Harness::run(specification);
}
