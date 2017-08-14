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

#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
#include "ci_test_config.h"

#include "TestFramework.hpp"
#include <vector>

using namespace utest::v1;

// Static variables for managing the dynamic list of pins
std::vector< vector <PinMap> > TestFramework::pinout(TS_NC);
std::vector<unsigned int> TestFramework::pin_iterators(TS_NC);

Timer timer;
int clocked_dio_toggle;

// Initialize a test framework object
TestFramework test_framework;

void dio_toggled(void) 
{
	clocked_dio_toggle = timer.read_us();
}


void test_digitalio_execute(PinName pin, float tolerance, int iterations) 
{
	DEBUG_PRINTF("Running digital io test on pin %d\n", pin);
    TEST_ASSERT_MESSAGE(pin != NC, "Pin is NC");

    PinName pinpair = TestFramework::find_pin_pair(pin);
    DigitalOut dout(pinpair);
    DigitalIn din(pin);

    // Verify Digital Input and Output can occur on the same pin
    dout = 0;
    TEST_ASSERT_MESSAGE(0 == din.read(),"Expected value to be 0, read value was not zero");
    dout = 1;
    TEST_ASSERT_MESSAGE(1 == din.read(),"Expected value to be 1, read value was not one");
    int us_timeout = 10;

    for (unsigned int i=0; i<iterations; i++) {

        // Setup
    	dout = 0;
    	clocked_dio_toggle = 0;
	    timer.reset();
	    InterruptIn iin(pin);
	    iin.rise(dio_toggled);

        // Start the timer and toggle the DIO. Wait for a response in the callback
    	timer.start();
    	dout = 1;
    	wait_us(us_timeout*2);

        // Stop the timer and reset the IRQ
	    iin.disable_irq();
	    timer.stop();
	    // DEBUG_PRINTF("DIO pin %d has response delay: %dus\n", pin, clocked_dio_toggle);
	    TEST_ASSERT_MESSAGE(clocked_dio_toggle < us_timeout, "DIO took longer than the designated timeout to respond");
    }
}


utest::v1::control_t test_level2_digitalio(const size_t call_count) 
{
	return TestFramework::test_level2_framework(TestFramework::DigitalIO, TestFramework::CITS_DigitalIO, &test_digitalio_execute, 0.02, 100);
}


Case cases[] = {
	Case("Level 2 - Digital In/Out Range test (all pins)", test_level2_digitalio, TestFramework::greentea_failure_handler),
};


int main() 
{
	// Formulate a specification and run the tests based on the Case array
	Specification specification(TestFramework::test_setup<30>, cases);
    return !Harness::run(specification);
}
