//#error [NOT_SUPPORTED] PWM tests are still being written and validated, not for public use yet. 

#if !DEVICE_PWMOUT
  #error PWMOUT not supported on this platform, add 'DEVICE_PWMOUT' deffinition to your platform.
#endif

#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
//#include "rtos.h"

using namespace utest::v1;

#define DUTY_CYCLE_STEP  0.1
#define MIN_DUTY_CYCLE   0.1
#define MAX_DUTY_CYCLE   0.9

volatile int rise_count;
void cbfn_rise(void){
    rise_count++;
}

volatile int fall_count; 
void cbfn_fall(void){
    fall_count++;
}

// Template to set one Analog pin as input and then cycle through the rest as outputs.
// As you turn more pins on the voltage on the ain pin will go up.
void PWM_Test_slave(PinName pwm_out_pin, PinName int_in_pin, int period_in_seconds, int duty_cycle_percent)
{
    PwmOut pwm(pwm_out_pin);
    InterruptIn iin(int_in_pin);
    rise_count = 0; // reset counters
    fall_count = 0;
    pwm.period(period_in_seconds);    // set PWM period
    pwm.write(duty_cycle_percent); // set duty cycle

    // TODO: impliment ticket to count time and compare number of PWM ticks that occurred
}

// Template to itterate through a PWM pin, takes in period and tries 10%-100% duty cycle in intervals of 10%
template <PinName pwm_out_pin, PinName int_in_pin, int period_in_seconds> 
void PWM_Test()
{
    int x = 0;
    for(x = MIN_DUTY_CYCLE; x < MAX_DUTY_CYCLE; x = x+DUTY_CYCLE_STEP){ // itterate duty cycle test
        PWM_Test_slave(pwm_out_pin, int_in_pin, period_in_seconds, x);
    }
}

// test if software constructor / destructor works
void pwm_define_test(){
    PwmOut pwm0(MBED_CONF_APP_PWM_0);
    PwmOut pwm1(MBED_CONF_APP_PWM_1);
    PwmOut pwm2(MBED_CONF_APP_PWM_2);
    PwmOut pwm3(MBED_CONF_APP_PWM_3);

    pwm0.period(1.0f);
    pwm1.period(1.0f);
    pwm2.period(1.0f);
    pwm3.period(1.0f);

    pwm0.write(0.5f);
    pwm1.write(0.5f);
    pwm2.write(0.5f);
    pwm3.write(0.5f);

    TEST_ASSERT_MESSAGE(true,"The fact that it hasnt errored out proves this passes the sniff test");
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
    Case("Test Pwm definable", pwm_define_test,greentea_failure_handler),                                        // test pwm object contructor works
    //Case("Test PWM on PWM_0", PWM_Test<MBED_CONF_APP_PWM_0,MBED_CONF_APP_DIO_2,4>,greentea_failure_handler), // 4second period, 50% duty cycle
    //Case("Test PWM on PWM_1", PWM_Test<MBED_CONF_APP_PWM_1,MBED_CONF_APP_DIO_4,4.0f>,greentea_failure_handler), // 4second period, 50% duty cycle
    //Case("Test PWM on PWM_2", PWM_Test<MBED_CONF_APP_PWM_2,MBED_CONF_APP_DIO_7,4.0f>,greentea_failure_handler), // 4second period, 50% duty cycle
    //Case("Test PWM on PWM_3", PWM_Test<MBED_CONF_APP_PWM_3,MBED_CONF_APP_DIO_8,4.0f>,greentea_failure_handler)  // 4second period, 50% duty cycle
};

Specification specification(test_setup, cases);

// Entry point into the tests
int main() {
    return !Harness::run(specification);
}
