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
#include <cmath>
#include <string>
#include <iostream>
#include <sstream>
#include <stdarg.h>

#define TEST_STRING_MAX 100

class TestFramework {
public:

	enum Type {
		AnalogInput,
		AnalogOutput,
		DigitalIO,
		PWM,
		I2C_SDA,
		I2C_SCL,
		SPI_CLK,
		SPI_MISO,
		SPI_MOSI,
		SPI_CS,
		BusIO,
		CITS_AnalogInput,
		CITS_AnalogOutput,
		CITS_DigitalIO,
		CITS_PWM,
		CITS_I2C_SDA,
		CITS_I2C_SCL,
		CITS_SPI_MOSI,
		CITS_SPI_MISO,
		CITS_SPI_CLK,
		CITS_SPI_CS,
		TS_NC
	};

	static std::vector< std::vector <PinMap> > pinout;
	static std::vector<unsigned int> pin_iterators;

	/**
	  * Constructor for the test framework. Add all of the PinMaps to the pinout, as well as CI test shield pins
	**/
	TestFramework();


	template <int timeout>
	static utest::v1::status_t test_setup(const size_t number_of_cases) {
	    GREENTEA_SETUP(timeout, "rand_provider");
	    return utest::v1::verbose_test_setup_handler(number_of_cases);
	}

	static utest::v1::status_t greentea_failure_handler(const utest::v1::Case *const source, const utest::v1::failure_t reason) {
	    greentea_case_failure_abort_handler(source, reason);
	    return utest::v1::STATUS_ABORT;
	}
	
	/**
	  * Find a pin within the array of type pintype
	  * @param PinName pin to search for
	  * @param Type pin type to in which the pin has to be a part of
	  * @return index where the pin is found. Returns -1 if can't be found
	**/
	static int find_pin(PinName pin, Type pintype);

	/**
	  * Find the pin that is connected to the specified pin via a resistor on the DIO HW bank
	  * @param PinName pin to find the corresponding pin connected by the resistor
	  * @return PinName pin that is connected to the input pin
	**/ 
	static PinName find_pin_pair(PinName pin);

	/**
	  * Check to see if the pin iterator is pointing at a valid pin in the pinout
	  * @param Type pin type to validate
	  * @return bool if the pin iterator is a valid pin in the pinout
	**/
	static bool check_size(Type pintype);

	/**
	  * Reset the iterator if all pins of the specified pin type have been looked at,<br>
	  *   or proceed to the next pin of the pin type if there are remaining pins
	  * @param Type pin type to reset
	  * @return control_t Case control statement to repeat or move on to the next case
	**/
	static utest::v1::control_t reset_iterator(Type pintype);

	/**
	  * Get the current pin for a specific pin type, and incrememnt the iterator for that pin type
	  * @param Type pin type to retrieve and increment
	  * @return PinName current pin
	**/
	static PinMap get_increment_pin(Type pintype);

	/**
	  * Find the resistor ladder pins that are not the specified pin
	  * @param PinName pin that should not be included in the resistor ladder
	  * @return vector<PinName> list of resistor ladder pins
	**/
	static std::vector<PinName> find_resistor_ladder_pins(PinName pin);

	/**
	  * Get a randomized seed from Greentea's host test
	  * @return unsigned int random seed
	**/
	static unsigned int get_seed();

	/**
	  * Find a matching pin based on HW blocks to the current pin. Iterate the corresponding pin after test case execution
	  * @param Callback to a function to run once a pin pairing has been found
	  * @return control_t Case control to repeat or move on to next test cases
	**/
	static utest::v1::control_t run_i2c(void (*execution_callback)(PinName, PinName));

	/**
	  * Find a pin set (CLK, MISO, MOSI, CS) based on HW blocks that matches the current pin. Iterate the corresponding pins after test execution
	  * @param Callback to a function to run once a pin pairing has been found
	  * @return control_t Case control to repeat or move on to the next test cases
	**/
	static utest::v1::control_t run_spi(void (*execution_callback)(PinName, PinName, PinName, PinName));

	/**
	  * Find a pin of a specified test type and run a test case (passed in as a callback)
	  * @param Type type of pin to find to run the callback
	  * @param Type corresponding type of pin on the CI test shield
	  * @param Callback to the test case to run once a pin is found
	  * @param float data in float form to pass to the execution callback. Depends on callback
	  * @param int data in int form to pass to the execution callback. Depends on callback
	  * @return control_t identify to repeat or proceed to the next test case corresponding to the Specification
	**/
	static utest::v1::control_t test_level1_framework(Type pintype, Type testtype, void (*execution_callback)(PinName, float, int), float floatdata, int intdata);

	/**
	  * Find pins of a specified test type and run a test case (passed in as a callback)
	  *   Difference from the test_level1_framework function is this runs on ALL pins of a specified test type
	  * @param Type type of pin to find to run the callback
	  * @param Type corresponding type of pin on the CI test shield
	  * @param Callback to the test case to run once a pin is found
	  * @param float data in float form to pass to the execution callback. Depends on callback
	  * @param int data in int form to pass to the execution callback. Depends on callback
	  * @return control_t identify to repeat or proceed to the next test case corresponding to the Specification
	**/
	static utest::v1::control_t test_level2_framework(Type pintype, Type testtype, void (*execution_callback)(PinName, float, int), float floatdata, int intdata);


private:

	/**
	  * Add all of the CI Test Shield pins to the pinout vector
	**/
	void setup_cits_pins();

	/**
	  * Construct a pinmap from a pin
	**/
	PinMap construct_pinmap(PinName pin);

	/**
	  * Put pins from the specified pinmap into the pinout array indexed by pin type
	  * @param PinMap[] Found in the hal, specifies all the pins with a certain type
	  * @param Type which type the pins belong to (used to index into pinout)
	**/
	void map_pins(const PinMap pinmap[], Type pintype);

};

#endif //TESTFRAMEWORK_H
