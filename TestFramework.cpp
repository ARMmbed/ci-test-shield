/*
 * Copyright (c) 2016 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "TestFramework.hpp"

/*/////////////////////////////////////////////////////////////////////////////
//								Initialization								 //
/////////////////////////////////////////////////////////////////////////////*/

TestFramework::TestFramework() {
	map_pins(PinMap_ADC, AnalogInput);
	map_pins(PinMap_DAC, AnalogOutput);
	map_pins(PinMap_PWM, DigitalIO);
	map_pins(PinMap_ADC, DigitalIO);
	map_pins(PinMap_PWM, PWM);
	map_pins(PinMap_I2C_SDA, I2C_SDA);
	map_pins(PinMap_I2C_SCL, I2C_SCL);
	map_pins(PinMap_SPI_SCLK, SPI_CLK);
	map_pins(PinMap_SPI_MOSI, SPI_MOSI);
	map_pins(PinMap_SPI_MISO, SPI_MISO);
	map_pins(PinMap_SPI_SSEL, SPI_CS);
	setup_cits_pins();
}

void TestFramework::setup_cits_pins() {
	pinout[CITS_AnalogInput].push_back({MBED_CONF_APP_AIN_0, 0, 0});
	pinout[CITS_AnalogInput].push_back({MBED_CONF_APP_AIN_1, 0, 0});
	pinout[CITS_AnalogInput].push_back({MBED_CONF_APP_AIN_2, 0, 0});
	pinout[CITS_AnalogInput].push_back({MBED_CONF_APP_AIN_3, 0, 0});
	pinout[CITS_AnalogInput].push_back({MBED_CONF_APP_AIN_4, 0, 0});
	pinout[CITS_AnalogInput].push_back({MBED_CONF_APP_AIN_5, 0, 0});
	pinout[CITS_AnalogOutput].push_back({MBED_CONF_APP_AOUT, 0, 0});
	pinout[CITS_DigitalIO].push_back({MBED_CONF_APP_DIO_0, 0, 0});
	pinout[CITS_DigitalIO].push_back({MBED_CONF_APP_DIO_1, 0, 0});
	pinout[CITS_DigitalIO].push_back({MBED_CONF_APP_DIO_2, 0, 0});
	pinout[CITS_DigitalIO].push_back({MBED_CONF_APP_DIO_3, 0, 0});
	pinout[CITS_DigitalIO].push_back({MBED_CONF_APP_DIO_4, 0, 0});
	pinout[CITS_DigitalIO].push_back({MBED_CONF_APP_DIO_5, 0, 0});
	pinout[CITS_DigitalIO].push_back({MBED_CONF_APP_DIO_6, 0, 0});
	pinout[CITS_DigitalIO].push_back({MBED_CONF_APP_DIO_7, 0, 0});
	pinout[CITS_DigitalIO].push_back({MBED_CONF_APP_DIO_8, 0, 0});
	pinout[CITS_DigitalIO].push_back({MBED_CONF_APP_DIO_9, 0, 0});
	pinout[CITS_PWM].push_back({MBED_CONF_APP_PWM_0, 0, 0});
	pinout[CITS_PWM].push_back({MBED_CONF_APP_PWM_1, 0, 0});
	pinout[CITS_PWM].push_back({MBED_CONF_APP_PWM_2, 0, 0});
	pinout[CITS_PWM].push_back({MBED_CONF_APP_PWM_3, 0, 0});
	pinout[CITS_I2C_SDA].push_back({MBED_CONF_APP_I2C_SDA, 0, 0});
	pinout[CITS_I2C_SCL].push_back({MBED_CONF_APP_I2C_SCL, 0, 0});
	pinout[CITS_SPI_CLK].push_back({MBED_CONF_APP_SPI_CLK, 0, 0});
	pinout[CITS_SPI_MISO].push_back({MBED_CONF_APP_SPI_MISO, 0, 0});
	pinout[CITS_SPI_MOSI].push_back({MBED_CONF_APP_SPI_MOSI, 0, 0});
	pinout[CITS_SPI_CS].push_back({MBED_CONF_APP_SPI_CS, 0, 0});
}

/**
  * Put pins from the specified pinmap into the pinout array indexed by pin type
  * @param PinMap[] Found in the hal, specifies all the pins with a certain type
  * @param Type which type the pins belong to (used to index into pinout)
**/
void TestFramework::map_pins(const PinMap pinmap[], Type pintype) {
	int i = 0;
	while(pinmap[i].pin != (PinName)NC) {
		// printf("Pin %d\n", pinmap[i].pin);
		bool alreadyExists = false;
		for (unsigned int j=0; j<pinout[pintype].size(); j++) {
			if (pinout[pintype][j].pin == pinmap[i].pin)
				alreadyExists = true;
		}
		if (!alreadyExists) {
			pinout[pintype].push_back(pinmap[i]);
		}
		i++;
	}
}

/**
  * Find a pin within the array of type pintype
  * @param PinName pin to search for
  * @param Type pin type to in which the pin has to be a part of
  * @return index where the pin is found. Returns -1 if can't be found
**/
int TestFramework::find_pin(PinName pin, Type pintype) {
	for (unsigned int i=0; i<pinout[pintype].size(); i++) {
		if (pin == pinout[pintype][i].pin)
			return i;
	}
	return -1;
}

PinName TestFramework::find_pin_pair(PinName pin) {
	PinName pinpair = pin;
    for (unsigned int i=0; i<pinout[CITS_DigitalIO].size(); i++) {
    	if (pinout[CITS_DigitalIO][i].pin == pin) {
    		pinpair = pinout[CITS_DigitalIO][i+(i%2 ? -1 : 1)].pin;
    		break;
    	}
    }
    return pinpair;
}

/**
  * Reset the iterator if all pins of the specified pin type have been looked at,<br>
  *   or proceed to the next pin of the pin type if there are remaining pins
  * @param Type pin type to reset
  * @return control_t Case control statement to repeat or move on to the next case
**/
utest::v1::control_t TestFramework::reset_iterator(Type pintype) {
	if (pin_iterators[pintype] < pinout[pintype].size())
		return utest::v1::CaseRepeatAll;
	else {
		pin_iterators[pintype] = 0;
		return utest::v1::CaseNext;
	}
}

/**
  * Get the current pin for a specific pin type, and incrememnt the iterator for that pin type
  * @param Type pin type to retrieve and increment
  * @return PinName current pin
**/
PinMap TestFramework::get_increment_pin(Type pintype) {
	return pinout[pintype][pin_iterators[pintype]++];
}

/**
  * Find the resistor ladder pins that are not the specified pin
  * @param PinName pin that should not be included in the resistor ladder
  * @return vector<PinName> list of resistor ladder pins
**/
std::vector<PinName> TestFramework::find_resistor_ladder_pins(PinName pin) {
	std::vector<PinName> resistor_ladder_pins;
	for (unsigned int i = 0; i<pinout[CITS_AnalogInput].size(); i++) {
		if (pinout[CITS_AnalogInput][i].pin != pin)
			resistor_ladder_pins.push_back(pinout[CITS_AnalogInput][i].pin);
	}
	return resistor_ladder_pins;
}

/*/////////////////////////////////////////////////////////////////////////////
//								level0 										 //
/////////////////////////////////////////////////////////////////////////////*/

/**
  * Find a matching pin based on HW blocks to the current pin. Iterate the corresponding pin after test case execution
  * @param Callback to a function to run once a pin pairing has been found
  * @return control_t Case control to repeat or move on to next test cases
**/
utest::v1::control_t TestFramework::run_i2c(void (*execution_callback)(PinName, PinName)) {
	PinMap sda_pin = pinout[I2C_SDA][pin_iterators[I2C_SDA]];
	int tag = 0;

	while (pin_iterators[I2C_SCL] < pinout[I2C_SCL].size()) {
    	if (pinout[I2C_SCL][pin_iterators[I2C_SCL]].peripheral == sda_pin.peripheral) {
    		if (tag)
				return utest::v1::CaseRepeatAll;
    		else {
	    		execution_callback(sda_pin.pin, pinout[I2C_SCL][pin_iterators[I2C_SCL]].pin);
	    		tag = 1;
	    	}
    	}
    	pin_iterators[I2C_SCL]++;
	}
	if (!tag) TEST_ASSERT(false);
	pin_iterators[I2C_SDA]++;
	pin_iterators[I2C_SCL] = 0;
	return reset_iterator(I2C_SDA);
}

/**
  * Find a pin set (CLK, MISO, MOSI, CS) based on HW blocks that matches the current pin. Iterate the corresponding pins after test execution
  * @param Callback to a function to run once a pin pairing has been found
  * @return control_t Case control to repeat or move on to the next test cases
**/
utest::v1::control_t TestFramework::run_spi(void (*execution_callback)(PinName, PinName, PinName, PinName)) {
	if (pin_iterators[SPI_CLK] < pinout[SPI_CLK].size()) {
		while (pin_iterators[SPI_MISO] < pinout[SPI_MISO].size()) {
			if (pinout[SPI_MISO][pin_iterators[SPI_MISO]].peripheral == pinout[SPI_CLK][pin_iterators[SPI_CLK]].peripheral) {
				while (pin_iterators[SPI_MOSI] < pinout[SPI_MOSI].size()) {
					if (pinout[SPI_MOSI][pin_iterators[SPI_MOSI]].peripheral == pinout[SPI_CLK][pin_iterators[SPI_CLK]].peripheral) {
						while (pin_iterators[SPI_CS] < pinout[SPI_CS].size()) {
							if (pinout[SPI_MOSI][pin_iterators[SPI_MOSI]].peripheral == pinout[SPI_CLK][pin_iterators[SPI_CLK]].peripheral) {
								execution_callback(pinout[SPI_CLK][pin_iterators[SPI_CLK]++].pin,
													pinout[SPI_MISO][pin_iterators[SPI_MISO]].pin,
													pinout[SPI_MOSI][pin_iterators[SPI_MOSI]].pin,
													pinout[SPI_CS][pin_iterators[SPI_CS]].pin);
								pin_iterators[SPI_MISO] = 0;
								pin_iterators[SPI_MOSI] = 0;
								pin_iterators[SPI_CS] = 0;
								return utest::v1::CaseRepeatAll;
							}
							pin_iterators[SPI_CS]++;
						}
						TEST_ASSERT(false);
					}
					pin_iterators[SPI_MOSI]++;
				}
				TEST_ASSERT(false);
			}
			pin_iterators[SPI_MISO]++;
		}
		TEST_ASSERT(false);
	}
	pin_iterators[SPI_CLK] = 0;
	execution_callback(MBED_CONF_APP_SPI_CLK, MBED_CONF_APP_SPI_MISO, MBED_CONF_APP_SPI_MOSI, MBED_CONF_APP_SPI_CS);
	return utest::v1::CaseNext;
}

/*/////////////////////////////////////////////////////////////////////////////
//								level1 										 //
/////////////////////////////////////////////////////////////////////////////*/

utest::v1::control_t TestFramework::test_level1_framework(Type pintype, Type testtype, void (*execution_callback)(PinName, float, int), float floatdata, int intdata) {
	PinName pin = pinout[pintype][pin_iterators[pintype]].pin;
	int index = find_pin(pin, testtype);
	int tag = 0;
	// State: Execute
	if (index != -1) {
		execution_callback(pin, floatdata, intdata);
		pin_iterators[pintype] = 0;
		return utest::v1::CaseNext;
	}
	while (pin_iterators[pintype] < pinout[pintype].size()) {
		// State: Increment iterator
		pin_iterators[pintype]++;
		pin = pinout[pintype][pin_iterators[pintype]].pin;
		index = find_pin(pin, testtype);
		if (index != -1) {
			// State: Execute
			execution_callback(pin, floatdata, intdata);
			pin_iterators[pintype] = 0;
			return utest::v1::CaseNext;
		}
	}
	// State: End test
	return reset_iterator(pintype);
}

// utest::v1::control_t TestFramework::test_level1_spi(const size_t call_count) {
// 	return test_level1_framework(SPI_CLK, CITS_SPI_CLK, &test_spi_execute, 0.05, 10);
// }

/*/////////////////////////////////////////////////////////////////////////////
//								level2 										 //
/////////////////////////////////////////////////////////////////////////////*/

utest::v1::control_t TestFramework::test_level2_framework(Type pintype, Type testtype, void (*execution_callback)(PinName, float, int), float floatdata, int intdata) {
	PinName pin = pinout[pintype][pin_iterators[pintype]].pin;
	int index = find_pin(pin, testtype);
	int tag = 0;
	// State: Execute
	if (index != -1) {
		execution_callback(pin, floatdata, intdata);
		tag = 1;
	}
	while (pin_iterators[pintype] < pinout[pintype].size()) {
		// State: Increment iterator
		pin_iterators[pintype]++;
		pin = pinout[pintype][pin_iterators[pintype]].pin;
		index = find_pin(pin, testtype);

		if (index != -1) {
			// State: End case
			if (tag) {
				TEST_ASSERT(true);
				return utest::v1::CaseRepeatAll;
			// State: Execute
			} else {
				execution_callback(pin, floatdata, intdata);
				tag = 1;
			}
		}
	}
	// State: End test
	return reset_iterator(pintype);
}

// utest::v1::control_t TestFramework::test_level2_i2c(const size_t call_count) {
// 	return test_level2_framework(I2C_SCL, CITS_I2C_SCL, &test_i2c_execute, 0.01, 100);
// }

// utest::v1::control_t TestFramework::test_level2_spi(const size_t call_count) {
// 	return test_level2_framework(SPI_CLK, CITS_SPI_CLK, &test_spi_execute, 0.01, 100);
// }

/*/////////////////////////////////////////////////////////////////////////////
//									Tests									 //
/////////////////////////////////////////////////////////////////////////////*/

// void TestFramework::test_spi_execute(PinName pin_clk, float null_float, int null_int) {
// 	char test_string_write[TEST_STRING_MAX] = "TestFramework test";

// 	PinName pin_mosi = pinout[SPI_MOSI][pin_iterators[SPI_CLK]].pin;
// 	PinName pin_miso = pinout[SPI_MISO][pin_iterators[SPI_CLK]].pin;
// 	PinName pin_cs = pinout[SPI_CS][pin_iterators[SPI_CLK]].pin;
// 	DEBUG_PRINTF("Running SPI constructor on CLK pin %d, MISO pin %d, MOSI pin %d, and CS pin %d\n", pin_clk, pin_miso, pin_mosi, pin_cs);
//     TEST_ASSERT_MESSAGE(pin_clk != NC, "SPI CLK pin is NC");
//     TEST_ASSERT_MESSAGE(pin_mosi != NC, "SPI MOSI Pin is NC");
//     TEST_ASSERT_MESSAGE(pin_miso != NC, "SPI MISO Pin is NC");
//     TEST_ASSERT_MESSAGE(pin_cs != NC, "SPI CS Pin is NC");

//     SDFileSystem sd(pin_mosi, pin_miso, pin_clk, pin_cs, "sd");
//     FILE *File_write = fopen("/sd/test_card.txt", "w"); // open File_write
//     TEST_ASSERT_MESSAGE(File_write != NULL,"SD Card is not present. Please insert an SD Card.");
//     TEST_ASSERT_MESSAGE(fprintf(File_write, test_string_write) > 0,"Writing File to sd card failed"); // write data
//     fclose(File_write);// close file on SD

//     FILE *File_read = fopen("/sd/test_card.txt", "r"); // open File_read
//     char test_string_read [TEST_STRING_MAX] = {0};
//     fgets(test_string_read,TEST_STRING_MAX,File_read); // read string from the file
//     DEBUG_PRINTF("Read '%s' in read test\nString comparison returns %d\n",test_string_read,strcmp(test_string_read,test_string_write));
//     TEST_ASSERT_MESSAGE(strcmp(test_string_read,test_string_write) == 0,"String read does not match string written"); // test that strings match

//     // fclose(File);// close file on SD

//     TEST_ASSERT(true);
// }