#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"


using namespace utest::v1;

template <PinName d_pin>
void test_AnalogOut_NC()
{
    TEST_ASSERT_MESSAGE(d_pin != NC, "Pin is NC");
}

utest::v1::status_t test_setup(const size_t number_of_cases)
{
    // Setup Greentea using a reasonable timeout in seconds
    GREENTEA_SETUP(5, "default_auto");
    return verbose_test_setup_handler(number_of_cases);
}

utest::v1::status_t greentea_failure_handler(const Case* const source, const failure_t reason)
{
    greentea_case_failure_abort_handler(source, reason);
    return STATUS_CONTINUE;	
}

Case cases[] = {
    Case("AnalogOut - is AOUT connected?", test_AnalogOut_NC<MBED_CONF_APP_AOUT>, greentea_failure_handler),

};

Specification specification(test_setup, cases);

int main() {
    return !Harness::run(specification);
}
