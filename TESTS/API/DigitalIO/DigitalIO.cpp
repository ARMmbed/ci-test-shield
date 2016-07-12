#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
#include "rtos.h"

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

// Test cases
Case cases[] = {
    Case("Testing Digital I/O on D2", DigitalIO_Test<D2,D3>),
    Case("Testing Digital I/O on D3", DigitalIO_Test<D3,D2>),
    Case("Testing Digital I/O on D4", DigitalIO_Test<D4,D5>),
    Case("Testing Digital I/O on D5", DigitalIO_Test<D5,D4>),
    Case("Testing Digital I/O on D6", DigitalIO_Test<D6,D7>),
    Case("Testing Digital I/O on D7", DigitalIO_Test<D7,D6>),
    Case("Testing Digital I/O on D8", DigitalIO_Test<D8,D9>),
    Case("Testing Digital I/O on D9", DigitalIO_Test<D9,D8>),
    Case("Testing Digital I/O on D0", DigitalIO_Test<D0,D1>),
    Case("Testing Digital I/O on D1", DigitalIO_Test<D1,D0>),

};

Specification specification(test_setup, cases);

// Entry point into the tests
int main() {
    return !Harness::run(specification);
}