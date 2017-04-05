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
#if !DEVICE_PWMOUT
  #error [NOT_SUPPORTED] PWMOUT not supported on this platform, add 'DEVICE_PWMOUT' definition to your platform.
#endif

#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
#include "ci_test_config.h"

using namespace utest::v1;

template <PinName pwm_pin> 
void PWM_Test()
{
    PwmOut pwm(pwm_pin);
    DEBUG_PRINTF("Tested pin %d for Arduino digital in/out capabilities\n", pwm_pin);
    TEST_ASSERT(true);
}

utest::v1::status_t test_setup(const size_t number_of_cases) {
    GREENTEA_SETUP(30, "default_auto");
    return verbose_test_setup_handler(number_of_cases);
}

utest::v1::status_t greentea_failure_handler(const Case *const source, const failure_t reason) {
    greentea_case_failure_abort_handler(source, reason);
    return STATUS_ABORT;
}

Case cases[] = {
    Case("Arduino - PWM Constructor on PWM_0", PWM_Test<MBED_CONF_APP_PWM_0>, greentea_failure_handler),
    Case("Arduino - PWM Constructor on PWM_1", PWM_Test<MBED_CONF_APP_PWM_1>, greentea_failure_handler),
    Case("Arduino - PWM Constructor on PWM_2", PWM_Test<MBED_CONF_APP_PWM_2>, greentea_failure_handler),
    Case("Arduino - PWM Constructor on PWM_3", PWM_Test<MBED_CONF_APP_PWM_3>, greentea_failure_handler),
};

int main() {
    // Formulate a specification and run the tests based on the Case array
    Specification specification(test_setup, cases);
    return !Harness::run(specification);
}
