[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE) [![#yourfirstpr](https://img.shields.io/badge/first--timers--only-friendly-blue.svg)](https://github.com/nanoframework/Home/blob/master/CONTRIBUTING.md) 
 [![Discord](https://img.shields.io/discord/478725473862549535.svg)](https://discord.gg/gCyBu8T)


![nanoFramework logo](https://github.com/nanoframework/Home/blob/master/resources/logo/nanoFramework-repo-logo.png)

-----
Document Language: [English](README.md) | [中文简体](README.zh-cn.md)

### Welcome to the **nanoFramework** Interpreter repository!

## Build status

| Component | Build Status | Build Status (develop) |
|:-|---|---|
| nanoBooter + nanoCLR | [![Build Status](https://dev.azure.com/nanoframework/nf-interpreter/_apis/build/status/nanoframework.nf-interpreter?branchName=master)](https://dev.azure.com/nanoframework/nf-interpreter/_build/latest?definitionId=34?branchName=master) | [![Build Status](https://dev.azure.com/nanoframework/nf-interpreter/_apis/build/status/nanoframework.nf-interpreter?branchName=develop)](https://dev.azure.com/nanoframework/nf-interpreter/_build/latest?definitionId=34?branchName=develop) |
| Win32 test project | [![Build Status](https://dev.azure.com/nanoframework/nf-interpreter/_apis/build/status/nanoframework.nf-interpreter?branchName=master)](https://dev.azure.com/nanoframework/nf-interpreter/_build/latest?definitionId=34?branchName=master) | [![Build Status](https://dev.azure.com/nanoframework/nf-interpreter/_apis/build/status/nanoframework.nf-interpreter?branchName=develop)](https://dev.azure.com/nanoframework/nf-interpreter/_build/latest?definitionId=34?branchName=develop) |

## Firmware for reference boards

Each of the following ZIP files contains the image files for nanoBooter and nanoCLR in various formats (HEX, BIN and DFU). They should be flashed in the target boards using an appropriate software utility.

The **stable** versions are RTM builds with the smallest possible size. They include the latest stable version. The debugging feature is disabled and only minimal (or none) error messages.

The **preview** versions are continuous builds of the reference targets. They include the latest version of all features and bug corrections. They also have the debugging feature enabled along with detailed error messages.

| Target | Stable | Preview |
|:-|---|---|
| ST_STM32F429I_DISCOVERY | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/ST_STM32F429I_DISCOVERY/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/ST_STM32F429I_DISCOVERY/_latestVersion) | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images-dev/ST_STM32F429I_DISCOVERY/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images-dev/ST_STM32F429I_DISCOVERY/_latestVersion) |
| ST_NUCLEO64_F091RC | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/ST_NUCLEO64_F091RC/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/ST_NUCLEO64_F091RC/_latestVersion) | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images-dev/ST_NUCLEO64_F091RC/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images-dev/ST_NUCLEO64_F091RC/_latestVersion) |
| ST_STM32F769I_DISCOVERY | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/ST_STM32F769I_DISCOVERY/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/ST_STM32F769I_DISCOVERY/_latestVersion) | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images-dev/ST_STM32F769I_DISCOVERY/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images-dev/ST_STM32F769I_DISCOVERY/_latestVersion) |
| ORGPAL_PALTHREE | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/ORGPAL_PALTHREE/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/ORGPAL_PALTHREE/_latestVersion) | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images-dev/ORGPAL_PALTHREE/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images-dev/ORGPAL_PALTHREE/_latestVersion) |
| MBN_QUAIL | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/MBN_QUAIL/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/MBN_QUAIL/_latestVersion) | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images-dev/MBN_QUAIL/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images-dev/MBN_QUAIL/_latestVersion) |
| NETDUINO3_WIFI | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/NETDUINO3_WIFI/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/NETDUINO3_WIFI/_latestVersion) | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images-dev/NETDUINO3_WIFI/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images-dev/NETDUINO3_WIFI/_latestVersion) |
| ESP32_WROOM_32 | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/ESP32_WROOM_32/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/ESP32_WROOM_32/_latestVersion) | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images-dev/ESP32_WROOM_32/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images-dev/ESP32_WROOM_32/_latestVersion) |
| TI_CC1352R1_LAUNCHXL | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/TI_CC1352R1_LAUNCHXL/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/TI_CC1352R1_LAUNCHXL/_latestVersion) | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images-dev/TI_CC1352R1_LAUNCHXL/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images-dev/TI_CC1352R1_LAUNCHXL/_latestVersion) |
| TI_CC3220SF_LAUNCHXL | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/TI_CC3220SF_LAUNCHXL/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/TI_CC3220SF_LAUNCHXL/_latestVersion) | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images-dev/TI_CC3220SF_LAUNCHXL/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images-dev/TI_CC3220SF_LAUNCHXL/_latestVersion) |
| NXP_MIMXRT1060_EVK | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/NXP_MIMXRT1060_EVK/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/NXP_MIMXRT1060_EVK/_latestVersion) | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images-dev/NXP_MIMXRT1060_EVK/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images-dev/NXP_MIMXRT1060_EVK/_latestVersion) |

The above firmware builds include support for the class libraries and features marked below.

<details>
  <summary>Click to expand!</summary>

  | Target                  | Gpio               | Spi                | I2c                | Pwm                | Adc                | Dac                | Serial             | OneWire            | Events             | SWO                | Networking         | Large Heap         |
  |:-:                      |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |
  | ST_STM32F429I_DISCOVERY | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: |
  | ST_NUCLEO64_F091RC      | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    |                    | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    |                    |
  | ST_STM32F769I_DISCOVERY | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
  | ORGPAL_PALTHREE | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |  | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
  | MBN_QUAIL               | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    |                    | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    |                    |                    |
  | NETDUINO3_WIFI          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    |                    |                    |
  | ESP32_WROOM_32          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: |                    |
  | TI_CC1352R1_LAUNCHXL    | :heavy_check_mark: |  |  |  |  |                    |                    |                    |  |                    |  |                    |
  | TI_CC3220SF_LAUNCHXL    | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    |                    |                    | :heavy_check_mark: |                    | :heavy_check_mark: |                    |
  | NXP_MIMXRT1060_EVK           | :heavy_check_mark: |  |  |  |  |  | :heavy_check_mark:  |                    | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: |
</details>

<details>
  <summary>This repo contains:</summary>

  * **nanoFramework** agnostic blocks
    * [CLR](src/CLR)
    * [HAL](src/HAL)
    * [PAL](src/PAL)
  * Target reference for CMSIS OS
    * [ChibiOS](targets/CMSIS-OS/ChibiOS)
      * Reference target boards
        * [Mikrobus QUAIL](targets/CMSIS-OS/ChibiOS/MBN_QUAIL)
        * [OrgPal PalThree](targets/CMSIS-OS/ChibiOS/ORGPAL_PALTHREE)
        * [ST NUCLEO64 F091RC](targets/CMSIS-OS/ChibiOS/ST_NUCLEO64_F091RC)
        * [ST STM32F429I DISCOVERY](targets/CMSIS-OS/ChibiOS/ST_STM32F429I_DISCOVERY)
        * [ST STM32F769I DISCOVERY](targets/CMSIS-OS/ChibiOS/ST_STM32F769I_DISCOVERY)
        * [Wilderness Labs Netduino3 WiFi](targets/CMSIS-OS/ChibiOS/NETDIUNO3_WIFI)
      * ChibiOS overlay for **nanoFramework**
        * [STM32 1.Wire driver](targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/src/stm32_onewire)
        * [STM32 CRC32 driver](targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/src/stm32_crc)
        * [STM32 Flash driver](targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/src/stm32_flash)
        * [STM32 Flexible Memory Controller driver](targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/src/stm32_fsmc)
        * [STM32 Random number generator driver](targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/src/stm32_rng)
  * Target reference for FreeRTOS
    * [ESP32_WROOM_32](targets/FreeRTOS_ESP32/ESP32_WROOM_32)
    * [NXP_MIMXRT1060_EVK](targets/FreeRTOS/NXP/NXP_MIMXRT1060_EVK)
  * Target references for TI SimpleLink
    * [TI CC1352R1_LAUNCHXL](targets/TI-SimpleLink/TI_CC1352R1_LAUNCHXL)
    * [TI CC3220SF_LAUNCHXL](targets/TI-SimpleLink/TI_CC3220SF_LAUNCHXL)
  * Target reference for other OSes
    * [Win32 OS (test project only at this time)](targets/os/win32)
  * [CMake files for the build system](CMake)
</details>

## Feedback and documentation

For documentation, providing feedback, issues and finding out how to contribute please refer to the [Home repo](https://github.com/nanoframework/Home).

Join our Discord community [here](https://discord.gg/gCyBu8T).

## Credits

The list of contributors to this project can be found at [CONTRIBUTORS](https://github.com/nanoframework/Home/blob/master/CONTRIBUTORS.md).

## License

The nanoFramework Interpreter is licensed under the [MIT license](LICENSE.md).

## Code of Conduct

This project has adopted the code of conduct defined by the Contributor Covenant to clarify expected behavior in our community.
For more information see the [.NET Foundation Code of Conduct](https://dotnetfoundation.org/code-of-conduct).

### .NET Foundation

This project is supported by the [.NET Foundation](https://dotnetfoundation.org).
