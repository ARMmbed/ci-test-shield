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

#define BUS_SIZE 16

using namespace utest::v1;

// Static variables for managing the dynamic list of pins
std::vector< vector <PinMap> > TestFramework::pinout(TS_NC);
std::vector<unsigned int> TestFramework::pin_iterators(TS_NC);

// Initialize a test framework object
TestFramework test_framework;

utest::v1::control_t test_level0_businout(const size_t call_count) {
	DEBUG_PRINTF("Running bus input/output constructor\n");

	std::vector<PinName> pins;       // vector of pins that the bus will contain
	while(pins.size() < BUS_SIZE){   // continually add pins until the bus is full
    // check if more pins are available to add to bus
		if(TestFramework::check_size(TestFramework::BusIO)) {
      // fetch new pin
		 	pins.push_back(test_framework.get_increment_pin(TestFramework::BusIO).pin);  
    } 

    // if no more suitable pins are available, then add No-Connects until bus is full
    else{
			pins.push_back((PinName)NC);	
    }
	}

  DEBUG_PRINTF("Pins assigned to bus:\n  pin[0] = %#x\n  pin[1] = %#x\n  pin[2] = %#x\n  pin[3] = %#x\n  pin[4] = %#x\n  pin[5] = %#x\n  pin[6] = %#x\n  pin[7] = %#x\n  pin[8] = %#x\n  pin[9] = %#x\n  pin[10] = %#x\n  pin[11] = %#x\n  pin[12] = %#x\n  pin[13] = %#x\n  pin[14] = %#x\n  pin[15] = %#x\n",pins[0],pins[1],pins[2],pins[3],pins[4],pins[5],pins[6],pins[7],pins[8],pins[9],pins[10],pins[11],pins[12],pins[13],pins[14],pins[15]);
	
  // construct the bus with assigned pins
  BusInOut bio(pins[0],pins[1],pins[2],pins[3],pins[4],pins[5],pins[6],pins[7],pins[8],pins[9],pins[10],pins[11],pins[12],pins[13],pins[14],pins[15]);

	return test_framework.reset_iterator(TestFramework::BusIO);
}

Case cases[] = {
	Case("Level 0 - Bus Input/Output Constructor", test_level0_businout, TestFramework::greentea_failure_handler),
};

int main() {
	// Formulate a specification and run the tests based on the Case array
	Specification specification(TestFramework::test_setup<30>, cases);
  return !Harness::run(specification);
}
