# ci-test-shield
This repo contains the eagle schematics for the ARM mbed CI Test shield.

## How to reproduce
0. (Optional) Download eagle files
1. Buy PCB from OSHPark or other service
2. Buy Components from Element14 or other vendor
3. Buy Headers from Samtec or other vendor
4. Assemble board
For more information and links see the [Releases](https://github.com/ARMmbed/ci-test-shield/releases) page.

## Software
To run the tests associated with the ci-test-shield follow these steps:
1. Install [mbed-cli](https://github.com/armmbed/mbed-cli) tool.
2. Set your MCU using the `mbed config target xxx`, where xxx is the MCU target name
3. Set your toolchain using the `mbed config toolchain xxx`, where xxx is the toolchain
4. Grab dependencies by running the `mbed deploy` command
5. Run the tests with the test shield plugged into your board. The command will look like `mbed test -n tests-api-uart,tests-api-analogio,tests-api-digitalio,tests-api-i2c,tests-api-uart,tests-api-interruptin,tests-api-spi `, or just run `mbed test` to run all tests, including system test.

If any tests fail then your platform is exhibiting unexpected behavior. More often than not this indicates a bug in your software or our of spec hardware. For maximum mbed compatibility all tests should pass on your platform. 

## More information
For more information see the [Releases](https://github.com/ARMmbed/ci-test-shield/releases) page.

## License
Apache 2.0 - feel free to re-use, tweak, modify for personal or business use.

## Images
![Schematic](./ci-test-shield-sch.PNG)
![Board Layout](./ci-test-shield-brd.PNG)
