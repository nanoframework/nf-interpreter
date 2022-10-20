# Ericsson Ardesco Cellular IoT Reference Platform

The Ardesco is a cellular IoT reference platform based on the Nordic Semiconductor nrf9160.

The Ardesco supports:
    Low power cellular connectivity using NB-IoT and CAT-M1
    Temperature, Humidity, Air Quality, Accelerometer sensors
    Ambient Light sensor
    Bluethooth 5
    NFC
    3-color LED
    Button
    Piezzo buzzer
    8MB flash
    USB-C
    Debug interface and Grove interface for Ardesco development kit
    1250 mAh Re-chargeable Battery

Size: 45x80x12.7mm

## Documentation

- Detailed documentation for the Ardesco can be found [here](https://iot.developer.ericsson.com/en/develop/device-development/ardesco).

- More detailed information on building and flashing the .NET NF, see the readme.md file in the Nordic directory immediately above this one.

- This build has been tested using the Ardesco "Combi".

- This build target only supports the nrf9160 on the Ardesco. Support for building the nanoFramework for the nrf52840 on Ardesco may be supported in the future.

- NOTE: Network support using the cellular connectivity function on the Ardesco is not supported in the initial release. It is targeted for support in the near future.

## Board resources used in this build:

The current build uses UART1 to communicate with the Visual Studio debugger. UART1 is configured so that it uses the Grove connector on the Ardesco Combi. Developers should connect at standard FTDI board to convert the low voltage serial to a standard USB virtual COM port. The connections are as follows:

    Function   Grove Color   Grove Pin
       GND      Black            4
       RX       Yellow           1
       TX       White            2

The logging output is directed out UART0 which by default is directed out the USB virtual COM ports.

The 3 color LED is connected via GPIO to the 9160:
    LED Color     GPIO Pin
     Red            2
     Green          3
     Blue           4

The user accessable button is available through GPIO
    Function      GPIO pin
     Button          6

The I2C bus is preconfigured by the RTOS on boot. Use I2C2 to access the environmental sensors. The BME680 is at the default address. The .NET BME680 driver has been tested and works. However, the high RAM use of this driver can cause problems for larger programs.

## Floating point

The current build is set to add support for single-precision floating point.
Meaning that `System.Math` API supports only the `float` overloads. The `double` ones will throw a `NotImplementedException`.

## nanoBooter

This hardware uses a proprietary bootloader.

## Flashing nanoCLR

The Ardesco requires external JTAG debugging hardware to flash. The Ardesco Combi as a standard 10 pin debugging connector labelled "CORTEX".
