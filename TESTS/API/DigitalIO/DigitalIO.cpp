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

#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"


using namespace utest::v1;

// Template to test paired Digital IO pins, meant to be re-used multiple times
template <PinName dout_pin, PinName din_pin> 
void DigitalIO_Test()
{
    DigitalOut dout(dout_pin);
    DigitalIn din(din_pin);
    // test 0
    dout = 0;
    TEST_ASSERT_MESSAGE(0 == din.read(),"Expected value to be 0, read value was not zero");
    // test 1
    dout = 1;
    TEST_ASSERT_MESSAGE(1 == din.read(),"Expected value to be 1, read value was not one");
}

utest::v1::status_t test_setup(const size_t number_of_cases) {
    // Setup Greentea using a reasonable timeout in seconds
    GREENTEA_SETUP(30, "default_auto");
    return verbose_test_setup_handler(number_of_cases);
}

// Handle test failures, keep testing, dont stop
utest::v1::status_t greentea_failure_handler(const Case *const source, const failure_t reason) {
    greentea_case_failure_abort_handler(source, reason);
    return STATUS_CONTINUE;
}

// Test cases
// TODO: take pinnames from config file
Case cases[] = {
    Case("Digital I/O on DIO_3/DIO_2", DigitalIO_Test<MBED_CONF_APP_DIO_2,MBED_CONF_APP_DIO_3>,greentea_failure_handler),
    Case("Digital I/O on DIO_2/DIO_3", DigitalIO_Test<MBED_CONF_APP_DIO_3,MBED_CONF_APP_DIO_2>,greentea_failure_handler),
    Case("Digital I/O on DIO_5/DIO_4", DigitalIO_Test<MBED_CONF_APP_DIO_4,MBED_CONF_APP_DIO_5>,greentea_failure_handler),
    Case("Digital I/O on DIO_4/DIO_5", DigitalIO_Test<MBED_CONF_APP_DIO_5,MBED_CONF_APP_DIO_4>,greentea_failure_handler),
    Case("Digital I/O on DIO_7/DIO_6", DigitalIO_Test<MBED_CONF_APP_DIO_6,MBED_CONF_APP_DIO_7>,greentea_failure_handler),
    Case("Digital I/O on DIO_6/DIO_7", DigitalIO_Test<MBED_CONF_APP_DIO_7,MBED_CONF_APP_DIO_6>,greentea_failure_handler),
    Case("Digital I/O on DIO_9/DIO_8", DigitalIO_Test<MBED_CONF_APP_DIO_8,MBED_CONF_APP_DIO_9>,greentea_failure_handler),
    Case("Digital I/O on DIO_8/DIO_9", DigitalIO_Test<MBED_CONF_APP_DIO_9,MBED_CONF_APP_DIO_8>,greentea_failure_handler),
    //Case("Digital I/O on DIO_1/DIO_0", DigitalIO_Test<MBED_CONF_APP_DIO_0,MBED_CONF_APP_DIO_1>,greentea_failure_handler),
    //Case("Digital I/O on DIO_0/DIO_1", DigitalIO_Test<MBED_CONF_APP_DIO_1,MBED_CONF_APP_DIO_0>,greentea_failure_handler),

};

Specification specification(test_setup, cases);

// Entry point into the tests
int main()
{
    return !Harness::run(specification);
}
