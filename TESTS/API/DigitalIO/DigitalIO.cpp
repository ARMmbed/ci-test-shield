#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
#include "rtos.h"

using namespace utest::v1;


// Tests that assert are considered failing
void test_D0() {
    DigitalOut dout(D0);
    DigitalIn din(D1);
    // test 0
    dout = 0;
    TEST_ASSERT(0 == din.read());
    // test 1
    dout = 1;
    TEST_ASSERT(1 == din.read());
}

// Tests that assert are considered failing
void test_D1() {
    DigitalOut dout(D1);
    DigitalIn din(D0);
    // test 0
    dout = 0;
    TEST_ASSERT(0 == din.read());
    // test 1
    dout = 1;
    TEST_ASSERT(1 == din.read());
}

// Tests that assert are considered failing
void test_D2() {
    DigitalOut dout(D2);
    DigitalIn din(D3);
    // test 0
    dout = 0;
    TEST_ASSERT(0 == din.read());
    // test 1
    dout = 1;
    TEST_ASSERT(1 == din.read());
}

// Tests that assert are considered failing
void test_D3() {
    DigitalOut dout(D3);
    DigitalIn din(D2);
    // test 0
    dout = 0;
    TEST_ASSERT(0 == din.read());
    // test 1
    dout = 1;
    TEST_ASSERT(1 == din.read());
}

// Tests that assert are considered failing
void test_D4() {
    DigitalOut dout(D4);
    DigitalIn din(D5);
    // test 0
    dout = 0;
    TEST_ASSERT(0 == din.read());
    // test 1
    dout = 1;
    TEST_ASSERT(1 == din.read());
}

// Tests that assert are considered failing
void test_D5() {
    DigitalOut dout(D5);
    DigitalIn din(D4);
    // test 0
    dout = 0;
    TEST_ASSERT(0 == din.read());
    // test 1
    dout = 1;
    TEST_ASSERT(1 == din.read());
}

// Tests that assert are considered failing
void test_D6() {
    DigitalOut dout(D6);
    DigitalIn din(D7);
    // test 0
    dout = 0;
    TEST_ASSERT(0 == din.read());
    // test 1
    dout = 1;
    TEST_ASSERT(1 == din.read());
}

// Tests that assert are considered failing
void test_D7() {
    DigitalOut dout(D7);
    DigitalIn din(D6);
    // test 0
    dout = 0;
    TEST_ASSERT(0 == din.read());
    // test 1
    dout = 1;
    TEST_ASSERT(1 == din.read());
}

// Tests that assert are considered failing
void test_D8() {
    DigitalOut dout(D8);
    DigitalIn din(D9);
    // test 0
    dout = 0;
    TEST_ASSERT(0 == din.read());
    // test 1
    dout = 1;
    TEST_ASSERT(1 == din.read());
}

// Tests that assert are considered failing
void test_D9() {
    DigitalOut dout(D9);
    DigitalIn din(D8);
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
    Case("Testing Digital I/O on D2", test_D2),
    Case("Testing Digital I/O on D3", test_D3),
    Case("Testing Digital I/O on D4", test_D4),
    Case("Testing Digital I/O on D5", test_D5),
    Case("Testing Digital I/O on D6", test_D6),
    Case("Testing Digital I/O on D7", test_D7),
    Case("Testing Digital I/O on D8", test_D8),
    Case("Testing Digital I/O on D9", test_D9),
    Case("Testing Digital I/O on D0", test_D0),
    Case("Testing Digital I/O on D1", test_D1),

};

Specification specification(test_setup, cases);

// Entry point into the tests
int main() {
    return !Harness::run(specification);
}