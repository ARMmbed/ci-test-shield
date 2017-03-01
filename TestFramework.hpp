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
#include <cmath>

class TestFramework {
public:

	enum Type {
		AnalogInput,
		AnalogOutput,
		DigitalIO,
		PWM,
		CITS_AnalogInput,
		CITS_AnalogOutput,
		CITS_DigitalIO,
		CITS_PWM,
		RiseCount,
		FallCount,
		DutyCount,
		LastRiseTime,
		TS_NC
	};

	// Analog In tests
	static utest::v1::control_t test_l0_analogin(const size_t call_count);
	static utest::v1::control_t test_l1_analogin(const size_t call_count);
	static utest::v1::control_t test_l2_analogin(const size_t call_count);

	// Analog Out tests
	static utest::v1::control_t test_l0_analogout(const size_t call_count);
	static utest::v1::control_t test_l1_analogout(const size_t call_count);
	static utest::v1::control_t test_l2_analogout(const size_t call_count);

	// Digitial IO tests
	static utest::v1::control_t test_l0_digitalio(const size_t call_count);
	static utest::v1::control_t test_l1_digitalio(const size_t call_count);
	static utest::v1::control_t test_l2_digitalio(const size_t call_count);

	// PWM tests
	static utest::v1::control_t test_l0_pwm(const size_t call_count);

	template <int dutycycle, int period> 
	static utest::v1::control_t test_l1_pwm(const size_t call_count) {
		return test_l1_framework(PWM, CITS_PWM, &test_pwm_execute, dutycycle*0.01f, period);
	}

	template <int dutycycle, int period> 
	static utest::v1::control_t test_l2_pwm(const size_t call_count) {
		return test_l2_framework(PWM, CITS_PWM, &test_pwm_execute, dutycycle*0.01f, period);
	}

	static std::vector< std::vector <PinName> > pinout;
	static std::vector<int> pin_iterators;
	static Timer duty_timer;

	TestFramework();

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

	static utest::v1::control_t reset_iterator(Type pintype);

	static utest::v1::control_t test_l1_framework(Type pintype, Type testtype, void (*execution_callback)(PinName, float, int), float floatdata, int intdata);
	static utest::v1::control_t test_l2_framework(Type pintype, Type testtype, void (*execution_callback)(PinName, float, int), float floatdata, int intdata);

	// Helper functions
	static void test_analogin_execute(PinName pin, float tolerance, int iterations);
	static void test_digitalio_execute(PinName pin, float tolerance, int iterations);
	static void test_analogout_execute(PinName pin, float tolerance, int iterations);
	static void test_pwm_execute(PinName pin, float dutycycle, int period);

	static volatile int fall_count;
	static volatile int rise_count;
	static void callback_pwm_fall(void);
	static void callback_pwm_rise(void);

};

#endif //TESTFRAMEWORK_H