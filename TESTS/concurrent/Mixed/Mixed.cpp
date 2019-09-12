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

#elif !DEVICE_ANALOGIN
  #error [NOT_SUPPORTED] AnalogIn not supported on this platform, add 'DEVICE_ANALOGIN' definition to your platform.

#endif             // !DEVICE_SPI or !DEVICE_I2C or !DEVICE_ANALOGIN


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

Thread Thread_I2C(osPriorityNormal, OS_STACK_SIZE/2);  // thread used in multithread tests
Thread Thread_SPI(osPriorityNormal, OS_STACK_SIZE/2);  // thread used in multithread tests
Thread Thread_GPIO(osPriorityNormal, OS_STACK_SIZE/2); // thread used in multithread tests
osThreadId Multi_Thread_ID;               // thread id for main function controlling multithread tests
char Test_String[TEST_STRING_MAX] = {0};  // reference string used in testing
volatile bool GPIO_Result = false;        // flag used in GPIO testing

// Fill array with random characters. TODO: make this string a randomly generated thing
void init_string()
{
    for(int x = 0; x < TEST_STRING_MAX-1; x++){
        Test_String[x] = 'A' + (rand() % 26);
    }
    Test_String[TEST_STRING_MAX-1] = 0;
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
    DEBUG_PRINTF("\r\n****\r\n I2C thread test:\r\n Address = `%d`\r\n Num Bytes Written = `%d` \r\n Num Bytes Read = `%d` \r\n String written = `%s` \r\n String read = `%s` \r\n****\r\n",address,num_written,num_read,Test_String,read_string);
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
    DEBUG_PRINTF("\r\n****\r\n SPI thread test:\r\n String written = `%s` \r\n String read = `%s` \r\n****\r\n",Test_String,read_string);

    // close, unmount, and deinitialize
    fclose(File);                                                     // close file on SD
    fs.unmount();
    sd.deinit();
    osSignalSet(Multi_Thread_ID, 0x2);                                // signal completion of thread
}


// Callback for all InterruptInput functions
void cbfn(void)
{
    GPIO_Result = true;
}


void test_GPIO()
{
    // ***********************
    // Initialize GPIO pins
    // ***********************
    DEBUG_PRINTF("\r\n****\r\n GPIO thread test\r\n****\r\n");
    // Pins for DIO test
    DigitalOut d_out(MBED_CONF_APP_DIO_2);
    DigitalIn d_in(MBED_CONF_APP_DIO_3);

    // Pins for AnalogIn test
    AnalogIn a_in(MBED_CONF_APP_AIN_0);
    BusInOut aBus_outputs(MBED_CONF_APP_AIN_1,MBED_CONF_APP_AIN_2,MBED_CONF_APP_AIN_3,MBED_CONF_APP_AIN_4,MBED_CONF_APP_AIN_5);

    // Pins for InterruptIn test
    InterruptIn int_in(MBED_CONF_APP_DIO_4);
    DigitalOut int_out(MBED_CONF_APP_DIO_5);

    // ***********************
    // Begin concurrent GPIO testing
    // ***********************
    aBus_outputs.output();
    int analogOut_value= 0;
    aBus_outputs = 0;
    float prev_analog_value = 0;
    for(int iter = 0; iter<5; iter++) { 
        // DigitalIO test
        d_out = 0;  // test 0
        wait_ns(100);
        TEST_ASSERT_MESSAGE(0 == d_in.read(),"Expected value to be 0, read value was not zero");
        d_out = 1;  // test 1
        wait_ns(100);
        TEST_ASSERT_MESSAGE(1 == d_in.read(),"Expected value to be 1, read value was not one");

        // AnalogIn test
        prev_analog_value = a_in.read();
        analogOut_value = (analogOut_value << 1) + 1;
        aBus_outputs = analogOut_value;
        TEST_ASSERT_MESSAGE(a_in.read() > prev_analog_value,"Analog Input did not increment.");

        // Rising Edge InterruptIn test
        GPIO_Result = false;
        int_out = 0;
        int_in.rise(cbfn);
        int_out = 1;
        wait(0); // dummy wait to get volatile result value
        TEST_ASSERT_MESSAGE(GPIO_Result,"cbfn was not triggered on rising edge of pin");

        // Falling Edge InterruptIn test
        GPIO_Result = false;
        int_out = 1;
        int_in.fall(cbfn);
        int_out = 0;
        wait(0); // dummy wait to get volatile result value
        TEST_ASSERT_MESSAGE(GPIO_Result,"cbfn was not triggered on falling edge of pin");
    }
    osSignalSet(Multi_Thread_ID, 0x4);                                // signal completion of thread
}


// Test Comms and GPIO concurrently in multiple threads. Note, when working with multiple threads, the debugging print statments called by the below threads may get garbled together due to context switches mid exection of a thread.

void test_multiple_threads()
{
    Multi_Thread_ID = Thread::gettid();                               // update thread id for this thread
    Thread_I2C.start(callback(test_I2C));                             // kick off threads
    Thread_SPI.start(callback(test_SPI));                             // kick off threads
    Thread_GPIO.start(callback(test_GPIO));                           // kick off threads
    wait(0.1);                                                        // allow time for debug print statements to complete.

    // Use this to wait for both signaling events to occur
    // Internaly the code is doing something like this:
    //   if ((signal3 | signal2 | signal1) & 0x7 == 0x7) then exit, else wait
    osSignalWait(0x7, osWaitForever);
}


// Test Comms and GPIO concurrently in a single thread.
void test_single_thread()
{
    // *******************************
    // Initialize variables and APIs
    // *******************************
    
    // DIO test initializations
    DigitalOut d_out(MBED_CONF_APP_DIO_2);
    DigitalIn d_in(MBED_CONF_APP_DIO_3);

    // AnalogIn test initializations
    AnalogIn a_in(MBED_CONF_APP_AIN_0);
    BusInOut aBus_outputs(MBED_CONF_APP_AIN_1,MBED_CONF_APP_AIN_2,MBED_CONF_APP_AIN_3,MBED_CONF_APP_AIN_4,MBED_CONF_APP_AIN_5);
    aBus_outputs.output();
    int analogOut_value= 0;
    aBus_outputs = 0;
    float prev_analog_value = 0;

    // InterruptIn test initializations
    InterruptIn int_in(MBED_CONF_APP_DIO_4);
    DigitalOut int_out(MBED_CONF_APP_DIO_5);

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
    
    // ******************************
    // Begin concurrent API testing
    // ******************************
    for(int iter = 0; iter<5; iter++) { 
        DEBUG_PRINTF("\r\n****\r\n Single thread test loop #%d\r\n****\r\n",iter);

        // DigitalIO test
        d_out = 0;  // test 0
        wait_ns(100);
        TEST_ASSERT_MESSAGE(0 == d_in.read(),"Expected value to be 0, read value was not zero");
        d_out = 1;  // test 1
        wait_ns(100);
        TEST_ASSERT_MESSAGE(1 == d_in.read(),"Expected value to be 1, read value was not one");

        // AnalogIn test
        prev_analog_value = a_in.read();
        analogOut_value = (analogOut_value << 1) + 1;
        aBus_outputs = analogOut_value;
        TEST_ASSERT_MESSAGE(a_in.read() > prev_analog_value,"Analog Input did not increment.");

        // Rising Edge InterruptIn test
        GPIO_Result = false;
        int_out = 0;
        int_in.rise(cbfn);
        int_out = 1;
        wait(0); // dummy wait to get volatile result value
        TEST_ASSERT_MESSAGE(GPIO_Result,"cbfn was not triggered on rising edge of pin");

        // Falling Edge InterruptIn test
        GPIO_Result = false;
        int_out = 1;
        int_in.fall(cbfn);
        int_out = 0;
        wait(0); // dummy wait to get volatile result value
        TEST_ASSERT_MESSAGE(GPIO_Result,"cbfn was not triggered on falling edge of pin");

        // Write to EEPROM using I2C
        num_written = memory.write(address,Test_String,TEST_STRING_MAX);

        // Write to SD card using SPI
        fs.mount(&sd);
        FILE *File = fopen("/sd/test_sd_w.txt", "w");
        TEST_ASSERT_MESSAGE(File != NULL,"SD Card is not present. Please insert an SD Card.");
        int error = fprintf(File, Test_String);                           // write data
        TEST_ASSERT_MESSAGE(error > 0,"Writing file to sd card failed");  // error checking       
        fclose(File);                                                     // close file on SD

        // Read back from EEPROM
        num_read = memory.read(address,(char *)read_string_i2c,TEST_STRING_MAX);

        // Read back from SD card
        File = fopen("/sd/test_sd_w.txt", "r");                   
        fgets((char *)read_string_spi,TEST_STRING_MAX,File);              // read string from the file

        // Test results for I2C
        TEST_ASSERT_EQUAL_STRING_MESSAGE(Test_String,(char *)read_string_i2c,"String read does not match the string written"); // test that strings match
        TEST_ASSERT_EQUAL_MESSAGE(num_written,num_read,"Number of bytes written does not match the number of bytes read");

        // Test results for SPI
        TEST_ASSERT_EQUAL_STRING_MESSAGE(Test_String,(char *)read_string_spi,"String read does not match the string written"); // test that strings match

        // Close file and unmount SD card
        fclose(File);            // close file on SD    
        fs.unmount();            // unmount
        sd.deinit();             // deinitialize SD
    }
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
    Case("Concurrent testing of Comms and GPIO in a single thread",test_single_thread,greentea_failure_handler),
#if !MBED_CONF_APP_SINGLE_THREAD_ONLY
    Case("Concurrent testing of Comms and GPIO in multiple threads",test_multiple_threads,greentea_failure_handler),
#endif /* MBED_CONF_APP_SINGLE_THREAD_ONLY */
};


Specification specification(test_setup, cases);

// Entry point into the tests
int main() {
    init_string();
    return !Harness::run(specification);
}
