[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE) [![#yourfirstpr](https://img.shields.io/badge/first--timers--only-friendly-blue.svg)](https://github.com/nanoframework/Home/blob/main/CONTRIBUTING.md) [![Hosted By: Cloudsmith](https://img.shields.io/badge/OSS%20hosting%20by-cloudsmith-blue?logo=cloudsmith&style=flat-square)](https://cloudsmith.com)
 [![Discord](https://img.shields.io/discord/478725473862549535.svg)](https://discord.gg/gCyBu8T)

![nanoFramework logo](https://github.com/nanoframework/Home/blob/main/resources/logo/nanoFramework-repo-logo.png)

-----
Document Language: [English](README.md) | [中文简体](README.zh-cn.md)

### Welcome to the .NET **nanoFramework** interpreter repository!

## Build status

| Component | Build Status |
|:-|---|
| nanoBooter + nanoCLR | [![Build Status](https://dev.azure.com/nanoframework/nf-interpreter/_apis/build/status/nf-interpreter?repoName=nanoframework%2Fnf-interpreter&branchName=main)](https://dev.azure.com/nanoframework/nf-interpreter/_build/latest?definitionId=34&repoName=nanoframework%2Fnf-interpreter&branchName=main) |
| Win32 test project | [![Build Status](https://dev.azure.com/nanoframework/nf-interpreter/_apis/build/status/nf-interpreter?repoName=nanoframework%2Fnf-interpreter&branchName=main)](https://dev.azure.com/nanoframework/nf-interpreter/_build/latest?definitionId=34&repoName=nanoframework%2Fnf-interpreter&branchName=main) |

## .NET nanoFramework interpreter for reference boards

Each of the linked files contain the firmware binaries for nanoBooter (if applicable) and nanoCLR in various formats (`.HEX`, `.BIN` and `.DFU`). They should be flashed to the target board using [nanoff](https://github.com/nanoframework/nanoFirmwareFlasher) or the appropriate software utility depending on the target MCU.

Firmware versions contained below are released via the `main` branch, and are treated as `RTM` builds as per our stable release mandate. These releases are compiled with optimizations and the smallest possible size. For these builds, the debugging features are also disabled and only contain no (or minimal) detailed error messages.

We also have a [Community Targets](https://github.com/nanoframework/nf-Community-Targets) repository where you can find firmware versions for several other popular boards that have been contibuted by the community, although limited support is provided.

### ESP32 modules and boards

| Target | Version |
|:-|---|
| ESP32_PSRAM_REV0 | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_PSRAM_REV0/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_PSRAM_REV0/latest/) |
| ESP32_REV0 | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_REV0/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_REV0/latest/) |
| ESP32_PSRAM_XTAL26_REV0 | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_PSRAM_XTAL26_REV0/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_PSRAM_XTAL26_REV0/latest/) |
| ESP32_PSRAM_REV3 | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_PSRAM_REV3/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_PSRAM_REV3/latest/) |
| ESP32_REV3 | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_REV3/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_REV3/latest/) |
| ESP32_BLE_REV0 | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_BLE_REV0/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_BLE_REV0/latest/) |
| ESP32_BLE_REV3 | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_BLE_REV3/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_BLE_REV3/latest/) |
| ESP_WROVER_KIT | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP_WROVER_KIT/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP_WROVER_KIT/latest/) |
| ESP32_PICO | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_PICO/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_PICO/latest/) |
| ESP32_LILYGO | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_LILYGO/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_LILYGO/latest/) |
| FEATHER_S2 | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/FEATHER_S2/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/FEATHER_S2/latest/) |
| KALUGA_1 | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/KALUGA_1/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/KALUGA_1/latest/) |
| ESP32_C3 | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_C3/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_C3/latest/) |
| ESP32_C3_REV3 | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_C3_REV3/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_C3_REV3/latest/) |
| ESP32_OLIMEX | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_OLIMEX/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_OLIMEX/latest/) |

### M5Stack

| Target | Version |
|:-|---|
| [M5Core](https://docs.m5stack.com/en/core/gray) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/M5Core/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/M5Core/latest/) |
| [M5StickC](https://docs.m5stack.com/en/core/m5stickc) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/M5StickC/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/M5StickC/latest/) |
| [M5StickCPlus](https://docs.m5stack.com/en/core/m5stickc_plus) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/M5StickCPlus/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/M5StickCPlus/latest/) |
| [M5Core2](https://docs.m5stack.com/en/core/core2) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/M5Core2/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/M5Core2/latest/) |

### STM32 boards and chip based

| Target | Version |
|:-|---|
| ST_STM32F429I_DISCOVERY | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ST_STM32F429I_DISCOVERY/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ST_STM32F429I_DISCOVERY/latest/) |
| ST_NUCLEO64_F091RC | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ST_NUCLEO64_F091RC/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ST_NUCLEO64_F091RC/latest/) |
| ST_STM32F769I_DISCOVERY | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ST_STM32F769I_DISCOVERY/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ST_STM32F769I_DISCOVERY/latest/) |
| ORGPAL_PALTHREE | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ORGPAL_PALTHREE/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ORGPAL_PALTHREE/latest/) |

### Silicon Labs Giant Gecko boards

| Target | Version |
|:-|---|
| SL_STK3701A | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/SL_STK3701A/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/SL_STK3701A/latest/) |
| SL_STK3701A_REVB | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/SL_STK3701A_REVB/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/SL_STK3701A_REVB/latest/) |

### NXP boards

| Target | Version |
|:-|---|
| NXP_MIMXRT1060_EVK | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/NXP_MIMXRT1060_EVK/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/NXP_MIMXRT1060_EVK/latest/) |

### TI boards

| Target | Version |
|:-|---|
| TI_CC1352R1_LAUNCHXL_868 | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/TI_CC1352R1_LAUNCHXL_868/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/TI_CC1352R1_LAUNCHXL_868/latest/) |
| TI_CC1352R1_LAUNCHXL_915 | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/TI_CC1352R1_LAUNCHXL_915/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/TI_CC1352R1_LAUNCHXL_915/latest/) |
| TI_CC3220SF_LAUNCHXL | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/TI_CC3220SF_LAUNCHXL/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/TI_CC3220SF_LAUNCHXL/latest/) |

The above .NET nanoFramework interpreter builds include support for the class libraries and features marked below.

<details>
  <summary>Click to expand!</summary>

   | Target                  | Gpio               | Spi                | I2c                | Pwm                | Adc                | Dac                | Serial             | OneWire            | Events             | SWO                | Networking         | Bluetooth BLE    | Large Heap         | UI         |
  |:-:                      |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |
  | ESP32_PSRAM_REV0          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: |                    | :heavy_check_mark: |                    |
  | ESP32_REV0          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: |                    | :heavy_check_mark: |                    |
  | ESP32_PSRAM_XTAL26_REV0          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: |                    | :heavy_check_mark: |                    |
  | ESP32_PSRAM_REV3          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: |                    | :heavy_check_mark: |                    |
  | ESP32_REV3          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: |                    | :heavy_check_mark: |                    |
  | ESP32_BLE_REV0      | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: |                    |                    |
  | ESP32_BLE_REV3      | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: |                    |                    |
  | ESP_WROVER_KIT          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: |
  | ESP32_PICO          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: |   |                    |                    |
  | ESP32_LILYGO          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: Wi-Fi + Ethernet |  |             |                    |
  | FEATHER_S2          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: |  |                    |                    |
  | KALUGA_1          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: |  |                    | :heavy_check_mark: |
  | ESP32_C3          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: |                    | | |
  | ESP32_OLIMEX          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: Wi-Fi + Ethernet  |  | :heavy_check_mark: |                    |
  | M5Core          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: Wi-Fi  |  | :heavy_check_mark: |                    |
  | M5StickC          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: Wi-Fi |  | :heavy_check_mark: |                    |
  | M5StickCPlus          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: |  :heavy_check_mark: Wi-Fi  |  | :heavy_check_mark: |                    |
  | M5Core2          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: Wi-Fi |  | :heavy_check_mark: |                    |
  | ST_STM32F429I_DISCOVERY | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    |                    | :heavy_check_mark: |                    |
  | ST_NUCLEO64_F091RC      | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    |                    | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    |                    |                    |                    |
  | ST_STM32F769I_DISCOVERY | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: |
  | ORGPAL_PALTHREE | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: |                    |
  | SL_STK3701A_REVB | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |  | :heavy_check_mark: |  | :heavy_check_mark: | :heavy_check_mark: |  |                    |  |  |
  | SL_STK3701A | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |  | :heavy_check_mark: |  | :heavy_check_mark: | :heavy_check_mark: |  |                    |  |  |
  | TI_CC1352R1_LAUNCHXL    | :heavy_check_mark: |  |  |  |  |                    |                    |                    |  |                    |  |                    |                    |                    |
  | TI_CC3220SF_LAUNCHXL    | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    |                    |                    | :heavy_check_mark: |                    | :heavy_check_mark: |                    |                    |                    |
  | NXP_MIMXRT1060_EVK           | :heavy_check_mark: |  |  |  |  |  | :heavy_check_mark:  |                    | :heavy_check_mark: |                    | :heavy_check_mark: |                    | :heavy_check_mark: |                    |
</details>

<details>
  <summary>This repo contains:</summary>

  * **nanoFramework** agnostic blocks
    * [CLR](src/CLR)
    * [HAL](src/HAL)
    * [PAL](src/PAL)
  * Target reference for CMSIS OS
    * [ChibiOS](targets/ChibiOS)
      * Reference target boards
        * [OrgPal PalThree](targets/ChibiOS/ORGPAL_PALTHREE)
        * [ST NUCLEO64 F091RC](targets/ChibiOS/ST_NUCLEO64_F091RC)
        * [ST STM32F429I DISCOVERY](targets/ChibiOS/ST_STM32F429I_DISCOVERY)
        * [ST STM32F769I DISCOVERY](targets/ChibiOS/ST_STM32F769I_DISCOVERY)
      * ChibiOS overlay for **nanoFramework**
        * [STM32 1.Wire driver](targets/ChibiOS/_nf-overlay/os/hal/src/stm32_onewire)
        * [STM32 CRC32 driver](targets/ChibiOS/_nf-overlay/os/hal/src/stm32_crc)
        * [STM32 Flash driver](targets/ChibiOS/_nf-overlay/os/hal/src/stm32_flash)
        * [STM32 Flexible Memory Controller driver](targets/ChibiOS/_nf-overlay/os/hal/src/stm32_fsmc)
        * [STM32 Random number generator driver](targets/ChibiOS/_nf-overlay/os/hal/src/stm32_rng)
  * Target reference for FreeRTOS
    * [ESP32_REV0](targets/ESP32/ESP32_REV0)
    * [NXP_MIMXRT1060_EVK](targets/FreeRTOS/NXP/NXP_MIMXRT1060_EVK)
  * Target references for Azure RTOS
    * [Silabs Giant Gecko EVB](targets/AzureRTOS/Silabs/SL_STK3701A)
  * Target references for TI SimpleLink
    * [TI CC1352R1_LAUNCHXL](targets/TI_SimpleLink/TI_CC1352R1_LAUNCHXL)
    * [TI CC3220SF_LAUNCHXL](targets/TI_SimpleLink/TI_CC3220SF_LAUNCHXL)
  * Target reference for other OSes
    * [Win32 OS (test project only at this time)](targets/os/win32)
  * [CMake files for the build system](CMake)
</details>

## Feedback and documentation

For documentation, providing feedback, issues and finding out how to contribute please refer to the [Home repo](https://github.com/nanoframework/Home).

Join our Discord community [here](https://discord.gg/gCyBu8T).

## Credits

The list of contributors to this project can be found at [CONTRIBUTORS](https://github.com/nanoframework/Home/blob/main/CONTRIBUTORS.md).

## License

The nanoFramework Interpreter is licensed under the [MIT license](LICENSE.md).

## Code of Conduct

This project has adopted the code of conduct defined by the Contributor Covenant to clarify expected behavior in our community.
For more information see the [.NET Foundation Code of Conduct](https://dotnetfoundation.org/code-of-conduct).

### .NET Foundation

This project is supported by the [.NET Foundation](https://dotnetfoundation.org).
