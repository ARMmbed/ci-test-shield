#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
#include "rtos.h"

using namespace utest::v1;
#define UART_SENTINAL 'x'

Serial uart(D1,D0);

// Test single Write / Read
void test_uart_single(){
	
	char value = '0';
	uart.putc(UART_SENTINAL);
	//printf("putc = %c\n",UART_SENTINAL);
	value = uart.getc();
	//printf("getc = %c\n\n",value);
	TEST_ASSERT(UART_SENTINAL == value);
	if(UART_SENTINAL == value){
	//	printf("Test Finished Successfully!\n");
		TEST_ASSERT(true);
	} else{
	//	printf("Test Failed! \n");
		TEST_ASSERT(false);
	}
	TEST_ASSERT(true);
}

utest::v1::status_t test_setup(const size_t number_of_cases) {
    // Setup Greentea using a reasonable timeout in seconds
    GREENTEA_SETUP(40, "default_auto");
    return verbose_test_setup_handler(number_of_cases);
}

// Test cases
Case cases[] = {
    Case("Testing UART on D1/D0, single byte W/R ", test_uart_single),
};

Specification specification(test_setup, cases);

// Entry point into the tests
int main() {
    return !Harness::run(specification);
}