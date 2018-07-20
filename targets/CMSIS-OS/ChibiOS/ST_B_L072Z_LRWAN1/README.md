## Configuration of Chibios, HAL and MCU ##

For a successful build the following changes are required:

In _halconf.g_ (in both nanoBooter and nanoCLR folders), when compared with a default file:
- HAL_USE_SERIAL to TRUE
- SERIAL_DEFAULT_BITRATE to 115200
- SERIAL_BUFFERS_SIZE has to be at least 64 (2x the Wire Protocol packet size) otherwise the transmission will be garbled as the packets overrun each other

In _mcuconf.h_ (in both nanoBooter and nanoCLR folders), when compared with a default file:
- STM32_SERIAL_USE_USART2 to TRUE


## Definitions for each image ## 

Any defines specific to an image (nanoBooter or nanoCLR) that are to be made available throughout the code should be added to the _target_board.h.in_. There is one in [nanoBooter](nanoBooter/target_board.h.in) and another in [nanoCLR](nanoCLR/target_board.h.in) directories, allowing different defines for each one.
Defines that are common to a target board should be added to the [_target_common.h.in_](target_common.h.in).
The content of these files is parsed by CMake. It can include CMake variables. Check the documentation of CMake config files [here](https://cmake.org/cmake/help/v3.7/command/configure_file.html?highlight=configure_file).


## nanoCLR image start address ##

The start address of nanoCLR (it's vector table) is defined in the linker file (STM32L072xZ_CLR.ld)[nanoCLR/STM32L072xZ_CLR.ld] by setting the initial address of the _flash_ region. The total size has to be the flash region size less the space reserved for nanoBooter.


**NOTE: this configuration was successfully tested in a ST_STM32L0_LRWAN1 board using the virtual COM port through the ST Link USB connection.**
