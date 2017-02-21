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
#ifndef TESTFRAMEWORK_H
#define TESTFRAMEWORK_H

#include "cmsis.h"
#include "pinmap.h"
#include "PeripheralPins.h"
#include "mbed.h"

#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
#include "ci_test_config.h"

#include <vector>

using namespace std;

class TestFramework {
public:

	enum Type {
		AnalogInput,
		AnalogOutput,
		DigitalIO
	};

	// Analog pin iterator
	static int analog_pin_index;
	// Analog input pins
	static vector<PinName> analog_in_pins;
	// Analog output pins
	static vector<PinName> analog_out_pins;

	// Analog In tests
	static utest::v1::control_t test_l0_analogin(const size_t call_count);
	// Analog Out tests
	static utest::v1::control_t test_l0_analogout(const size_t call_count);

	TestFramework();

private:

	/**
	  * Gather all pins in a pinmap defined by the HAL and store them in the corresponding allPins vector
	  * @param PinMap[] from the HAL
	  * @param Type specifying which pin type to save the pins as
	**/
	void map_pins(const PinMap pinmap[], std::vector<PinName> * pin_list);

};

#endif //TESTFRAMEWORK_H