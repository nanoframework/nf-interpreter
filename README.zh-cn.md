[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE) [![#yourfirstpr](https://img.shields.io/badge/first--timers--only-friendly-blue.svg)](https://github.com/nanoframework/Home/blob/master/CONTRIBUTING.md) 
 [![Discord](https://img.shields.io/discord/478725473862549535.svg)](https://discord.gg/gCyBu8T)


![nanoFramework logo](https://github.com/nanoframework/Home/blob/master/resources/logo/nanoFramework-repo-logo.png)

-----
文档语言: [English](README.md) | [中文简体](README.zh-cn.md)

### 欢迎使用 **nanoFramework** 解析器！

## 编译状态

| 组件 | 编译状态 | 编译状态（开发） |
|:-|---|---|
| nanoBooter + nanoCLR | [![Build Status](https://dev.azure.com/nanoframework/nf-interpreter/_apis/build/status/nanoframework.nf-interpreter?branchName=master)](https://dev.azure.com/nanoframework/nf-interpreter/_build/latest?definitionId=34?branchName=master) | [![Build Status](https://dev.azure.com/nanoframework/nf-interpreter/_apis/build/status/nanoframework.nf-interpreter?branchName=develop)](https://dev.azure.com/nanoframework/nf-interpreter/_build/latest?definitionId=34?branchName=develop) |
| Win32 test project | [![Build Status](https://dev.azure.com/nanoframework/nf-interpreter/_apis/build/status/nanoframework.nf-interpreter?branchName=master)](https://dev.azure.com/nanoframework/nf-interpreter/_build/latest?definitionId=34?branchName=master) | [![Build Status](https://dev.azure.com/nanoframework/nf-interpreter/_apis/build/status/nanoframework.nf-interpreter?branchName=develop)](https://dev.azure.com/nanoframework/nf-interpreter/_build/latest?definitionId=34?branchName=develop) |

## 评估板固件

以下每个ZIP文件包括了nanoBooter和nanoCLR镜像（HEX，BIN，DFU）。可以使用相应烧写工具把它们写入目标板卡中。

**稳定** 版是RTM最小大小编译。它包含了最后稳定版本，关闭调试功能，仅有最少或没有错误信息。
**预览** 版是目标板持续编译。它包含所有功能和错误修正的最后版本，也包括调试信息和详细错误信息。


| 目标 | 稳定 | 预览 |
|:-|---|---|
| ST_STM32F429I_DISCOVERY | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/ST_STM32F429I_DISCOVERY/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/ST_STM32F429I_DISCOVERY/_latestVersion) | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images-dev/ST_STM32F429I_DISCOVERY/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images-dev/ST_STM32F429I_DISCOVERY/_latestVersion) |
| ST_NUCLEO64_F091RC | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/ST_NUCLEO64_F091RC/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/ST_NUCLEO64_F091RC/_latestVersion) | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images-dev/ST_NUCLEO64_F091RC/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images-dev/ST_NUCLEO64_F091RC/_latestVersion) |
| ST_STM32F769I_DISCOVERY | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/ST_STM32F769I_DISCOVERY/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/ST_STM32F769I_DISCOVERY/_latestVersion) | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images-dev/ST_STM32F769I_DISCOVERY/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images-dev/ST_STM32F769I_DISCOVERY/_latestVersion) |
| MBN_QUAIL | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/MBN_QUAIL/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/MBN_QUAIL/_latestVersion) | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images-dev/MBN_QUAIL/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images-dev/MBN_QUAIL/_latestVersion) |
| NETDUINO3_WIFI | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/NETDUINO3_WIFI/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/NETDUINO3_WIFI/_latestVersion) | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images-dev/NETDUINO3_WIFI/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images-dev/NETDUINO3_WIFI/_latestVersion) |
| ESP32_WROOM_32 | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/ESP32_WROOM_32/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/ESP32_WROOM_32/_latestVersion) | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images-dev/ESP32_WROOM_32/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images-dev/ESP32_WROOM_32/_latestVersion) |
| TI_CC1352R1_LAUNCHXL | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/TI_CC1352R1_LAUNCHXL/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/TI_CC1352R1_LAUNCHXL/_latestVersion) | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images-dev/TI_CC1352R1_LAUNCHXL/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images-dev/TI_CC1352R1_LAUNCHXL/_latestVersion) |
| TI_CC3220SF_LAUNCHXL | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/TI_CC3220SF_LAUNCHXL/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/TI_CC3220SF_LAUNCHXL/_latestVersion) | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images-dev/TI_CC3220SF_LAUNCHXL/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images-dev/TI_CC3220SF_LAUNCHXL/_latestVersion) |
| NXP_MIMXRT1060_EVK | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images/NXP_MIMXRT1060_EVK/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images/NXP_MIMXRT1060_EVK/_latestVersion) | [ ![Download](https://api.bintray.com/packages/nfbot/nanoframework-images-dev/NXP_MIMXRT1060_EVK/images/download.svg) ](https://bintray.com/nfbot/nanoframework-images-dev/NXP_MIMXRT1060_EVK/_latestVersion) |

以上固件支持以下类库和功能。

<details>
  <summary>点击展开</summary>

  | Target                  | Gpio               | Spi                | I2c                | Pwm                | Adc                | Dac                | Serial             | OneWire            | Events             | SWO                | Networking         | Large Heap         |
  |:-:                      |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |:-:                 |
  | ST_STM32F429I_DISCOVERY | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: |
  | ST_NUCLEO64_F091RC      | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    |                    | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    |                    |
  | ST_STM32F769I_DISCOVERY | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
  | MBN_QUAIL               | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    |                    | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    |                    |                    |
  | NETDUINO3_WIFI          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    |                    |                    |
  | ESP32_WROOM_32          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: |                    |
  | TI_CC1352R1_LAUNCHXL    | :heavy_check_mark: |  |  |  |  |                    |                    |                    |  |                    |  |                    |
  | TI_CC3220SF_LAUNCHXL    | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    |                    |                    | :heavy_check_mark: |                    | :heavy_check_mark: |                    |
  | NXP_MIMXRT1060_EVK           | :heavy_check_mark: |  |  |  |  |  | :heavy_check_mark:  |                    | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: |
</details>

<details>
  <summary>本库包括</summary>

  * **nanoFramework** 核心板块
    * [CLR](src/CLR)
    * [HAL](src/HAL)
    * [PAL](src/PAL)
  * CMSIS OS 移植
    * [ChibiOS](targets/CMSIS-OS/ChibiOS)
      * 板卡移植
        * [Mikrobus QUAIL](targets/CMSIS-OS/ChibiOS/MBN_QUAIL)
        * [ST NUCLEO64 F091RC](targets/CMSIS-OS/ChibiOS/ST_NUCLEO64_F091RC)
        * [ST STM32F429I DISCOVERY](targets/CMSIS-OS/ChibiOS/ST_STM32F429I_DISCOVERY)
        * [ST STM32F769I DISCOVERY](targets/CMSIS-OS/ChibiOS/ST_STM32F769I_DISCOVERY)
        * [Wilderness Labs Netduino3 WiFi](targets/CMSIS-OS/ChibiOS/NETDIUNO3_WIFI)
      * ChibiOS 增强 **nanoFramework**
        * [STM32 1.Wire driver](targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/src/stm32_onewire)
        * [STM32 CRC32 driver](targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/src/stm32_crc)
        * [STM32 Flash driver](targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/src/stm32_flash)
        * [STM32 Flexible Memory Controller driver](targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/src/stm32_fsmc)
        * [STM32 Random number generator driver](targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/src/stm32_rng)
  * FreeRTOS 移植
    * [ESP32_WROOM_32](targets/FreeRTOS_ESP32/ESP32_WROOM_32)
    * [NXP_MIMXRT1060_EVK](targets/FreeRTOS/NXP/NXP_MIMXRT1060_EVK)
  * TI SimpleLink 移植
    * [TI CC1352R1_LAUNCHXL](targets/TI-SimpleLink/TI_CC1352R1_LAUNCHXL)
    * [TI CC3220SF_LAUNCHXL](targets/TI-SimpleLink/TI_CC3220SF_LAUNCHXL)
  * 其它系统移植
    * [Win32 OS (test project only at this time)](targets/os/win32)
  * [CMake 编译文件](CMake)
</details>

## 文档反馈

有关文档、提供反馈、问题以及如何做出贡献的信息，请参阅 [Home repo](https://github.com/nanoframework/Home).

加入我们的讨论社区 [here](https://discord.gg/gCyBu8T).


## Credits

本项目贡献者可在 [CONTRIBUTORS](https://github.com/nanoframework/Home/blob/master/CONTRIBUTORS.md) 中找到。


## 授权

**nanoFramework** 解析器基于 [MIT license](LICENSE.md) 授权。


## 行为准则
本项目采用了 [Contributor Covenant](CODE_OF_CONDUCT.md) 规范来阐明社区预期行为。
