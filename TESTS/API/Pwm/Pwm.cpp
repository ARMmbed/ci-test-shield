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

Timer duty_timer;
volatile int last_rise_time;
volatile int duty_rise_count;
void duty_cbfn_rise(void){
    duty_rise_count++;
    last_rise_time = duty_timer.read_ms();
}

volatile int duty_running_count;
volatile int duty_fall_count; 
void duty_cbfn_fall(void){
    duty_fall_count++;
    if(last_rise_time == 0){
        // do nothing
    } else{
        duty_running_count = duty_running_count + (duty_timer.read_ms() - last_rise_time);
    }
}

// Template to test the Duty cycle API on PWM Pins. This test will collect a sample size of tests and average out the length of the duty cycle
// the averaged duty cycle is then compared against the expectd duty cycle. 
void PWM_Duty_slave(PinName pwm_out_pin, PinName int_in_pin, int period_in_ms, float duty_cycle_percent)
{
    #define NUM_TESTS 30
    PwmOut pwm(pwm_out_pin);
    InterruptIn iin(int_in_pin);
    duty_rise_count = 0; // reset counters
    duty_fall_count = 0;
    last_rise_time = 0;
    duty_running_count = 0;
    duty_timer.reset();
    iin.rise(duty_cbfn_rise);
    iin.fall(duty_cbfn_fall);
    printf("\r\n*****\r\n period = %fs, duty cycle = %f ",(float)period_in_ms / 1000, duty_cycle_percent);

    pwm.period((float)period_in_ms / 1000); // set PWM period
    duty_timer.start();
    pwm.write(duty_cycle_percent); // set duty cycle
    wait_ms(NUM_TESTS*period_in_ms);
    iin.disable_irq(); // This is here because otherwise it fails on some platforms
    duty_timer.stop();

    float avgTime = (float)duty_running_count / NUM_TESTS;
    float expectedTime = (float)period_in_ms * duty_cycle_percent;

    printf("\r\n rise_count = %d, fall count = %d, TotalTime = %d, avgTime = %f, expected time = %f ",duty_rise_count,duty_fall_count,duty_running_count,avgTime, expectedTime);

    float ten_percent  = expectedTime * 0.1;
    float five_percent = expectedTime * 0.05;
    float one_percent  = expectedTime * 0.01;

    printf("\r\n .10 = %f, .05 = %f, .01 = %f\r\n*****\r\n",ten_percent,five_percent,one_percent);

    TEST_ASSERT_MESSAGE( std::abs(expectedTime - avgTime) <= ten_percent, "Greater than 10 percent variance between expected and measured duty cycle");
    TEST_ASSERT_MESSAGE( std::abs(expectedTime - avgTime) <= five_percent,"Greater than 5  percent variance between expected and measured duty cycle");
    //TEST_ASSERT_MESSAGE( std::abs(expectedTime - avgTime) <= one_percent, "Greater than 1  percent variance between expected and measured duty cycle");
}

// Template to itterate through a PWM pin, takes in period and tries 10%-100% duty cycle in intervals of 10%
template <PinName pwm_out_pin, PinName int_in_pin, int period_in_miliseconds> 
void PWM_DutyCycle_Test()
{
    #define DUTY_CYCLE_STEP  0.2f
    #define MIN_DUTY_CYCLE   0.1f
    #define MAX_DUTY_CYCLE   0.9f

    float x = 0;
    for(x = MIN_DUTY_CYCLE; x < MAX_DUTY_CYCLE; x = x+DUTY_CYCLE_STEP){ // itterate duty cycle test
        PWM_Duty_slave(pwm_out_pin, int_in_pin, period_in_miliseconds, x);
        //printf("\r\n**************\r\n expected 10 cycles, saw %d rise, %d fall\r\n*******\r\n",duty_rise_count,duty_fall_count);
        //TEST_ASSERT_MESSAGE(100 == rise_count,"Number of cycles not equivalent to amount expected\r\n");
    }
}


volatile int rise_count;
void cbfn_rise(void){
    rise_count++;
}

volatile int fall_count; 
void cbfn_fall(void){
    fall_count++;
}

// Template to test that a PWM signal has the correct length by measuring the number of rise and fall
// interrupts during a specified number of tests. 
template <PinName pwm_out_pin, PinName int_in_pin, int period_in_miliseconds, int num_tests> 
void PWM_Period_Test()
{
    // Initialize PWM, InterruptIn, Timer, and Rising / Falling edge counts
    fall_count = 0;
    rise_count = 0;
    PwmOut pwm(pwm_out_pin);
    InterruptIn iin(int_in_pin);
    iin.rise(cbfn_rise);
    iin.fall(cbfn_fall);
    pwm.period((float)period_in_miliseconds/1000);
    
    //Start Testing
    pwm.write(0.5f); // 50% duty cycle
    wait_ms(num_tests * period_in_miliseconds); // wait for pwm to run and counts to add up
    iin.disable_irq(); // This is here because otherwise it fails on some platforms
    int rc = rise_count; // grab the numbers to work with as the pwm may continue going
    int fc = fall_count;

    int expected_count = num_tests;

    float ten_percent  = (expected_count * 0.1f);
    float five_percent = (expected_count * 0.05f);
    float one_percent  = (expected_count * 0.01f);

    // fudge factor
    if(ten_percent  < 1){ten_percent  = 1;}
    if(five_percent < 1){five_percent = 1;}
    if(one_percent  < 1){one_percent  = 1;}

    printf("\r\n*****\r\n rise count = %d, fall count = %d, expected count = %d",rc,fc,expected_count);
    printf("\r\n .10 = %f, .05 = %f, .01 = %f",ten_percent,five_percent,one_percent);

    TEST_ASSERT_MESSAGE( std::abs(rc-fc) <= ten_percent, "There was more than 10percent variance in number of rise vs fall cycles");
    TEST_ASSERT_MESSAGE( std::abs(rc-fc) <= five_percent,"There was more than  5percent variance in number of rise vs fall cycles");
    //TEST_ASSERT_MESSAGE( std::abs(rc-fc) <= one_percent, "There was more than  1percent variance in number of rise vs fall cycles");

    printf("\r\n abs(expected-rc) = %d",std::abs(expected_count - rc));
    TEST_ASSERT_MESSAGE( std::abs(expected_count - rc) <= ten_percent, "There was more than 10 percent variance in number of rise cycles seen and number expected.");
    TEST_ASSERT_MESSAGE( std::abs(expected_count - rc) <= five_percent,"There was more than  5 percent variance in number of rise cycles seen and number expected.");
    //TEST_ASSERT_MESSAGE( std::abs(expected_count - rc) <= one_percent, "There was more than  1 percent variance in number of rise cycles seen and number expected.");

    printf("\r\n abs(expected-fc) = %d\r\n*****\r\n",std::abs(expected_count - fc));
    TEST_ASSERT_MESSAGE( std::abs(expected_count - fc) <= ten_percent, "There was more than 10 percent variance in number of fall cycles seen and number expected.");
    TEST_ASSERT_MESSAGE( std::abs(expected_count - fc) <= five_percent,"There was more than  5 percent variance in number of fall cycles seen and number expected.");
    //TEST_ASSERT_MESSAGE( std::abs(expected_count - fc) <= one_percent, "There was more than  1 percent variance in number of fall cycles seen and number expected.");
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
    GREENTEA_SETUP(200, "default_auto");
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
    // Test Creat PwmOut Objects
//     Case("Pwm object definable", pwm_define_test,greentea_failure_handler),   // test pwm object contructor works

    // Test Frequency length by counting rise / fall ticks
     Case("PWM_0 Frequency 10ms",  PWM_Period_Test< MBED_CONF_APP_PWM_0, MBED_CONF_APP_DIO_2, 10,  1000 >, greentea_failure_handler),  // Test at 10ms 100 times, default 50%duty cycle
     Case("PWM_0 Frequency 30ms",  PWM_Period_Test< MBED_CONF_APP_PWM_0, MBED_CONF_APP_DIO_2, 30,  100 >, greentea_failure_handler),  // Test at 30ms 100 times, default 50%duty cycle
     Case("PWM_0 Frequency 100ms", PWM_Period_Test< MBED_CONF_APP_PWM_0, MBED_CONF_APP_DIO_2, 100, 100 >, greentea_failure_handler),  // Test at 100ms 100 times, default 50%duty cycle
     //Case("PWM_0 Frequency 500ms", PWM_Period_Test< MBED_CONF_APP_PWM_0, MBED_CONF_APP_DIO_2, 500,  20 >, greentea_failure_handler), // Test at 500ms 20 times, default 50%duty cycle
     Case("PWM_1 Frequency 10ms",  PWM_Period_Test< MBED_CONF_APP_PWM_1, MBED_CONF_APP_DIO_4, 10,  1000 >, greentea_failure_handler),  // Test at 10ms 100 times, default 50%duty cycle
     Case("PWM_1 Frequency 30ms",  PWM_Period_Test< MBED_CONF_APP_PWM_1, MBED_CONF_APP_DIO_4, 30,  100 >, greentea_failure_handler),  // Test at 30ms 100 times, default 50%duty cycle
     Case("PWM_1 Frequency 100ms", PWM_Period_Test< MBED_CONF_APP_PWM_1, MBED_CONF_APP_DIO_4, 100, 100 >, greentea_failure_handler),  // Test at 100ms 100 times, default 50%duty cycle
     //Case("PWM_1 Frequency 500ms", PWM_Period_Test< MBED_CONF_APP_PWM_1, MBED_CONF_APP_DIO_4, 500,  20 >, greentea_failure_handler), // Test at 500ms 20 times, default 50%duty cycle
     Case("PWM_2 Frequency 10ms",  PWM_Period_Test< MBED_CONF_APP_PWM_2, MBED_CONF_APP_DIO_7, 10,  1000 >, greentea_failure_handler),  // Test at 10ms 100 times, default 50%duty cycle
     Case("PWM_2 Frequency 30ms",  PWM_Period_Test< MBED_CONF_APP_PWM_2, MBED_CONF_APP_DIO_7, 30,  100 >, greentea_failure_handler),  // Test at 30ms 100 times, default 50%duty cycle
     Case("PWM_2 Frequency 100ms", PWM_Period_Test< MBED_CONF_APP_PWM_2, MBED_CONF_APP_DIO_7, 100, 100 >, greentea_failure_handler),  // Test at 100ms 100 times, default 50%duty cycle
     //Case("PWM_2 Frequency 500ms", PWM_Period_Test< MBED_CONF_APP_PWM_2, MBED_CONF_APP_DIO_7, 500,  20 >, greentea_failure_handler), // Test at 500ms 20 times, default 50%duty cycle
     Case("PWM_3 Frequency 10ms",  PWM_Period_Test< MBED_CONF_APP_PWM_3, MBED_CONF_APP_DIO_8, 10,  1000 >, greentea_failure_handler),  // Test at 10ms 100 times, default 50%duty cycle
     Case("PWM_3 Frequency 30ms",  PWM_Period_Test< MBED_CONF_APP_PWM_3, MBED_CONF_APP_DIO_8, 30,  100 >, greentea_failure_handler),  // Test at 30ms 100 times, default 50%duty cycle
     Case("PWM_3 Frequency 100ms", PWM_Period_Test< MBED_CONF_APP_PWM_3, MBED_CONF_APP_DIO_8, 100, 100 >, greentea_failure_handler),  // Test at 100ms 100 times, default 50%duty cycle
     //Case("PWM_3 Frequency 500ms", PWM_Period_Test< MBED_CONF_APP_PWM_3, MBED_CONF_APP_DIO_8, 500,  20 >, greentea_failure_handler),  // Test at 500ms 20 times, default 50%duty cycle
    
    // Test Duty Cycle width (10%->90%)
        Case("PWM_0 Duty Cycle 10ms",  PWM_DutyCycle_Test< MBED_CONF_APP_PWM_0, MBED_CONF_APP_DIO_2, 10 >, greentea_failure_handler),   // Test 10ms Frequency with 10% to 90% duty cycle
        Case("PWM_0 Duty Cycle 30ms",  PWM_DutyCycle_Test< MBED_CONF_APP_PWM_0, MBED_CONF_APP_DIO_2, 30 >, greentea_failure_handler),   // Test 30ms Frequency with 10% to 90% duty cycle
        Case("PWM_0 Duty Cycle 100ms", PWM_DutyCycle_Test< MBED_CONF_APP_PWM_0, MBED_CONF_APP_DIO_2, 100>, greentea_failure_handler),   // Test 100ms Frequency with 10% to 90% duty cycle
        //Case("PWM_0 Duty Cycle 500ms",  PWM_DutyCycle_Test< MBED_CONF_APP_PWM_0, MBED_CONF_APP_DIO_2, 500 >, greentea_failure_handler),   // Test 500ms Frequency with 10% to 90% duty cycle
        Case("PWM_1 Duty Cycle 10ms",  PWM_DutyCycle_Test< MBED_CONF_APP_PWM_1, MBED_CONF_APP_DIO_4, 10 >, greentea_failure_handler),   // Test 10ms Frequency with 10% to 90% duty cycle
        Case("PWM_1 Duty Cycle 30ms",  PWM_DutyCycle_Test< MBED_CONF_APP_PWM_1, MBED_CONF_APP_DIO_4, 30 >, greentea_failure_handler),   // Test 30ms Frequency with 10% to 90% duty cycle
        Case("PWM_1 Duty Cycle 100ms", PWM_DutyCycle_Test< MBED_CONF_APP_PWM_1, MBED_CONF_APP_DIO_4, 100>, greentea_failure_handler),   // Test 100ms Frequency with 10% to 90% duty cycle
        //Case("PWM_1 Duty Cycle 500ms",  PWM_DutyCycle_Test< MBED_CONF_APP_PWM_1, MBED_CONF_APP_DIO_4, 500>, greentea_failure_handler),    // Test 500ms Frequency with 10% to 90% duty cycle
        Case("PWM_2 Duty Cycle 10ms",  PWM_DutyCycle_Test< MBED_CONF_APP_PWM_2, MBED_CONF_APP_DIO_7, 10 >, greentea_failure_handler),   // Test 10ms Frequency with 10% to 90% duty cycle
        Case("PWM_2 Duty Cycle 30ms",  PWM_DutyCycle_Test< MBED_CONF_APP_PWM_2, MBED_CONF_APP_DIO_7, 30 >, greentea_failure_handler),   // Test 30ms Frequency with 10% to 90% duty cycle
        Case("PWM_2 Duty Cycle 100ms", PWM_DutyCycle_Test< MBED_CONF_APP_PWM_2, MBED_CONF_APP_DIO_7, 100>, greentea_failure_handler),   // Test 100ms Frequency with 10% to 90% duty cycle
        //Case("PWM_2 Duty Cycle 500ms",  PWM_DutyCycle_Test< MBED_CONF_APP_PWM_2, MBED_CONF_APP_DIO_7, 500>, greentea_failure_handler),    // Test 500ms Frequency with 10% to 90% duty cycle
        Case("PWM_3 Duty Cycle 10ms",  PWM_DutyCycle_Test< MBED_CONF_APP_PWM_3, MBED_CONF_APP_DIO_8, 10 >, greentea_failure_handler),   // Test 10ms Frequency with 10% to 90% duty cycle
        Case("PWM_3 Duty Cycle 30ms",  PWM_DutyCycle_Test< MBED_CONF_APP_PWM_3, MBED_CONF_APP_DIO_8, 30 >, greentea_failure_handler),   // Test 30ms Frequency with 10% to 90% duty cycle
        Case("PWM_3 Duty Cycle 100ms", PWM_DutyCycle_Test< MBED_CONF_APP_PWM_3, MBED_CONF_APP_DIO_8, 100>, greentea_failure_handler),   // Test 100ms Frequency with 10% to 90% duty cycle
        //Case("PWM_3 Duty Cycle 500ms",  PWM_DutyCycle_Test< MBED_CONF_APP_PWM_3, MBED_CONF_APP_DIO_8, 500>, greentea_failure_handler),    // Test 500ms Frequency with 10% to 90% duty cycle
};

Specification specification(test_setup, cases);

// Entry point into the tests
int main() {
    return !Harness::run(specification);
}
