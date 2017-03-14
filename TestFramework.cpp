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

utest::v1::control_t TestFramework::test_level1_digitalio(const size_t call_count) {
	return test_level1_framework(DigitalIO, CITS_DigitalIO, &test_digitalio_execute, 0.05, 10);
}

utest::v1::control_t TestFramework::test_level1_analogin(const size_t call_count) {
	return test_level1_framework(AnalogInput, CITS_AnalogInput, &test_analogin_execute, 0.05, 10);
}

utest::v1::control_t TestFramework::test_level1_analogout(const size_t call_count) {
	return test_level1_framework(AnalogOutput, CITS_AnalogOutput, &test_analogout_execute, 0.05, 10);
}

// EXISTS IN HEADER - DO NOT UNCOMMENT - HERE FOR REFERENCE
// template <int dutycycle, int period> 
// utest::v1::control_t TestFramework::test_level1_pwm(const size_t call_count) {
// 	return test_level1_framework(PWM, CITS_PWM, &test_pwm_execute, dutycycle*0.01f, period);
// }

utest::v1::control_t TestFramework::test_level1_i2c(const size_t call_count) {
	return test_level1_framework(I2C_SCL, CITS_I2C_SCL, &test_i2c_execute, 0.05, 10);
}

utest::v1::control_t TestFramework::test_level1_spi(const size_t call_count) {
	return test_level1_framework(SPI_CLK, CITS_SPI_CLK, &test_spi_execute, 0.05, 10);
}

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

utest::v1::control_t TestFramework::test_level2_digitalio(const size_t call_count) {
	return test_level2_framework(DigitalIO, CITS_DigitalIO, &test_digitalio_execute, 0.01, 100);
}

utest::v1::control_t TestFramework::test_level2_analogin(const size_t call_count) {
	return test_level2_framework(AnalogInput, CITS_AnalogInput, &test_analogin_execute, 0.01, 100);
}

utest::v1::control_t TestFramework::test_level2_analogout(const size_t call_count) {
	return test_level2_framework(AnalogOutput, CITS_AnalogOutput, &test_analogout_execute, 0.01, 100);
}

// EXISTS IN HEADER - DO NOT UNCOMMENT - HERE FOR REFERENCE
// template <int dutycycle, int period> 
// utest::v1::control_t TestFramework::test_level2_pwm(const size_t call_count) {
// 	return test_level2_framework(PWM, CITS_PWM, &test_pwm_execute, dutycycle*0.01f, period);
// }

utest::v1::control_t TestFramework::test_level2_i2c(const size_t call_count) {
	return test_level2_framework(I2C_SCL, CITS_I2C_SCL, &test_i2c_execute, 0.01, 100);
}

utest::v1::control_t TestFramework::test_level2_spi(const size_t call_count) {
	return test_level2_framework(SPI_CLK, CITS_SPI_CLK, &test_spi_execute, 0.01, 100);
}

/*/////////////////////////////////////////////////////////////////////////////
//									Tests									 //
/////////////////////////////////////////////////////////////////////////////*/


    // AnalogIn ain(ain_pin);
    // BusInOut outputs(dout_pin1,dout_pin2,dout_pin3,dout_pin4,dout_pin5);
    // outputs.output();
    // int x = 0;
    // int y= 0;
    // outputs = 0;
    // float prev_value = 0;
    // for(x = 0; x<5; x++) {
    //     DEBUG_PRINTF("X=%d\n",x);
    //     prev_value = ain.read();
    //     y = (y<<1) + 1;
    //     outputs = y;
    //     DEBUG_PRINTF("outputs=0x%x\nprevValue=%f\nain=%f\n\n",y,prev_value,ain.read());
    //     TEST_ASSERT_MESSAGE(ain.read() > prev_value,"Analog Input did not increment. Check that you have assigned valid pins in mbed_app.json file")
    // }
    // DEBUG_PRINTF("Finished the Test\n");
    // TEST_ASSERT(true);



void TestFramework::test_analogin_execute(PinName pin, float tolerance, int iterations) {
	DEBUG_PRINTF("Running analog input range test on pin %d\n", pin);
    TEST_ASSERT_MESSAGE(pin != NC, "Pin is NC");

    for (unsigned int i=0; i<iterations; i++) {

	    // Case 1: Digital and Analog non-conflicting
		AnalogIn ain(pin);
	    DigitalOut dout(MBED_CONF_APP_DIO_0);
	    dout=0;
	    float compare1 = ain.read();
	    dout=1;
	    float compare2 = ain.read();
	    TEST_ASSERT_MESSAGE(true, "Digital writes prevent analog inputs");
	    TEST_ASSERT_MESSAGE(abs(compare1-compare2)<tolerance, "Digital write changed the analog input");

	    // Case 2: Dual purpose analog/digital
	    // DigitalOut dout2(pin);
	    // dout2=0;
	    // dout2=1;
	    // TEST_ASSERT_MESSAGE(true, "Analog pins do not dual purpose as digital IO");
	    // float input = ain.read();
	    // dout2=0;
	    // TEST_ASSERT_MESSAGE(input > (1-tolerance) && input < (1+tolerance), "Analog pin did not match digital output");
	}
}

void TestFramework::test_analogout_execute(PinName pin, float tolerance, int iterations) {
	DEBUG_PRINTF("Running analog input range test on pin %d\n", pin);
    TEST_ASSERT_MESSAGE(pin != NC, "Pin is NC");

    for (unsigned int i=0; i<iterations; i++) {
		AnalogIn ain(pin);
	    float input = 0.0f;
		AnalogOut aout(pin);
		aout = 0.5;
	    TEST_ASSERT_MESSAGE(true, "Analog input pins do not dual purpose as analog output pins");

		for (float i=0.0f; i<=1.0f; i+=0.1f) {
			aout=i;
			input = ain.read();
			TEST_ASSERT_MESSAGE(input>=(i-tolerance) && input<=(i+tolerance), "Analog input matches analog output");
		}
	}
}

void TestFramework::test_digitalio_execute(PinName pin, float tolerance, int iterations) {
	DEBUG_PRINTF("Running digital io test on pin %d\n", pin);
    TEST_ASSERT_MESSAGE(pin != NC, "Pin is NC");

    PinName pinpair = find_pin_pair(pin);

    for (unsigned int i=0; i<iterations; i++) {
	    DigitalOut dout(pinpair);
	    DigitalIn din(pin);
	    dout = 0;
	    TEST_ASSERT_MESSAGE(0 == din.read(),"Expected value to be 0, read value was not zero");
	    dout = 1;
	    TEST_ASSERT_MESSAGE(1 == din.read(),"Expected value to be 1, read value was not one");
	}
}

void TestFramework::callback_pwm_rise(void) {
    pin_iterators[RiseCount]++;
    pin_iterators[LastRiseTime] = duty_timer.read_ms();
}

void TestFramework::callback_pwm_fall(void) {
    pin_iterators[FallCount]++;
    if (pin_iterators[LastRiseTime] != 0)
        pin_iterators[DutyCount] = pin_iterators[DutyCount] + (duty_timer.read_ms() - pin_iterators[LastRiseTime]);
}

void TestFramework::test_pwm_execute(PinName pin, float dutycycle, int period) {
	DEBUG_PRINTF("Running pwm test on pin %d\n", pin);
    TEST_ASSERT_MESSAGE(pin != NC, "Pin is NC");

    float tolerance = 0.05;
    int iterations = 100;
    float calculated_percent = iterations * tolerance;
    if (calculated_percent < 1) calculated_percent = 1;

    // Initialize PWM, InterruptIn, Timer, and Rising / Falling edge counts
    pin_iterators[FallCount] = 0;
    pin_iterators[RiseCount] = 0;
    pin_iterators[LastRiseTime] = 0;
    pin_iterators[DutyCount] = 0;
    PwmOut pwm(pin);
    duty_timer.reset();
    InterruptIn iin(find_pin_pair(pin));
    iin.rise(callback_pwm_rise);
    iin.fall(callback_pwm_fall);
    DEBUG_PRINTF("Period set to: %f, duty cycle set to: %f\n", (float)period/1000, dutycycle);
    pwm.period((float)period/1000);
    
    //Start Testing
    duty_timer.start();
    pwm.write(dutycycle); // 50% duty cycle
    DEBUG_PRINTF("Waiting for %dms\n", iterations * period);
    wait_ms(iterations * period); // wait for pwm to run and counts to add up
    iin.disable_irq(); // This is here because otherwise it fails on some platforms
    duty_timer.stop();
    int rc = pin_iterators[RiseCount]; // grab the numbers to work with as the pwm may continue going
    int fc = pin_iterators[FallCount];
    float avgTime = (float)pin_iterators[DutyCount] / iterations;
    float expectedTime = (float)period * dutycycle;

    DEBUG_PRINTF("rise count = %d, fall count = %d, expected count = %d\n", rc, fc, iterations);
    TEST_ASSERT_MESSAGE( std::abs(rc-fc) <= calculated_percent, "There was more than a specific variance in number of rise vs fall cycles");
    TEST_ASSERT_MESSAGE( std::abs(iterations - rc) <= calculated_percent, "There was more than a specific variance in number of rise cycles seen and number expected.");
    TEST_ASSERT_MESSAGE( std::abs(iterations - fc) <= calculated_percent, "There was more than a specific variance in number of fall cycles seen and number expected.");
    TEST_ASSERT_MESSAGE( std::abs(expectedTime - avgTime) <= calculated_percent,"Greater than a specific variance between expected and measured duty cycle");
}

void TestFramework::test_i2c_execute(PinName pin_scl, float null_float, int null_int) {
	PinName pin_sda = pinout[I2C_SDA][pin_iterators[I2C_SCL]].pin;
	DEBUG_PRINTF("Running I2C constructor on SDA pin %d and SCL pin %d\n", pin_sda, pin_scl);
    TEST_ASSERT_MESSAGE(pin_sda != NC, "SDA Pin is NC");
    TEST_ASSERT_MESSAGE(pin_scl != NC, "SCL Pin is NC");

    I2C i2c(pin_sda, pin_scl);
    I2CEeprom memory(pin_sda,pin_scl,MBED_CONF_APP_I2C_EEPROM_ADDR,32,0);
    char test = 'A' + rand()%26;
    char read;
    int r = 0;
    int w = 0;
    w = memory.write(1,test);
    r = memory.read(1,read);
    DEBUG_PRINTF("Num Bytes Read = %d\nNum Bytes Written = %d\nRead byte = `%c`\nWritten Byte = `%c`\n",r,w,read,test);

    TEST_ASSERT_EQUAL_MESSAGE(test,read,"Character Read does not equal character written!");
    TEST_ASSERT_MESSAGE(test == read, "character written does not match character read")

    TEST_ASSERT(true);
}

void TestFramework::test_spi_execute(PinName pin_clk, float null_float, int null_int) {
	char test_string_write[TEST_STRING_MAX] = "TestFramework test";

	PinName pin_mosi = pinout[SPI_MOSI][pin_iterators[SPI_CLK]].pin;
	PinName pin_miso = pinout[SPI_MISO][pin_iterators[SPI_CLK]].pin;
	PinName pin_cs = pinout[SPI_CS][pin_iterators[SPI_CLK]].pin;
	DEBUG_PRINTF("Running SPI constructor on CLK pin %d, MISO pin %d, MOSI pin %d, and CS pin %d\n", pin_clk, pin_miso, pin_mosi, pin_cs);
    TEST_ASSERT_MESSAGE(pin_clk != NC, "SPI CLK pin is NC");
    TEST_ASSERT_MESSAGE(pin_mosi != NC, "SPI MOSI Pin is NC");
    TEST_ASSERT_MESSAGE(pin_miso != NC, "SPI MISO Pin is NC");
    TEST_ASSERT_MESSAGE(pin_cs != NC, "SPI CS Pin is NC");

    SDFileSystem sd(pin_mosi, pin_miso, pin_clk, pin_cs, "sd");
    FILE *File_write = fopen("/sd/test_card.txt", "w"); // open File_write
    TEST_ASSERT_MESSAGE(File_write != NULL,"SD Card is not present. Please insert an SD Card.");
    TEST_ASSERT_MESSAGE(fprintf(File_write, test_string_write) > 0,"Writing File to sd card failed"); // write data
    fclose(File_write);// close file on SD

    FILE *File_read = fopen("/sd/test_card.txt", "r"); // open File_read
    char test_string_read [TEST_STRING_MAX] = {0};
    fgets(test_string_read,TEST_STRING_MAX,File_read); // read string from the file
    DEBUG_PRINTF("Read '%s' in read test\nString comparison returns %d\n",test_string_read,strcmp(test_string_read,test_string_write));
    TEST_ASSERT_MESSAGE(strcmp(test_string_read,test_string_write) == 0,"String read does not match string written"); // test that strings match

    // fclose(File);// close file on SD

    TEST_ASSERT(true);
}