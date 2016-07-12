#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
#include "rtos.h"

using namespace utest::v1;

AnalogIn ain(A0);
AnalogOut aout(A5);

//Test Analog Output on A5
void AnalogOutput_Test(){
    
    float valueOff = 0;
    float valueOn = 0;
    aout = 0;
    valueOff = ain.read();
    aout = 0.5;
    valueOn = ain.read();
    printf("***** valueOff = %f, valueOn = %f \n",valueOff, valueOn);
    TEST_ASSERT(valueOn > valueOff);
    valueOff = ain.read();
    aout = 1.0;
    valueOn = ain.read();
    printf("***** valueOff = %f, valueOn = %f \n",valueOff, valueOn);
    TEST_ASSERT(valueOn > valueOff);
}

utest::v1::status_t test_setup(const size_t number_of_cases) {
    // Setup Greentea using a reasonable timeout in seconds
    GREENTEA_SETUP(30, "default_auto");
    return verbose_test_setup_handler(number_of_cases);
}

// Test cases
Case cases[] = {
    Case("Test Analog Output on A5", AnalogOutput_Test),
};

Specification specification(test_setup, cases);

// Entry point into the tests
int main() {
    return !Harness::run(specification);
}
