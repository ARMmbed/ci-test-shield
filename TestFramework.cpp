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
#include "TestFramework.hpp"

///////////////////////////////////////////////////////////////////////////////
//								Initialization								 //
///////////////////////////////////////////////////////////////////////////////

TestFramework::TestFramework() {
	map_pins(PinMap_ADC, AnalogInput);
	map_pins(PinMap_DAC, AnalogOutput);
	map_pins(PinMap_PWM, DigitalIO);
	map_pins(PinMap_ADC, DigitalIO);
	map_pins(PinMap_PWM, PWM);
	setup_cits_pins();
}

void TestFramework::setup_cits_pins() {
	pinout[CITS_AnalogInput].push_back(MBED_CONF_APP_AIN_0);
	pinout[CITS_AnalogInput].push_back(MBED_CONF_APP_AIN_1);
	pinout[CITS_AnalogInput].push_back(MBED_CONF_APP_AIN_2);
	pinout[CITS_AnalogInput].push_back(MBED_CONF_APP_AIN_3);
	pinout[CITS_AnalogInput].push_back(MBED_CONF_APP_AIN_4);
	pinout[CITS_AnalogInput].push_back(MBED_CONF_APP_AIN_5);
	pinout[CITS_AnalogOutput].push_back(MBED_CONF_APP_AOUT);
	pinout[CITS_DigitalIO].push_back(MBED_CONF_APP_DIO_0);
	pinout[CITS_DigitalIO].push_back(MBED_CONF_APP_DIO_1);
	pinout[CITS_DigitalIO].push_back(MBED_CONF_APP_DIO_2);
	pinout[CITS_DigitalIO].push_back(MBED_CONF_APP_DIO_3);
	pinout[CITS_DigitalIO].push_back(MBED_CONF_APP_DIO_4);
	pinout[CITS_DigitalIO].push_back(MBED_CONF_APP_DIO_5);
	pinout[CITS_DigitalIO].push_back(MBED_CONF_APP_DIO_6);
	pinout[CITS_DigitalIO].push_back(MBED_CONF_APP_DIO_7);
	pinout[CITS_DigitalIO].push_back(MBED_CONF_APP_DIO_8);
	pinout[CITS_DigitalIO].push_back(MBED_CONF_APP_DIO_9);
	pinout[CITS_PWM].push_back(MBED_CONF_APP_PWM_0);
	pinout[CITS_PWM].push_back(MBED_CONF_APP_PWM_1);
	pinout[CITS_PWM].push_back(MBED_CONF_APP_PWM_2);
	pinout[CITS_PWM].push_back(MBED_CONF_APP_PWM_3);

}

void TestFramework::map_pins(const PinMap pinmap[], Type pintype) {
	int i = 0;
	while(pinmap[i].pin != (PinName)NC) {
		bool alreadyExists = false;
		for (unsigned int j=0; j<pinout[pintype].size(); j++) {
			if (pinout[pintype][j] == pinmap[i].pin)
				alreadyExists = true;
		}
		if (!alreadyExists) {
			pinout[pintype].push_back(pinmap[i].pin);
		}
		i++;
	}
}

int TestFramework::find_pin(PinName pin, Type pintype) {
	for (unsigned int i=0; i<pinout[pintype].size(); i++) {
		if (pin == pinout[pintype][i])
			return i;
	}
	return -1;
}

PinName TestFramework::find_pin_pair(PinName pin) {
	PinName pinpair = pin;
    for (unsigned int i=0; i<pinout[CITS_DigitalIO].size(); i++) {
    	if (pinout[CITS_DigitalIO][i] == pin) {
    		pinpair = pinout[CITS_DigitalIO][i+(i%2 ? -1 : 1)];
    		break;
    	}
    }
    return pinpair;
}

utest::v1::control_t TestFramework::reset_iterator(Type pintype) {
	if (pin_iterators[pintype] < pinout[pintype].size())
		return utest::v1::CaseRepeatAll;
	else {
		pin_iterators[pintype] = 0;
		return utest::v1::CaseNext;
	}
}

///////////////////////////////////////////////////////////////////////////////
//									L0 										 //
///////////////////////////////////////////////////////////////////////////////

utest::v1::control_t TestFramework::test_l0_analogin(const size_t call_count) {
	PinName pin = pinout[AnalogInput][pin_iterators[AnalogInput]++];
	DEBUG_PRINTF("Running analog input constructor on pin %d\n", pin);
    TEST_ASSERT_MESSAGE(pin != NC, "Pin is NC");

	AnalogIn ain(pin);

	TEST_ASSERT(true);
	return reset_iterator(AnalogInput);
}

utest::v1::control_t TestFramework::test_l0_analogout(const size_t call_count) {
	PinName pin = pinout[AnalogOutput][pin_iterators[AnalogOutput]++];
	DEBUG_PRINTF("Running analog output constructor on pin %d\n", pin);
    TEST_ASSERT_MESSAGE(pin != NC, "Pin is NC");

	AnalogOut ain(pin);

	TEST_ASSERT(true);
	return reset_iterator(AnalogOutput);
}

utest::v1::control_t TestFramework::test_l0_digitalio(const size_t call_count) {
	PinName pin = pinout[DigitalIO][pin_iterators[DigitalIO]++];
	DEBUG_PRINTF("Running digital io constructor on pin %d\n", pin);
    TEST_ASSERT_MESSAGE(pin != NC, "Pin is NC");

	DigitalOut dout(pin);
	DigitalIn din(pin);

	TEST_ASSERT(true);
	return reset_iterator(DigitalIO);
}

utest::v1::control_t TestFramework::test_l0_pwm(const size_t call_count) {
	PinName pin = pinout[PWM][pin_iterators[PWM]++];
	DEBUG_PRINTF("Running pwm constructor on pin %d\n", pin);
    TEST_ASSERT_MESSAGE(pin != NC, "Pin is NC");

	PwmOut pwm(pin);
    pwm.period(1.0f);
    pwm.write(0.5f);

	TEST_ASSERT(true);
	return reset_iterator(PWM);
}

///////////////////////////////////////////////////////////////////////////////
//									L1 										 //
///////////////////////////////////////////////////////////////////////////////

utest::v1::control_t TestFramework::test_l1_framework(Type pintype, Type testtype, void (*execution_callback)(PinName, float, int), float floatdata, int intdata) {
	PinName pin = pinout[pintype][pin_iterators[pintype]];
	int index = find_pin(pin, testtype);
	int tag = 0;
	// State: Execute
	if (index != -1) {
		execution_callback(pin, floatdata, intdata);
		pin_iterators[pintype] = 0;
		return utest::v1::CaseNext;
	}
	while (pin_iterators[pintype] < pinout[pintype].size()) {
		// State: Increment iterator
		pin_iterators[pintype]++;
		pin = pinout[pintype][pin_iterators[pintype]];
		index = find_pin(pin, testtype);

		if (index != -1) {
			// State: Execute
			execution_callback(pin, floatdata, intdata);
			pin_iterators[pintype] = 0;
			return utest::v1::CaseNext;
		}
	}
	// State: End test
	return reset_iterator(pintype);
}

utest::v1::control_t TestFramework::test_l1_digitalio(const size_t call_count) {
	return test_l1_framework(DigitalIO, CITS_DigitalIO, &test_digitalio_execute, 0.05, 10);
}

utest::v1::control_t TestFramework::test_l1_analogin(const size_t call_count) {
	return test_l1_framework(AnalogInput, CITS_AnalogInput, &test_analogin_execute, 0.05, 10);
}

utest::v1::control_t TestFramework::test_l1_analogout(const size_t call_count) {
	return test_l1_framework(AnalogOutput, CITS_AnalogOutput, &test_analogout_execute, 0.05, 10);
}

// EXISTS IN HEADER - DO NOT UNCOMMENT - HERE FOR REFERENCE
// template <int dutycycle, int period> 
// utest::v1::control_t TestFramework::test_l1_pwm(const size_t call_count) {
// 	return test_l1_framework(PWM, CITS_PWM, &test_pwm_execute, dutycycle*0.01f, period);
// }

///////////////////////////////////////////////////////////////////////////////
//									L2 										 //
///////////////////////////////////////////////////////////////////////////////

utest::v1::control_t TestFramework::test_l2_framework(Type pintype, Type testtype, void (*execution_callback)(PinName, float, int), float floatdata, int intdata) {
	PinName pin = pinout[pintype][pin_iterators[pintype]];
	int index = find_pin(pin, testtype);
	int tag = 0;
	// State: Execute
	if (index != -1) {
		execution_callback(pin, floatdata, intdata);
		tag = 1;
	}
	while (pin_iterators[pintype] < pinout[pintype].size()) {
		// State: Increment iterator
		pin_iterators[pintype]++;
		pin = pinout[pintype][pin_iterators[pintype]];
		index = find_pin(pin, testtype);

		if (index != -1) {
			// State: End case
			if (tag) {
				TEST_ASSERT(true);
				return utest::v1::CaseRepeatAll;
			// State: Execute
			} else {
				execution_callback(pin, floatdata, intdata);
				tag = 1;
			}
		}
	}
	// State: End test
	return reset_iterator(pintype);
}

utest::v1::control_t TestFramework::test_l2_digitalio(const size_t call_count) {
	return test_l2_framework(DigitalIO, CITS_DigitalIO, &test_digitalio_execute, 0.01, 100);
}

utest::v1::control_t TestFramework::test_l2_analogin(const size_t call_count) {
	return test_l2_framework(AnalogInput, CITS_AnalogInput, &test_analogin_execute, 0.01, 100);
}

utest::v1::control_t TestFramework::test_l2_analogout(const size_t call_count) {
	return test_l1_framework(AnalogOutput, CITS_AnalogOutput, &test_analogout_execute, 0.01, 100);
}

// EXISTS IN HEADER - DO NOT UNCOMMENT - HERE FOR REFERENCE
// template <int dutycycle, int period> 
// utest::v1::control_t TestFramework::test_l2_pwm(const size_t call_count) {
// 	return test_l2_framework(PWM, CITS_PWM, &test_pwm_execute, dutycycle*0.01f, period);
// }

///////////////////////////////////////////////////////////////////////////////
//									Tests									 //
///////////////////////////////////////////////////////////////////////////////

void TestFramework::test_analogin_execute(PinName pin, float tolerance, int iterations) {
	DEBUG_PRINTF("Running analog input range test on pin %d\n", pin);
    TEST_ASSERT_MESSAGE(pin != NC, "Pin is NC");

    for (unsigned int i=0; i<iterations; i++) {

	    // Case 1: Digital and Analog non-conflicting
		AnalogIn ain(pin);
	    DigitalOut dout(MBED_CONF_APP_DIO_0);
	    dout=0;
	    float compare1 = ain.read();
	    dout=1;
	    float compare2 = ain.read();
	    TEST_ASSERT_MESSAGE(true, "Digital writes prevent analog inputs");
	    TEST_ASSERT_MESSAGE(abs(compare1-compare2)<tolerance, "Digital write changed the analog input");

	    // Case 2: Dual purpose analog/digital
	    // DigitalOut dout2(pin);
	    // dout2=0;
	    // dout2=1;
	    // TEST_ASSERT_MESSAGE(true, "Analog pins do not dual purpose as digital IO");
	    // float input = ain.read();
	    // dout2=0;
	    // TEST_ASSERT_MESSAGE(input > (1-tolerance) && input < (1+tolerance), "Analog pin did not match digital output");

	    // Case 3: Range test
	    float input = 0.0f;
		AnalogOut aout(MBED_CONF_APP_AOUT);
		aout = 0.5;
	    TEST_ASSERT_MESSAGE(true, "Analog input pins do not dual purpose as analog output pins");
		for (float i=0.0f; i<=1.0f; i+=0.1f) {
			aout=i;
			input = ain.read();
			TEST_ASSERT_MESSAGE(input>=(i-tolerance) && input<=(i+tolerance), "Analog input matches analog output");
		}
	}
}

void TestFramework::test_analogout_execute(PinName pin, float tolerance, int iterations) {

}

void TestFramework::test_digitalio_execute(PinName pin, float tolerance, int iterations) {
	DEBUG_PRINTF("Running digital io test on pin %d\n", pin);
    TEST_ASSERT_MESSAGE(pin != NC, "Pin is NC");

    PinName pinpair = find_pin_pair(pin);

    for (unsigned int i=0; i<iterations; i++) {
	    DigitalOut dout(pinpair);
	    DigitalIn din(pin);
	    dout = 0;
	    TEST_ASSERT_MESSAGE(0 == din.read(),"Expected value to be 0, read value was not zero");
	    dout = 1;
	    TEST_ASSERT_MESSAGE(1 == din.read(),"Expected value to be 1, read value was not one");
	}
}

void TestFramework::callback_pwm_rise(void) {
    pin_iterators[RiseCount]++;
    pin_iterators[LastRiseTime] = duty_timer.read_ms();
}

void TestFramework::callback_pwm_fall(void) {
    pin_iterators[FallCount]++;
    if (pin_iterators[LastRiseTime] != 0)
        pin_iterators[DutyCount] = pin_iterators[DutyCount] + (duty_timer.read_ms() - pin_iterators[LastRiseTime]);
}

void TestFramework::test_pwm_execute(PinName pin, float dutycycle, int period) {
	DEBUG_PRINTF("Running pwm test on pin %d\n", pin);
    TEST_ASSERT_MESSAGE(pin != NC, "Pin is NC");

    float tolerance = 0.05;
    int iterations = 100;
    float calculated_percent = iterations * tolerance;
    if (calculated_percent < 1) calculated_percent = 1;

    // Initialize PWM, InterruptIn, Timer, and Rising / Falling edge counts
    pin_iterators[FallCount] = 0;
    pin_iterators[RiseCount] = 0;
    pin_iterators[LastRiseTime] = 0;
    pin_iterators[DutyCount] = 0;
    PwmOut pwm(pin);
    duty_timer.reset();
    InterruptIn iin(find_pin_pair(pin));
    iin.rise(callback_pwm_rise);
    iin.fall(callback_pwm_fall);
    DEBUG_PRINTF("Period set to: %f, duty cycle set to: %f\n", (float)period/1000, dutycycle);
    pwm.period((float)period/1000);
    
    //Start Testing
    duty_timer.start();
    pwm.write(dutycycle); // 50% duty cycle
    DEBUG_PRINTF("Waiting for %dms\n", iterations * period);
    wait_ms(iterations * period); // wait for pwm to run and counts to add up
    iin.disable_irq(); // This is here because otherwise it fails on some platforms
    duty_timer.stop();
    int rc = pin_iterators[RiseCount]; // grab the numbers to work with as the pwm may continue going
    int fc = pin_iterators[FallCount];
    float avgTime = (float)pin_iterators[DutyCount] / iterations;
    float expectedTime = (float)period * dutycycle;

    DEBUG_PRINTF("rise count = %d, fall count = %d, expected count = %d\n", rc, fc, iterations);
    TEST_ASSERT_MESSAGE( std::abs(rc-fc) <= calculated_percent, "There was more than a specific variance in number of rise vs fall cycles");
    TEST_ASSERT_MESSAGE( std::abs(iterations - rc) <= calculated_percent, "There was more than a specific variance in number of rise cycles seen and number expected.");
    TEST_ASSERT_MESSAGE( std::abs(iterations - fc) <= calculated_percent, "There was more than a specific variance in number of fall cycles seen and number expected.");
    TEST_ASSERT_MESSAGE( std::abs(expectedTime - avgTime) <= calculated_percent,"Greater than a specific variance between expected and measured duty cycle");
}