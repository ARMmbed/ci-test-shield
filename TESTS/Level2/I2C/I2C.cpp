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
#include <I2CEeprom.h>
#include <vector>
#include <stdlib.h>

using namespace utest::v1;

// Static variables for managing the dynamic list of pins
std::vector< vector <PinMap> > TestFramework::pinout(TS_NC);
std::vector<unsigned int> TestFramework::pin_iterators(TS_NC);

// Initialize a test framework object
TestFramework test_framework;
char NULL_STR[128] = {0};

template <int iterations>
utest::v1::control_t test_level2_i2c(const size_t call_count) {

	// Check to see if the CI test shield I2C pins are connected to the board
	PinName pin_scl = MBED_CONF_APP_I2C_SCL;
	PinName pin_sda = MBED_CONF_APP_I2C_SDA;
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

	// Generate a random string
    char test_string[128] = {0};
    for (int i=0; i<iterations; i++) 
    	test_string[i] = 'A' + rand()%26;
    int location = rand() % 200 + 1;

    char initial_read[128] = {0};
    char should_be_null[128] = {0};
    char read_data[128] = {0};

    int initial_read_bytes = memory.read(location, initial_read, iterations);
    int initial_write_bytes = memory.write(location, NULL_STR, iterations);
    int secondary_read_bytes = memory.read(location, should_be_null, iterations);

    int w = memory.write(location, test_string, iterations);
    int r = memory.read(location, read_data, iterations);

    DEBUG_PRINTF("Wrote %d bytes: %s\nRead %d bytes: %s\n", w, test_string, r, read_data);
    TEST_ASSERT_MESSAGE(strcmp(initial_read,should_be_null) != 0,"Write never occurred. Null characters never got written");
    TEST_ASSERT_MESSAGE(strcmp(NULL_STR,should_be_null) == 0,"Read is not working. Write occurred yet read grabbed different text");
    TEST_ASSERT_MESSAGE(strcmp(test_string,read_data) == 0,"Read and write data does not match");

    TEST_ASSERT(true);
    return utest::v1::CaseNext;
}

Case cases[] = {
	Case("Level 2 - I2C test - 1 byte (all pin set)", test_level2_i2c<1>, TestFramework::greentea_failure_handler),
	Case("Level 2 - I2C test - 10 byte (all pin set)", test_level2_i2c<10>, TestFramework::greentea_failure_handler),
	Case("Level 2 - I2C test - 100 byte (all pin set)", test_level2_i2c<100>, TestFramework::greentea_failure_handler),
};
	
int main() {
	// Formulate a specification and run the tests based on the Case array
	Specification specification(TestFramework::test_setup<30>, cases);
    return !Harness::run(specification);
}
