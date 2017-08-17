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
 * 
 */
#include "TestFramework.hpp"

/*/////////////////////////////////////////////////////////////////////////////
//								Initialization								 //
/////////////////////////////////////////////////////////////////////////////*/
TestFramework::TestFramework() 
{
    #ifdef DEVICE_ANALOGIN
	map_pins(PinMap_ADC, AnalogInput);
    map_pins(PinMap_ADC, DigitalIO);
    #endif  // DEVICE_ANALOGIN

    #ifdef DEVICE_ANALOGOUT
    map_pins(PinMap_DAC, AnalogOutput);
  	// map_pins(PinMap_DAC, DigitalIO); grabbing pin number defined in PinNames.c, instead of the intended pin translation from mbed_app.json
    #endif  // DEVICE_ANALOGOUT

    #ifdef DEVICE_I2C
	map_pins(PinMap_I2C_SDA, DigitalIO);
	map_pins(PinMap_I2C_SCL, DigitalIO);
    map_pins(PinMap_I2C_SDA, I2C_SDA);
	map_pins(PinMap_I2C_SCL, I2C_SCL);
    #endif  // DEVICE_I2C

    #ifdef DEVICE_PWMOUT
    map_pins(PinMap_PWM, DigitalIO);
	map_pins(PinMap_PWM, PWM);
    #endif  // DEVICE_PWMOUT

    #ifdef DEVICE_SPI
    map_pins(PinMap_SPI_SCLK, DigitalIO);
	map_pins(PinMap_SPI_MOSI, DigitalIO);
	map_pins(PinMap_SPI_MISO, DigitalIO);
	map_pins(PinMap_SPI_SSEL, DigitalIO);
    map_pins(PinMap_SPI_SCLK, SPI_CLK);
	map_pins(PinMap_SPI_MOSI, SPI_MOSI);
	map_pins(PinMap_SPI_MISO, SPI_MISO);
	map_pins(PinMap_SPI_SSEL, SPI_CS);
    #endif  // DEVICE_SPI

	pinout[BusIO] = pinout[DigitalIO];
	setup_cits_pins();
}


void TestFramework::setup_cits_pins() 
{
	pinout[CITS_AnalogInput].push_back(construct_pinmap(MBED_CONF_APP_AIN_0));
	pinout[CITS_AnalogInput].push_back(construct_pinmap(MBED_CONF_APP_AIN_1));
	pinout[CITS_AnalogInput].push_back(construct_pinmap(MBED_CONF_APP_AIN_2));
	pinout[CITS_AnalogInput].push_back(construct_pinmap(MBED_CONF_APP_AIN_3));
	pinout[CITS_AnalogInput].push_back(construct_pinmap(MBED_CONF_APP_AIN_4));
	pinout[CITS_AnalogInput].push_back(construct_pinmap(MBED_CONF_APP_AIN_5));
	pinout[CITS_AnalogOutput].push_back(construct_pinmap(MBED_CONF_APP_AOUT));
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


PinMap TestFramework::construct_pinmap(PinName pin) 
{
    PinMap pinmap = {pin, 0, 0};
    return pinmap;
}


void TestFramework::map_pins(const PinMap pinmap[], Type pintype) 
{
    int i = 0;
    while(pinmap[i].pin != (PinName)NC) {
        bool alreadyExists = false;
        for (unsigned int j=0; j<pinout[pintype].size(); j++) {
            if (pinout[pintype][j].pin == pinmap[i].pin){
                alreadyExists = true;
            }
        }
        // push each valid pin onto the pinmap once for each pin. 
        #ifdef TARGET_STM
        // if testing on STM boards, do not add ADC internal channels as they will cause failures. Also do not include USBTX, and USBRX pins
        if ((!alreadyExists) && (pinmap[i].pin != USBTX) && (pinmap[i].pin != USBRX) && (pinmap[i].pin != ADC_TEMP) && (pinmap[i].pin != ADC_VREF) && (pinmap[i].pin != ADC_VBAT)){ 

        #else // else if not testing on STM boards
        // do not include USBTX and USBRX pins
        if ((!alreadyExists) && (pinmap[i].pin != USBTX) && (pinmap[i].pin != USBRX)){
        
        #endif // TARGET_STM
            pinout[pintype].push_back(pinmap[i]); // add pin to end of pinmap
        }
        i++;
    }
}


int TestFramework::find_pin(PinName pin, Type pintype) 
{
    // scan through all pins of specified type looking for a match
	for (unsigned int i=0; i<pinout[pintype].size(); i++) {
        // match is found, return index of matching pin
		if (pin == pinout[pintype][i].pin)
			return i;
	}
    // no matching pins found
	return -1;
}


PinName TestFramework::find_pin_pair(PinName pin) 
{
	PinName pinpair = pin;
    for (unsigned int i=0; i<pinout[CITS_DigitalIO].size(); i++) {
    	if (pinout[CITS_DigitalIO][i].pin == pin) {
    		pinpair = pinout[CITS_DigitalIO][i+(i%2 ? -1 : 1)].pin;
    		break;
    	}
    }
    return pinpair;
}


bool TestFramework::check_size(Type pintype) 
{
    return (pin_iterators[pintype] < pinout[pintype].size());
}


utest::v1::control_t TestFramework::reset_iterator(Type pintype) 
{
	if (check_size(pintype)) {
		return utest::v1::CaseRepeatAll;
    }
	else {
		pin_iterators[pintype] = 0;
		return utest::v1::CaseNext;
	}
}


PinMap TestFramework::get_increment_pin(Type pintype) 
{
    return pinout[pintype][pin_iterators[pintype]++];
}


std::vector<PinName> TestFramework::find_resistor_ladder_pins(PinName pin) 
{
	std::vector<PinName> resistor_ladder_pins;
	for (unsigned int i = 0; i<pinout[CITS_AnalogInput].size(); i++) {
		if (pinout[CITS_AnalogInput][i].pin != pin)
			resistor_ladder_pins.push_back(pinout[CITS_AnalogInput][i].pin);
	}
	return resistor_ladder_pins;
}


unsigned int TestFramework::get_seed() 
{
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
utest::v1::control_t TestFramework::run_i2c(void (*execution_callback)(PinName, PinName)) 
{
	PinMap sda_pin = pinout[I2C_SDA][pin_iterators[I2C_SDA]];
    // Tag is used to identify if a test case had been executed (true) or not (false)
	bool tag = false;

	// Itereate through the SCL pins to find a pin that matches the HW block of the SDA pin
	while (pin_iterators[I2C_SCL] < pinout[I2C_SCL].size()) {
    if (pinout[I2C_SCL][pin_iterators[I2C_SCL]].peripheral == sda_pin.peripheral) {
    	// Execution has already occurred, but another SCL pin was found that matches the SDA pin. Queue up another test case
    	if (tag) {return utest::v1::CaseRepeatAll;}
		// Matching SCL pin was found. Run the callback with the found pins
    	else {
	  		execution_callback(sda_pin.pin, pinout[I2C_SCL][pin_iterators[I2C_SCL]].pin);
	  		tag = true;
	  	}
    }
    pin_iterators[I2C_SCL]++;
	}
	// All the SCL pins have been found for the identified SDA pin. Increment the SDA pin
	if (!tag) {TEST_ASSERT(false);}
	pin_iterators[I2C_SDA]++;
	pin_iterators[I2C_SCL] = 0;
	// Check to see if there are any more SDA pins available. Move on to the next test case if invalid and reset the counters
	return reset_iterator(I2C_SDA);
}


utest::v1::control_t TestFramework::run_spi(void (*execution_callback)(PinName, PinName, PinName, PinName)) 
{
    bool tag = false;  // determines if execution_callback() was ran
  
    // Iterate through all CLK pins, finding the remaining SPI pins that have matching HW blocks 
	while (pin_iterators[SPI_CLK] < pinout[SPI_CLK].size()) {

		// Iterate through all the MISO pins
		while (pin_iterators[SPI_MISO] < pinout[SPI_MISO].size()) {
		
			// Iterate through all the MOSI pins
			while (pin_iterators[SPI_MOSI] < pinout[SPI_MOSI].size()) {
	
                // Iterate through all the CS pins
				while (pin_iterators[SPI_CS] < pinout[SPI_CS].size()) {
                    // create local variables to help make code easier to read
                    PinMap CLK = pinout[SPI_CLK][pin_iterators[SPI_CLK]];
                    PinMap MISO = pinout[SPI_MISO][pin_iterators[SPI_MISO]];
                    PinMap MOSI = pinout[SPI_MOSI][pin_iterators[SPI_MOSI]];
                    PinMap CS = pinout[SPI_CS][pin_iterators[SPI_CS]];

                    // ensure that chosen MISO, MOSI, and CS pins match the CLK pin's HW block
                    if ((MISO.peripheral == CLK.peripheral) && (MOSI.peripheral == CLK.peripheral) && (CS.peripheral == CLK.peripheral)){
                        // ensure that chosen SPI_CLK pin is not already assigned to another peripheral
                        if((CLK.pin != MISO.pin) && (CLK.pin != MOSI.pin) && (CLK.pin != CS.pin)){ 

                            // ensure that chosen SPI_MISO pin is not already assigned to another peripheral
                            if((MISO.pin != CLK.pin) && (MISO.pin != MOSI.pin) && (MISO.pin != CS.pin)){

                                // ensure that chosen SPI_MOSI pin is not already assigned to another peripheral
                                if((MOSI.pin != CLK.pin) && (MOSI.pin != MISO.pin) && (MOSI.pin != CS.pin)){
                          
                                    // ensure that chosen SPI_CS pin is not already assigned to another peripheral
                                    if((CS.pin != CLK.pin) && (CS.pin != MOSI.pin) && (CS.pin != MISO.pin)){
                                    
                                      // Found matching HW block pins. Run execution callback with them. 
                                      execution_callback(CLK.pin, MISO.pin, MOSI.pin, CS.pin);
                                      tag = true;
                                    }
                                }
                            }                         
                        }
                    }
                    pin_iterators[SPI_CS]++; // Increment and try next CS pin 
                    if(tag) {return utest::v1::CaseRepeatAll;}   
                }
                // Cycled through all CS pins. Reset CS iterator and increment MOSI iterator.
                pin_iterators[SPI_CS] = 0;
                pin_iterators[SPI_MOSI]++;   
            } 
            // Cycled through all MOSI pins. Reset MOSI iterator and increment MISO iterator.
            pin_iterators[SPI_MOSI] = 0;
            pin_iterators[SPI_MISO]++;     
        }
        // Cycled through all MISO pins. Reset MISO iterator and increment CLK iterator.
        pin_iterators[SPI_MISO] = 0;
        pin_iterators[SPI_CLK]++;
    }

	// All pins configurations have been iterated through. Reset pin iterators, run the CI test shield pin pair, and then move on to the next test case
	pin_iterators[SPI_CLK] = 0;
	pin_iterators[SPI_MISO] = 0;
	pin_iterators[SPI_MOSI] = 0;
	pin_iterators[SPI_CS] = 0;
	execution_callback(MBED_CONF_APP_SPI_CLK, MBED_CONF_APP_SPI_MISO, MBED_CONF_APP_SPI_MOSI, MBED_CONF_APP_SPI_CS);
	return utest::v1::CaseNext;
}


/*//////////////////////////////////////////////////////////////////////////////
//								level1 										  //
//																			  //
// Level 1 tests begin to test the full API for each hardware component.      //
// The API can only be tested on pins mapped to the CI test shield, so the    //
// following function iterates through all pins associated with the specified //
// hardware component until it finds a pin that is also mapped to the CI test //
// shield. Once a pin is found, it runs the callback passed in. Note that     //
// only one pin is tested for each hardware component.						  //
//////////////////////////////////////////////////////////////////////////////*/
utest::v1::control_t TestFramework::test_level1_framework(Type pintype, Type testtype, void (*execution_callback)(PinName, float, int), float floatdata, int intdata)
{
    while(check_size(pintype)) {
        // Get current pin specified by the stored iterator
        PinName pin = pinout[pintype][pin_iterators[pintype]].pin;

        // Check to see if current pin is available on the CI test shield
        int index = find_pin(pin, testtype);
        if (index != -1) {
            // Current pin is mapped to the CI test shield. Run the callback, reset the iterator, and run the next case
            execution_callback(pin, floatdata, intdata);
            pin_iterators[pintype] = 0;
            return utest::v1::CaseNext;
        }
        else{
            // Current pin was not mapped to CI test shield, try another pin
            pin_iterators[pintype]++;
        }
    }
}


/*/////////////////////////////////////////////////////////////////////////////
//								level2 										 //
//																			 //
// Level 2 tests test the full API for each hardware similar to level 1 but  //
// the difference is that the every pin mapped to the CI test shield (not    //
// just a single pin) is tested. In addition, the tolerance and iterations   //
// get a little bit more intense.											 //
/////////////////////////////////////////////////////////////////////////////*/
utest::v1::control_t TestFramework::test_level2_framework(Type pintype, Type testtype, void (*execution_callback)(PinName, float, int), float floatdata, int intdata) 
{
	// Get current pin specified by the stored iterator
	PinName pin = pinout[pintype][pin_iterators[pintype]].pin;
    // Check to see if that current pin is available on the CI test shield
	int index = find_pin(pin, testtype);
    // Tag is used to identify if a test case had been executed (true) or not (false)
	bool tag = false;
	// State: Execute
	if (index != -1) {
		// Pin was found, run the callback and specify the tag to true (run)
		execution_callback(pin, floatdata, intdata);
		tag = true;
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
				return utest::v1::CaseRepeatAll;
			// State: Execute
			// Pin was found and execution has not occurred yet
			} 
            else {
				execution_callback(pin, floatdata, intdata);
				tag = true;
			}
		}
	}
	// State: End test
	// All pins have been identified and run, reset the iterators and return back
	return reset_iterator(pintype);
}
