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
	printf("\t**** Interrupt Triggered!\n");
}

// Test Interrupt Input 
void test_intin_d0(){
	InterruptIn intin(D0);
	DigitalOut dout(D1);

	// Test Rising Edge InterruptIn
	//printf("***** Rising Edge Test \n");
	dout = 0;
	result = false;
	intin.rise(cbfn);
	dout = 1;
	wait(1);
	//printf("Value of result is : %d\n",result);
	TEST_ASSERT(result);

	// Test Falling Edge InterruptIn
	//printf("***** Falling Edge Test \n");
	dout = 1;
	result = false;
	intin.fall(cbfn);
	dout = 0;
	wait(1);
	//printf("Value of result is : %d\n",result);
	TEST_ASSERT(result);

	//printf("***** Test Finished! \n");
}

utest::v1::status_t test_setup(const size_t number_of_cases) {
    // Setup Greentea using a reasonable timeout in seconds
    GREENTEA_SETUP(40, "default_auto");
    return verbose_test_setup_handler(number_of_cases);
}

// Test cases
Case cases[] = {
    Case("Test InterruptIn on D0", test_intin_d0),
};

Specification specification(test_setup, cases);

// Entry point into the tests
int main() {
    return !Harness::run(specification);
}