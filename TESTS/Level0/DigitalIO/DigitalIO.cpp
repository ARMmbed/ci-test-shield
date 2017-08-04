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


utest::v1::control_t test_level0_digitalio(const size_t call_count) 
{
	PinMap pin = test_framework.get_increment_pin(TestFramework::DigitalIO);
	DEBUG_PRINTF("Running digital io constructor on pin %#x\n", pin.pin);
    TEST_ASSERT_MESSAGE(pin.pin != NC, "Pin is NC");

	DigitalOut dout(pin.pin);
	DigitalIn din(pin.pin);

	return test_framework.reset_iterator(TestFramework::DigitalIO);
}


Case cases[] = {
	Case("Level 0 - DigitalIO Constructor", test_level0_digitalio, TestFramework::greentea_failure_handler),
};


int main() 
{
	// Formulate a specification and run the tests based on the Case array
	Specification specification(TestFramework::test_setup<30>, cases);
    return !Harness::run(specification);
}
