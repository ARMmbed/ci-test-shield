// check if Seroa; is supported on this device
#if !DEVICE_SERIAL
  #error Serial (UART) not supported on this platform, add 'DEVICE_SERIAL' deffinition to your platform.
#endif

#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
//#include "rtos.h"

using namespace utest::v1;
#define UART_SENTINAL 'x'

Serial uart(MBED_CONF_APP_UART_TX,MBED_CONF_APP_UART_RX);

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
		TEST_ASSERT_MESSAGE(true,"characters match!");
	} else{
	//	printf("Test Failed! \n");
		TEST_ASSERT_MESSAGE(false,"characters do not match, possible corruption?");
	}
}

// TODO: add more verbose tests
// - different speeds (9600 / 115200 / ...etc)
// - flush buffers
// - write longer strings
// - special characters
// - send break characters
// ...etc

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
// TODO: take pins from config file or from pinnames.h
Case cases[] = {
    Case("Testing UART on UART_RX/UART_TX, single byte W/R ", test_uart_single, greentea_failure_handler),
};

Specification specification(test_setup, cases);

// Entry point into the tests
int main() {
    return !Harness::run(specification);
}