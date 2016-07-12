#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
#include "rtos.h"

using namespace utest::v1;

// Template to set one Analog pin as input and then cycle through the rest as outputs.
// As you turn more pins on the voltage on the ain pin will go up.
template <PinName ain_pin, PinName dout_pin1, PinName dout_pin2, PinName dout_pin3, PinName dout_pin4, PinName dout_pin5> 
void AnalogInput_Test()
{
    AnalogIn ain(ain_pin);
    BusInOut outputs(dout_pin1,dout_pin2,dout_pin3,dout_pin4,dout_pin5);
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

// // Test Analog In
// void test_A0in(){
//     AnalogIn ain(A0);
//     BusInOut outputs(A1,A2,A3,A4,A5);
//     outputs.output();
//     int x = 0;
//     int y= 0;
//     outputs = y;
//     float prev_value = 0;
//     for(x = 0; x<5; x++) {
// //        printf("X=%d\n",x);
// //        printf("outputs=0x%x\nprevValue=%f\nain=%f\n\n",y,prev_value,ain.read());
//         TEST_ASSERT(ain.read() > prev_value)
//         prev_value = ain.read();
//         y = (y<<1) + 1;
//         outputs = y;
//     }
// //    printf("Finished the Test\n");
//     TEST_ASSERT(true);
// }


utest::v1::status_t test_setup(const size_t number_of_cases) {
    // Setup Greentea using a reasonable timeout in seconds
    GREENTEA_SETUP(30, "default_auto");
    return verbose_test_setup_handler(number_of_cases);
}

// Test cases
Case cases[] = {
    Case("Test Analog Input on A0", AnalogInput_Test<A0,A1,A2,A3,A4,A5>),
    Case("Test Analog Input on A1", AnalogInput_Test<A1,A2,A3,A4,A5,A0>),
    Case("Test Analog Input on A2", AnalogInput_Test<A2,A3,A4,A5,A0,A1>),
    Case("Test Analog Input on A3", AnalogInput_Test<A3,A4,A5,A0,A1,A2>),
    Case("Test Analog Input on A4", AnalogInput_Test<A4,A5,A0,A1,A2,A3>),
    Case("Test Analog Input on A5", AnalogInput_Test<A5,A0,A1,A2,A3,A4>),
};

Specification specification(test_setup, cases);

// Entry point into the tests
int main() {
    return !Harness::run(specification);
}
