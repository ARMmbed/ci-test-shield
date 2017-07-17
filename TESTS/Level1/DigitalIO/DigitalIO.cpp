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

Timer timer;
int clocked_dio_toggle;

// Initialize a test framework object
TestFramework test_framework;

void dio_toggled(void) {
	clocked_dio_toggle = timer.read_us();
}

void test_digitalio_execute(PinName pin, float tolerance, int iterations) {
  TEST_ASSERT_MESSAGE(pin != NC, "Pin is NC");

  PinName pinpair = TestFramework::find_pin_pair(pin);
  DEBUG_PRINTF("Running digital io test on pin %#x and its pin-pair %#x", pin, pinpair);

  DigitalOut dout(pinpair);
  DigitalIn din(pin);

  // Verify Digital Input and Output can occur on the same pin
  dout = 0;
  TEST_ASSERT_MESSAGE(0 == din.read(),"Expected value to be 0, read value was not zero");
  dout = 1;
  TEST_ASSERT_MESSAGE(1 == din.read(),"Expected value to be 1, read value was not one");
}

utest::v1::control_t test_level1_digitalio(const size_t call_count) {
	return TestFramework::test_level1_framework(TestFramework::DigitalIO, TestFramework::CITS_DigitalIO, &test_digitalio_execute, 0.05, 10);
}

Case cases[] = {
	Case("Level 1 - Digital In/Out Range test (single pin)", test_level1_digitalio, TestFramework::greentea_failure_handler),
};

int main() {
	// Formulate a specification and run the tests based on the Case array
	Specification specification(TestFramework::test_setup<30>, cases);
  return !Harness::run(specification);
}
