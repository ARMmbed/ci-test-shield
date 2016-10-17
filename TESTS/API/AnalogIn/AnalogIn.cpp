#if !DEVICE_ANALOGIN
  #error [NOT_SUPPORTED] AnalogIn not supported on this platform, add 'DEVICE_ANALOGIN' deffinition to your platform.
#endif

#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
//#include "rtos.h"

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
    float prev_value = -0.999999;
    float curr_value;
    for(x = 0; x<5; x++) {
        curr_value = ain.read();
//        printf("X=%d\n",x);
//        printf("outputs=0x%x\nprevValue=%f\nain=%f\n\n",y,prev_value,curr_value);
        TEST_ASSERT_MESSAGE(curr_value > prev_value,"Analog Input did not increment. Check that you have assigned valid pins in mbed_app.json file")
        prev_value = curr_value;
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

// Handle test failures, keep testing, dont stop
utest::v1::status_t greentea_failure_handler(const Case *const source, const failure_t reason) {
    greentea_case_failure_abort_handler(source, reason);
    return STATUS_CONTINUE;
}

// Test cases
// TODO: take pin names from config file or generate from pinmap file
Case cases[] = {
    Case("Analog Input on AIN_0", AnalogInput_Test<MBED_CONF_APP_AIN_0,MBED_CONF_APP_AIN_1,MBED_CONF_APP_AIN_2,MBED_CONF_APP_AIN_3,MBED_CONF_APP_AIN_4,MBED_CONF_APP_AIN_5>,greentea_failure_handler),
    Case("Analog Input on AIN_1", AnalogInput_Test<MBED_CONF_APP_AIN_1,MBED_CONF_APP_AIN_2,MBED_CONF_APP_AIN_3,MBED_CONF_APP_AIN_4,MBED_CONF_APP_AIN_5,MBED_CONF_APP_AIN_0>,greentea_failure_handler),
    Case("Analog Input on AIN_2", AnalogInput_Test<MBED_CONF_APP_AIN_2,MBED_CONF_APP_AIN_3,MBED_CONF_APP_AIN_4,MBED_CONF_APP_AIN_5,MBED_CONF_APP_AIN_0,MBED_CONF_APP_AIN_1>,greentea_failure_handler),
    Case("Analog Input on AIN_3", AnalogInput_Test<MBED_CONF_APP_AIN_3,MBED_CONF_APP_AIN_4,MBED_CONF_APP_AIN_5,MBED_CONF_APP_AIN_0,MBED_CONF_APP_AIN_1,MBED_CONF_APP_AIN_2>,greentea_failure_handler),
    Case("Analog Input on AIN_4", AnalogInput_Test<MBED_CONF_APP_AIN_4,MBED_CONF_APP_AIN_5,MBED_CONF_APP_AIN_0,MBED_CONF_APP_AIN_1,MBED_CONF_APP_AIN_2,MBED_CONF_APP_AIN_3>,greentea_failure_handler),
    Case("Analog Input on AIN_5", AnalogInput_Test<MBED_CONF_APP_AIN_5,MBED_CONF_APP_AIN_0,MBED_CONF_APP_AIN_1,MBED_CONF_APP_AIN_2,MBED_CONF_APP_AIN_3,MBED_CONF_APP_AIN_4>,greentea_failure_handler),
};

Specification specification(test_setup, cases);

// Entry point into the tests
int main() {
    return !Harness::run(specification);
}
