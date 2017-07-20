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
#include "ci_test_config.h"

using namespace utest::v1;

volatile bool Result = false;

// Callback for all InterruptInput functions
void cbfn(void){
  Result = true;
  DEBUG_PRINTF("\tInterrupt triggered!\n");
}


// Template to test DigitalIO, Analog, and Interrupt pins. Meant to be re-used multiple times
template <PinName digitalOut_pin, PinName digitalIn_pin, PinName analogIn_pin, PinName analogBus_pin1, PinName analogBus_pin2, PinName analogBus_pin3, PinName analogBus_pin4, PinName analogBus_pin5> 


void GPIO_Test(){
  // ***********************
  // Run DIO test
  // ***********************
  DEBUG_PRINTF("Starting DIO test with pins:\n\tdigitalIn_pin = %#x\n\tdigitalOut_pin = %#x\n", digitalIn_pin, digitalOut_pin);
  DigitalOut dout(digitalOut_pin);
  DigitalIn din(digitalIn_pin);
  // test 0
  dout = 0;
  TEST_ASSERT_MESSAGE(0 == din.read(),"Expected value to be 0, read value was not zero");
  // test 1
  dout = 1;
  TEST_ASSERT_MESSAGE(1 == din.read(),"Expected value to be 1, read value was not one");
  DEBUG_PRINTF("Finished DIO test\n\n");


  // ***********************
  // Run AnalogIn test
  // ***********************
  DEBUG_PRINTF("Starting AnalogIn test with pins:\n\tanalogIn_pin = %#x\n\tanalogBus_pin1 = %#x\n\tanalogBus_pin2 = %#x\n\tanalogBus_pin3 = %#x\n\tanalogBus_pin4 = %#x\n\tanalogBus_pin5 = %#x\n",analogIn_pin,analogBus_pin1,analogBus_pin2,analogBus_pin3,analogBus_pin4,analogBus_pin5 );
  AnalogIn ain(analogIn_pin);
  BusInOut outputs(analogBus_pin1,analogBus_pin2,analogBus_pin3,analogBus_pin4,analogBus_pin5);
  outputs.output();
  int x = 0;
  int y= 0;
  outputs = 0;
  float prev_value = 0;
  for(x = 0; x<5; x++) {
    DEBUG_PRINTF("X=%d\n",x);
    prev_value = ain.read();
    y = (y<<1) + 1;
    outputs = y;
    DEBUG_PRINTF("outputs=0x%x\nprevValue=%f\nain=%f\n\n",y,prev_value,ain.read());
    TEST_ASSERT_MESSAGE(ain.read() > prev_value,"Analog Input did not increment. Check that you have assigned valid pins in mbed_app.json file")
  }
  DEBUG_PRINTF("Finished AnalogIn test.\n\n");


  // ***********************
  // Run InterruptIn test
  // ***********************
  DEBUG_PRINTF("Starting InterruptIn test with pins:\n\tInterruptIn_pin = %#x\n\tdigitalOut_pin = %#x\n", digitalIn_pin, digitalOut_pin);
  InterruptIn intin(digitalIn_pin);

  // Test Rising Edge InterruptIn
  DEBUG_PRINTF("*****\nRising Edge Test\n");
  Result = false;
  dout = 0;
  intin.rise(cbfn);
  dout = 1;
  wait(0); // dummy wait to get volatile result value
  TEST_ASSERT_MESSAGE(Result,"cbfn was not triggered on rising edge of pin");

  // Test Falling Edge InterruptIn
  DEBUG_PRINTF("*****\nFalling Edge Test\n");
  Result = false;
  dout = 1;
  intin.fall(cbfn);
  dout = 0;
  wait(0); // dummy wait to get volatile result value
  TEST_ASSERT_MESSAGE(Result,"cbfn was not triggered on falling edge of pin");
  DEBUG_PRINTF("Finished InterruptIn test.\n\n");
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
Case cases[] = {
  Case("GPIO test on DIO_2, DIO_3, AIN_0", GPIO_Test<MBED_CONF_APP_DIO_2,MBED_CONF_APP_DIO_3, MBED_CONF_APP_AIN_0,MBED_CONF_APP_AIN_1,MBED_CONF_APP_AIN_2,MBED_CONF_APP_AIN_3,MBED_CONF_APP_AIN_4,MBED_CONF_APP_AIN_5>,greentea_failure_handler),

  Case("GPIO test on DIO_3, DIO_2, AIN_1", GPIO_Test<MBED_CONF_APP_DIO_3,MBED_CONF_APP_DIO_2, MBED_CONF_APP_AIN_1,MBED_CONF_APP_AIN_2,MBED_CONF_APP_AIN_3,MBED_CONF_APP_AIN_4,MBED_CONF_APP_AIN_5,MBED_CONF_APP_AIN_0>,greentea_failure_handler),

  Case("GPIO test on DIO_4, DIO_5, AIN_2", GPIO_Test<MBED_CONF_APP_DIO_4,MBED_CONF_APP_DIO_5, MBED_CONF_APP_AIN_2,MBED_CONF_APP_AIN_3,MBED_CONF_APP_AIN_4,MBED_CONF_APP_AIN_5,MBED_CONF_APP_AIN_0,MBED_CONF_APP_AIN_1>,greentea_failure_handler),

  Case("GPIO test on DIO_5, DIO_4, AIN_3", GPIO_Test<MBED_CONF_APP_DIO_5,MBED_CONF_APP_DIO_4, MBED_CONF_APP_AIN_3,MBED_CONF_APP_AIN_4,MBED_CONF_APP_AIN_5,MBED_CONF_APP_AIN_0,MBED_CONF_APP_AIN_1,MBED_CONF_APP_AIN_2>,greentea_failure_handler),

  Case("GPIO test on DIO_6, DIO_7, AIN_4", GPIO_Test<MBED_CONF_APP_DIO_6,MBED_CONF_APP_DIO_7, MBED_CONF_APP_AIN_4,MBED_CONF_APP_AIN_5,MBED_CONF_APP_AIN_0,MBED_CONF_APP_AIN_1,MBED_CONF_APP_AIN_2,MBED_CONF_APP_AIN_3>,greentea_failure_handler),

  Case("GPIO test on DIO_7, DIO_6, AIN_5", GPIO_Test<MBED_CONF_APP_DIO_7,MBED_CONF_APP_DIO_6, MBED_CONF_APP_AIN_5,MBED_CONF_APP_AIN_0,MBED_CONF_APP_AIN_1,MBED_CONF_APP_AIN_2,MBED_CONF_APP_AIN_3,MBED_CONF_APP_AIN_4>,greentea_failure_handler),

  Case("GPIO test on DIO_8, DIO_9, AIN_5", GPIO_Test<MBED_CONF_APP_DIO_8,MBED_CONF_APP_DIO_9, MBED_CONF_APP_AIN_5,MBED_CONF_APP_AIN_0,MBED_CONF_APP_AIN_1,MBED_CONF_APP_AIN_2,MBED_CONF_APP_AIN_3,MBED_CONF_APP_AIN_4>,greentea_failure_handler),

  Case("GPIO test on DIO_9, DIO_8, AIN_5", GPIO_Test<MBED_CONF_APP_DIO_9,MBED_CONF_APP_DIO_8, MBED_CONF_APP_AIN_5,MBED_CONF_APP_AIN_0,MBED_CONF_APP_AIN_1,MBED_CONF_APP_AIN_2,MBED_CONF_APP_AIN_3,MBED_CONF_APP_AIN_4>,greentea_failure_handler),

};


Specification specification(test_setup, cases);


// Entry point into the tests
int main(){
  return !Harness::run(specification);
}
