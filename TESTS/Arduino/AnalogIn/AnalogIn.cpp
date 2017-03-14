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

// Template to set one Analog pin as input and then cycle through the rest as outputs.
// As you turn more pins on the voltage on the ain pin will go up.
template <PinName ain_pin> 
void AnalogInput_Test()
{
    AnalogIn ain(ain_pin);
    DEBUG_PRINTF("Tested pin %d for Arduino analog in capabilities\n", ain_pin);
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
	Case("Arduino - Analog Input Constructor on AIN_0", AnalogInput_Test<MBED_CONF_APP_AIN_0>, greentea_failure_handler),
	Case("Arduino - Analog Input Constructor on AIN_1", AnalogInput_Test<MBED_CONF_APP_AIN_1>, greentea_failure_handler),
	Case("Arduino - Analog Input Constructor on AIN_2", AnalogInput_Test<MBED_CONF_APP_AIN_2>, greentea_failure_handler),
	Case("Arduino - Analog Input Constructor on AIN_3", AnalogInput_Test<MBED_CONF_APP_AIN_3>, greentea_failure_handler),
	Case("Arduino - Analog Input Constructor on AIN_4", AnalogInput_Test<MBED_CONF_APP_AIN_4>, greentea_failure_handler),
	Case("Arduino - Analog Input Constructor on AIN_5", AnalogInput_Test<MBED_CONF_APP_AIN_5>, greentea_failure_handler),
};

int main() {
	// Formulate a specification and run the tests based on the Case array
	Specification specification(test_setup, cases);
    return !Harness::run(specification);
}
