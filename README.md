[![Discord](https://img.shields.io/discord/478725473862549535.svg)](https://discord.gg/gCyBu8T)


![nanoFramework logo](https://github.com/nanoframework/Home/blob/master/resources/logo/nanoFramework-repo-logo.png)

-----

### Welcome to the **nanoFramework** Interpreter repository!

This repo contains:

* **nanoFramework** agnostic blocks
  * [CLR](src/CLR)
  * [HAL](src/HAL)
  * [PAL](src/PAL)
* Target reference for CMSIS OS
  * [ChibiOS](targets/CMSIS-OS/ChibiOS)
    * Reference target boards
      * [Mikrobus QUAIL](targets/CMSIS-OS/ChibiOS/MBN_QUAIL)
      * [ST NUCLEO64 F091RC](targets/CMSIS-OS/ChibiOS/ST_NUCLEO64_F091RC)
      * [ST NUCLEO144 F746ZG](targets/CMSIS-OS/ChibiOS/ST_NUCLEO144_F746ZG)
      * [ST STM32F4 DISCOVERY](targets/CMSIS-OS/ChibiOS/ST_STM32F4_DISCOVERY)
      * [ST STM32F429I DISCOVERY](targets/CMSIS-OS/ChibiOS/ST_STM32F429I_DISCOVERY)
      * [ST STM32F769I DISCOVERY](targets/CMSIS-OS/ChibiOS/ST_STM32F769I_DISCOVERY)
      * [Wilderness Labs Netduino3 WiFi](targets/CMSIS-OS/ChibiOS/NETDIUNO3_WIFI)
    * ChibiOS overlay for **nanoFramework**
      * [STM32 CRC32 driver](targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/src/stm32_crc)
      * [STM32 Flash driver](targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/src/stm32_flash)
      * [STM32 Flexible Memory Controller driver](targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/src/stm32_fsmc)
      * [STM32 Random number generator driver](targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/src/stm32_rng)
* Target reference for FreeRTOS
  * [ESP32_DevKitC](targets/FreeRTOS/ESP32_DevKitC)
* Target reference for other OSes
  * [Win32 OS (test project only at this time)](targets/os/win32)
* [CMake files for the build system](CMake)


## Build status

| Component | Build Status | Build Status (develop) |
|:-|---|---|
| nanoBooter + nanoCLR | [![Build status](https://ci.appveyor.com/api/projects/status/gvkj25wv62eoikoq?svg=true)](https://ci.appveyor.com/project/nfbot/nf-interpreter-852pb) | [![Build status](https://ci.appveyor.com/api/projects/status/gvkj25wv62eoikoq/branch/develop?svg=true)](https://ci.appveyor.com/project/nfbot/nf-interpreter-852pb/branch/develop) | 
| Win32 test project | [![Build status](https://ci.appveyor.com/api/projects/status/gvkj25wv62eoikoq?svg=true)](https://ci.appveyor.com/project/nfbot/nf-interpreter-852pb) | [![Build status](https://ci.appveyor.com/api/projects/status/gvkj25wv62eoikoq/branch/develop?svg=true)](https://ci.appveyor.com/project/nfbot/nf-interpreter-852pb/branch/develop) |


## Firmware for reference boards


Each of the following ZIP files contains the image files for nanoBooter and nanoCLR in various formats (HEX, BIN and DFU). They should be flashed in the target boards using an appropriate software utility.

The **stable** versions are RTM builds with the smallest possible size. They include the latest stable version. The debugging feature is disabled and only minimal (or none) error messages.

The **preview** versions are continuous builds of the reference targets. They include the latest version of all features and bug corrections. They also have the debugging feature enabled along with detailed error messages.


| Target | Stable | Preview |
|:-|---|---|
| ST_STM32F4_DISCOVERY | []() |  [![Download](https://api.bintray.com/packages/nfbot/nanoframework-images-dev/ST_STM32F4_DISCOVERY/images/download.svg)](https://bintray.com/nfbot/nanoframework-images-dev/ST_STM32F4_DISCOVERY/_latestVersion) |
| ST_STM32F429I_DISCOVERY | []() | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images-dev/ST_STM32F429I_DISCOVERY/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images-dev/ST_STM32F429I_DISCOVERY/_latestVersion) |
| ST_NUCLEO64_F091RC | []() | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images-dev/ST_NUCLEO64_F091RC/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images-dev/ST_NUCLEO64_F091RC/_latestVersion) |
| ST_NUCLEO144_F746ZG | []() | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images-dev/ST_NUCLEO144_F746ZG/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images-dev/ST_NUCLEO144_F746ZG/_latestVersion) |
| ST_STM32F769I_DISCOVERY | []() | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images-dev/ST_STM32F769I_DISCOVERY/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images-dev/ST_STM32F769I_DISCOVERY/_latestVersion) |
| MBN_QUAIL | []() | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images-dev/MBN_QUAIL/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images-dev/MBN_QUAIL/_latestVersion) |
| NETDUINO3_WIFI | []() | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images-dev/NETDUINO3_WIFI/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images-dev/NETDUINO3_WIFI/_latestVersion) |
| ESP32_DEVKITC | []() | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images-dev/ESP32_DEVKITC/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images-dev/ESP32_DEVKITC/_latestVersion) |


The above firmware builds include support for the class libraries and features marked bellow.


| Target | Gpio | Spi | I2c | Pwm | Adc | Serial | Events | SWO | Networking | Large Heap |
|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
| ST_STM32F4_DISCOVERY | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | | |
| ST_STM32F429I_DISCOVERY | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | | :heavy_check_mark: |
| ST_NUCLEO64_F091RC | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | | |
| ST_NUCLEO144_F746ZG | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | | |
| ST_STM32F769I_DISCOVERY | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | | :heavy_check_mark: |
| MBN_QUAIL | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | | :heavy_check_mark: | :heavy_check_mark: | | | | |
| NETDUINO3_WIFI | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark:  | :heavy_check_mark: | :heavy_check_mark: | | | | |
| ESP32_DEVKITC | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | | :heavy_check_mark: | | | | |


## Feedback and documentation

For documentation, providing feedback, issues and finding out how to contribute please refer to the [Home repo](https://github.com/nanoframework/Home).

Join our Discord community [here](https://discord.gg/gCyBu8T).


## Credits

The list of contributors to this project can be found at [CONTRIBUTORS](https://github.com/nanoframework/Home/blob/master/CONTRIBUTORS.md).


## License

The nanoFramework Interpreter is licensed under the [Apache 2.0 license](http://www.apache.org/licenses/LICENSE-2.0).


## Code of Conduct
This project has adopted the code of conduct defined by the [Contributor Covenant](http://contributor-covenant.org/)
to clarify expected behavior in our community.
