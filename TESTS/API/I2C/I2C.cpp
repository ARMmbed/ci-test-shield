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

// Fill array with random characters
void init_string(char* buffer, int len){
    int x = 0;
    for(x = 0; x < len; x++){
        buffer[x] = 'A' + (rand() % 26);
    }
    buffer[len-1] = 0; // add \0 to end of string
    printf("\r\n****\r\nBuffer Len = `%d`, String = `%s`\r\n****\r\n",len,buffer);
}

// a test to see if the temperature can be read. A I2C failure returns a 0
template<PinName sda, PinName scl, int expected_temperature, int delta_in_temperature>
void test_lm75b(){
    LM75B  temperature(sda, scl);
    float temp = temperature.temp();
    printf("\r\n****\r\nTEST LM75B : Temperature Read = `%f`\r\n****\r\n",temp);
    TEST_ASSERT_MESSAGE(0 != temperature.open(),"Failed to open sensor");
    //TEST_ASSERT_MESSAGE(NULL != temperature.temp(),"Invalid value NULL returned");
    // TEST_ASSERT_MESSAGE(50 > temperature.temp(),"Its too Hot (>10C), Faulty Sensor?");
    // TEST_ASSERT_MESSAGE(0 < temperature.temp(),"Its Too Cold (<0C), Faulty Sensor?");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(delta_in_temperature, expected_temperature, temp,"Temperature is not within specified range");
}

// Template to write arbitrary data to arbitrary address and check the data is written correctly
template<PinName sda, PinName scl,int size_of_data, int address>
void flash_WR(){
    I2CEeprom memory(sda,scl,address,32,0);
    int num_read = 0;
    int num_written = 0;
    char test_string[size_of_data] = {0};
    char read_string[size_of_data] = {0};
    init_string(test_string,size_of_data); // populate test_string with random characters
    //printf("\r\n****\r\n Test String = `%s` \r\n****\r\n",test_string);

    num_written = memory.write(address,test_string,size_of_data);
    num_read = memory.read(address,read_string,size_of_data);

    printf("\r\n****\r\n Address = `%d`\r\n Len = `%d`\r\n Num Bytes Written = `%d` \r\n Num Bytes Read = `%d` \r\n Written String = `%s` \r\n Read String = `%s` \r\n****\r\n",address,size_of_data,num_written,num_read,test_string,read_string);

    TEST_ASSERT_MESSAGE(strcmp(test_string,read_string) == 0,"String Written != String Read");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(test_string,read_string,"String read does not match the string written");
    TEST_ASSERT_EQUAL_MESSAGE(num_written,num_read,"Number of bytes written does not match the number of bytes read");

}

//// A test to write the EEprom
//void test_eeprom_W(){
//    I2CEeprom memory(sda,scl, address, 32, 0);
//    int num_bytes = 0;
//    num_bytes = memory.write(addr,I2C_TEST_STRING, I2C_TEST_STRING_MAX);
//    printf("\r\n****\r\n WRITE: Num bytes written = `%d`\r\n****\r\n",num_bytes);
//	TEST_ASSERT_EQUAL_MESSAGE(num_bytes, I2C_TEST_STRING_MAX,"All data not written."); // check data was written
//}
//
//
//// A test to read the EEprom
//void test_eeprom_R(){
//    I2CEeprom memory(MBED_CONF_APP_I2C_SDA,MBED_CONF_APP_I2C_SCL, MBED_CONF_APP_I2C_EEPROM_ADDR, 32, 0);
//	char read_string[I2C_TEST_STRING_MAX] = {0};
//    int read_bytes = 0;
//    read_bytes = memory.read(addr,read_string,I2C_TEST_STRING_MAX);
//
//    printf("\r\n****\r\nREAD: Num bytes read = `%d`, string read = `%s`\r\n****\r\n",read_bytes,read_string);
//    TEST_ASSERT_EQUAL_MESSAGE(read_bytes, I2C_TEST_STRING_MAX,"Failed to read the correct number of bytes.");
//    TEST_ASSERT_EQUAL_STRING_MESSAGE(I2C_TEST_STRING,read_string,"data read does not match data written! bad EEPROM?"); // check data integrity
//    TEST_ASSERT_MESSAGE(strcmp(I2C_TEST_STRING,read_string) == 0,"String written does not match string read"); // double check strings with strcmp
//}
//
//// A test to Write then read the eeprom
// void test_eeprom_WR(){
//    int address = rand()%250000;
//    I2CEeprom memory(MBED_CONF_APP_I2C_SDA,MBED_CONF_APP_I2C_SCL, MBED_CONF_APP_I2C_EEPROM_ADDR, 32, 0);
//    int read_bytes = 0;
//    int write_bytes = 0;
//    char read_string[I2C_TEST_STRING_MAX] = {0};
//
//    write_bytes = memory.write(address,I2C_TEST_STRING,I2C_TEST_STRING_MAX);
//    read_bytes  = memory.read(address,read_string,I2C_TEST_STRING_MAX);
//
//    printf("\r\n*****\r\nWRITE READ: Bytes Written = `%d`\r\nBytes Read = `%d` \r\nString Written = `%s`\r\nString Read = `%s`\r\n*****\r\n",write_bytes,read_bytes,I2C_TEST_STRING,read_string);
//
//    TEST_ASSERT_MESSAGE(write_bytes == read_bytes,"Number of bytes written does not match number of bytes read");
//    TEST_ASSERT_EQUAL_STRING_MESSAGE(I2C_TEST_STRING,read_string,"String read does not match string written");
//    TEST_ASSERT_MESSAGE(strcmp(I2C_TEST_STRING,read_string) == 0,"String written does not match string read"); // double check strings with strcmp
//
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
    Case("I2C -  LM75B Temperature Read",test_lm75b<MBED_CONF_APP_I2C_SDA,MBED_CONF_APP_I2C_SCL,25,20>,greentea_failure_handler),
	//Case("I2C -  EEProm Write",test_eeprom_W,greentea_failure_handler),
	//Case("I2C -  EEProm Read",test_eeprom_R,greentea_failure_handler),
	//Case("I2C -  EEProm WR",test_eeprom_WR,greentea_failure_handler),
    //Case("I2C -  EEProm WR 2Bytes",flash_WR<MBED_CONF_APP_I2C_SDA,MBED_CONF_APP_I2C_SCL,2,1>,greentea_failure_handler),
    Case("I2C -  EEProm WR 10Bytes",flash_WR<MBED_CONF_APP_I2C_SDA,MBED_CONF_APP_I2C_SCL,10,1>,greentea_failure_handler),
    Case("I2C -  EEProm WR 100 Bytes",flash_WR<MBED_CONF_APP_I2C_SDA,MBED_CONF_APP_I2C_SCL,100,1>,greentea_failure_handler),
};

Specification specification(test_setup, cases);

// Entry point into the tests
int main() {
    return !Harness::run(specification);
}