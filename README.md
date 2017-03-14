# ci-test-shield
This repo contains the test code for the ARM mbed CI Test shield. For the hardware schematics please see the [mbed HDK](https://github.com/ARMmbed/mbed-HDK/tree/master/Production%20Design%20Projects/CITestShield). Make sure to use the latest version of the hardware to be fully compatible with the tests!

If any tests fail then your platform is exhibiting unexpected behavior. More often than not this indicates a bug in your software port. For maximum mbed compatibility all tests should pass on your platform.

If you platform does not support a specific feature, such as AnalogOut, then that test will fail or error out. This is expected behavior, please make sure to note this in your communications. 

## What to use the CI Test Shield for?
The CI Test Shield has several uses.

1. Test mbed-os software port. Useful for mbed partners and community contributors.
2. Continuous Integration testing during development and during maintenance release cycles. 
3. SD Card storage can be used for firmware update development. 

## What is tested?
Current API's that are tested by the CI Test shield are as follows

1. `DigitalIO`, `InterruptIn`, `PwmOut` - tested by Loopback on pins D0-9
2. `AnalogIO` - tested by resistor star network.
3. `I2C` - tested with temperature sensor and EEPROM memory.
4. `SPI` - tested with SD Card. 
5. `UART` - tested by greentea working


#### Coming Soon

* `Ticker/Timer/Time`

## Hardware : Where to get CI Test Shield?

1. Buy pre-made shield (coming soon)
2. DIY - buy [PCB from OSHPark](https://oshpark.com/shared_projects/azR9r1l1) and [parts from Octopart](https://octopart.com/bom-tool/ZgvU783O) and assemble it yourself! 
3. Custom - use the [mbed HDK](http://github.com/ARMmbed/mbed-HDK/tree/master/Production%20Design%20Projects/CITestShield) to modify the design to match the headers on your board. 

## Software : How to use?
To run the tests associated with the ci-test-shield follow these steps:

#### Pre-requisites

0. Install [mbed-cli](https://github.com/armmbed/mbed-cli) tool (need version 1.0.0 or greater). Please verify that you installed [mbed-cli requirements](https://github.com/ARMmbed/mbed-cli#requirements)
1. Install [mbed-os requirements](https://github.com/ARMmbed/mbed-os/blob/master/requirements.txt)
2. `mbed import https://github.com/ARMmbed/ci-test-shield.git` Clone this repo to your computer using mbed import. 

#### Usage

0. `cd ci-test-shield`- Navigate to the repo on your command line. 
1. `mbed target auto` - Automatically detect board plugged into computer and set it as the target
3. `mbed toolchain GCC_ARM` - Select compiler. You can also use IAR, UVISION, or any other toolchain listed in `mbed toolchain --supported`
4. [Optional] Update mbed-os and tests to latest version. Do nothing to use the last stable version.
5. `mbed test -n tests-* --app-config .\mbed_app.json` - Run CI Test Shield tests. 

## Customization  
All pin mappings are done in the `mbed_app.json` file. If you need to remap or override you can do so here. Please note that all pin mappings will be pre-pended with `MBED_APP_CONF_`. 

#### Different Pins
If your board has peripherals on pins that differ from the Arduino R3 Header layout you will need to manually fly wire them into the appropriate header and adjust the `mbed_app.json` file.

For example: the NXP K64F board does not have AnalogOut on A5, instead it is on an inside header pin called DAC0_OUT that is not broken out to the Arduino R3 Headers. To account for this we manually fly wire the DAC0_OUT pin to the A5 pin on the CI Test shield and add the following to the `target_overrides` section of the `mbed_app.json` file. 

```json
    "target_overrides": {
        "K64F": {
             "AOUT": "DAC0_OUT"
        }
    }
```
The above code essentially says "if the target is `K64F` then `#define AOUT DAC0_OUT`", this allows you to fly wire pins and only modify things in one file instead of having to modify all the tests themselves. 

#### Non-Arduino Header
If you are testing a board that does not have Arduino R3 style headers then you will need to either fly wire all the pins across manually, or if you want a more permanent solution spin your own version of the CI Test shield with headers that map to your board. 

Either way, you will need to heavily modify the `mbed_app.json` file and redefine every pin mapping since your platform will not have the Arduino Header D0-15 or A0-5 pin aliases. 

For more on how config files work see the [mbed OS docs](https://docs.mbed.com/docs/mbed-os-handbook/en/latest/advanced/config_system/).

## Troubleshooting
- Use the `-v` flag for verbose debug and failure messages that can help you troubleshoot. 
- To enable debug messages, set ``DEBUG_MSG`` to 1 in the ./mbed_app.json file
- Try running a clean build with `mbed test --clean -n tests-* --app-config .\mbed_app.json -v`
- Make sure your board is detected to run tests. Use the `mbed detect` command to verify your board is recognized and has a COM port assigned. If the Serial driver isn't working then tests cant run. 
- make sure you are using the latest version of greentea and mbed-cli. Try running `pip install -U mbed-cli mbed-ls mbed-greentea` to update them all to their latest versions. 
- Tests in the `assumptions` folder should all pass, if any of the assumptions tests fail then their associated test will likely also fail. For example, AnalogIn pins should also be DigitalOut capable for the AnalogIn tests to work, so the assumptions test for AnalogIn tests this functionality. 
- If all tests are failing try bending aside pins D0/D1 on the CI Test Shield. Some platforms only have 1 Serial peripheral and will tie together the debug serial channel and the loopback serial channel, which causes problems for the communications for the testing platform. 
- Make sure to put an SD Card into the microSD slot for SPI Tests to work.
- If you are seeing any weird problems please contact support@mbed.com for help. 

#### Known Issues 
- The latest version of DAP-Link interface firmware should be used as old versions may not support the testing framework. 
- AnalogIn pins must also support DigitalOut
- Tests require mbed-cli version > 0.9.9 .

## More information
For more information see the [Releases](https://github.com/ARMmbed/ci-test-shield/releases) page.

## License
Apache 2.0 - feel free to re-use, tweak, modify for personal or business use.
