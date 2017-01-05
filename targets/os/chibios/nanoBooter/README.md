For the ChibiOS test app to build succesfully the following changes are required:

In _halconf.g_ (when compared with a default file)
- HAL_USE_SERIAL to TRUE
- SERIAL_DEFAULT_BITRATE to 115200

In _mcuconf.h_ (when compared with a default file)
- STM32_SERIAL_USE_USART2 to TRUE

When making the build the first time it will fail with an error about a duplicate definition of __dso_handle.
Edit the file _../various/cpp_wrappers/syscalls_cpp.hpp_ located in the ChibiOS source folder and comment the line where it's being defined there.

NOTE: this configuration was sucessfully tested in a NUCLEO_F091RC board using the virtual COM port through the ST Link USB connection.