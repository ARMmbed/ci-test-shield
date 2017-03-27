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
 *
 * @author Michael Ray
 * @since 3/22/2017
 * @version 1.0.0
 * 
 */
#include "TestFramework.hpp"

/*/////////////////////////////////////////////////////////////////////////////
//								Initialization								 //
/////////////////////////////////////////////////////////////////////////////*/

/**
  * Constructor for the test framework. Add all of the PinMaps to the pinout, as well as CI test shield pins
**/
TestFramework::TestFramework() {
	map_pins(PinMap_ADC, AnalogInput);
	map_pins(PinMap_DAC, AnalogOutput);
	map_pins(PinMap_PWM, DigitalIO);
	map_pins(PinMap_ADC, DigitalIO);
	// map_pins(PinMap_DAC, DigitalIO);
	map_pins(PinMap_I2C_SDA, DigitalIO);
	map_pins(PinMap_I2C_SCL, DigitalIO);
	map_pins(PinMap_SPI_SCLK, DigitalIO);
	map_pins(PinMap_SPI_MOSI, DigitalIO);
	// map_pins(PinMap_SPI_MISO, DigitalIO);
	map_pins(PinMap_SPI_SSEL, DigitalIO);
	pinout[BusIO] = pinout[DigitalIO];
	map_pins(PinMap_PWM, PWM);
	map_pins(PinMap_I2C_SDA, I2C_SDA);
	map_pins(PinMap_I2C_SCL, I2C_SCL);
	map_pins(PinMap_SPI_SCLK, SPI_CLK);
	map_pins(PinMap_SPI_MOSI, SPI_MOSI);
	map_pins(PinMap_SPI_MISO, SPI_MISO);
	map_pins(PinMap_SPI_SSEL, SPI_CS);
	setup_cits_pins();
}

/**
  * Add all of the CI Test Shield pins to the pinout vector
**/
void TestFramework::setup_cits_pins() {
	pinout[CITS_AnalogInput].push_back(construct_pinmap(MBED_CONF_APP_AIN_0));
	pinout[CITS_AnalogInput].push_back(construct_pinmap(MBED_CONF_APP_AIN_1));
	pinout[CITS_AnalogInput].push_back(construct_pinmap(MBED_CONF_APP_AIN_2));
	pinout[CITS_AnalogInput].push_back(construct_pinmap(MBED_CONF_APP_AIN_3));
	pinout[CITS_AnalogInput].push_back(construct_pinmap(MBED_CONF_APP_AIN_4));
	pinout[CITS_AnalogInput].push_back(construct_pinmap(MBED_CONF_APP_AIN_5));
	pinout[CITS_AnalogOutput].push_back(construct_pinmap(MBED_CONF_APP_AOUT));
	pinout[CITS_DigitalIO].push_back(construct_pinmap(MBED_CONF_APP_DIO_0));
	pinout[CITS_DigitalIO].push_back(construct_pinmap(MBED_CONF_APP_DIO_1));
	pinout[CITS_DigitalIO].push_back(construct_pinmap(MBED_CONF_APP_DIO_2));
	pinout[CITS_DigitalIO].push_back(construct_pinmap(MBED_CONF_APP_DIO_3));
	pinout[CITS_DigitalIO].push_back(construct_pinmap(MBED_CONF_APP_DIO_4));
	pinout[CITS_DigitalIO].push_back(construct_pinmap(MBED_CONF_APP_DIO_5));
	pinout[CITS_DigitalIO].push_back(construct_pinmap(MBED_CONF_APP_DIO_6));
	pinout[CITS_DigitalIO].push_back(construct_pinmap(MBED_CONF_APP_DIO_7));
	pinout[CITS_DigitalIO].push_back(construct_pinmap(MBED_CONF_APP_DIO_8));
	pinout[CITS_DigitalIO].push_back(construct_pinmap(MBED_CONF_APP_DIO_9));
	pinout[CITS_PWM].push_back(construct_pinmap(MBED_CONF_APP_PWM_0));
	pinout[CITS_PWM].push_back(construct_pinmap(MBED_CONF_APP_PWM_1));
	pinout[CITS_PWM].push_back(construct_pinmap(MBED_CONF_APP_PWM_2));
	pinout[CITS_PWM].push_back(construct_pinmap(MBED_CONF_APP_PWM_3));
	pinout[CITS_I2C_SDA].push_back(construct_pinmap(MBED_CONF_APP_I2C_SDA));
	pinout[CITS_I2C_SCL].push_back(construct_pinmap(MBED_CONF_APP_I2C_SCL));
	pinout[CITS_SPI_CLK].push_back(construct_pinmap(MBED_CONF_APP_SPI_CLK));
	pinout[CITS_SPI_MISO].push_back(construct_pinmap(MBED_CONF_APP_SPI_MISO));
	pinout[CITS_SPI_MOSI].push_back(construct_pinmap(MBED_CONF_APP_SPI_MOSI));
	pinout[CITS_SPI_CS].push_back(construct_pinmap(MBED_CONF_APP_SPI_CS));
}

PinMap TestFramework::construct_pinmap(PinName pin) {
	PinMap pinmap = {pin, 0, 0};
	return pinmap;
}

/**
  * Put pins from the specified pinmap into the pinout array indexed by pin type
  * @param PinMap[] Found in the hal, specifies all the pins with a certain type
  * @param Type which type the pins belong to (used to index into pinout)
**/
void TestFramework::map_pins(const PinMap pinmap[], Type pintype) {
	int i = 0;
	while(pinmap[i].pin != (PinName)NC) {
		bool alreadyExists = false;
		// printf("Pin %d\n", pinmap[i].pin);
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

/**
  * Find the pin that is connected to the specified pin via a resistor on the DIO HW bank
  * @param PinName pin to find the corresponding pin connected by the resistor
  * @return PinName pin that is connected to the input pin
**/ 
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
  * Check to see if the pin iterator is pointing at a valid pin in the pinout
  * @param Type pin type to validate
  * @return bool if the pin iterator is a valid pin in the pinout
**/
bool TestFramework::check_size(Type pintype) {
	return (pin_iterators[pintype] < pinout[pintype].size());
}

/**
  * Reset the iterator if all pins of the specified pin type have been looked at,<br>
  *   or proceed to the next pin of the pin type if there are remaining pins
  * @param Type pin type to reset
  * @return control_t Case control statement to repeat or move on to the next case
**/
utest::v1::control_t TestFramework::reset_iterator(Type pintype) {
	if (check_size(pintype))
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

/**
  * Get a randomized seed from Greentea's host test
  * @return unsigned int random seed
**/
unsigned int TestFramework::get_seed() {
	// Send key value pair to greentea host tests
    greentea_send_kv("return_rand", "seed");
    char key[16] = {0};
    char value[32] = {0};
    unsigned int seed;
    // Gather the response from the host test
    greentea_parse_kv(key, value, sizeof(key), sizeof(value));
    // Convert the string result to an unsigned int
    stringstream strValue;
	strValue << value;
	strValue >> seed;
	return seed;
}

/*/////////////////////////////////////////////////////////////////////////////
//								level0 										 //
//																			 //
// Level 0 tests test the constructor and deconstructor of each pin of a     //
// specified API. Every pin available for the specified API (ex. Analog in,  //
// digital io, SPI), gets tested regardless of if it is connected to the CI  //
// test shield.																 //
/////////////////////////////////////////////////////////////////////////////*/

/**
  * Find a matching pin based on HW blocks to the current pin. Iterate the corresponding pin after test case execution
  * @param Callback to a function to run once a pin pairing has been found
  * @return control_t Case control to repeat or move on to next test cases
**/
utest::v1::control_t TestFramework::run_i2c(void (*execution_callback)(PinName, PinName)) {
	PinMap sda_pin = pinout[I2C_SDA][pin_iterators[I2C_SDA]];
	int tag = 0;

	// Itereate through the SCL pins to find a pin that matches the HW block of the SDA pin
	while (pin_iterators[I2C_SCL] < pinout[I2C_SCL].size()) {
    	if (pinout[I2C_SCL][pin_iterators[I2C_SCL]].peripheral == sda_pin.peripheral) {

    		// Execution has already occurred, but another SCL pin was found that matches the SDA pin. Queue up another test case
    		if (tag)
				return utest::v1::CaseRepeatAll;
			// Matching SCL pin was found. Run the callback with the found pins
    		else {
	    		execution_callback(sda_pin.pin, pinout[I2C_SCL][pin_iterators[I2C_SCL]].pin);
	    		tag = 1;
	    	}
    	}
    	pin_iterators[I2C_SCL]++;
	}
	// All the SCL pins have been found for the identified SDA pin. Increment the SDA pin
	if (!tag) { TEST_ASSERT(false);}
	pin_iterators[I2C_SDA]++;
	pin_iterators[I2C_SCL] = 0;
	// Check to see if the SDA pin. Move on to the next test case if invalid and reset the counters
	return reset_iterator(I2C_SDA);
}

/**
  * Find a pin set (CLK, MISO, MOSI, CS) based on HW blocks that matches the current pin. Iterate the corresponding pins after test execution
  * @param Callback to a function to run once a pin pairing has been found
  * @return control_t Case control to repeat or move on to the next test cases
**/
utest::v1::control_t TestFramework::run_spi(void (*execution_callback)(PinName, PinName, PinName, PinName)) {
	// Check to make sure a CLK pin is still available
	if (pin_iterators[SPI_CLK] < pinout[SPI_CLK].size()) {

		// Iterate through the MISO pins to find a matching HW block pin to the CLK pin
		while (pin_iterators[SPI_MISO] < pinout[SPI_MISO].size()) {
			if (pinout[SPI_MISO][pin_iterators[SPI_MISO]].peripheral == pinout[SPI_CLK][pin_iterators[SPI_CLK]].peripheral) {

				// Iterate through the MOSI pins to find a matching HW block pin to the CLK pin
				while (pin_iterators[SPI_MOSI] < pinout[SPI_MOSI].size()) {
					if (pinout[SPI_MOSI][pin_iterators[SPI_MOSI]].peripheral == pinout[SPI_CLK][pin_iterators[SPI_CLK]].peripheral) {

						// Iterate through the CS pins to find a matching HW block pin to the CLK pin
						while (pin_iterators[SPI_CS] < pinout[SPI_CS].size()) {
							if (pinout[SPI_MOSI][pin_iterators[SPI_MOSI]].peripheral == pinout[SPI_CLK][pin_iterators[SPI_CLK]].peripheral) {

								// Found all matching HW block pins. Run the execution callback with the identified pins
								execution_callback(pinout[SPI_CLK][pin_iterators[SPI_CLK]++].pin,
													pinout[SPI_MISO][pin_iterators[SPI_MISO]].pin,
													pinout[SPI_MOSI][pin_iterators[SPI_MOSI]].pin,
													pinout[SPI_CS][pin_iterators[SPI_CS]].pin);
								// Reset the iterators
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
	// All CLK pins have been iterated through. Reset the CLK pin iterator, run the CI test shield pin pair, and then move on to the next test case
	pin_iterators[SPI_CLK] = 0;
	execution_callback(MBED_CONF_APP_SPI_CLK, MBED_CONF_APP_SPI_MISO, MBED_CONF_APP_SPI_MOSI, MBED_CONF_APP_SPI_CS);
	return utest::v1::CaseNext;
}

/*/////////////////////////////////////////////////////////////////////////////
//								level1 										 //
//																			 //
// Level 1 tests begin to test the full API for each hardware component.     //
// The API can only be tested on pins mapped to the CI test shield, so the   //
// following function iterates through all pins associated with the specified//
// hardware component to find pins mapped to the CI test shield. Once a pin  //
// is found, it runs the callback passed in. Note that only one pin is tested//
// for each hardware component.												 //
/////////////////////////////////////////////////////////////////////////////*/

/**
  * Find a pin of a specified test type and run a test case (passed in as a callback)
  * @param Type type of pin to find to run the callback
  * @param Type corresponding type of pin on the CI test shield
  * @param Callback to the test case to run once a pin is found
  * @param float data in float form to pass to the execution callback. Depends on callback
  * @param int data in int form to pass to the execution callback. Depends on callback
  * @return control_t identify to repeat or proceed to the next test case corresponding to the Specification
**/
utest::v1::control_t TestFramework::test_level1_framework(Type pintype, Type testtype, void (*execution_callback)(PinName, float, int), float floatdata, int intdata) {
	// Get current pin specified by the stored iterator
	PinName pin = pinout[pintype][pin_iterators[pintype]].pin;
	// Check to see if that current pin is available on the CI test shield
	int index = find_pin(pin, testtype);
	// State: Execute
	if (index != -1) {
		// Current pin was mapped to the CI test shield so run the callback and reset the iterator
		execution_callback(pin, floatdata, intdata);
		pin_iterators[pintype] = 0;
		return utest::v1::CaseNext;
	}
	while (check_size(pintype)) {
		// State: Increment iterator
		pin_iterators[pintype]++;
		pin = pinout[pintype][pin_iterators[pintype]].pin;
		// Check to see if the current pin is available on the CI test shield
		index = find_pin(pin, testtype);

		if (index != -1) {
			// State: Execute
			// Pin was found, so execute the callback and end the test case. Make sure to reset the iterator
			execution_callback(pin, floatdata, intdata);
			pin_iterators[pintype] = 0;
			return utest::v1::CaseNext;
		}
	}
	// State: End test
	// All pins have been identified and run, reset the iterators and return back
	return reset_iterator(pintype);
}

/*/////////////////////////////////////////////////////////////////////////////
//								level2 										 //
//																			 //
// Level 2 tests test the full API for each hardware similar to level 1 but  //
// the difference is that the every pin mapped to the CI test shield (not    //
// just a single pin) is tested. In addition, the tolerance and iterations   //
// get a little bit more intense.											 //
/////////////////////////////////////////////////////////////////////////////*/

/**
  * Find pins of a specified test type and run a test case (passed in as a callback)
  *   Difference from the test_level1_framework function is this runs on ALL pins of a specified test type
  * @param Type type of pin to find to run the callback
  * @param Type corresponding type of pin on the CI test shield
  * @param Callback to the test case to run once a pin is found
  * @param float data in float form to pass to the execution callback. Depends on callback
  * @param int data in int form to pass to the execution callback. Depends on callback
  * @return control_t identify to repeat or proceed to the next test case corresponding to the Specification
**/
utest::v1::control_t TestFramework::test_level2_framework(Type pintype, Type testtype, void (*execution_callback)(PinName, float, int), float floatdata, int intdata) {
	// Get current pin specified by the stored iterator
	PinName pin = pinout[pintype][pin_iterators[pintype]].pin;
	// Check to see if that current pin is available on the CI test shield
	int index = find_pin(pin, testtype);
	// Tag is used to identify if a test case had been executed (1) or not (0)
	int tag = 0;
	// State: Execute
	if (index != -1) {
		// Pin was found, run the callback and specify the tag to 1 (run)
		execution_callback(pin, floatdata, intdata);
		tag = 1;
	}
	// Search the remaining pins of the pintype to find a pin on the CI test shield
	while (check_size(pintype)) {
		// State: Increment iterator
		pin_iterators[pintype]++;
		pin = pinout[pintype][pin_iterators[pintype]].pin;
		// Check to see if the current pin is available on the CI test shield
		index = find_pin(pin, testtype);

		if (index != -1) {
			// State: End case
			// Pin was found, but execution had already occurred so queue up another test case
			if (tag) {
				TEST_ASSERT(true);
				return utest::v1::CaseRepeatAll;
			// State: Execute
			// Pin was found and execution has not occurred yet
			} else {
				execution_callback(pin, floatdata, intdata);
				tag = 1;
			}
		}
	}
	// State: End test
	// All pins have been identified and run, reset the iterators and return back
	return reset_iterator(pintype);
}