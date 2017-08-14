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

// Initialize a test framework object
TestFramework test_framework;

// test that all pins can be marked as BusIn
void busin_define_test()
{
    BusIn bin(MBED_CONF_APP_DIO_2,MBED_CONF_APP_DIO_3,MBED_CONF_APP_DIO_4,MBED_CONF_APP_DIO_5,MBED_CONF_APP_DIO_6,MBED_CONF_APP_DIO_7,MBED_CONF_APP_DIO_8,MBED_CONF_APP_DIO_9,MBED_CONF_APP_SPI_CS,MBED_CONF_APP_SPI_MOSI,MBED_CONF_APP_AIN_0,MBED_CONF_APP_AIN_1,MBED_CONF_APP_AIN_2,MBED_CONF_APP_AIN_3,MBED_CONF_APP_AIN_4,MBED_CONF_APP_AIN_5);
    int x = 0;
    x = bin.read();
    TEST_ASSERT_MESSAGE(true,"The fact that it hasnt error out proves this passes the sniff test");
}


// test that all pins can be marked as BusOut
void busout_define_test()
{
    BusOut bout(MBED_CONF_APP_DIO_2,MBED_CONF_APP_DIO_3,MBED_CONF_APP_DIO_4,MBED_CONF_APP_DIO_5,MBED_CONF_APP_DIO_6,MBED_CONF_APP_DIO_7,MBED_CONF_APP_DIO_8,MBED_CONF_APP_DIO_9,MBED_CONF_APP_SPI_CS,MBED_CONF_APP_SPI_MOSI,MBED_CONF_APP_AIN_0,MBED_CONF_APP_AIN_1,MBED_CONF_APP_AIN_2,MBED_CONF_APP_AIN_3,MBED_CONF_APP_AIN_4,MBED_CONF_APP_AIN_5);
    bout = 0;
    volatile int x = 0;
    while(x < 0xFF){
        DEBUG_PRINTF("value of x is: 0x%x\r\n",x);
        x++;
        bout.write(x);
    }
    TEST_ASSERT_MESSAGE(true,"The fact that it hasnt error out proves this passes the sniff test");
}


// test that all pins can be marked as BusInOut
void businout_define_test()
{
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
        bio2.input();
        volatile int y = bio2.read();
        DEBUG_PRINTF("value of x,bio is: 0x%x, 0x%x\r\n",x,y);
        TEST_ASSERT_MESSAGE(y == x,"Value read on bus does not equal value written. ");
    }

    x = 0x00;
    while(x < 0xF){
        x = (x<<1) +1;
        bio2.output();
        bio2 = x;
        bio1.input();
        volatile int y = bio1.read();
        DEBUG_PRINTF("value of x,bio is: 0x%x, 0x%x\r\n",x,y);
        TEST_ASSERT_MESSAGE(y == x,"Value read on bus does not equal value written. ");
    }

    TEST_ASSERT_MESSAGE(true,"The fact that it hasnt error out proves this passes the sniff test");
}

// Test writing from one bus to another
void businout_bidirectional_test()
{
    BusIn bin(MBED_CONF_APP_DIO_2,MBED_CONF_APP_DIO_4,MBED_CONF_APP_DIO_6,MBED_CONF_APP_DIO_8);
    BusOut bout(MBED_CONF_APP_DIO_3,MBED_CONF_APP_DIO_5,MBED_CONF_APP_DIO_7,MBED_CONF_APP_DIO_9);
    bout = 0;
    volatile int x = 0;
    while(x < 0x0F) {
        x++;
        bout.write(x);
        DEBUG_PRINTF("value of bin,bout,x is: 0x%x, 0x%x, 0x%x\r\n",bin.read(),bout.read(),x);
        TEST_ASSERT_MESSAGE(bin.read() == bout.read(),"Value read on bin does not equal value written on bout. ")
    }
    TEST_ASSERT_MESSAGE(true,"The fact that it hasnt error out proves this passes the sniff test");
}


Case cases[] = {
    Case("Level 2 - Bus Input definable", busin_define_test,  TestFramework::greentea_failure_handler),
    Case("Level 2 - Bus Output definable", busout_define_test, TestFramework::greentea_failure_handler),
    Case("Level 2 - Bus Input/Output range test", businout_define_test, TestFramework::greentea_failure_handler),
    Case("Level 2 - Bus Input/Output range test 2", businout_bidirectional_test, TestFramework::greentea_failure_handler),
};


// Entry point into the tests
int main() 
{
	// Formulate a specification and run the tests based on the Case array
	Specification specification(TestFramework::test_setup<30>, cases);
    return !Harness::run(specification);
}
