# Nordic Semiconductor 52840DK

The development kit board for the Nordic Semiconductor nrf52840.

## Documentation

- Detailed documentation for this target reference can be found [here](https://www.nordicsemi.com/Products/Development-hardware/nRF52840-DK).

- More detailed information on building and flashing the .NET NF on Nordic devices, see the
readme.md file in the Nordic directory immediately above this one.

- This build expects the 52840DK to be set in its configuration as described in the 
User Guide.

- This build has been tested using the Nordic Connect SDK v2.0.0. 

- This build has been tested on 52840DK version 1.1.0. 

## Board resources used in this build:

The current build uses UART1 to communicate with the Visual Studio debugger. The pins for UART1
are exposed as follows:
    Function   Label   Connector  
       GND      GND       P4      
       RX       P1.01     P3      
       TX       P1.02     P3      

The logging output is directed out UART0 which by default is directed out
one of the USB virtual COM ports.

The LEDs are available via GPIO:
    LED      GPIO Pin   
     1          13
     2          14
     3          15
     4          16

The buttons are available via GPIO 
   Button    GPIO pin (default)
     1          11
     2          12
     3          24
     4          25

Other GPIOs are exposed on the board in Arduino-style connectors and are 
clearly labled on the board.

## I2C

I2C functionality is working in this build. However, an I2C bus must be declared in an .overlay file and CONFIG_I2C=y must be set in the prj.conf for the bus to be instantiated by the Zephyr OS.

## Floating point

The current build is set to add support for single-precision floating point.
Meaning that `System.Math` API supports only the `float` overloads. The `double` ones will throw a `NotImplementedException`.

## nanoBooter

This hardware uses a proprietary bootloader.

## Building nanoCLR for the 52840DK

Follow the instructions in the Nordic directory for installing the Nordic Connect SDK. This must 
be done manually before building.

The instructions in the Nordic directory provide the generic instructions for building. Specifically for this target, type:
    cmake --preset Nordic_52840DK
    cmake –build --preset Nordic_52840DK

The resulting binary named zephyr.hex will be located in the build directory under 
    .\build\targets\Nordic\52840DK\zephyr
as in
    .\build\targets\Nordic\52840DK\zephyr\zephyr.hex

## Flashing nanoCLR

The 52840DK hardware contains integrated JTAG debugging hardware. There are two USB connectors on the 52840DK. The micro-USB connector at the end of the board is used for programming/debugging the 52840 and provides logging output. The micro-USB on the bottom side of the board is used for testing the USB functionality of the 52840. nanoFramework support for USB connections via this connector are not supported at this time.
