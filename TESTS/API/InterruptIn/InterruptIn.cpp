#if !DEVICE_INTERRUPTIN
	#error InterruptIn is not supported on this platform, add 'DEVICE_INTERRUPTIN' deffinition to your platform.
#endif

#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
//#include "rtos.h"

using namespace utest::v1;

volatile bool result = false;

// Callback for all InterruptInput functions
void cbfn(void){
	result = true;
	//printf("\t**** Interrupt Triggered!\n");
}

// Template to check Falling edge and Risign edge interrupts.
template <PinName int_pin, PinName dout_pin> 
void InterruptInTest()
{
	result = false;
	InterruptIn intin(int_pin);
	DigitalOut dout(dout_pin);

	// Test Rising Edge InterruptIn
	//printf("***** Rising Edge Test \n");
	dout = 0;
	result = false;
	intin.rise(cbfn);
	dout = 1;
	wait(0); // dummy wait to get volatile result value
	//printf("Value of result is : %d\n",result);
	TEST_ASSERT_MESSAGE(result,"cbfn was not triggered on rising edge of pin");

	// Test Falling Edge InterruptIn
	//printf("***** Falling Edge Test \n");
	dout = 1;
	result = false;
	intin.fall(cbfn);
	dout = 0;
	wait(0); // dummy wait to get volatile result value
	//printf("Value of result is : %d\n",result);
	TEST_ASSERT_MESSAGE(result,"cbfn was not triggered on falling edge of pin");
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
// TODO: take pins from pinnames.h or from config file
Case cases[] = {
    Case("Test InterruptIn on D0", InterruptInTest<MBED_CONF_APP_DIO_0,MBED_CONF_APP_DIO_1>,greentea_failure_handler),
    Case("Test InterruptIn on D1", InterruptInTest<MBED_CONF_APP_DIO_1,MBED_CONF_APP_DIO_0>,greentea_failure_handler),
    Case("Test InterruptIn on D2", InterruptInTest<MBED_CONF_APP_DIO_2,MBED_CONF_APP_DIO_3>,greentea_failure_handler),
    Case("Test InterruptIn on D3", InterruptInTest<MBED_CONF_APP_DIO_3,MBED_CONF_APP_DIO_2>,greentea_failure_handler),
    Case("Test InterruptIn on D4", InterruptInTest<MBED_CONF_APP_DIO_4,MBED_CONF_APP_DIO_5>,greentea_failure_handler),
    Case("Test InterruptIn on D5", InterruptInTest<MBED_CONF_APP_DIO_5,MBED_CONF_APP_DIO_4>,greentea_failure_handler),
    Case("Test InterruptIn on D6", InterruptInTest<MBED_CONF_APP_DIO_6,MBED_CONF_APP_DIO_7>,greentea_failure_handler),
    Case("Test InterruptIn on D7", InterruptInTest<MBED_CONF_APP_DIO_7,MBED_CONF_APP_DIO_6>,greentea_failure_handler),
    Case("Test InterruptIn on D8", InterruptInTest<MBED_CONF_APP_DIO_8,MBED_CONF_APP_DIO_9>,greentea_failure_handler),
    Case("Test InterruptIn on D9", InterruptInTest<MBED_CONF_APP_DIO_9,MBED_CONF_APP_DIO_8>,greentea_failure_handler),
};

Specification specification(test_setup, cases);

// Entry point into the tests
int main() {
    return !Harness::run(specification);
}