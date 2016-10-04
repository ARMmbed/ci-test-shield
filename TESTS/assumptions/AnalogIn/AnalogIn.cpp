#if !DEVICE_ANALOGIN
  #error [NOT_SUPPORTED] AnalogIn not supported on this platform, add 'DEVICE_ANALOGIN' deffinition to your platform.
#endif

#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
//#include "rtos.h"

using namespace utest::v1;

// test for existance of DEVICE_ANALOGIN macro. If it doesnt exist then the device doesnt support the AnalogIn API
void Macro_Test(){
    #ifndef DEVICE_ANALOGIN
        TEST_ASSERT_MESSAGE(false,"Macr `DEVICE_ANALOGIN` not defined. This platform does not support AnalogIn.")
    #endif
}

// Test if the AnalogInput can also do DigitalIO. Falure expects a timeout. 
// typical failure is a run time assert deep in mbed library. 
// CI Test shield assumes AnalogIn pins can double duty as DigitalOut pins.
void Can_DigitalOut(){
    DigitalOut dout0(MBED_CONF_APP_AIN_0);
    DigitalOut dout1(MBED_CONF_APP_AIN_1);
    DigitalOut dout2(MBED_CONF_APP_AIN_2);
    DigitalOut dout3(MBED_CONF_APP_AIN_3);
    DigitalOut dout4(MBED_CONF_APP_AIN_4);
    DigitalOut dout5(MBED_CONF_APP_AIN_5);
    TEST_ASSERT_MESSAGE(true,"We got to this point, we're goog to go.");
}

// Test that AnalogIn ports can be AnalogIn and Digital Out at same time
//  meant to catch boards that turn all pins to analog in instead of just 1 pin
void Can_AnalogIn_and_DigitalOut(){
    AnalogIn ain(MBED_CONF_APP_AIN_0);
    DigitalOut dout(MBED_CONF_APP_AIN_1);
    dout=0;
    ain.read();
    dout=1;
    ain.read();
    TEST_ASSERT_MESSAGE(true,"We got to this point, we're goog to go.");
}

utest::v1::status_t test_setup(const size_t number_of_cases) {
    // Setup Greentea using a reasonable timeout in seconds
    GREENTEA_SETUP(5, "default_auto");
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
    Case("Test existence of `DEVICE_ANALOGIN` macro", Macro_Test,greentea_failure_handler),
    Case("Test AnalogIn can do DigitalOut", Can_DigitalOut,greentea_failure_handler),
    Case("Test AnalogIn can co-exist with DigitalOut", Can_AnalogIn_and_DigitalOut,greentea_failure_handler),

};

Specification specification(test_setup, cases);

// Entry point into the tests
int main() {
    return !Harness::run(specification);
}
