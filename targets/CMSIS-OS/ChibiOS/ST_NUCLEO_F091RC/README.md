For the a successful build the following changes are required:

In _halconf.g_ (in both nanoBooter and nanoCLR folders), when compared with a default file:
- HAL_USE_SERIAL to TRUE
- SERIAL_DEFAULT_BITRATE to 115200

In _mcuconf.h_ (in both nanoBooter and nanoCLR folders), when compared with a default file:
- STM32_SERIAL_USE_USART2 to TRUE

In _chconf.h_ (_**only**_ for nanoCLR folder), when compared with a default file:
- set the CORTEX_VTOR_INIT with the appropriate address of the vector table for nanoCLR

NOTE: this configuration was successfully tested in a NUCLEO_F091RC board using the virtual COM port through the ST Link USB connection.
