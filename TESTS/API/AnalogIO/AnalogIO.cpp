#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
#include "rtos.h"

using namespace utest::v1;

// Test Analog In
void test_A0in(){
    AnalogIn ain(A0);
    BusInOut outputs(A1,A2,A3,A4,A5);
    outputs.output();
    int x = 0;
    int y= 0;
    outputs = y;
    float prev_value = 0;
    for(x = 0; x<5; x++) {
//        printf("X=%d\n",x);
//        printf("outputs=0x%x\nprevValue=%f\nain=%f\n\n",y,prev_value,ain.read());
        TEST_ASSERT(ain.read() > prev_value)
        prev_value = ain.read();
        y = (y<<1) + 1;
        outputs = y;
    }
//    printf("Finished the Test\n");
    TEST_ASSERT(true);
}


utest::v1::status_t test_setup(const size_t number_of_cases) {
    // Setup Greentea using a reasonable timeout in seconds
    GREENTEA_SETUP(30, "default_auto");
    return verbose_test_setup_handler(number_of_cases);
}

// Test cases
Case cases[] = {
    Case("Test Analog Input on A0", test_A0in),
};

Specification specification(test_setup, cases);

// Entry point into the tests
int main() {
    return !Harness::run(specification);
}