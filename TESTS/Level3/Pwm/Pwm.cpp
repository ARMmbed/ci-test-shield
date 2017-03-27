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

#if !DEVICE_PWMOUT
  #error [NOT_SUPPORTED] PWMOUT not supported on this platform, add 'DEVICE_PWMOUT' definition to your platform.
#endif

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

// Initialize a test framework object
TestFramework test_framework;

Timer timer;
int rise_count;
int fall_count;
int last_rise_time;
int duty_count;

// Callback for a PWM rising edge
void callback_pwm_rise(void) {
    rise_count++;
    last_rise_time = timer.read_ms();
}

// Callback for a PWM falling edge
void callback_pwm_fall(void) {
    fall_count++;
    if (last_rise_time != 0)
        duty_count = duty_count + (timer.read_ms() - last_rise_time);
}

void test_pwm_execute(PinName pin, float tolerance, int waveform_count, float dutycycle, int period) {

    // Initialize
    float calculated_percent = waveform_count * tolerance;
    if (calculated_percent < 1) calculated_percent = 1.0f;
    fall_count = 0;
    rise_count = 0;
    last_rise_time = 0;
    duty_count = 0;

    PwmOut pwm(pin);
    timer.reset();
    InterruptIn iin(TestFramework::find_pin_pair(pin));
    iin.rise(callback_pwm_rise);
    iin.fall(callback_pwm_fall);

    // Set the period
	DEBUG_PRINTF("Duty cycle: %f, Period: %d\n", dutycycle, period);
    pwm.period((float)period/1000);
    pwm.write(0);
    DEBUG_PRINTF("Waiting for %dms\n", waveform_count * period);

    // Start the timer, write the duty cycle, and wait for completion
    timer.start();
    pwm.write(dutycycle);
    wait_ms(waveform_count * period);

    // Stop the timer, reset the IRQ for interrupts (cause it may not work on some platforms) and clear the PWM duty cycle
    pwm.write(0);
    iin.disable_irq(); // This is here because otherwise it fails on some platforms
    timer.stop();

    // Run post calculations for verification
    int rc = rise_count;
    int fc = fall_count;
    float avgTime = (float)duty_count / waveform_count;
    float expectedTime = (float)period * dutycycle;
    // DEBUG_PRINTF("Expected time: %f, Avg Time: %f\n", expectedTime, avgTime);
    // DEBUG_PRINTF("rise count = %d, fall count = %d, expected count = %d\n", rc, fc, waveform_count);
    TEST_ASSERT_MESSAGE( std::abs(rc-fc) <= calculated_percent, "There was more than a specific variance in number of rise vs fall cycles");
    TEST_ASSERT_MESSAGE( std::abs(waveform_count - rc) <= calculated_percent, "There was more than a specific variance in number of rise cycles seen and number expected.");
    TEST_ASSERT_MESSAGE( std::abs(waveform_count - fc) <= calculated_percent, "There was more than a specific variance in number of fall cycles seen and number expected.");
    // TEST_ASSERT_MESSAGE( std::abs(expectedTime - avgTime) <= calculated_percent,"Greater than a specific variance between expected and measured duty cycle");

}

// Test case that combines period and duty cycles
void test_pwm(PinName pin, float tolerance, int iterations) {
	DEBUG_PRINTF("Running pwm test on pin %d\n", pin);
    TEST_ASSERT_MESSAGE(pin != NC, "Pin is NC");

    // Get a random seed from the Greentea host test
    srand(TestFramework::get_seed());

    for (int i=0; i<iterations; i++) {
        int period = rand()%91 + 10;
        float dutycycle = (rand()%81 + 10)/100.0;
        test_pwm_execute(pin, tolerance, 20, dutycycle, period);
    }
}

template <int iterations>
utest::v1::control_t test_level2_pwm(const size_t call_count) {
	return TestFramework::test_level2_framework(TestFramework::PWM, TestFramework::CITS_PWM, &test_pwm, 0.05f, iterations);
}

Case cases[] = {
	Case("Level 3 - PWM randomized", test_level2_pwm<10>, TestFramework::greentea_failure_handler),
};

int main() {
	// Formulate a specification and run the tests based on the Case array
	Specification specification(TestFramework::test_setup<200>, cases);
    return !Harness::run(specification);
}
