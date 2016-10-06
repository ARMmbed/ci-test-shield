//#error [NOT_SUPPORTED] PWM tests are still being written and validated, not for public use yet. 

#if !DEVICE_PWMOUT
  #error PWMOUT not supported on this platform, add 'DEVICE_PWMOUT' deffinition to your platform.
#endif

#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
#include <cmath>

using namespace utest::v1;


volatile int rise_count;
void cbfn_rise(void){
    rise_count++;
}

volatile int fall_count; 
void cbfn_fall(void){
    fall_count++;
}

// Test to see if PWM, Timer and Interrupt test can all play nice together
void pwm_interrupt_timer_test(){
    // Initialize PWM, InterruptIn, Timer, and Rising / Falling edge counts
    Timer timer;
    PwmOut pwm(MBED_CONF_APP_PWM_0);
    InterruptIn x(MBED_CONF_APP_DIO_2);
    InterruptIn y(D4);
    x.rise(cbfn_rise);
    x.fall(cbfn_fall);
    fall_count = 0;
    rise_count = 0;
    pwm.period(0.01f); // 10ms period
    timer.reset();
    //Start Testing
    timer.start();
    pwm.write(0.5f); // 50% duty cycle
    timer.stop();
    x.disable_irq(); // Try enabling this if the test fails
    printf("\r\n*****\r\n Test is complete, returning now, this shouldnt hang, if this ERROR's then your platform has a problem.\r\n*****\r\n");
    TEST_ASSERT_MESSAGE(true,"If we exit this test and return then it all works just fine.");
}


utest::v1::status_t test_setup(const size_t number_of_cases) {
    // Setup Greentea using a reasonable timeout in seconds
    GREENTEA_SETUP(10, "default_auto");
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
    Case("PwmOut Assumption - pwm + int + timer objects can co-exist", pwm_interrupt_timer_test,greentea_failure_handler),
};

Specification specification(test_setup, cases);

// Entry point into the tests
int main() {
    return !Harness::run(specification);
}
