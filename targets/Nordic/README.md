# Nordic Semiconductor Connect SDK Support

This platform supports hardware that runs the Nordic Connect SDK based on the Zephyr RTOS. As Zephyr handles most low level functions, porting to new hardware based on the Nordic nrf9160 or nrf52840 should be quite simple.

Target boards will need a "board directory" as described in the NCS documentation. The build is configured to look in the `targets/Nordic/_zephyr_boards` directory.

If the board directory is elsewhere, modify the cmakelist.txt in your target hardware directory to append to the BOARD_ROOT value. If your board directory was located in c:\myboards\arm\<board_name>, add the line:
    list(APPEND BOARD_ROOT "c:\myboards")

Zephyr then looks under the architecture name such as `arm` to find the proper board directory.

Current support is provided for GPIO, and I2C. More support is planned for the future.

## Documentation

- Detailed documentation for the Nordic Connect SDK can be found [here](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/index.html).

- More detailed information on the supported targets can be found in the target directories below.

- This build has been tested using the Nordic Connect SDK v2.0.0.

## Pre-requisites

## nanoBooter

None of the Nordic targets implements nanoBooter. Instead it uses Zephyr bootloader.

## Flashing nanoCLR

The resulting merged.hex file can be programmed with a standard JTAG debugger into the hardware. The 
Nordic Connect SDK tools have a tool "Programmer" that can be launched from the Nordic Connect for Desktop.

Documentation for the programmer can be found [here](https://infocenter.nordicsemi.com/index.jsp?topic=%2Fug_nc_programmer%2FUG%2Fnrf_connect_programmer%2Fncp_introduction.html)
