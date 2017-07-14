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

void test_busio_execute(PinName pin, float tolerance, int iterations) {
	DEBUG_PRINTF("Running analog input range test on pin %d\n", pin);
    TEST_ASSERT_MESSAGE(pin != NC, "Pin is NC");

    PinName pin_pair = TestFramework::find_pin_pair(pin);
    BusInOut bio1(pin);
    BusInOut bio2(pin_pair);

    bio1.output();
    bio2.input();
    bio1 = 0;
    TEST_ASSERT_MESSAGE(bio2.read()==0, "Value read on bus does not equal value written.");
    bio1 = 1;
    TEST_ASSERT_MESSAGE(bio2.read()==1, "Value read on bus does not equal value written.");
}

utest::v1::control_t test_level1_busio(const size_t call_count) {
	return TestFramework::test_level2_framework(TestFramework::BusIO, TestFramework::CITS_DigitalIO, &test_busio_execute, 0.05, 10);
}

Case cases[] = {
	Case("Level 1 - Bus Input/Output pair test", test_level1_busio, TestFramework::greentea_failure_handler),
};

int main() {
	// Formulate a specification and run the tests based on the Case array
	Specification specification(TestFramework::test_setup<30>, cases);
    return !Harness::run(specification);
}
