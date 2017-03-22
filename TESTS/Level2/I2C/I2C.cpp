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
#if !DEVICE_I2C
  #error [NOT_SUPPORTED] I2C not supported on this platform, add 'DEVICE_I2C' definition to your platform.
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
#include <I2CEeprom.h>
#include <vector>
#include <stdlib.h>

using namespace utest::v1;

// Static variables for managing the dynamic list of pins
std::vector< vector <PinMap> > TestFramework::pinout(TS_NC);
std::vector<int> TestFramework::pin_iterators(TS_NC);

// Initialize a test framework object
TestFramework test_framework;

utest::v1::status_t test_setup(const size_t number_of_cases) {
    GREENTEA_SETUP(30, "rand_provider");
    return verbose_test_setup_handler(number_of_cases);
}

utest::v1::status_t greentea_failure_handler(const Case *const source, const failure_t reason) {
    greentea_case_failure_abort_handler(source, reason);
    return STATUS_ABORT;
}

template <int iterations>
utest::v1::control_t test_level2_i2c(const size_t call_count) {

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

	srand(TestFramework::get_seed());

    I2CEeprom memory(pin_sda,pin_scl,MBED_CONF_APP_I2C_EEPROM_ADDR,32,0);

    for (int i=0; i<2; i++) {

	    char test_string[128] = {0};
	    for (int i=0; i<iterations; i++) 
	    	test_string[i] = 'A' + rand()%26;

	    char read[128] = {0};
	    int r = 0;
	    int w = 0;
	    int location = rand() % 200 + 1;
	    w = memory.write(location, test_string, iterations);
	    r = memory.read(location, read, iterations);
	    DEBUG_PRINTF("Num Bytes Read = %d, Num Bytes Written = %d\nRead bytes = %s\nWritten Bytes = %s\n",r,w,read,test_string);
	    TEST_ASSERT_MESSAGE(strcmp(read,test_string) == 0,"String read does not match string written");
	}

    TEST_ASSERT(true);
    return utest::v1::CaseNext;
}

Case cases[] = {
	Case("Level 2 - I2C test - 1 byte (all pin set)", test_level2_i2c<1>, greentea_failure_handler),
	Case("Level 2 - I2C test - 10 byte (all pin set)", test_level2_i2c<10>, greentea_failure_handler),
	Case("Level 2 - I2C test - 100 byte (all pin set)", test_level2_i2c<100>, greentea_failure_handler),
};
	
Specification specification(test_setup, cases);

int main() {
    return !Harness::run(specification);
}
