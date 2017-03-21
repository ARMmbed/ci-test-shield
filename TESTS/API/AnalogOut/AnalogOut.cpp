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

// check if AnalogOut is supported on this device
#if !DEVICE_ANALOGOUT
  #error [NOT_SUPPORTED] AnalogOut not supported on this platform, add 'DEVICE_ANALOGOUT' definition to your platform. 
#endif

#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
#include "ci_test_config.h"
//#include "rtos.h"

using namespace utest::v1;

// AnalogOut should be defined
void analogout_pins()
{
    TEST_ASSERT_MESSAGE(MBED_CONF_APP_AOUT != NC, "Pin MBED_CONF_APP_AOUT is defined as NC");
}

// Template to set one Analog pin as input and then cycle through the rest as outputs.
// As you turn more pins on the voltage on the ain pin will go up.
template <PinName aout_pin, PinName ain_pin> 
void AnalogOutput_Test()
{
    AnalogIn ain(ain_pin);
    AnalogOut aout(aout_pin);
    float valueOff = 0;
    float valueOn = 0;
    aout = 0;
    valueOff = ain.read();
    aout = 0.5;
    valueOn = ain.read();
    DEBUG_PRINTF("\r\n***** valueOff = %f, valueOn = %f \r\n",valueOff, valueOn);
    TEST_ASSERT_MESSAGE((0.4f < valueOn) && (0.6f > valueOn), "Value is not in expected range of ~0.5f");
    TEST_ASSERT_MESSAGE(valueOn > valueOff,"Value has not increased, expected 0.5");
    valueOff = ain.read();
    aout = 1.0;
    valueOn = ain.read();
    DEBUG_PRINTF("\r\n***** valueOff = %f, valueOn = %f \r\n",valueOff, valueOn);
    TEST_ASSERT_MESSAGE((0.9f < valueOn) && (1.1f > valueOn), "Value is not in expected range of ~0.5f");
    TEST_ASSERT_MESSAGE(valueOn > valueOff,"Value has not increased, expected 1.0");
}

utest::v1::status_t test_setup(const size_t number_of_cases) {
    // Setup Greentea using a reasonable timeout in seconds
    GREENTEA_SETUP(30, "default_auto");
    return verbose_test_setup_handler(number_of_cases);
}

// Handle test failures, keep testing, don't stop
utest::v1::status_t greentea_failure_handler(const Case *const source, const failure_t reason) {
    greentea_case_failure_abort_handler(source, reason);
    return STATUS_CONTINUE;
}

// Test cases
Case cases[] = {
    Case("AnalogOut Pins defined", analogout_pins, greentea_failure_handler),
    Case("AnalogOut on AOUT", AnalogOutput_Test<MBED_CONF_APP_AOUT,MBED_CONF_APP_AIN_0>,greentea_failure_handler),
};

Specification specification(test_setup, cases);

// Entry point into the tests
int main()
{
    return !Harness::run(specification);
}
