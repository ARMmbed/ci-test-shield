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

	TestFramework();

	static utest::v1::control_t reset_iterator(Type pintype);

	static PinMap get_increment_pin(Type pintype);
	
	static PinName find_pin_pair(PinName pin);

	static int find_pin(PinName pin, Type pintype);

	static std::vector<PinName> find_resistor_ladder_pins(PinName pin);

	static utest::v1::control_t run_i2c(void (*execution_callback)(PinName, PinName));

	static utest::v1::control_t run_spi(void (*execution_callback)(PinName, PinName, PinName, PinName));

	static utest::v1::control_t test_level1_framework(Type pintype, Type testtype, void (*execution_callback)(PinName, float, int), float floatdata, int intdata);

	static utest::v1::control_t test_level2_framework(Type pintype, Type testtype, void (*execution_callback)(PinName, float, int), float floatdata, int intdata);

	static unsigned int get_seed();

	template <int timeout>
	static utest::v1::status_t test_setup(const size_t number_of_cases) {
	    GREENTEA_SETUP(timeout, "rand_provider");
	    return utest::v1::verbose_test_setup_handler(number_of_cases);
	}

	static utest::v1::status_t greentea_failure_handler(const utest::v1::Case *const source, const utest::v1::failure_t reason) {
	    greentea_case_failure_abort_handler(source, reason);
	    return utest::v1::STATUS_ABORT;
	}

private:

	void map_pins(const PinMap pinmap[], Type pintype);

	void setup_cits_pins();

	PinMap construct_pinmap(PinName pin);

};

#endif //TESTFRAMEWORK_H