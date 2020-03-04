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

//#error [NOT_SUPPORTED] PWM tests are still being written and validated, not for public use yet. 

#if !DEVICE_PWMOUT
  #error [NOT_SUPPORTED] PWMOUT not supported on this platform, add 'DEVICE_PWMOUT' definition to your platform.
#endif

#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
#include <cmath>
#include "ci_test_config.h"

using namespace utest::v1;

volatile int rise_count;
void cbfn_rise(void){
  rise_count++;
}

// Template to test that a PWM signal has the correct length by measuring the number of rise interrupts
// interrupts during a specified number of tests. 
template <PinName pwm_out_pin, PinName int_in_pin, int period_in_miliseconds, int num_tests> 
void PWM_Period_Test(){
  // Initialize PWM, InterruptIn, Timer, and Rising / Falling edge counts
  rise_count = 0;
  PwmOut pwm(pwm_out_pin);
  InterruptIn iin(int_in_pin);
  iin.rise(cbfn_rise);
  pwm.period((float)period_in_miliseconds/1000);
  
  //Start Testing
  pwm.write(0.5f); // 50% duty cycle
  thread_sleep_for(num_tests * period_in_miliseconds); // wait for pwm to run and counts to add up
  iin.disable_irq(); // This is here because otherwise it fails on some platforms
  int rc = rise_count; // grab the numbers to work with as the pwm may continue going

  int expected_count = num_tests;

  float ten_percent  = (expected_count * 0.1f);
  float five_percent = (expected_count * 0.05f);
  float one_percent  = (expected_count * 0.01f);

  // fudge factor
  if(ten_percent < 1){
    ten_percent = 1;
  }
  if(five_percent < 1){
    five_percent = 1;
  }
  if(one_percent < 1){
    one_percent = 1;
  }

  DEBUG_PRINTF("\r\n*****\r\n rise count = %d, expected count = %d",rc,expected_count);
  DEBUG_PRINTF("\r\n .10 = %f, .05 = %f, .01 = %f",ten_percent,five_percent,one_percent);

  DEBUG_PRINTF("\r\n abs(expected-rc) = %d",std::abs(expected_count - rc));
  TEST_ASSERT_MESSAGE( std::abs(expected_count - rc) <= ten_percent, "There was more than 10 percent variance in number of rise cycles seen and number expected.");
  TEST_ASSERT_MESSAGE( std::abs(expected_count - rc) <= five_percent,"There was more than  5 percent variance in number of rise cycles seen and number expected.");
  //TEST_ASSERT_MESSAGE( std::abs(expected_count - rc) <= one_percent, "There was more than  1 percent variance in number of rise cycles seen and number expected.");
}

// test if software constructor / destructor works
void pwm_define_test(){
  PwmOut pwm0(MBED_CONF_APP_PWM_0);
  PwmOut pwm1(MBED_CONF_APP_PWM_1);
  PwmOut pwm2(MBED_CONF_APP_PWM_2);
  PwmOut pwm3(MBED_CONF_APP_PWM_3);

  pwm0.period(1.0f);
  pwm1.period(1.0f);
  pwm2.period(1.0f);
  pwm3.period(1.0f);

  pwm0.write(0.5f);
  pwm1.write(0.5f);
  pwm2.write(0.5f);
  pwm3.write(0.5f);

  TEST_ASSERT_MESSAGE(true,"The fact that it hasnt errored out proves this passes the sniff test");
}

utest::v1::status_t test_setup(const size_t number_of_cases){
  // Setup Greentea using a reasonable timeout in seconds
  GREENTEA_SETUP(200, "default_auto");
  return verbose_test_setup_handler(number_of_cases);
}

// Handle test failures, keep testing, dont stop
utest::v1::status_t greentea_failure_handler(const Case *const source, const failure_t reason){
  greentea_case_failure_abort_handler(source, reason);
  return STATUS_CONTINUE;
}

// Test cases
// TODO: take pin names from config file or generate from pinmap file
Case cases[] = {
  // test pwm object constructor works
  Case("Pwm object definable", pwm_define_test,greentea_failure_handler),   

  // Test Frequency length by counting rise ticks
  Case("PWM_0 Frequency 30ms",  PWM_Period_Test< MBED_CONF_APP_PWM_0, MBED_CONF_APP_DIO_2, 30,  100 >, greentea_failure_handler),  // Test at 30ms 100 times, default 50%duty cycle
  Case("PWM_1 Frequency 30ms",  PWM_Period_Test< MBED_CONF_APP_PWM_1, MBED_CONF_APP_DIO_4, 30,  100 >, greentea_failure_handler),  // Test at 30ms 100 times, default 50%duty cycle
  Case("PWM_2 Frequency 30ms",  PWM_Period_Test< MBED_CONF_APP_PWM_2, MBED_CONF_APP_DIO_7, 30,  100 >, greentea_failure_handler),  // Test at 30ms 100 times, default 50%duty cycle
  Case("PWM_3 Frequency 30ms",  PWM_Period_Test< MBED_CONF_APP_PWM_3, MBED_CONF_APP_DIO_8, 30,  100 >, greentea_failure_handler),  // Test at 30ms 100 times, default 50%duty cycle

};

Specification specification(test_setup, cases);

// Entry point into the tests
int main() 
{
    return !Harness::run(specification);
}
