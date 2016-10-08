// Waiting for public release to enable, currently there are problems with the SD driver
//#error [NOT_SUPPORTED] Waiting on public release of SD FileSystem API

#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
#include "SDFileSystem.h" 

// check if SPI is supported on this device
#if !DEVICE_SPI
  #error SPI is not supported on this platform, add 'DEVICE_SPI' deffinition to your platform.
#endif

using namespace utest::v1;

#define SD_TEST_STRING "hello world from the most awesome test shield!"

// Test object contructor / destructor
void test_object(){
	SPI(MBED_CONF_APP_SPI_MOSI, MBED_CONF_APP_SPI_MISO, MBED_CONF_APP_SPI_CLK, MBED_CONF_APP_SPI_CS);
	TEST_ASSERT_MESSAGE(true,"If the tests hangs here then there is a problem with the SPI Object"); // helpful debug message for if the test hangs
}

// Test for SD Card being present on the shield
void test_card_present(){
	SDFileSystem sd(MBED_CONF_APP_SPI_MOSI, MBED_CONF_APP_SPI_MISO, MBED_CONF_APP_SPI_CLK, MBED_CONF_APP_SPI_CS,"sd");
	FILE *File = fopen("/sd/card-present.txt", "w");   // open file
    TEST_ASSERT_MESSAGE(File != NULL,"SD Card is not present. Please insert an SD Card.");
}

// Test SD Card Write
void test_sd_w(){
	SDFileSystem sd(MBED_CONF_APP_SPI_MOSI, MBED_CONF_APP_SPI_MISO, MBED_CONF_APP_SPI_CLK, MBED_CONF_APP_SPI_CS,"sd");
	FILE *File = fopen("/sd/test_sd_w.txt", "w");   // open file
    TEST_ASSERT_MESSAGE(File != NULL,"SD Card is not present. Please insert an SD Card.");
    fprintf(File, SD_TEST_STRING);             // write data
    // TODO: test that the file was written correctly
    fclose(File);                              // close file on SD
    TEST_ASSERT(true);
}

// Test SD Card Read
void test_sd_r(){
	SDFileSystem sd(MBED_CONF_APP_SPI_MOSI, MBED_CONF_APP_SPI_MISO, MBED_CONF_APP_SPI_CLK, MBED_CONF_APP_SPI_CS,"sd");
	FILE *File = fopen("/sd/test_sd_w.txt", "r");   // open file
	TEST_ASSERT_MESSAGE(File != NULL,"SD Card is not present. Please insert an SD Card.");
    
    // TODO: read the file and grab the string
    //fprintf(File, "CI Test Shield!");            // write data
    fclose(File);                                // close file on SD

    // TODO: strcmp on read string against SD_TEST_STRING
}

// Test SD Card Read / Write in single test for completeness
void test_sd_rw(){
	// TODO: Impliment write / read to a different file.
}

utest::v1::status_t test_setup(const size_t number_of_cases) {
    // Setup Greentea using a reasonable timeout in seconds
    GREENTEA_SETUP(40, "default_auto");
    return verbose_test_setup_handler(number_of_cases);
}

// Handle test failures, keep testing, dont stop
utest::v1::status_t greentea_failure_handler(const Case *const source, const failure_t reason) {
    greentea_case_failure_abort_handler(source, reason);
    return STATUS_CONTINUE;
}

// Test cases
 Case cases[] = {
     Case("SPI - Object Definable", test_object,greentea_failure_handler),
     Case("SPI - SD card exists", 	test_card_present,greentea_failure_handler),
     Case("SPI - SD Write", 		test_sd_w,greentea_failure_handler),
     // Case("SPI - SD Read", 			test_sd_r,greentea_failure_handler),
     // Case("SPI - SD Read/Write", 	test_sd_rw,greentea_failure_handler),
 };

 Specification specification(test_setup, cases);

// // Entry point into the tests
int main() {
    return !Harness::run(specification);
}