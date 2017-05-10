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

#if defined(DEBUG_MSG) && (DEBUG_MSG == 1)
#define TEST_TIMEOUT 30
#else
#define TEST_TIMEOUT 60
#endif

// test that all pins can be marked as BusIn
void busin_define_test(){
    BusIn bin(MBED_CONF_APP_DIO_2,MBED_CONF_APP_DIO_3,MBED_CONF_APP_DIO_4,MBED_CONF_APP_DIO_5,MBED_CONF_APP_DIO_6,MBED_CONF_APP_DIO_7,MBED_CONF_APP_DIO_8,MBED_CONF_APP_DIO_9,MBED_CONF_APP_SPI_CS,MBED_CONF_APP_SPI_MOSI,MBED_CONF_APP_AIN_0,MBED_CONF_APP_AIN_1,MBED_CONF_APP_AIN_2,MBED_CONF_APP_AIN_3,MBED_CONF_APP_AIN_4,MBED_CONF_APP_AIN_5);
    int x = 0;
    x = bin.read();
    TEST_ASSERT_MESSAGE(true,"The fact that it hasnt error out proves this passes the sniff test");
}

// test that all pins can be marked as BusOut
void busout_define_test(){
    BusOut bout(MBED_CONF_APP_DIO_2,MBED_CONF_APP_DIO_3,MBED_CONF_APP_DIO_4,MBED_CONF_APP_DIO_5,MBED_CONF_APP_DIO_6,MBED_CONF_APP_DIO_7,MBED_CONF_APP_DIO_8,MBED_CONF_APP_DIO_9,MBED_CONF_APP_SPI_CS,MBED_CONF_APP_SPI_MOSI,MBED_CONF_APP_AIN_0,MBED_CONF_APP_AIN_1,MBED_CONF_APP_AIN_2,MBED_CONF_APP_AIN_3,MBED_CONF_APP_AIN_4,MBED_CONF_APP_AIN_5);
    bout = 0;
    volatile int x = 0;
    while(x < 0xFF){
        DEBUG_PRINTF("\r\n*********\r\nvalue of x is: 0x%x\r\n********\r\n",x);
        x++;
        bout.write(x);
    }
    TEST_ASSERT_MESSAGE(true,"The fact that it hasnt error out proves this passes the sniff test");
}

// test that all pins can be marked as BusInOut
void businout_define_test(){
    BusInOut bio1(MBED_CONF_APP_DIO_2,MBED_CONF_APP_DIO_4,MBED_CONF_APP_DIO_6,MBED_CONF_APP_DIO_8);
    BusInOut bio2(MBED_CONF_APP_DIO_3,MBED_CONF_APP_DIO_5,MBED_CONF_APP_DIO_7,MBED_CONF_APP_DIO_9);
    bio1.output();
    bio2.input();
    bio1 = 0x00;
    volatile int x = 0x00;
    
    while(x < 0xF){
        x = (x<<1) +1;
        bio1.output();
        bio1 = x;
        wait(1);
        bio2.input();
        wait(1);
        volatile int y = bio2.read();
        DEBUG_PRINTF("\r\n*********\r\nvalue of x,bio is: 0x%x, 0x%x\r\n********\r\n",x,y);
        TEST_ASSERT_MESSAGE(y == x,"Value read on bus does not equal value written. ");
    }

    x = 0x00;
    while(x < 0xF){
        x = (x<<1) +1;
        bio2.output();
        bio2 = x;
        wait(1);
        bio1.input();
        wait(1);
        volatile int y = bio1.read();
        DEBUG_PRINTF("\r\n*********\r\nvalue of x,bio is: 0x%x, 0x%x\r\n********\r\n",x,y);
        TEST_ASSERT_MESSAGE(y == x,"Value read on bus does not equal value written. ");
    }

    TEST_ASSERT_MESSAGE(true,"The fact that it hasnt error out proves this passes the sniff test");
}

// Test writing from one bus to another
void businout_bidirectional_test(){
    BusIn bin(MBED_CONF_APP_DIO_2,MBED_CONF_APP_DIO_4,MBED_CONF_APP_DIO_6,MBED_CONF_APP_DIO_8);
    BusOut bout(MBED_CONF_APP_DIO_3,MBED_CONF_APP_DIO_5,MBED_CONF_APP_DIO_7,MBED_CONF_APP_DIO_9);
    bout = 0;
    volatile int x = 0;
    while(x < 0x0F){
        x++;
        bout.write(x);
        DEBUG_PRINTF("\r\n*********\r\nvalue of bin,bout,x is: 0x%x, 0x%x, 0x%x\r\n********\r\n",bin.read(),bout.read(),x);
        TEST_ASSERT_MESSAGE(bin.read() == bout.read(),"Value read on bin does not equal value written on bout. ")
    }
    TEST_ASSERT_MESSAGE(true,"The fact that it hasnt error out proves this passes the sniff test");
}

utest::v1::status_t test_setup(const size_t number_of_cases) {
    // Setup Greentea using a reasonable timeout in seconds
    GREENTEA_SETUP(TEST_TIMEOUT, "default_auto");
    return verbose_test_setup_handler(number_of_cases);
}

// Handle test failures, keep testing, don't stop
utest::v1::status_t greentea_failure_handler(const Case *const source, const failure_t reason) {
    greentea_case_failure_abort_handler(source, reason);
    return STATUS_CONTINUE;
}

// Test cases
// TODO: take pin names from config file or generate from pinmap file
Case cases[] = {
    Case("BusIn definable", busin_define_test,greentea_failure_handler),
    Case("BusOut definable", busout_define_test,greentea_failure_handler),
    Case("BusInOut to BusInOut", businout_define_test,greentea_failure_handler),
    Case("BusIn to BusOut test", businout_bidirectional_test,greentea_failure_handler),
};

Specification specification(test_setup, cases);

// Entry point into the tests
int main()
{
    return !Harness::run(specification);
}
