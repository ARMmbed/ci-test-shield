#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
#include "rtos.h"

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
	TEST_ASSERT(result);

	// Test Falling Edge InterruptIn
	//printf("***** Falling Edge Test \n");
	dout = 1;
	result = false;
	intin.fall(cbfn);
	dout = 0;
	wait(0); // dummy wait to get volatile result value
	//printf("Value of result is : %d\n",result);
	TEST_ASSERT(result);
}

utest::v1::status_t test_setup(const size_t number_of_cases) {
    // Setup Greentea using a reasonable timeout in seconds
    GREENTEA_SETUP(40, "default_auto");
    return verbose_test_setup_handler(number_of_cases);
}

// Test cases
Case cases[] = {
    Case("Test InterruptIn on D0", InterruptInTest<D0,D1>),
    Case("Test InterruptIn on D1", InterruptInTest<D1,D0>),
    Case("Test InterruptIn on D2", InterruptInTest<D2,D3>),
    Case("Test InterruptIn on D3", InterruptInTest<D3,D2>),
    Case("Test InterruptIn on D4", InterruptInTest<D4,D5>),
    Case("Test InterruptIn on D5", InterruptInTest<D5,D4>),
    Case("Test InterruptIn on D6", InterruptInTest<D6,D7>),
    Case("Test InterruptIn on D7", InterruptInTest<D7,D6>),
    Case("Test InterruptIn on D8", InterruptInTest<D8,D9>),
    Case("Test InterruptIn on D9", InterruptInTest<D9,D8>),
};

Specification specification(test_setup, cases);

// Entry point into the tests
int main() {
    return !Harness::run(specification);
}