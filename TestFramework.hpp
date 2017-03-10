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
#include <I2CEeprom.h>
#include "SDFileSystem.h" 

#include <vector>
#include <cmath>
#include <string>

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
		RiseCount,
		FallCount,
		DutyCount,
		LastRiseTime,
		TS_NC
	};

	// Analog In tests
	static utest::v1::control_t test_level1_analogin(const size_t call_count);
	static utest::v1::control_t test_level2_analogin(const size_t call_count);

	// Analog Out tests
	static utest::v1::control_t test_level1_analogout(const size_t call_count);
	static utest::v1::control_t test_level2_analogout(const size_t call_count);

	// Digitial IO tests
	static utest::v1::control_t test_level1_digitalio(const size_t call_count);
	static utest::v1::control_t test_level2_digitalio(const size_t call_count);

	// PWM tests

	template <int dutycycle, int period> 
	static utest::v1::control_t test_level1_pwm(const size_t call_count) {
		return test_level1_framework(PWM, CITS_PWM, &test_pwm_execute, dutycycle*0.01f, period);
	}

	template <int dutycycle, int period> 
	static utest::v1::control_t test_level2_pwm(const size_t call_count) {
		return test_level2_framework(PWM, CITS_PWM, &test_pwm_execute, dutycycle*0.01f, period);
	}

	// I2C tests
	static utest::v1::control_t test_level1_i2c(const size_t call_count);
	static utest::v1::control_t test_level2_i2c(const size_t call_count);

	// SPI tests
	static utest::v1::control_t test_level1_spi(const size_t call_count);
	static utest::v1::control_t test_level2_spi(const size_t call_count);

	// Static variables
	static std::vector< std::vector <PinMap> > pinout;
	static std::vector<int> pin_iterators;
	static Timer duty_timer;

	TestFramework();

	static utest::v1::control_t reset_iterator(Type pintype);

	PinMap get_increment_pin(Type pintype);



	static utest::v1::control_t run_i2c(void (*execution_callback)(PinName, PinName));

	static utest::v1::control_t run_spi(void (*execution_callback)(PinName, PinName, PinName, PinName));

	// static int spi_helper(void (*execution_callback)(PinName, PinName, PinName, PinName), int pin_iterator, int * tag);

	static utest::v1::control_t test_level1_framework(Type pintype, Type testtype, void (*execution_callback)(PinName, float, int), float floatdata, int intdata);

	static utest::v1::control_t test_level2_framework(Type pintype, Type testtype, void (*execution_callback)(PinName, float, int), float floatdata, int intdata);


private:

	/**
	  * Gather all pins in a pinmap defined by the HAL and store them in the corresponding allPins vector
	  * @param PinMap[] from the HAL
	  * @param Type specifying which pin type to save the pins as
	**/
	void map_pins(const PinMap pinmap[], Type pintype);
	void setup_cits_pins();
	static int find_pin(PinName pin, Type pintype);
	static PinName find_pin_pair(PinName pin);

	// Helper functions
	static void test_analogin_execute(PinName pin, float tolerance, int iterations);
	static void test_digitalio_execute(PinName pin, float tolerance, int iterations);
	static void test_analogout_execute(PinName pin, float tolerance, int iterations);
	static void test_pwm_execute(PinName pin, float dutycycle, int period);
	static void test_i2c_execute(PinName pin, float tolerance, int iterations);
	static void test_spi_execute(PinName pin, float tolerance, int iterations);

	static volatile int fall_count;
	static volatile int rise_count;
	static void callback_pwm_fall(void);
	static void callback_pwm_rise(void);

};

#endif //TESTFRAMEWORK_H