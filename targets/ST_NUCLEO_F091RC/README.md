For the ChibiOS test app to build succesfully the following changes are required:

In _halconf.g_ (when compared with a default file)
- HAL_USE_SERIAL to TRUE
- SERIAL_DEFAULT_BITRATE to 115200

In _mcuconf.h_ (when compared with a default file)
- STM32_SERIAL_USE_USART2 to TRUE

NOTE: this configuration was sucessfully tested in a NUCLEO_F091RC board using the virtual COM port through the ST Link USB connection.
