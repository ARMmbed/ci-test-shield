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
  #error [NOT_SUPPORTED] AnalogIn not supported on this platform, add 'DEVICE_ANALOGIN' definition to your platform.
#endif

#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
#include "ci_test_config.h"

using namespace utest::v1;

volatile bool Result = false;

// Callback for all InterruptInput functions
void cbfn(void)
{
    Result = true;
}


// Template to test DigitalIO, AnalogIn, and InterruptIn pins. Meant to be re-used multiple times
template <PinName digitalOut_pin, PinName digitalIn_pin, PinName analogIn_pin, PinName analogBus_pin1, PinName analogBus_pin2, PinName analogBus_pin3, PinName analogBus_pin4, PinName analogBus_pin5, PinName interruptIn_pin, PinName interruptOut_pin> 


void GPIO_Test()
{
    // ***********************
    // Initialize API pins
    // ***********************
    DEBUG_PRINTF("Initializing API pins\n");
    // Pins for DIO test
    DigitalOut d_out(digitalOut_pin);
    DigitalIn d_in(digitalIn_pin);

    // Pins for AnalogIn test
    AnalogIn a_in(analogIn_pin);
    BusInOut aBus_outputs(analogBus_pin1,analogBus_pin2,analogBus_pin3,analogBus_pin4,analogBus_pin5);

    // Pins for InterruptIn test
    InterruptIn int_in(interruptIn_pin);
    DigitalOut int_out(interruptOut_pin);

    // ***********************
    // Begin concurrent API testing
    // ***********************
    DEBUG_PRINTF("Begining concurrent API testing\n");
    aBus_outputs.output();
    int analogOut_value= 0;
    aBus_outputs = 0;
    float prev_analog_value = 0;
    for(int iter = 0; iter<5; iter++) { 
        // DigitalIO test
        d_out = 0;  // test 0
        TEST_ASSERT_MESSAGE(0 == d_in.read(),"Expected value to be 0, read value was not zero");
        d_out = 1;  // test 1
        TEST_ASSERT_MESSAGE(1 == d_in.read(),"Expected value to be 1, read value was not one");

        // AnalogIn test
        prev_analog_value = a_in.read();
        analogOut_value = (analogOut_value << 1) + 1;
        aBus_outputs = analogOut_value;
        TEST_ASSERT_MESSAGE(a_in.read() > prev_analog_value,"Analog Input did not increment.");

        // Rising Edge InterruptIn test
        Result = false;
        int_out = 0;
        int_in.rise(cbfn);
        int_out = 1;
        wait(0); // dummy wait to get volatile result value
        TEST_ASSERT_MESSAGE(Result,"cbfn was not triggered on rising edge of pin");

        // Falling Edge InterruptIn test
        Result = false;
        int_out = 1;
        int_in.fall(cbfn);
        int_out = 0;
        wait(0); // dummy wait to get volatile result value
        TEST_ASSERT_MESSAGE(Result,"cbfn was not triggered on falling edge of pin");
    }
}


utest::v1::status_t test_setup(const size_t number_of_cases)
{
    // Setup Greentea using a reasonable timeout in seconds
    GREENTEA_SETUP(30, "default_auto");
    return verbose_test_setup_handler(number_of_cases);
}


// Handle test failures, keep testing, dont stop
utest::v1::status_t greentea_failure_handler(const Case *const source, const failure_t reason)
{
    greentea_case_failure_abort_handler(source, reason);
    return STATUS_CONTINUE;
}


// Test cases
// Runs three different APIs concurrently, DIO, AnalogIn, and InterruptIn
Case cases[] = {
    Case("Concurrent testing of DIO(D2,D3), AnalogIn(A0), and InterruptIn(D4,D5)",GPIO_Test<MBED_CONF_APP_DIO_2,MBED_CONF_APP_DIO_3,MBED_CONF_APP_AIN_0,MBED_CONF_APP_AIN_1,MBED_CONF_APP_AIN_2,MBED_CONF_APP_AIN_3,MBED_CONF_APP_AIN_4,MBED_CONF_APP_AIN_5,MBED_CONF_APP_DIO_4,MBED_CONF_APP_DIO_5>,greentea_failure_handler),

    Case("Concurrent testing of DIO(D3,D2), AnalogIn(A1), and InterruptIn(D5,D4)",GPIO_Test<MBED_CONF_APP_DIO_3,MBED_CONF_APP_DIO_2, MBED_CONF_APP_AIN_1,MBED_CONF_APP_AIN_2,MBED_CONF_APP_AIN_3,MBED_CONF_APP_AIN_4,MBED_CONF_APP_AIN_5,MBED_CONF_APP_AIN_0,MBED_CONF_APP_DIO_5,MBED_CONF_APP_DIO_4>,greentea_failure_handler),
    
    Case("Concurrent testing of DIO(D4,D5), AnalogIn(A2), and InterruptIn(D6,D7)",GPIO_Test<MBED_CONF_APP_DIO_4,MBED_CONF_APP_DIO_5, MBED_CONF_APP_AIN_2,MBED_CONF_APP_AIN_3,MBED_CONF_APP_AIN_4,MBED_CONF_APP_AIN_5,MBED_CONF_APP_AIN_0,MBED_CONF_APP_AIN_1,MBED_CONF_APP_DIO_6,MBED_CONF_APP_DIO_7>,greentea_failure_handler),

    Case("Concurrent testing of DIO(D5,D4), AnalogIn(A3), and InterruptIn(D7,D6)",GPIO_Test<MBED_CONF_APP_DIO_5,MBED_CONF_APP_DIO_4, MBED_CONF_APP_AIN_3,MBED_CONF_APP_AIN_4,MBED_CONF_APP_AIN_5,MBED_CONF_APP_AIN_0,MBED_CONF_APP_AIN_1,MBED_CONF_APP_AIN_2,MBED_CONF_APP_DIO_7,MBED_CONF_APP_DIO_6>,greentea_failure_handler),

    Case("Concurrent testing of DIO(D6,D7), AnalogIn(A4), and InterruptIn(D8,D9)",GPIO_Test<MBED_CONF_APP_DIO_6,MBED_CONF_APP_DIO_7, MBED_CONF_APP_AIN_4,MBED_CONF_APP_AIN_5,MBED_CONF_APP_AIN_0,MBED_CONF_APP_AIN_1,MBED_CONF_APP_AIN_2,MBED_CONF_APP_AIN_3,MBED_CONF_APP_DIO_8,MBED_CONF_APP_DIO_9>,greentea_failure_handler),

    Case("Concurrent testing of DIO(D7,D6), AnalogIn(A5), and InterruptIn(D9,D8)",GPIO_Test<MBED_CONF_APP_DIO_7,MBED_CONF_APP_DIO_6, MBED_CONF_APP_AIN_5,MBED_CONF_APP_AIN_0,MBED_CONF_APP_AIN_1,MBED_CONF_APP_AIN_2,MBED_CONF_APP_AIN_3,MBED_CONF_APP_AIN_4,MBED_CONF_APP_DIO_9,MBED_CONF_APP_DIO_8>,greentea_failure_handler),

    Case("Concurrent testing of DIO(D8,D9), AnalogIn(A5), and InterruptIn(D2,D3)",GPIO_Test<MBED_CONF_APP_DIO_8,MBED_CONF_APP_DIO_9, MBED_CONF_APP_AIN_5,MBED_CONF_APP_AIN_0,MBED_CONF_APP_AIN_1,MBED_CONF_APP_AIN_2,MBED_CONF_APP_AIN_3,MBED_CONF_APP_AIN_4,MBED_CONF_APP_DIO_2,MBED_CONF_APP_DIO_3>,greentea_failure_handler),

    Case("Concurrent testing of DIO(D9,D8), AnalogIn(A5), and InterruptIn(D3,D2)",GPIO_Test<MBED_CONF_APP_DIO_9,MBED_CONF_APP_DIO_8, MBED_CONF_APP_AIN_5,MBED_CONF_APP_AIN_0,MBED_CONF_APP_AIN_1,MBED_CONF_APP_AIN_2,MBED_CONF_APP_AIN_3,MBED_CONF_APP_AIN_4,MBED_CONF_APP_DIO_3,MBED_CONF_APP_DIO_2>,greentea_failure_handler),
};


Specification specification(test_setup, cases);


// Entry point into the tests
int main()
{
    return !Harness::run(specification);
}
