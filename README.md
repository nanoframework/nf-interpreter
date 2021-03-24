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
| ESP32_WROOM_32 | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/ESP32_WROOM_32/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/ESP32_WROOM_32/_latestVersion) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP32_WROOM_32.zip/latest/x/?render=true&badge_token=gAAAAABgWxsQ0EOQ1_NxwC74J7T4CPbdGB3unrMqV-ALAnO7NmQeLcrlPWAehyXUQ3_4EsiE30fOp-boZmAADZwTcQBKv8-gJTmJhUP0DeZJCMsdWf4lBP4%3D)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP32_WROOM_32.zip/latest/) |
| ESP_WROVER_KIT | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/ESP_WROVER_KIT/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/ESP_WROVER_KIT/_latestVersion) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP_WROVER_KIT.zip/latest/x/?render=true&badge_token=gAAAAABgWxs0sy_Z0SJKVtabJLFxB-wercZM99WqrRSZZVSfcOeW2EjIhyu3MPq36A2-St-aqhmoVxICvlJM0Fg2JXx46vLcuM1vpL2AnKbEaPbpFd2tY64%3D)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP_WROVER_KIT.zip/latest/) |
| ESP32_PICO | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/ESP32_PICO/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/ESP32_PICO/_latestVersion) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP32_PICO.zip/latest/x/?render=true&badge_token=gAAAAABgWxtNdQxouHYolWQFagVq8taYLJzrxmbrP-_3yisFna9pGplAkfsCxyc-JhjD9c6KvIPjPKRId6E6-X9RswLd9vKx3pf4dX1TIcPF26YJG9uCTc4%3D)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP32_PICO.zip/latest/) |
| ST_STM32F429I_DISCOVERY | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/ST_STM32F429I_DISCOVERY/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/ST_STM32F429I_DISCOVERY/_latestVersion) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ST_STM32F429I_DISCOVERY.zip/latest/x/?render=true&badge_token=gAAAAABgWxtgAxHQjTJGznPOINUoT_Ej3tdlloXGP3tyipuu1bigH-NFmKsDiJqOFZhkYzo0HuekIdfn_af2KBfO-IrHuHd-9FrqP7nFVvuIpriyBU29jKM%3D)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ST_STM32F429I_DISCOVERY.zip/latest/) |
| ST_NUCLEO64_F091RC | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/ST_NUCLEO64_F091RC/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/ST_NUCLEO64_F091RC/_latestVersion) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ST_NUCLEO64_F091RC.zip/latest/x/?render=true&badge_token=gAAAAABgWxuEsfOKMwTQ4p8tNlgFltGI-TQyh6hS-c4m2fn3cOiiCoH2Zs_A9blCjwIA_ldqDk7OdlvCONxjn7si9t-xqPyHyAT8j-1cnAuwtKpY9iV1-_o%3D)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ST_NUCLEO64_F091RC.zip/latest/) |
| ST_STM32F769I_DISCOVERY | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/ST_STM32F769I_DISCOVERY/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/ST_STM32F769I_DISCOVERY/_latestVersion) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ST_STM32F769I_DISCOVERY.zip/latest/x/?render=true&badge_token=gAAAAABgWxuSyThy9rWl2BFzOgy1r4LRv1KYZe_vU3VicNMvPjsbwg3ps-x_V1aC8yiSoIXwIP8aPw3Ph6-wCpJZqUJduVtpp2s_UR6P2ccwtoOMJGjlvJ8%3D)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ST_STM32F769I_DISCOVERY.zip/latest/) |
| ORGPAL_PALTHREE | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/ORGPAL_PALTHREE/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/ORGPAL_PALTHREE/_latestVersion) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ORGPAL_PALTHREE.zip/latest/x/?render=true&badge_token=gAAAAABgWxuvhPup6KikfsjlSNopnl1YJj6kwaw3ZKb4ePHYbU-kp3YcyQ5fggVV_eyGSa9Gh53-TIJ5RIbFMV8gvKnvDCTczJW8CpelDLyMgP373Ey10hw%3D)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ORGPAL_PALTHREE.zip/latest/) |
| NETDUINO3_WIFI | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/NETDUINO3_WIFI/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/NETDUINO3_WIFI/_latestVersion) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/NETDUINO3_WIFI.zip/latest/x/?render=true&badge_token=gAAAAABgWxvAxFISErASMvjPfBiilCIOBObv-i9TAxVYbL0_kXOsLOGv_5CFTD9sQ_NHjSB4TNhTB4AnA0aed7VG-Vu9koWwHxMQs-1Bo_LlB9tufBhubGE%3D)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/NETDUINO3_WIFI.zip/latest/) |
| TI_CC1352R1_LAUNCHXL_868 | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/TI_CC1352R1_LAUNCHXL_868/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/TI_CC1352R1_LAUNCHXL_868/_latestVersion) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/TI_CC1352R1_LAUNCHXL_868.zip/latest/x/?render=true&badge_token=gAAAAABgWxvs4c8caJtRfrxQMUln4iKcBPy3j_L6K3BNZRBKFt5YjIhU9qSFzoPqyM0s4RGTFrUUTg1VN6YID7MkrGf3PeZT_2dW9mR5VdFkIGnrnVW9vtA%3D)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/TI_CC1352R1_LAUNCHXL_868.zip/latest/) |
| TI_CC1352R1_LAUNCHXL_915 | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/TI_CC1352R1_LAUNCHXL_915/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/TI_CC1352R1_LAUNCHXL_915/_latestVersion) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/TI_CC1352R1_LAUNCHXL_915.zip/latest/x/?render=true&badge_token=gAAAAABgWxv9FGSgTGts-1jHwH4R9rCD74Ry-HDqgamGyMxcIFB4w0ByEzLiVXRFvTwpromS_Zi2uMRnXHLPP4XEO9lJmGJbHEDrexyss9tSU9jHx_C2ybc%3D)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/TI_CC1352R1_LAUNCHXL_915.zip/latest/) |
| TI_CC3220SF_LAUNCHXL | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/TI_CC3220SF_LAUNCHXL/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/TI_CC3220SF_LAUNCHXL/_latestVersion) | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images-dev/TI_CC3220SF_LAUNCHXL/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images-dev/TI_CC3220SF_LAUNCHXL/_latestVersion) |
| NXP_MIMXRT1060_EVK | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/NXP_MIMXRT1060_EVK/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/NXP_MIMXRT1060_EVK/_latestVersion) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/NXP_MIMXRT1060_EVK.zip/latest/x/?render=true&badge_token=gAAAAABgWxwUlaLK-cw0e71em0ksycYwgy8Xxc44JLI3_ZwxxrszUOEzLud2mKaWr6GUuPjaw84h9BoKzf_1ksd5eZ1n1IpeYOmpGmMWbEYrYrQCAeZNFCw%3D)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/NXP_MIMXRT1060_EVK.zip/latest/) |

The above firmware builds include support for the class libraries and features marked below.

<details>
  <summary>Click to expand!</summary>

  | Target                  | Gpio               | Spi                | I2c                | Pwm                | Adc                | Dac                | Serial             | OneWire            | Events             | SWO                | Networking         | Large Heap         | UI         |
  |:-:                      |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |
  | ESP32_WROOM_32          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: |                    |
  | ESP_WROVER_KIT          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
  | ESP32_PICO          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: |                    |
  | ST_STM32F429I_DISCOVERY | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: |                    |
  | ST_NUCLEO64_F091RC      | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    |                    | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    |                    |                    |
  | ST_STM32F769I_DISCOVERY | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
  | ORGPAL_PALTHREE | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
  | MBN_QUAIL               | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    |                    | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    |                    |                    |                    |
  | NETDUINO3_WIFI          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    |                    |                    |                    |
  | TI_CC1352R1_LAUNCHXL_868 | :heavy_check_mark: |  |  |  |  |                    |                    |                    |  |                    |  |                    |                    |
  | TI_CC1352R1_LAUNCHXL_915 | :heavy_check_mark: |  |  |  |  |                    |                    |                    |  |                    |  |                    |                    |
  | TI_CC3220SF_LAUNCHXL    | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    |                    |                    | :heavy_check_mark: |                    | :heavy_check_mark: |                    |                    |
  | NXP_MIMXRT1060_EVK           | :heavy_check_mark: |  |  |  |  |  | :heavy_check_mark:  |                    | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: |                    |
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
