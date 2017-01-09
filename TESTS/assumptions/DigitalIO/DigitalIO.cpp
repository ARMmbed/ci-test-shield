#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"


using namespace utest::v1;

template <PinName d_pin>
void test_DigitalIO_NC()
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
	Case("DigitalIO - is pin 0 connected?", test_DigitalIO_NC<MBED_CONF_APP_DIO_0>, greentea_failure_handler),
	Case("DigitalIO - is pin 1 connected?", test_DigitalIO_NC<MBED_CONF_APP_DIO_1>, greentea_failure_handler),
	Case("DigitalIO - is pin 2 connected?", test_DigitalIO_NC<MBED_CONF_APP_DIO_2>, greentea_failure_handler),
	Case("DigitalIO - is pin 3 connected?", test_DigitalIO_NC<MBED_CONF_APP_DIO_3>, greentea_failure_handler),
	Case("DigitalIO - is pin 4 connected?", test_DigitalIO_NC<MBED_CONF_APP_DIO_4>, greentea_failure_handler),
	Case("DigitalIO - is pin 5 connected?", test_DigitalIO_NC<MBED_CONF_APP_DIO_5>, greentea_failure_handler),
	Case("DigitalIO - is pin 6 connected?", test_DigitalIO_NC<MBED_CONF_APP_DIO_6>, greentea_failure_handler),
	Case("DigitalIO - is pin 7 connected?", test_DigitalIO_NC<MBED_CONF_APP_DIO_7>, greentea_failure_handler),
	Case("DigitalIO - is pin 8 connected?", test_DigitalIO_NC<MBED_CONF_APP_DIO_8>, greentea_failure_handler),
	Case("DigitalIO - is pin 9 connected?", test_DigitalIO_NC<MBED_CONF_APP_DIO_9>, greentea_failure_handler),

};

Specification specification(test_setup, cases);

int main() {
	return !Harness::run(specification);
}