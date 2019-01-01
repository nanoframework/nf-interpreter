For a successful build the following changes are required:

In _halconf.g_ (in both nanoBooter and nanoCLR folders), when compared with a default file:
- HAL_USE_SERIAL to TRUE
- HAL_USE_SERIAL_USB to TRUE
- HAL_USE_USB to TRUE
- SERIAL_DEFAULT_BITRATE to 115200

In _mcuconf.h_ (in both nanoBooter and nanoCLR folders), when compared with a default file:
- STM32_SERIAL_USE_USART2 to TRUE
- STM32_USB_USE_OTG1 to TRUE

In _chconf.h_ (_**only**_ for nanoCLR folder), when compared with a default file: 
- set the CORTEX_VTOR_INIT with the appropriate address of the vector table for nanoCLR

NOTE: this configuration was successfully tested in an ST_NUCLEO144_F746ZG board using the Serial over USB connection on USB port 1 that creates a virtual COM port.


## Floating point

The current build is set to add support for single-precision floating point.
Meaning that `System.Math` API supports only the `float` overloads. The `double` ones will throw a `NotImplementedException`.
