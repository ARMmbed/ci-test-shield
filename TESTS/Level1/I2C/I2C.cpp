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
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <sstream>

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

void test_i2c_execute(PinName pin_scl, float null_float, int iterations) {
	PinName pin_sda = TestFramework::pinout[TestFramework::I2C_SDA][TestFramework::pin_iterators[TestFramework::I2C_SCL]].pin;
	DEBUG_PRINTF("Running I2C constructor on SDA pin %d and SCL pin %d\n", pin_sda, pin_scl);
    TEST_ASSERT_MESSAGE(pin_sda != NC, "SDA Pin is NC");
    TEST_ASSERT_MESSAGE(pin_scl != NC, "SCL Pin is NC");

    greentea_send_kv("return_rand", "seed");
    char key[16] = {0};
    char value[32] = {0};
    unsigned int seed;
    greentea_parse_kv(key, value, sizeof(key), sizeof(value));
    stringstream strValue;
	strValue << value;
	strValue >> seed;
	srand(seed);

    I2C i2c(pin_sda, pin_scl);
    I2CEeprom memory(pin_sda,pin_scl,MBED_CONF_APP_I2C_EEPROM_ADDR,32,0);

    std::vector<char> test_string;
    for (int i=0; i<iterations; i++)
    	test_string.push_back('A' + rand()%26);

    char read;
    int r = 0;
    int w = 0;
    w = memory.write(1,test);
    r = memory.read(1,read);
    DEBUG_PRINTF("Num Bytes Read = %d\nNum Bytes Written = %d\nRead byte = `%c`\nWritten Byte = `%c`\n",r,w,read,test);

    TEST_ASSERT_EQUAL_MESSAGE(test,read,"Character Read does not equal character written!");
    TEST_ASSERT_MESSAGE(test == read, "character written does not match character read")

    TEST_ASSERT(true);
}

template <int iterations>
utest::v1::control_t test_level1_i2c(const size_t call_count) {
	return TestFramework::test_level1_framework(TestFramework::I2C_SCL, TestFramework::CITS_I2C_SCL, &test_i2c_execute, 0.0f, iterations);
}

Case cases[] = {
	Case("Level 1 - I2C test - 1 byte (single pin set)", test_level1_i2c<1>, greentea_failure_handler),
	Case("Level 1 - I2C test - 10 byte (single pin set)", test_level1_i2c<10>, greentea_failure_handler),
	Case("Level 1 - I2C test - 100 byte (single pin set)", test_level1_i2c<100>, greentea_failure_handler),
};

int main() {
	// Formulate a specification and run the tests based on the Case array
	Specification specification(test_setup, cases);
    return !Harness::run(specification);
}
