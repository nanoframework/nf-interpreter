# Nordic Semiconductor 9160DK

The development kit board for the Nordic Semiconductor nrf9160.

## Documentation

- Detailed documentation for this target reference can be found [here](https://www.nordicsemi.com/Products/Development-hardware/nrf9160-dk).

- More detailed information on building and flashing the .NET NF on Nordic devices, see the [readme](../README.md).

- This build expects the 9160DK to be set in its configuration as described in the 9160DK User Guide.

- This build has been tested using the Nordic Connect SDK v2.0.0.

- This build has been tested on 9160DK version 0.8.2. There are changes to the newer versions of this board but these aren't expected to impact the functionality in this build.

- This build target only supports the nrf9160 on the 9160DK. While there is an nrf52840 on the 9160DK board, it is used for as the board controller, not the standard target. Refer to the 52840DK target for builds targeting the nrf52840.

- NOTE: Network support using the cellular connectivity function on the nrf9160 is not supported in this initial release. It is targeted for support in the near future.

## Board resources used in this build

The current build uses UART1 to communicate with the Visual Studio debugger. UART1 is 
by default is directed out one of the USB virtual COM ports.

The logging output is directed out UART0 which by default is directed out
one of the USB virtual COM ports.

The LEDs are available via GPIO:
    LED      GPIO Pin   
     1          2
     2          3
     3          4
     4          5

The buttons are available via GPIO
   Button    GPIO pin
     1          6
     2          7

Other GPIOs are exposed on the board in Arduino-style connectors and are 
clearly labelled on the board.

## I2C

I2C functionality is working in this build. By default, I2C1 is exposed through the Arduino compatible connector.

## Floating point

The current build is set to add support for single-precision floating point.
Meaning that `System.Math` API supports only the `float` overloads. The `double` ones will throw a `NotImplementedException`.

## nanoBooter

This hardware uses a proprietary bootloader.

## Flashing nanoCLR

The 9160DK hardware contains integrated JTAG debugging hardware. The micro-USB connector at the end of the board is used for programming/debugging the 9160 and provides logging output.
