#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
//#include "rtos.h"

using namespace utest::v1;

// Template to test paired Digital IO pins, meant to be re-used multiple times
template <PinName dout_pin, PinName din_pin> 
void DigitalIO_Test()
{
    DigitalOut dout(dout_pin);
    DigitalIn din(din_pin);
    // test 0
    dout = 0;
    TEST_ASSERT(0 == din.read());
    // test 1
    dout = 1;
    TEST_ASSERT(1 == din.read());
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
// TODO: take pinnames from config file
Case cases[] = {
    Case("Testing Digital I/O on D2", DigitalIO_TestC,greentea_failure_handler),
    Case("Testing Digital I/O on D3", DigitalIO_Test<CITS_DIO_3,CITS_DIO_2>,greentea_failure_handler),
    Case("Testing Digital I/O on D4", DigitalIO_Test<CITS_DIO_4,CITS_DIO_5>,greentea_failure_handler),
    Case("Testing Digital I/O on D5", DigitalIO_Test<CITS_DIO_5,CITS_DIO_4>,greentea_failure_handler),
    Case("Testing Digital I/O on D6", DigitalIO_Test<CITS_DIO_6,CITS_DIO_7>,greentea_failure_handler),
    Case("Testing Digital I/O on D7", DigitalIO_Test<CITS_DIO_7,CITS_DIO_6>,greentea_failure_handler),
    Case("Testing Digital I/O on D8", DigitalIO_Test<CITS_DIO_8,CITS_DIO_9>,greentea_failure_handler),
    Case("Testing Digital I/O on D9", DigitalIO_Test<CITS_DIO_9,CITS_DIO_8>,greentea_failure_handler),
//    Case("Testing Digital I/O on D0", DigitalIO_Test<CITS_DIO_0,CITS_DIO_1>,greentea_failure_handler),
//    Case("Testing Digital I/O on D1", DigitalIO_Test<CITS_DIO_1,CITS_DIO_0>,greentea_failure_handler),

};

Specification specification(test_setup, cases);

// Entry point into the tests
int main() {
    return !Harness::run(specification);
}
