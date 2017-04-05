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

#if !DEVICE_I2C
  #error [NOT_SUPPORTED] I2C not supported on this platform, add 'DEVICE_I2C' definition to your platform.
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

void construct_i2c(PinName sda, PinName scl) {
	DEBUG_PRINTF("Running I2C Constructor on SDA pin %d and SCL pin %d\n", sda, scl);
    TEST_ASSERT_MESSAGE(sda != NC, "SDA Pin is NC");
    TEST_ASSERT_MESSAGE(scl != NC, "SCL Pin is NC");

    I2C i2c(sda, scl);

	TEST_ASSERT(true);
}

utest::v1::control_t test_level0_i2c(const size_t call_count) {
	return TestFramework::run_i2c(&construct_i2c);
}

Case cases[] = {
	Case("Level 0 - I2C Constructor", test_level0_i2c, TestFramework::greentea_failure_handler),
};

int main() {
	// Formulate a specification and run the tests based on the Case array
	Specification specification(TestFramework::test_setup<30>, cases);
    return !Harness::run(specification);
}
