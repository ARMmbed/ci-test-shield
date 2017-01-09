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

#if !DEVICE_ANALOGIN
    #error [NOT_SUPPORTED] AnalogIn not supported on this platform, add 'DEVICE_ANALOGIN' deffinition to your platform.
#endif

#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
//#include "rtos.h"

using namespace utest::v1;

// test for existence of DEVICE_ANALOGIN macro. If it doesn't exist then the device doesn't support the AnalogIn API
void Macro_Test(){
#ifndef DEVICE_ANALOGIN
    TEST_ASSERT_MESSAGE(false,"Macr `DEVICE_ANALOGIN` not defined. This platform does not support AnalogIn.")
#endif
}

// Test if the AnalogInput can also do DigitalIO. Failure expects a timeout. 
// typical failure is a run time assert deep in mbed library. 
// CI Test shield assumes AnalogIn pins can double duty as DigitalOut pins.
template <PinName ain_pin>
void can_digitalout(){
    DigitalOut dout(ain_pin);
    TEST_ASSERT_MESSAGE(true,"We got to this point, we're goog to go.");
}

// Test that AnalogIn ports can be AnalogIn and Digital Out at same time
//  meant to catch boards that turn all pins to analog in instead of just 1 pin
void Can_AnalogIn_and_DigitalOut()
{
    AnalogIn ain(MBED_CONF_APP_AIN_0);
    DigitalOut dout(MBED_CONF_APP_AIN_1);
    dout=0;
    ain.read();
    dout=1;
    ain.read();
    TEST_ASSERT_MESSAGE(true,"We got to this point, we're goog to go.");
}

// Test that the pins are actually connected
template <PinName d_pin>
void test_AnalogIn_NC()
{
	TEST_ASSERT_MESSAGE(d_pin != NC, "Pin is NC");
}

utest::v1::status_t test_setup(const size_t number_of_cases)
{
	// Setup Greentea using a reasonable timeout in seconds
	GREENTEA_SETUP(5, "default_auto");
	return verbose_test_setup_handler(number_of_cases);
}

utest::v1::status_t greentea_failure_handler(const Case* const source, const failure_t reason)
{
    greentea_case_failure_abort_handler(source, reason);
    return STATUS_CONTINUE;	
}


// Test cases
// TODO: take pin names from config file or generate from pinmap file
Case cases[] = {
    Case("AnalogIn - existence of `DEVICE_ANALOGIN` macro", Macro_Test,greentea_failure_handler),
	Case("AnalogIn - is pin 0 connected?", test_AnalogIn_NC<MBED_CONF_APP_AIN_0>, greentea_failure_handler),
	Case("AnalogIn - is pin 1 connected?", test_AnalogIn_NC<MBED_CONF_APP_AIN_1>, greentea_failure_handler),
	Case("AnalogIn - is pin 2 connected?", test_AnalogIn_NC<MBED_CONF_APP_AIN_2>, greentea_failure_handler),
	Case("AnalogIn - is pin 3 connected?", test_AnalogIn_NC<MBED_CONF_APP_AIN_3>, greentea_failure_handler),
	Case("AnalogIn - is pin 4 connected?", test_AnalogIn_NC<MBED_CONF_APP_AIN_4>, greentea_failure_handler),
	Case("AnalogIn - is pin 5 connected?", test_AnalogIn_NC<MBED_CONF_APP_AIN_5>, greentea_failure_handler),
    Case("AnalogIn - can co-exist with DigitalOut", Can_AnalogIn_and_DigitalOut,greentea_failure_handler),
    Case("AnalogIn 0 - pin can do DigitalOut", can_digitalout<MBED_CONF_APP_AIN_0>,greentea_failure_handler),
    Case("AnalogIn 1 - pin can do DigitalOut", can_digitalout<MBED_CONF_APP_AIN_1>,greentea_failure_handler),
    Case("AnalogIn 2 - pin can do DigitalOut", can_digitalout<MBED_CONF_APP_AIN_2>,greentea_failure_handler),
    Case("AnalogIn 3 - pin can do DigitalOut", can_digitalout<MBED_CONF_APP_AIN_3>,greentea_failure_handler),
    Case("AnalogIn 4 - pin can do DigitalOut", can_digitalout<MBED_CONF_APP_AIN_4>,greentea_failure_handler),
    Case("AnalogIn 5 - pin can do DigitalOut", can_digitalout<MBED_CONF_APP_AIN_5>,greentea_failure_handler),

};

Specification specification(test_setup, cases);

// Entry point into the tests
int main() {
    return !Harness::run(specification);
}
