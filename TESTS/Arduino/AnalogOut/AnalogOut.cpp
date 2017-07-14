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
#if !DEVICE_ANALOGOUT
  #error [NOT_SUPPORTED] AnalogOut not supported on this platform, add 'DEVICE_ANALOGOUT' definition to your platform.
#endif

#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
#include "ci_test_config.h"

using namespace utest::v1;

template <PinName aout_pin> 
void AnalogOutput_Test(){
  AnalogOut ain(aout_pin);
  DEBUG_PRINTF("Tested pin %d for Arduino analog out capabilities\n", aout_pin);
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
  Case("Arduino - Analog Output Constructor on AOUT_0", AnalogOutput_Test<MBED_CONF_APP_AOUT>, greentea_failure_handler),
};

int main() {
  // Formulate a specification and run the tests based on the Case array
  Specification specification(test_setup, cases);
  return !Harness::run(specification);
}
