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

#if !DEVICE_I2C
  #error [NOT_SUPPORTED] I2C not supported on this platform, add 'DEVICE_I2C' definition to your platform.
#endif

#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
#include "ci_test_config.h"

#include "TestFramework.hpp"
#include <I2CEeprom.h>
#include <vector>
#include <stdlib.h>

using namespace utest::v1;

// Static variables for managing the dynamic list of pins
std::vector< vector <PinMap> > TestFramework::pinout(TS_NC);
std::vector<unsigned int> TestFramework::pin_iterators(TS_NC);

// Initialize a test framework object
TestFramework test_framework;

template <int iterations>
utest::v1::control_t test_level1_i2c(const size_t call_count) {

	// Check to see if the CI test shield I2C pins are connected to the board
	PinName pin_scl = MBED_CONF_APP_I2C_SCL;
	PinName pin_sda = MBED_CONF_APP_I2C_SDA;
	printf("SCL pin: %d\n", pin_scl);
	printf("SDA pin: %d\n", pin_sda);
	if (TestFramework::find_pin(pin_scl, TestFramework::I2C_SCL)==-1 ||
		TestFramework::find_pin(pin_sda, TestFramework::I2C_SDA)==-1) {
		TEST_ASSERT(false);
		return utest::v1::CaseNext;
	}

	DEBUG_PRINTF("Running I2C constructor on SDA pin %d and SCL pin %d\n", pin_sda, pin_scl);
    TEST_ASSERT_MESSAGE(pin_sda != NC, "SDA Pin is NC");
    TEST_ASSERT_MESSAGE(pin_scl != NC, "SCL Pin is NC");

    // Get a random seed from the Greentea host test
	srand(TestFramework::get_seed());

    I2CEeprom memory(pin_sda,pin_scl,MBED_CONF_APP_I2C_EEPROM_ADDR,32,0);

    // Run twice
    for (int i=0; i<2; i++) {

    	// Generate a random string
	    char test_string[128] = {0};
	    for (int i=0; i<iterations; i++) 
	    	test_string[i] = 'A' + rand()%26;

	    // Write the random string to a specified location, and then read back to verify both strings are the same
	    char read[128] = {0};
	    int r = 0;
	    int w = 0;
	    w = memory.write(1, test_string, iterations);
	    r = memory.read(1, read, iterations);
	    DEBUG_PRINTF("Num Bytes Read = %d, Num Bytes Written = %d\nRead bytes = %s\nWritten Bytes = %s\n",r,w,read,test_string);
	    TEST_ASSERT_MESSAGE(strcmp(read,test_string) == 0,"String read does not match string written");
	}

    TEST_ASSERT(true);
    return utest::v1::CaseNext;
}

Case cases[] = {
	Case("Level 1 - I2C test - 1 byte (single pin set)", test_level1_i2c<1>, TestFramework::greentea_failure_handler),
	Case("Level 1 - I2C test - 10 byte (single pin set)", test_level1_i2c<10>, TestFramework::greentea_failure_handler),
	Case("Level 1 - I2C test - 100 byte (single pin set)", test_level1_i2c<100>, TestFramework::greentea_failure_handler),
};

int main() {
	// Formulate a specification and run the tests based on the Case array
	Specification specification(TestFramework::test_setup<30>, cases);
    return !Harness::run(specification);
}
