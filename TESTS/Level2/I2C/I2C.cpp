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

template <int num_bytes>
utest::v1::control_t test_level2_i2c(const size_t call_count) 
{
	// Assign I2C pins
	PinName pin_scl = MBED_CONF_APP_I2C_SCL;
	PinName pin_sda = MBED_CONF_APP_I2C_SDA;

    // Check to see if the CI test shield I2C pins are connected to the board
	if (TestFramework::find_pin(pin_scl, TestFramework::I2C_SCL)==-1 ||	
        TestFramework::find_pin(pin_sda, TestFramework::I2C_SDA)==-1) {
		TEST_ASSERT_MESSAGE(false,"CI test shield is not properly connected to default I2C pins, or pins are already assigned as greentea's UART pins.");
    }

    TEST_ASSERT_MESSAGE(pin_sda != NC, "SDA Pin is NC");
    TEST_ASSERT_MESSAGE(pin_scl != NC, "SCL Pin is NC");

    DEBUG_PRINTF("Running I2C constructor on SDA pin %#x and SCL pin %#x\n", pin_sda, pin_scl);
    I2CEeprom memory(pin_sda,pin_scl,MBED_CONF_APP_I2C_EEPROM_ADDR,32,0);

    // Get a random seed from the Greentea host test
	//srand(TestFramework::get_seed());  // TODO not working properly.

	// Generate a random string
    char test_string[128] = {0};
    for (int i=0; i<num_bytes; i++){ 
  	  test_string[i] = 'A' + rand()%26;
    }

    // Generate a random starting location for storing string
    int location = rand() % 200 + 1;

    // Test write/read of random string to random location
    char read_data[128] = {0};
    int w = memory.write(location, test_string, num_bytes);
    int r = memory.read(location, read_data, num_bytes);
    DEBUG_PRINTF("Wrote %d bytes: %s\nRead %d bytes: %s\nLocation: %d\n", w, test_string, r, read_data, location);
    TEST_ASSERT_MESSAGE(strcmp(test_string,read_data) == 0,"Read and write data does not match");

    return utest::v1::CaseNext;
}


Case cases[] = {
	Case("Level 2 - I2C test - 1 byte to random location", test_level2_i2c<1>, TestFramework::greentea_failure_handler),
	Case("Level 2 - I2C test - 10 bytes to random location", test_level2_i2c<10>, TestFramework::greentea_failure_handler),
	Case("Level 2 - I2C test - 100 bytes to random location", test_level2_i2c<100>, TestFramework::greentea_failure_handler),
};


int main() 
{
	// Formulate a specification and run the tests based on the Case array
	Specification specification(TestFramework::test_setup<30>, cases);
    return !Harness::run(specification);
}
