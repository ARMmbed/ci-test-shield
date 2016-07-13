#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
#include "rtos.h"
#include "TMP102.h"
#include <I2CEeprom.h>

using namespace utest::v1;

// TODO: take both A0 and 0x90 and put into config file

const char *convBool(bool value) { return value ? "true" : "false"; }

#define I2C_SENTINAL 'Z'
#define TMP102_ADDR 0x90
#define EEPROM_ADDR 0xA0

// a test to see if the temperature can be read. A I2C failure returns a 0
void test_tmp102(){
    TMP102 temperature(I2C_SDA, I2C_SCL, TMP102_ADDR); //A0 pin is connected to ground
	TEST_ASSERT(0 != temperature.read());
}

// A test to write the EEprom
void test_eeprom_W(){
    I2CEeprom memory(I2C_SDA,I2C_SCL, EEPROM_ADDR, 32, 0);
	TEST_ASSERT(memory.write(1, I2C_SENTINAL) == 1); // check data was written
}


// A test to read the EEprom
void test_eeprom_R(){
    I2CEeprom memory(I2C_SDA,I2C_SCL, EEPROM_ADDR, 32, 0);
	char value = 'a';
    TEST_ASSERT(memory.read(1,value) == 1); // check data was read
    TEST_ASSERT(I2C_SENTINAL == value); // check data integrity
}

// A test to Write then read the eeprom
// void test_eeprom_WR(){

// }

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
    Case("Testing TMP102 Temperature Read",test_tmp102,greentea_failure_handler),
	Case("Testing EEProm Write",test_eeprom_W,greentea_failure_handler),
	Case("Testing EEProm Read",test_eeprom_R,greentea_failure_handler),
//	Case("Testing EEProm WR",test_eeprom_WR,greentea_failure_handler),
};

Specification specification(test_setup, cases);

// Entry point into the tests
int main() {
    return !Harness::run(specification);
}