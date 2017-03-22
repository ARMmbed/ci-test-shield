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
#if !DEVICE_SPI
    #error SPI is not supported on this platform, add 'DEVICE_SPI' definition to your platform.
#endif

#include "cmsis.h"
#include "pinmap.h"
#include "PeripheralPins.h"

#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
#include "ci_test_config.h"

#include "TestFramework.hpp"
#include <vector>
#include "FATFileSystem.h"
#include "SDBlockDevice.h"

using namespace utest::v1;

// Static variables for managing the dynamic list of pins
std::vector< vector <PinMap> > TestFramework::pinout(TS_NC);
std::vector<int> TestFramework::pin_iterators(TS_NC);

// Initialize a test framework object
TestFramework test_framework;

utest::v1::status_t test_setup(const size_t number_of_cases) {
    GREENTEA_SETUP(30, "rand_provider");
    return verbose_test_setup_handler(number_of_cases);
}

utest::v1::status_t greentea_failure_handler(const Case *const source, const failure_t reason) {
    greentea_case_failure_abort_handler(source, reason);
    return STATUS_ABORT;
}

void test_spi_execute(PinName pin_clk, float null_float, int iterations) {

	PinName pin_mosi = MBED_CONF_APP_SPI_MOSI;
	PinName pin_miso = MBED_CONF_APP_SPI_MISO;
	PinName pin_cs = MBED_CONF_APP_SPI_CS;
	if (TestFramework::find_pin(pin_mosi, TestFramework::SPI_MOSI)==-1 ||
		TestFramework::find_pin(pin_miso, TestFramework::SPI_MISO)==-1 ||
		TestFramework::find_pin(pin_clk, TestFramework::SPI_CLK)==-1 ||
		TestFramework::find_pin(pin_cs, TestFramework::SPI_CS)==-1) {
		TEST_ASSERT(false);
		return;
	}

	DEBUG_PRINTF("Running SPI constructor on CLK pin %d, MISO pin %d, MOSI pin %d, and CS pin %d\n", pin_clk, pin_miso, pin_mosi, pin_cs);
    TEST_ASSERT_MESSAGE(pin_clk != NC, "SPI CLK pin is NC");
    TEST_ASSERT_MESSAGE(pin_mosi != NC, "SPI MOSI Pin is NC");
    TEST_ASSERT_MESSAGE(pin_miso != NC, "SPI MISO Pin is NC");
    TEST_ASSERT_MESSAGE(pin_cs != NC, "SPI CS Pin is NC");

    srand(TestFramework::get_seed());

    int error = 0; 
    SDBlockDevice sd(pin_mosi, pin_miso, pin_clk, pin_cs);
    FATFileSystem fs("sd");
    sd.init();
    error = fs.mount(&sd);
    TEST_ASSERT_MESSAGE(error==0,"SD file system mount failed.");

    for (int i=0; i<2; i++) {

	    char test_string[128] = {0};
	    for (int j=0; j<iterations; j++) 
	    	test_string[j] = 'A' + rand()%26;

	   	char file_name[] = "/sd/          .txt";
	   	for (int j=4; j<14; j++)
	   		file_name[j] = 'A' + rand()%26;

	    FILE *File_write = fopen(file_name, "w"); // open File_write
	    TEST_ASSERT_MESSAGE(File_write != NULL,"SD Card is not present. Please insert an SD Card.");
	    TEST_ASSERT_MESSAGE(fprintf(File_write, test_string) > 0,"Writing File to sd card failed"); // write data
	    fclose(File_write);// close file on SD

	    FILE *File_read = fopen(file_name, "r"); // open File_read
	    char test_string_read[128] = {0};
	    fgets(test_string_read, 128, File_read); // read string from the file
	    DEBUG_PRINTF("Read '%s' in read test\n",test_string_read);
	    DEBUG_PRINTF("File name: '%s'\n",file_name);
	    TEST_ASSERT_MESSAGE(strcmp(test_string_read,test_string) == 0,"String read does not match string written"); // test that strings match
	    fclose(File_read);// close file on SD
	    remove(file_name);
	}

    error = fs.unmount();
    TEST_ASSERT_MESSAGE(error==0,"SD file system unmount failed.");

    sd.deinit();
    TEST_ASSERT(true);
}

template <int iterations>
utest::v1::control_t test_level1_spi(const size_t call_count) {
	return TestFramework::test_level1_framework(TestFramework::SPI_CLK, TestFramework::CITS_SPI_CLK, &test_spi_execute, 0.05, iterations);
}

Case cases[] = {
	Case("Level 1 - SPI test - 1 byte (single pin set)", test_level1_spi<1>, greentea_failure_handler),
	Case("Level 1 - SPI test - 10 byte (single pin set)", test_level1_spi<10>, greentea_failure_handler),
	Case("Level 1 - SPI test - 100 byte (single pin set)", test_level1_spi<100>, greentea_failure_handler),
};

int main() {
	// Formulate a specification and run the tests based on the Case array
	Specification specification(test_setup, cases);
    return !Harness::run(specification);
}
