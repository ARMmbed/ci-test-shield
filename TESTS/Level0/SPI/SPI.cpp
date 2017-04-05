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

#if !DEVICE_SPI
    #error SPI is not supported on this platform, add 'DEVICE_SPI' definition to your platform.
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

void construct_i2c(PinName pin_clk, PinName pin_miso, PinName pin_mosi, PinName pin_cs) {
	DEBUG_PRINTF("Running SPI constructor on CLK pin %d, MISO pin %d, MOSI pin %d, and CS pin %d\n", pin_clk, pin_miso, pin_mosi, pin_cs);
    TEST_ASSERT_MESSAGE(pin_clk != NC, "SPI CLK pin is NC");
    TEST_ASSERT_MESSAGE(pin_mosi != NC, "SPI MOSI Pin is NC");
    TEST_ASSERT_MESSAGE(pin_miso != NC, "SPI MISO Pin is NC");
    TEST_ASSERT_MESSAGE(pin_cs != NC, "SPI CS Pin is NC");

    SPI(pin_mosi, pin_miso, pin_clk);
    DigitalOut cs(pin_cs);

	TEST_ASSERT(true);
}

utest::v1::control_t test_level0_spi(const size_t call_count) {
	return test_framework.run_spi(&construct_i2c);
}

Case cases[] = {
	Case("Level 0 - SPI Constructor", test_level0_spi, TestFramework::greentea_failure_handler),
};

int main() {
	// Formulate a specification and run the tests based on the Case array
	Specification specification(TestFramework::test_setup<30>, cases);
    return !Harness::run(specification);
}
