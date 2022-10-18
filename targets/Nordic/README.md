# Nordic Semiconductor Connect SDK Support

This target supports hardware that runs the Nordic Connect SDK based on the Zephyr
RTOS. As Zephyr handles most low level functions, porting to new hardware based
on the Nordic nrf9160 or nrf52840 should be quite simple. 

Boards will need a "board directory" as described in the NCS documentaion. If the board directory is in the default zephyr or default NCS board directories, it will be found during the build. The build is also configured to look in the targets/Nordic/boards directory. 

If the board directory is elsewhere, modify the cmakelist.txt in your target hardware directory to append to the BOARD_ROOT value. If your board directory was located in c:\myboards\arm\<board_name>, add the line:
    list(APPEND BOARD_ROOT "c:\myboards")

Zephyr then looks under the architecture name such as arm to find the proper board directory.

Current support is provided for GPIO, and I2C. More support is planned for the future.

## Documentation

- Detailed documentation for the Nordic Connect SDK can be found [here](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/index.html).

- More detailed information on the supported targets can be found in the target directories below.

- This build has been tested using the Nordic Connect SDK v2.0.0. 

## nanoBooter

None of the Nordic targets use the nanoBooter.

## Building nanoCLR for the Nordic Connect SDK

Only command line builds are supported. Building with VS Code is not supported at this time.

The current build does not support automatic downloading of the Nordic Connect SDK. This must 
be done manually before building.

To install the Nordic Connect SDK, go to [here](https://www.nordicsemi.com/Products/Development-tools/nRF-Connect-for-desktop) and click on "Downloads". Download the tool. Follow the instructions to install the "Toolchain Manager". 

Use the Toolchain Manager, to install the v2.0.0 version of the Nordic Connect SDK. 
Note: The build assumes the NCS is installed at it's default location, c:\ncs. However, if run from an NCS build prompt as discussed below, the build system should find the NCS install directory.

Once installed, use the Toolchain Manager to open a command line build prompt. In the Toolchain Manager, click on the the dropdown button and select "Open Command Prompt". Navigate to the root of the nf_interpreter folder directory. Type the following commands:

    cmake --preset <preset_target>
    cmake –build --preset <preset_target>

Where <preset_target> is the name of one of the currently supported targets. Current support is provided for:
    Nordic_9160DK   -  The Nordic Semiconductor development kit for the nrf9160
    Nordic_52840DK  -  The Nordic Semiconductor development kit for the nrf52840
    Nordic_Ardesco  -  The Ericsson cellular IoT demonstration platform.

The resulting binary named merged.hex will be located in the build directory under 
    .\build\targets\Nordic\<target_name>\zephyr
as in
    .\build\targets\Nordic\<target_name>\zephyr\merged.hex

## Flashing nanoCLR

The resulting merged.hex file can be programmed with a standard JTAG debugger into the hardware. The 
Nordic Connect SDK tools have a tool "Programmer" that can be launched from the Nordic Connect for Desktop.

Documentation for the programmer can be found [here](https://infocenter.nordicsemi.com/index.jsp?topic=%2Fug_nc_programmer%2FUG%2Fnrf_connect_programmer%2Fncp_introduction.html)
