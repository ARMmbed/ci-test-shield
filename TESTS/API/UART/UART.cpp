#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
#include "rtos.h"

using namespace utest::v1;

// Test single Write / Read
void test_uart_single(){
	Serial uart(D1,D0);
	char value = '0';
	uart.putc('x');
	printf("putc\n");
	value = uart.getc();
	printf("getc\n");
	TEST_ASSERT(value == 'x');
}

utest::v1::status_t test_setup(const size_t number_of_cases) {
    // Setup Greentea using a reasonable timeout in seconds
    GREENTEA_SETUP(40, "default_auto");
    return verbose_test_setup_handler(number_of_cases);
}

// Test cases
Case cases[] = {
    Case("Testing single uart W/R", test_uart_single),
};

Specification specification(test_setup, cases);

// Entry point into the tests
int main() {
    return !Harness::run(specification);
}