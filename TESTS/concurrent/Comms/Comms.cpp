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


#if !DEVICE_SPI    // check if SPI is supported on this device
    #error [NOT_SUPPORTED] SPI is not supported on this platform, add 'DEVICE_SPI' definition to your platform.

#elif !DEVICE_I2C  // check if I2C is supported on this device
    #error [NOT_SUPPORTED] I2C not supported on this platform, add 'DEVICE_I2C' definition to your platform.

#endif             // !DEVICE_SPI or !DEVICE_I2C


#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
#include "ci_test_config.h"
#include "FATFileSystem.h"
#include "SDBlockDevice.h"
#include <I2CEeprom.h>

using namespace utest::v1;

#define TEST_STRING_MAX 128

Thread Thread_I2C;                        // thread used in multithread tests
Thread Thread_SPI;                        // thread used in multithread tests
osThreadId Multi_Thread_ID;               // thread id for main function controlling multithread tests
char Test_String[TEST_STRING_MAX] = {0};  // reference string used in testing

// Fill array with random characters. TODO: make this string a randomly generated thing
void init_string()
{
    for(int x = 0; x < TEST_STRING_MAX-1; x++){
        Test_String[x] = 'A' + (rand() % 26);
    }
    Test_String[TEST_STRING_MAX-1] = 0;

    DEBUG_PRINTF("\r\n****\r\n Test_String Size(Bytes) = %d\r\n Test_String = %s\r\n****\r\n",TEST_STRING_MAX,Test_String);
}


// test I2C API by writting and reading from EEPROM
void test_I2C()
{
    // initialize variables and API
    int address = 0;                                   // starting address to write to
    int num_read = 0;                                  // will report number of bytes read
    int num_written = 0;                               // will report number of bytes written
    volatile char read_string[TEST_STRING_MAX] = {0};  // string that will be updated from reading EEPROM
    I2CEeprom memory(MBED_CONF_APP_I2C_SDA,MBED_CONF_APP_I2C_SCL,MBED_CONF_APP_I2C_EEPROM_ADDR,32,0);

    // write to EEPROM
    num_written = memory.write(address,Test_String,TEST_STRING_MAX);

    // read back from EEPROM
    num_read = memory.read(address,(char *)read_string,TEST_STRING_MAX);

    // test for equality
    TEST_ASSERT_EQUAL_STRING_MESSAGE(Test_String,(char *)read_string,"String read does not match the string written"); // test that strings match
    TEST_ASSERT_EQUAL_MESSAGE(num_written,num_read,"Number of bytes written does not match the number of bytes read");
    DEBUG_PRINTF("\r\n****\r\n I2C TEST:\r\n Address = `%d`\r\n Num Bytes Written = `%d` \r\n Num Bytes Read = `%d` \r\n String written = `%s` \r\n String read = `%s` \r\n****\r\n",address,num_written,num_read,Test_String,read_string);
    osSignalSet(Multi_Thread_ID, 0x1);                                // signal completion of thread
}


// test SPI API by writing and reading from SD card
void test_SPI()
{
    // initialze SD hardware, filesystem, and variables
    SDBlockDevice sd(MBED_CONF_APP_SPI_MOSI, MBED_CONF_APP_SPI_MISO, MBED_CONF_APP_SPI_CLK, MBED_CONF_APP_SPI_CS);
    FATFileSystem fs("sd", &sd);
    sd.init();
    fs.mount(&sd);
    FILE *File = fopen("/sd/test_sd_w.txt", "w");
    TEST_ASSERT_MESSAGE(File != NULL,"SD Card is not present. Please insert an SD Card.");
    volatile char read_string[TEST_STRING_MAX] = {0};

    // write data to SD card
    int error = fprintf(File, Test_String);                           // write data
    TEST_ASSERT_MESSAGE(error > 0,"Writing file to sd card failed");  // error checking       
    fclose(File);                                                     // close file on SD

    // read data from SD card
    File = fopen("/sd/test_sd_w.txt", "r");
    fgets((char *)read_string,TEST_STRING_MAX,File);                  // read string from the file

    // test for equality
    TEST_ASSERT_EQUAL_STRING_MESSAGE(Test_String,(char *)read_string,"String read does not match the string written"); // test that strings match
    DEBUG_PRINTF("\r\n****\r\n SPI TEST:\r\n String written = `%s` \r\n String read = `%s` \r\n****\r\n",Test_String,read_string);

    // close, unmount, and deinitialize
    fclose(File);                                                     // close file on SD
    fs.unmount();
    sd.deinit();
    osSignalSet(Multi_Thread_ID, 0x2);                                // signal completion of thread
}


// test I2C and SPI APIs concurrently in multiple threads
void test_multiple_threads()
{
    Multi_Thread_ID = Thread::gettid();                               // update thread id for this thread
    Thread_I2C.start(callback(test_I2C));                             // kick off threads
    Thread_SPI.start(callback(test_SPI));                             // kick off threads
    wait(0.1);                                                        // allow time for debug print statements to complete.

    // Use this to wait for both signaling events to occur
    // Internaly the code is doing something like this:
    //   if ((signal1 | signal2) & 0x3 == 0x3) then exit, else wait
    osSignalWait(0x3, osWaitForever);
}


// test I2C and SPI APIs concurrently in a single thread
void test_single_thread()
{
    // *******************************
    // Initialize variables and APIs
    // *******************************
    
    // I2C test initializations
    int address = 0;                                      // starting address to write to in EEPROM
    int num_read = 0;                                     // will report number of bytes read
    int num_written = 0;                                  // will report number of bytes written
    volatile char read_string_i2c[TEST_STRING_MAX] = {0}; // string that will be updated from reading EEPROM
    I2CEeprom memory(MBED_CONF_APP_I2C_SDA,MBED_CONF_APP_I2C_SCL,MBED_CONF_APP_I2C_EEPROM_ADDR,32,0);

    // SPI test initializations
    volatile char read_string_spi[TEST_STRING_MAX] = {0}; // string that will be updated from reading SD card
    SDBlockDevice sd(MBED_CONF_APP_SPI_MOSI, MBED_CONF_APP_SPI_MISO, MBED_CONF_APP_SPI_CLK, MBED_CONF_APP_SPI_CS);
    FATFileSystem fs("sd", &sd);
    sd.init();
    fs.mount(&sd);
    FILE *File = fopen("/sd/test_sd_w.txt", "w");
    TEST_ASSERT_MESSAGE(File != NULL,"SD Card is not present. Please insert an SD Card.");
    
    // ******************************
    // Begin concurrent API testing
    // ******************************
    // write to EEPROM using I2C
    num_written = memory.write(address,Test_String,TEST_STRING_MAX);

    // write to SD card using SPI
    int error = fprintf(File, Test_String);                           // write data
    TEST_ASSERT_MESSAGE(error > 0,"Writing file to sd card failed");  // error checking       
    fclose(File);                                                     // close file on SD

    // read back from EEPROM
    num_read = memory.read(address,(char *)read_string_i2c,TEST_STRING_MAX);

    // read data from SD card
    File = fopen("/sd/test_sd_w.txt", "r");                   
    fgets((char *)read_string_spi,TEST_STRING_MAX,File);                  // read string from the file

    // ******************************
    // Check results
    // ******************************

    // test results for I2C
    TEST_ASSERT_EQUAL_STRING_MESSAGE(Test_String,(char *)read_string_i2c,"String read does not match the string written"); // test that strings match
    TEST_ASSERT_EQUAL_MESSAGE(num_written,num_read,"Number of bytes written does not match the number of bytes read");
    DEBUG_PRINTF("\r\n****\r\n I2C TEST:\r\n Address = `%d`\r\n Num Bytes Written = `%d` \r\n Num Bytes Read = `%d` \r\n String written = `%s` \r\n String read = `%s` \r\n****\r\n",address,num_written,num_read,Test_String,read_string_i2c);

    // test results for SPI
    TEST_ASSERT_EQUAL_STRING_MESSAGE(Test_String,(char *)read_string_spi,"String read does not match the string written"); // test that strings match
    DEBUG_PRINTF("\r\n****\r\n SPI TEST:\r\n String written = `%s` \r\n String read = `%s` \r\n****\r\n",Test_String,read_string_spi);
    fclose(File);            // close file on SD    
    fs.unmount();            // unmount
    sd.deinit();             // deinitialize SD
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
    Case("Concurrent testing of I2C and SPI in a single thread",test_single_thread,greentea_failure_handler),
    Case("Concurrent testing of I2C and SPI in multiple threads",test_multiple_threads,greentea_failure_handler),
};


Specification specification(test_setup, cases);

// // Entry point into the tests
int main() {
    init_string();
    return !Harness::run(specification);
}
