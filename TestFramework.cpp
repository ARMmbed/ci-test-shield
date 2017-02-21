#include "TestFramework.hpp"

TestFramework::TestFramework() {
	map_pins(PinMap_ADC, &analog_in_pins);
	map_pins(PinMap_DAC, &analog_out_pins);
}

void TestFramework::map_pins(const PinMap pinmap[], std::vector<PinName> * pin_list) {
	int i = 0;
	while(pinmap[i].pin != (PinName)NC) {
		pin_list->push_back(pinmap[i].pin);
		i++;
	}
}

utest::v1::control_t TestFramework::test_l0_analogin(const size_t call_count) {
	DEBUG_PRINTF("Running analog input constructor on pin %d\n", analog_in_pins[analog_pin_index]);
	AnalogIn ain(analog_in_pins[analog_pin_index++]);
	TEST_ASSERT(true);
	if (call_count < analog_in_pins.size()) {
		return utest::v1::CaseRepeatAll;
	} else {
		analog_pin_index = 0;
		return utest::v1::CaseNext;
	}
}

utest::v1::control_t TestFramework::test_l0_analogout(const size_t call_count) {
	DEBUG_PRINTF("Running analog output constructor on pin %d\n", analog_out_pins[analog_pin_index]);
	AnalogOut aout(analog_out_pins[analog_pin_index++]);
	TEST_ASSERT(true);
	if (call_count < analog_out_pins.size()) {
		return utest::v1::CaseRepeatAll;
	} else {
		analog_pin_index = 0;
		return utest::v1::CaseNext;
	}
}