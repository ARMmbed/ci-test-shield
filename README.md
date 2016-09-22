# ci-test-shield
This repo contains the test code for the ARM mbed CI Test shield. For the hardware schematics please see the [mbed HDK](http://github.com/armmbed/mbed-hdk).

## Hardware : Where to get CI Test Shield?
1. Buy pre-made shield from LTEK [coming soon]
2. DIY - buy PCB from OSHPark and parts from Farnell and assemble it yourself! [see [releases](https://github.com/ARMmbed/ci-test-shield/releases) page page for more details]
3. Custom - use the mbed HDK to modify the design to match the headers on your board. 

## Software : How to use?
To run the tests associated with the ci-test-shield follow these steps:

1. Install [mbed-cli](https://github.com/armmbed/mbed-cli) tool.
2. Set your board using the `mbed config target xxx`, where xxx is the board name [protip: use `mbed target --supported` for list of supported boards].
3. Set your toolchain using the `mbed config toolchain xxx`, where xxx is the toolchain.
4. Grab dependencies by running the `mbed deploy` or `mbed update` command inside the top level of this repo.
5. Run the tests with the test shield plugged into your board. The command will look like `mbed test -n tests-api-*` to run all the CI Test Shield tests, or just run `mbed test` to run all tests, including mbed-os system test.

#### Customization : Non-Arduino Header
If you are testing a board that does not have Arduino R3 style headers then you will need to do the following:

1. Fly wire the relevant pins together between the board under test and the CI Test Shield. 
2. Modify the pin names in the `mbed_app.json` file to match your boards MCU pin names. For example if your UART is not on D0 / D1 but instead on `PIN_AWESOME` then you would need to change the `TEST_UART_TX:D0` key value pair to be `TEST_UART_TX:PIN_AWESOME`. For more on how config files work see the [mbed OS docs](https://github.com/ARMmbed/mbed-os/blob/master/docs/config_system.md#configuration-data-in-applications).
3. Run the tests using the config file `mbed test -n tests-api-* --app-config .\mbed_app.json`

If any tests fail then your platform is exhibiting unexpected behavior. More often than not this indicates a bug in your software port. For maximum mbed compatibility all tests should pass on your platform. 

## Troubleshooting
- make sure your board is detected to run tests. Use the `mbed detect` command to verify your board is recognized and has a COM port assigned. If the Serial driver isn't working tests cant run. 
- If you are seeing any weird problems please contact support@mbed.com for help. 

#### Known Issues
- SPI tests skip - This is known and is due to pending change in mbed OS, should be enabled as part of the mbed OS 5.2 Release. 
- The latest version of DAP-Link interface firmware should be used as old versions may not support the testing framework. 

## More information
For more information see the [Releases](https://github.com/ARMmbed/ci-test-shield/releases) page.

## License
Apache 2.0 - feel free to re-use, tweak, modify for personal or business use.
