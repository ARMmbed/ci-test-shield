#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"

using namespace utest::v1;


// test that all pins can be marked as busin
void busin_define_test(){
    BusIn bin(MBED_CONF_APP_DIO_2,MBED_CONF_APP_DIO_3,MBED_CONF_APP_DIO_4,MBED_CONF_APP_DIO_5,MBED_CONF_APP_DIO_6,MBED_CONF_APP_DIO_7,MBED_CONF_APP_DIO_8,MBED_CONF_APP_DIO_9,MBED_CONF_APP_SPI_CS,MBED_CONF_APP_SPI_MOSI,MBED_CONF_APP_AIN_0,MBED_CONF_APP_AIN_1,MBED_CONF_APP_AIN_2,MBED_CONF_APP_AIN_3,MBED_CONF_APP_AIN_4,MBED_CONF_APP_AIN_5);
    int x = 0;
    x = bin.read();
    TEST_ASSERT_MESSAGE(true,"The fact that it hasnt error out proves this passes the sniff test");
}

// test that all pins can be marked as busin
void busout_define_test(){
    BusOut bout(MBED_CONF_APP_DIO_2,MBED_CONF_APP_DIO_3,MBED_CONF_APP_DIO_4,MBED_CONF_APP_DIO_5,MBED_CONF_APP_DIO_6,MBED_CONF_APP_DIO_7,MBED_CONF_APP_DIO_8,MBED_CONF_APP_DIO_9,MBED_CONF_APP_SPI_CS,MBED_CONF_APP_SPI_MOSI,MBED_CONF_APP_AIN_0,MBED_CONF_APP_AIN_1,MBED_CONF_APP_AIN_2,MBED_CONF_APP_AIN_3,MBED_CONF_APP_AIN_4,MBED_CONF_APP_AIN_5);
    bout = 0;
    volatile int x = 0;
    while(x < 0xFF){
        //printf("\r\n*********\r\nvalue of x is: 0x%x\r\n********\r\n",x);
        x++;
        bout.write(x);
    }
    TEST_ASSERT_MESSAGE(true,"The fact that it hasnt error out proves this passes the sniff test");
}

utest::v1::status_t test_setup(const size_t number_of_cases) {
    // Setup Greentea using a reasonable timeout in seconds
    GREENTEA_SETUP(30, "default_auto");
    return verbose_test_setup_handler(number_of_cases);
}

// Handle test failures, keep testing, dont stop
utest::v1::status_t greentea_failure_handler(const Case *const source, const failure_t reason) {
    greentea_case_failure_abort_handler(source, reason);
    return STATUS_CONTINUE;
}

// Test cases
// TODO: take pin names from config file or generate from pinmap file
Case cases[] = {
    Case("Test BusIn definable", busin_define_test,greentea_failure_handler),
    Case("Test BusOut definable", busout_define_test,greentea_failure_handler),
};

Specification specification(test_setup, cases);

// Entry point into the tests
int main() {
    return !Harness::run(specification);
}
