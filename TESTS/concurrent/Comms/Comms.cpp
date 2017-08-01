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


// check if SPI is supported on this device
#if !DEVICE_SPI
    #error [NOT_SUPPORTED] SPI is not supported on this platform, add 'DEVICE_SPI' definition to your platform.
#endif

// check if I2C is supported on this device
#if !DEVICE_I2C
  #error [NOT_SUPPORTED] I2C not supported on this platform, add 'DEVICE_I2C' definition to your platform.
#endif

#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
#include "ci_test_config.h"
#include "FATFileSystem.h"
#include "SDBlockDevice.h"
#include <I2CEeprom.h>

using namespace utest::v1;

// TODO: make this string a randomly generated thing
#define TEST_STRING_MAX 100


// Fill array with random characters. Used in SPI testing.
char TEST_STRING[TEST_STRING_MAX] = {0};
void init_string()
{
    for(int x = 0; x < TEST_STRING_MAX-1; x++){
        TEST_STRING[x] = 'A' + (rand() % 26);
    }
    TEST_STRING[TEST_STRING_MAX-1] = 0;

    DEBUG_PRINTF("\r\n****\r\nTEST_STRING = %s\r\n****\r\n",TEST_STRING);
}


// test I2C API by writting and reading from EEPROM
void test_I2C()
{
    int size_of_data = 100;                            // number of bytes to write
    int address = 1;                                   // starting address to write to
    int num_read = 0;                                  // will report number of bytes read
    int num_written = 0;                               // will report number of bytes written
    volatile char read_string[size_of_data] = {0};     // string that will be updated from reading EEPROM

    I2CEeprom memory(MBED_CONF_APP_I2C_SDA,MBED_CONF_APP_I2C_SCL,MBED_CONF_APP_I2C_EEPROM_ADDR,32,0);
    num_written = memory.write(address,TEST_STRING,size_of_data);
    num_read = memory.read(address,(char *)read_string,size_of_data);

    TEST_ASSERT_EQUAL_STRING_MESSAGE(TEST_STRING,(char *)read_string,"String read does not match the string written");
    TEST_ASSERT_EQUAL_MESSAGE(num_written,num_read,"Number of bytes written does not match the number of bytes read");
    DEBUG_PRINTF("\r\n****\r\n I2C TEST:\r\n Address = `%d`\r\n Len = `%d`\r\n Num Bytes Written = `%d` \r\n Num Bytes Read = `%d` \r\n String written = `%s` \r\n String read = `%s` \r\n****\r\n",address,size_of_data,num_written,num_read,TEST_STRING,read_string);
}


// test SPI API by writing and reading from SD card
void test_SPI()
{
    // initialze sd hardware and filesystem
    SDBlockDevice sd(MBED_CONF_APP_SPI_MOSI, MBED_CONF_APP_SPI_MISO, MBED_CONF_APP_SPI_CLK, MBED_CONF_APP_SPI_CS);
    FATFileSystem fs("sd", &sd);
    sd.init();
    fs.mount(&sd);

    // write data
    FILE *File = fopen("/sd/test_sd_w.txt", "w");
    TEST_ASSERT_MESSAGE(File != NULL,"SD Card is not present. Please insert an SD Card.");
    TEST_ASSERT_MESSAGE(fprintf(File, TEST_STRING) > 0,"Writing file to sd card failed");             // write data
    fclose(File);                                                  // close file on SD

    // read data
    char read_string [TEST_STRING_MAX] = {0};
    File = fopen("/sd/test_sd_w.txt", "r");
    fgets(read_string,TEST_STRING_MAX,File);                    // read string from the file
    DEBUG_PRINTF("\r\n****\r\n SPI TEST:\r\n String written = `%s` \r\n String read = `%s` \r\n****\r\n",TEST_STRING,read_string);
    TEST_ASSERT_MESSAGE(strcmp(read_string,TEST_STRING) == 0,"String read does not match string written"); // test that strings match

    // close, unmount, and deinitialize
    fclose(File);                                                  // close file on SD
    fs.unmount();
    sd.deinit();
}


utest::v1::status_t test_setup(const size_t number_of_cases)
{
    // Setup Greentea using a reasonable timeout in seconds
    GREENTEA_SETUP(40, "default_auto");
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
    Case("SPI - SD Write and Read", test_SPI,greentea_failure_handler),
    Case("I2C - EEProm WR 100 Bytes",test_I2C,greentea_failure_handler),
};


Specification specification(test_setup, cases);

// // Entry point into the tests
int main() {
    init_string();
    return !Harness::run(specification);
}
