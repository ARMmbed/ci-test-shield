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

using namespace utest::v1;


#define I2C_TEST_STRING_MAX 100
int addr = 100;
char I2C_TEST_STRING[I2C_TEST_STRING_MAX] = {0};
void init_string(){
    int x = 0;
    for(x = 0; x < I2C_TEST_STRING_MAX-1; x++){
        I2C_TEST_STRING[x] = 'A' + (rand() % 26);
    }
    I2C_TEST_STRING[I2C_TEST_STRING_MAX] = 0; // add \0 to end of string
    addr = rand()% 100; // randomize the start address

    printf("\r\n****\r\nI2C Test addr = %d, String = %s\r\n****\r\n",addr,I2C_TEST_STRING);
}

// a test to see if the temperature can be read. A I2C failure returns a 0
void test_lm75b(){
    LM75B  temperature(MBED_CONF_APP_I2C_SDA, MBED_CONF_APP_I2C_SCL);
    int temp = temperature.temp();
    printf("\r\n****\r\nTemperature Read = `%f`\r\n****\r\n",temp);
    TEST_ASSERT_MESSAGE(0 != temperature.open(),"Failed to open sensor");
    //TEST_ASSERT_MESSAGE(NULL != temperature.temp(),"Invalid value NULL returned");
    TEST_ASSERT_MESSAGE(50 > temperature.temp(),"Its too Hot (>10C), Faulty Sensor?");
    TEST_ASSERT_MESSAGE(0 < temperature.temp(),"Its Too Cold (<0C), Faulty Sensor?");
}

// A test to write the EEprom
void test_eeprom_W(){
    I2CEeprom memory(MBED_CONF_APP_I2C_SDA,MBED_CONF_APP_I2C_SCL, MBED_CONF_APP_I2C_EEPROM_ADDR, 32, 0);
    int num_bytes = 0;
    num_bytes = memory.write(addr,I2C_TEST_STRING, I2C_TEST_STRING_MAX);
    printf("\r\n****\r\nNum bytes written = `%d`\r\n****\r\n",num_bytes);
	TEST_ASSERT_EQUAL_MESSAGE(num_bytes, I2C_TEST_STRING_MAX,"data not written, error with EEPROM?"); // check data was written
}


// A test to read the EEprom
void test_eeprom_R(){
    I2CEeprom memory(MBED_CONF_APP_I2C_SDA,MBED_CONF_APP_I2C_SCL, MBED_CONF_APP_I2C_EEPROM_ADDR, 32, 0);
	volatile char read_string[I2C_TEST_STRING_MAX] = {0};
    int read_bytes = 0;
    read_bytes = memory.read(addr,(char*)read_string,I2C_TEST_STRING_MAX);

    printf("\r\n****\r\nNum bytes read = `%d`, string read = `%s`\r\n****\r\n",read_bytes,read_string);
    TEST_ASSERT_EQUAL_MESSAGE(read_bytes, I2C_TEST_STRING_MAX,"Failed to read the correct number of bytes.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(I2C_TEST_STRING,read_string,"data read does not match data written! bad EEPROM?"); // check data integrity
}

// A test to Write then read the eeprom
 void test_eeprom_WR(){
    int address = rand()%250000;
    I2CEeprom memory(MBED_CONF_APP_I2C_SDA,MBED_CONF_APP_I2C_SCL, MBED_CONF_APP_I2C_EEPROM_ADDR, 32, 0);
    int read_bytes = 0;
    int write_bytes = 0;
    volatile char read_string[I2C_TEST_STRING_MAX] = {0};

    write_bytes = memory.write(address,I2C_TEST_STRING,I2C_TEST_STRING_MAX);
    read_bytes  = memory.read(address,(char *)read_string,I2C_TEST_STRING_MAX);

    printf("\r\n*****\r\nBytes Written = `%d`\r\nBytes Read = `%d` \r\nString Written = `%s`\r\nString Read = `%s`\r\n*****\r\n",write_bytes,read_bytes,I2C_TEST_STRING,read_string);

    TEST_ASSERT_MESSAGE(write_bytes == read_bytes,"Number of bytes written does not match number of bytes read");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(I2C_TEST_STRING,read_string,"String read does not match string written");

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
    Case("I2C -  LM75B Temperature Read",test_lm75b,greentea_failure_handler),
	Case("I2C -  EEProm Write",test_eeprom_W,greentea_failure_handler),
	Case("I2C -  EEProm Read",test_eeprom_R,greentea_failure_handler),
	Case("I2C -  EEProm WR",test_eeprom_WR,greentea_failure_handler),
};

Specification specification(test_setup, cases);

// Entry point into the tests
int main() {
    init_string();
    return !Harness::run(specification);
}