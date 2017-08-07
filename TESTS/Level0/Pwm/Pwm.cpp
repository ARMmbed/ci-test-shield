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
 * 
 */

#if !DEVICE_PWMOUT
  #error [NOT_SUPPORTED] PWMOUT not supported on this platform, add 'DEVICE_PWMOUT' definition to your platform.
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


utest::v1::control_t test_level0_pwm(const size_t call_count) 
{
	PinMap pin = test_framework.get_increment_pin(TestFramework::PWM);
	DEBUG_PRINTF("Running PWM constructor on pin %#x\n", pin.pin);
    TEST_ASSERT_MESSAGE(pin.pin != NC, "pin is NC");

	PwmOut pwm(pin.pin);
    pwm.period(1.0f);
    pwm.write(0.5f);

	return test_framework.reset_iterator(TestFramework::PWM);
}


Case cases[] = {
	Case("Level 0 - PWM Constructor", test_level0_pwm, TestFramework::greentea_failure_handler),
};


int main() 
{
	// Formulate a specification and run the tests based on the Case array
	Specification specification(TestFramework::test_setup<30>, cases);
    return !Harness::run(specification);
}