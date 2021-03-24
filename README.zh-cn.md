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

以上固件支持以下类库和功能。

<details>
  <summary>点击展开</summary>

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
