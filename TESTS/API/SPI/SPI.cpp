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

// Waiting for public release to enable, currently there are problems with the SD driver

#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
#include "SDFileSystem.h" 
#include "ci_test_config.h"

// check if SPI is supported on this device
#if !DEVICE_SPI
    #error SPI is not supported on this platform, add 'DEVICE_SPI' deffinition to your platform.
#endif

using namespace utest::v1;

// TODO: make this string a randomly generated thing
//#define SD_TEST_STRING "hello world from the most awesome test shield!"
#define SD_TEST_STRING_MAX 100

char SD_TEST_STRING[SD_TEST_STRING_MAX] = {0};

void init_string()
{
    int x = 0;
    for(x = 0; x < SD_TEST_STRING_MAX-1; x++){
        SD_TEST_STRING[x] = 'A' + (rand() % 26);
    }
    SD_TEST_STRING[SD_TEST_STRING_MAX-1] = 0;

    DEBUG_PRINTF("\r\n****\r\nSD Test String = %s\r\n****\r\n",SD_TEST_STRING);
}

// Test object constructor / destructor
void test_object()
{
    SPI(MBED_CONF_APP_SPI_MOSI, MBED_CONF_APP_SPI_MISO, MBED_CONF_APP_SPI_CLK);
    DigitalOut cs(MBED_CONF_APP_SPI_CS);
    TEST_ASSERT_MESSAGE(true,"If the tests hangs here then there is a problem with the SPI Object"); // helpful debug message for if the test hangs
}

// Test for SD Card being present on the shield
void test_card_present()
{
    SDFileSystem sd(MBED_CONF_APP_SPI_MOSI, MBED_CONF_APP_SPI_MISO, MBED_CONF_APP_SPI_CLK, MBED_CONF_APP_SPI_CS,"sd");
    FILE *File = fopen("/sd/card-present.txt", "w");   // open file
    TEST_ASSERT_MESSAGE(File != NULL,"SD Card is not present. Please insert an SD Card.");
    // TODO: add greentea trigger to stop testing if SD Card not present
}

// Test SD Card Write
void test_sd_w()
{
    SDFileSystem sd(MBED_CONF_APP_SPI_MOSI, MBED_CONF_APP_SPI_MISO, MBED_CONF_APP_SPI_CLK, MBED_CONF_APP_SPI_CS,"sd");
    FILE *File = fopen("/sd/test_sd_w.txt", "w");   // open file
    TEST_ASSERT_MESSAGE(File != NULL,"SD Card is not present. Please insert an SD Card.");
    TEST_ASSERT_MESSAGE(fprintf(File, SD_TEST_STRING) > 0,"Writing file to sd card failed");             // write data
    // TODO: test that the file was written correctly
    fclose(File);                              // close file on SD
    TEST_ASSERT(true);
}

// Test SD Card Read
void test_sd_r()
{
    char read_string [SD_TEST_STRING_MAX] = {0};
    SDFileSystem sd(MBED_CONF_APP_SPI_MOSI, MBED_CONF_APP_SPI_MISO, MBED_CONF_APP_SPI_CLK, MBED_CONF_APP_SPI_CS,"sd");
    FILE *File = fopen("/sd/test_sd_w.txt", "r");   // open file
    TEST_ASSERT_MESSAGE(File != NULL,"SD Card is not present. Please insert an SD Card.");
    fgets(read_string,SD_TEST_STRING_MAX,File); // read string from the file
    //fread(read_string,sizeof(char),sizeof(SD_TEST_STRING),File); // read the string and compare
    DEBUG_PRINTF("\r\n****\r\nRead '%s' in read test\r\n, string comparison returns %d\r\n****\r\n",read_string,strcmp(read_string,SD_TEST_STRING));
    TEST_ASSERT_MESSAGE(strcmp(read_string,SD_TEST_STRING) == 0,"String read does not match string written"); // test that strings match
    fclose(File);    // close file on SD
    TEST_ASSERT(true);
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
     Case("SPI - Object Definable", test_object,greentea_failure_handler),
     Case("SPI - SD card exists",     test_card_present,greentea_failure_handler),
     Case("SPI - SD Write",     test_sd_w,greentea_failure_handler),
     Case("SPI - SD Read",     test_sd_r,greentea_failure_handler),
     Case("SPI - SD 2nd Write",     test_sd_w,greentea_failure_handler),
     Case("SPI - SD 2nd Read",     test_sd_r,greentea_failure_handler),
 };

 Specification specification(test_setup, cases);

// // Entry point into the tests
int main() {
    init_string();
    return !Harness::run(specification);
}
