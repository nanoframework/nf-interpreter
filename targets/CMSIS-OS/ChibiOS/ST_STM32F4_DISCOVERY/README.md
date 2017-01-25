For the ChibiOS test app to build successfully the following changes are required:

In _halconf.g_ (when compared with a default file)
- HAL_USE_SERIAL to TRUE
- HAL_USE_SERIAL_USB to TRUE
- HAL_USE_USB to TRUE
- SERIAL_DEFAULT_BITRATE to 115200

In _mcuconf.h_ (when compared with a default file)
- STM32_SERIAL_USE_USART2 to TRUE
- STM32_USB_USE_OTG1 to TRUE

NOTE: this configuration was successfully tested in a ST_STM32F4_DISCOVERY board using the Serial over USB connection on USB port 1 that creates a virtual COM port.
