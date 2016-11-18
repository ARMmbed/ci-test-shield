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

// check if I2C is supported on this device
#if !DEVICE_I2C
  #error [NOT_SUPPORTED] I2C not supported on this platform, add 'DEVICE_I2C' deffinition to your platform.
#endif

#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
#include "LM75B.h"
#include <I2CEeprom.h>
#include "ci_test_config.h"

using namespace utest::v1;

// Fill array with random characters
void init_string(char* buffer, int len)
{
    int x = 0;
    for(x = 0; x < len; x++){
        buffer[x] = 'A' + (rand() % 26);
    }
    buffer[len-1] = 0; // add \0 to end of string
    DEBUG_PRINTF("\r\n****\r\nBuffer Len = `%d`, String = `%s`\r\n****\r\n",len,buffer);
}

// a test to see if the temperature can be read. A I2C failure returns a 0
template<PinName sda, PinName scl, int expected_temperature, int delta_in_temperature>
void test_lm75b()
{
    LM75B  temperature(sda, scl);
    float temp = temperature.temp();
    DEBUG_PRINTF("\r\n****\r\nTEST LM75B : Temperature Read = `%f`\r\n****\r\n",temp);
    TEST_ASSERT_MESSAGE(0 != temperature.open(),"Failed to open sensor");
    //TEST_ASSERT_MESSAGE(NULL != temperature.temp(),"Invalid value NULL returned");
    // TEST_ASSERT_MESSAGE(50 > temperature.temp(),"Its too Hot (>10C), Faulty Sensor?");
    // TEST_ASSERT_MESSAGE(0 < temperature.temp(),"Its Too Cold (<0C), Faulty Sensor?");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(delta_in_temperature, expected_temperature, temp,"Temperature is not within specified range");
}

// Template to write arbitrary data to arbitrary address and check the data is written correctly
template<PinName sda, PinName scl,int size_of_data, int address>
void flash_WR()
{
    I2CEeprom memory(sda,scl,MBED_CONF_APP_I2C_EEPROM_ADDR,32,0);
    int num_read = 0;
    int num_written = 0;
    volatile char test_string[size_of_data] = {0};
    volatile char read_string[size_of_data] = {0};
    init_string((char *)test_string,size_of_data); // populate test_string with random characters
    for(int x = 0; x< size_of_data;x++){
        read_string[x] = 0;
    }
    DEBUG_PRINTF("\r\n****\r\n Test String = `%s` \r\n****\r\n",test_string);

    num_written = memory.write(address,(char *)test_string,size_of_data);
    num_read = memory.read(address,(char *)read_string,size_of_data);

    TEST_ASSERT_MESSAGE(strcmp((char *)test_string,(char *)read_string) == 0,"String Written != String Read");
    TEST_ASSERT_MESSAGE(strcmp((char *)read_string,(char *)test_string) == 0,"String Written != String Read");
    TEST_ASSERT_EQUAL_STRING_MESSAGE((char *)test_string,(char *)read_string,"String read does not match the string written");
    TEST_ASSERT_EQUAL_STRING_MESSAGE((char *)read_string,(char *)test_string,"String read does not match the string written");
    TEST_ASSERT_EQUAL_MESSAGE(num_written,num_read,"Number of bytes written does not match the number of bytes read");
    DEBUG_PRINTF("\r\n****\r\n Address = `%d`\r\n Len = `%d`\r\n Num Bytes Written = `%d` \r\n Num Bytes Read = `%d` \r\n Written String = `%s` \r\n Read String = `%s` \r\n****\r\n",address,size_of_data,num_written,num_read,test_string,read_string);

}

// Test single byte R/W
template<PinName sda, PinName scl, int address>
void single_byte_WR()
{
    I2CEeprom memory(sda,scl,MBED_CONF_APP_I2C_EEPROM_ADDR,32,0);
    char test = 'A' + rand()%26;
    char read;
    int r = 0;
    int w = 0;
    w = memory.write(address,test);
    r = memory.read(address,read);
    DEBUG_PRINTF("\r\n****\r\n Num Bytes Read = %d \r\n Num Bytes Written = %d \r\n Read byte = `%c` \r\n Written Byte = `%c` \r\n****\r\n",r,w,read,test);

    TEST_ASSERT_EQUAL_MESSAGE(test,read,"Character Read does not equal character written!");
    TEST_ASSERT_MESSAGE(test == read, "character written does not match character read")
}

// Test initializing an I2C Object
template<PinName sda, PinName scl>
void test_object()
{
    I2C i2c(sda,scl);
    TEST_ASSERT_MESSAGE(true,"If you hang here your I2C Object has problems");
}

utest::v1::status_t test_setup(const size_t number_of_cases)
{
    // Setup Greentea using a reasonable timeout in seconds
    GREENTEA_SETUP(20, "default_auto");
    return verbose_test_setup_handler(number_of_cases);
}

// Handle test failures, keep testing, dont stop
utest::v1::status_t greentea_failure_handler(const Case *const source, const failure_t reason)
{
    greentea_case_failure_abort_handler(source, reason);
    return STATUS_CONTINUE;
}

// Test cases
Case cases[] = {
    Case("I2C -  Instantiation of I2C Object",test_object<MBED_CONF_APP_I2C_SDA,MBED_CONF_APP_I2C_SCL>,greentea_failure_handler),
    Case("I2C -  LM75B Temperature Read",test_lm75b<MBED_CONF_APP_I2C_SDA,MBED_CONF_APP_I2C_SCL,25,20>,greentea_failure_handler),
    Case("I2C -  EEProm WR Single Byte",single_byte_WR<MBED_CONF_APP_I2C_SDA,MBED_CONF_APP_I2C_SCL,1>,greentea_failure_handler),
    Case("I2C -  EEProm WR 2 Bytes",flash_WR<MBED_CONF_APP_I2C_SDA,MBED_CONF_APP_I2C_SCL,2,5>,greentea_failure_handler),
    Case("I2C -  EEProm WR 10  Bytes",flash_WR<MBED_CONF_APP_I2C_SDA,MBED_CONF_APP_I2C_SCL,10,100>,greentea_failure_handler),
    Case("I2C -  EEProm WR 100 Bytes",flash_WR<MBED_CONF_APP_I2C_SDA,MBED_CONF_APP_I2C_SCL,100,1000>,greentea_failure_handler),
};

Specification specification(test_setup, cases);

// Entry point into the tests
int main()
{
    return !Harness::run(specification);
}
