[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE) [![#yourfirstpr](https://img.shields.io/badge/first--timers--only-friendly-blue.svg)](https://github.com/nanoframework/Home/blob/main/CONTRIBUTING.md)  [![Hosted By: Cloudsmith](https://img.shields.io/badge/OSS%20hosting%20by-cloudsmith-blue?logo=cloudsmith&style=flat-square)](https://cloudsmith.com)
 [![Discord](https://img.shields.io/discord/478725473862549535.svg)](https://discord.gg/gCyBu8T)

![nanoFramework logo](https://github.com/nanoframework/Home/blob/main/resources/logo/nanoFramework-repo-logo.png)

-----
文档语言: [English](README.md) | [中文简体](README.zh-cn.md)

### 欢迎使用 .NET **nanoFramework** 解析器！

## 编译状态

| 组件 | 构建状态 |
|:-|---|
| nanoBooter + nanoCLR | [![Build Status](https://dev.azure.com/nanoframework/nf-interpreter/_apis/build/status/nf-interpreter?repoName=nanoframework%2Fnf-interpreter&branchName=main)](https://dev.azure.com/nanoframework/nf-interpreter/_build/latest?definitionId=34&repoName=nanoframework%2Fnf-interpreter&branchName=main) |
| Win32 test project | [![Build Status](https://dev.azure.com/nanoframework/nf-interpreter/_apis/build/status/nf-interpreter?repoName=nanoframework%2Fnf-interpreter&branchName=main)](https://dev.azure.com/nanoframework/nf-interpreter/_build/latest?definitionId=34&repoName=nanoframework%2Fnf-interpreter&branchName=main) |

## 支持泛型的预览版本

目前提供了支持 C# 泛型的 CLR 公共预览版。  
如果您想使用此功能，请参阅[这里](README-GENERICS.md)的文档。

## 评估板固件

以下每个ZIP文件包括了nanoBooter和nanoCLR镜像（HEX，BIN，DFU）。可以使用相应烧写工具把它们写入目标板卡中。

**版本** 版是RTM最小大小编译。它包含了最后版本版本，关闭调试功能，仅有最少或没有错误信息。
**预览** 版是目标板持续编译。它包含所有功能和错误修正的最后版本，也包括调试信息和详细错误信息。

### ESP32 modules and boards

| 目标 | 备注 | 版本 | 预览 |
|:---|:---|---|---|
| ESP32_REV0                         |                             | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_REV0/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_REV0/latest/) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP32_REV0/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP32_REV0/latest/) |
| ESP32_PSRAM_REV0                   |                             | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_PSRAM_REV0/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_PSRAM_REV0/latest/) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP32_PSRAM_REV0/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP32_PSRAM_REV0/latest/) |
| ESP32_BLE_REV0                     |                             | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_BLE_REV0/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_BLE_REV0/latest/) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP32_BLE_REV0/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP32_BLE_REV0/latest/) |
| ESP32_PSRAM_XTAL26_REV0            |                             | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_PSRAM_XTAL26_REV0/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_PSRAM_XTAL26_REV0/latest/) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP32_PSRAM_XTAL26_REV0/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP32_PSRAM_XTAL26_REV0/latest/) |
| ESP32_REV3                         |                             | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_REV3/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_REV3/latest/) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP32_REV3/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP32_REV3/latest/) |
| ESP32_PSRAM_REV3                   |                             | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_PSRAM_REV3/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_PSRAM_REV3/latest/) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP32_PSRAM_REV3/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP32_PSRAM_REV3/latest/) |
| ESP32_BLE_REV3                     |                             | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_BLE_REV3/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_BLE_REV3/latest/) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP32_BLE_REV3/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP32_BLE_REV3/latest/) |
| ESP32_PSRAM_BLE_GenericGraphic_REV3|                             | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_PSRAM_BLE_GenericGraphic_REV3/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_PSRAM_BLE_GenericGraphic_REV3/latest/) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP32_PSRAM_BLE_GenericGraphic_REV3/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP32_PSRAM_BLE_GenericGraphic_REV3/latest/) |
| ESP32_PSRAM_REV3_IPV6              | IPV6 & Thread via NCP         | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_PSRAM_REV3_IPV6/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_PSRAM_REV3_IPV6/latest/) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP32_PSRAM_REV3_IPV6/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP32_PSRAM_REV3_IPV6/latest/) |
| ESP_WROVER_KIT                     |                             | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP_WROVER_KIT/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP_WROVER_KIT/latest/) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP_WROVER_KIT/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP_WROVER_KIT/latest/) |
| ESP32_PICO                         |                             | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_PICO/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_PICO/latest/) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP32_PICO/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP32_PICO/latest/) |

### ESP32 boards with inbuilt Ethernet

| 目标 | 备注 | 版本 | 预览 |
|:---|:---|---|---|
| ESP32_ETHERNET_KIT_1.2| Poe, Wrover + 8mb psram       | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_ETHERNET_KIT_1.2/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_ETHERNET_KIT_1.2/latest/) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP32_ETHERNET_KIT_1.2/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP32_ETHERNET_KIT_1.2/latest/) |
| ESP32_OLIMEX          | Poe                         | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_OLIMEX/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_OLIMEX/latest/) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP32_OLIMEX/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP32_OLIMEX/latest/) |
| ESP32_OLIMEX_WROVER   | Poe, Wrover + 8mb psram       | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_OLIMEX_WROVER/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_OLIMEX_WROVER/latest/) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP32_OLIMEX_WROVER/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP32_OLIMEX_WROVER/latest/) |
| ESP32_WT32_ETH01      |                             | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_WT32_ETH01/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_WT32_ETH01/latest/) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP32_WT32_ETH01/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP32_WT32_ETH01/latest/) |
| ESP32_WESP32          | Poe                         | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_WESP32/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_WESP32/latest/) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP32_WESP32/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP32_WESP32/latest/) |
| ESP32_LILYGO          | Poe                         | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_LILYGO/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_LILYGO/latest/) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP32_LILYGO/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP32_LILYGO/latest/) |

### ESP32_S2 boards

| 目标 | 备注 | 版本 | 预览 |
|:---|:---|---|---|
| ESP32_S2_USB| S2 boards with direct USB connection to chip. Including Adafruit Feather_S2 | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_S2_USB/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_S2_USB/latest/) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP32_S2_USB/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP32_S2_USB/latest/) |
| ESP32_S2_UART| S2 boards with onboard USB->UART. This includes the Kaluga dev.board from Espressif | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_S2_UART/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_S2_UART/latest/) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP32_S2_UART/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP32_S2_UART/latest/) |

### ESP32_S3 boards

| 目标 | 备注 | 版本 | 预览 |
|:---|:---|---|---|
| ESP32_S3    | Display & Quad spiram support        | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_S3/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_S3/latest/)   | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP32_S3/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP32_S3/latest/)   |
| ESP32_S3_BLE| Display, BLE, Quad spiram support     | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_S3_BLE/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_S3_BLE/latest/) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP32_S3_BLE/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP32_S3_BLE/latest/) |
| ESP32_S3_ALL| Display, BLE, Octal spiram support   | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_S3_ALL/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_S3_ALL/latest/) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP32_S3_ALL/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP32_S3_ALL/latest/) |

### ESP32 risc-v boards

| 目标 | 备注 | 版本 | 预览 |
|:---|:---|---|---|
| ESP32_C3       | Uart -> VS (Beta C3 version)     | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_C3/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_C3/latest/)       | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP32_C3/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP32_C3/latest/)       |
| ESP32_C3_REV3  | Uart -> VS                       | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_C3_REV3/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_C3_REV3/latest/)  | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP32_C3_REV3/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP32_C3_REV3/latest/)  |
| XIAO_ESP32C3   | USB jtag -> VS                   | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/XIAO_ESP32C3/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/XIAO_ESP32C3/latest/)   | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/XIAO_ESP32C3/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/XIAO_ESP32C3/latest/)   |
| ESP32_C6_THREAD| USB jtag -> VS, OpenThread, Display| [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_C6_THREAD/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_C6_THREAD/latest/) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP32_C6_THREAD/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP32_C6_THREAD/latest/) |
| ESP32_H2_THREAD| USB jtag -> VS, OpenThread        | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ESP32_H2_THREAD/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ESP32_H2_THREAD/latest/)   | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ESP32_H2_THREAD/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ESP32_H2_THREAD/latest/)   |

### M5Stack

| 目标 | 备注 | 版本 | 预览 |
|:---|:---|---|---|
| [M5Core](https://docs.m5stack.com/en/core/gray)       |      | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/M5Core/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/M5Core/latest/)       | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/M5Core/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/M5Core/latest/)       |
| [M5StickC](https://docs.m5stack.com/en/core/m5stickc)   |      | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/M5StickC/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/M5StickC/latest/)   | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/M5StickC/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/M5StickC/latest/)   |
| [M5StickCPlus](https://docs.m5stack.com/en/core/m5stickc_plus) |      | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/M5StickCPlus/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/M5StickCPlus/latest/) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/M5StickCPlus/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/M5StickCPlus/latest/) |
| [M5Core2](https://docs.m5stack.com/en/core/core2)       |      | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/M5Core2/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/M5Core2/latest/)       | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/M5Core2/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/M5Core2/latest/)       |
| [AtomS3](https://docs.m5stack.com/en/core/AtomS3)       |      | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/AtomS3/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/AtomS3/latest/)       | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/AtomS3/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/AtomS3/latest/)       |

### STM32 boards and chip based

| 目标 | 版本 | 预览 |
|:---|---|---|
| ST_STM32F429I_DISCOVERY (B01) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ST_STM32F429I_DISCOVERY/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ST_STM32F429I_DISCOVERY/latest/)  | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ST_STM32F429I_DISCOVERY/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ST_STM32F429I_DISCOVERY/latest/)  |
| ST_NUCLEO64_F091RC            | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ST_NUCLEO64_F091RC/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ST_NUCLEO64_F091RC/latest/)             | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ST_NUCLEO64_F091RC/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ST_NUCLEO64_F091RC/latest/)             |
| ST_STM32F769I_DISCOVERY       | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ST_STM32F769I_DISCOVERY/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ST_STM32F769I_DISCOVERY/latest/)        | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ST_STM32F769I_DISCOVERY/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ST_STM32F769I_DISCOVERY/latest/)        |
| ORGPAL_PALTHREE               | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ORGPAL_PALTHREE/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ORGPAL_PALTHREE/latest/)                | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ORGPAL_PALTHREE/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ORGPAL_PALTHREE/latest/)                |
| ORGPAL_PALX                   | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/ORGPAL_PALX/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/ORGPAL_PALX/latest/)                    | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/ORGPAL_PALX/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/ORGPAL_PALX/latest/)                    |

### Silicon Labs Giant Gecko boards

| 目标 | 版本 | 预览 |
|:---|---|---|
| SL_STK3701A      | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/SL_STK3701A/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/SL_STK3701A/latest/)      | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/SL_STK3701A/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/SL_STK3701A/latest/)      |
| SL_STK3701A_REVB | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/SL_STK3701A_REVB/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/SL_STK3701A_REVB/latest/) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/SL_STK3701A_REVB/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/SL_STK3701A_REVB/latest/) |

### NXP boards

| 目标 | 版本 | 预览 |
|:---|---|---|
| NXP_MIMXRT1060_EVK    | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/NXP_MIMXRT1060_EVK/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/NXP_MIMXRT1060_EVK/latest/)    | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/NXP_MIMXRT1060_EVK/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/NXP_MIMXRT1060_EVK/latest/)    |

### TI boards

| 目标 | 版本 | 预览 |
|:---|---|---|
| TI_CC1352R1_LAUNCHXL_868 | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/TI_CC1352R1_LAUNCHXL_868/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/TI_CC1352R1_LAUNCHXL_868/latest/) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/TI_CC1352R1_LAUNCHXL_868/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/TI_CC1352R1_LAUNCHXL_868/latest/) |
| TI_CC1352R1_LAUNCHXL_915 | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/TI_CC1352R1_LAUNCHXL_915/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/TI_CC1352R1_LAUNCHXL_915/latest/) | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images-dev/raw/TI_CC1352R1_LAUNCHXL_915/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/detail/raw/TI_CC1352R1_LAUNCHXL_915/latest/) |
| TI_CC3220SF_LAUNCHXL     | [![Latest Version @ Cloudsmith](https://api-prd.cloudsmith.io/v1/badges/version/net-nanoframework/nanoframework-images/raw/TI_CC3220SF_LAUNCHXL/latest/x/?render=true)](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images/packages/detail/raw/TI_CC3220SF_LAUNCHXL/latest/)     |  |

以上固件支持以下类库和功能。

<details>
  <summary>点击展开</summary>

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
  | ESP32_S2_USB      | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: |  |                    |                    |
  | ESP32_S2_UART     | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: |  |                    | :heavy_check_mark: |
  | ESP32_C3          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: |                    | | |
  | XIAO_ESP32C3     | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: |                    | | |
  | ESP32_OLIMEX          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: Wi-Fi + Ethernet  |  | :heavy_check_mark: |                    |
  | M5Core          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: Wi-Fi  |  | :heavy_check_mark: |                    |
  | M5StickC          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: Wi-Fi |  | :heavy_check_mark: |                    |
  | M5StickCPlus          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: |  :heavy_check_mark: Wi-Fi  |  | :heavy_check_mark: |                    |
  | M5Core2          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: Wi-Fi |  | :heavy_check_mark: |                    |
  | ESP32_GenericDisplay_REV0 | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: Wi-Fi  |  | :heavy_check_mark: |                    |
  | ESP32_PSRAM_BLE_GenericGraphic_REV3          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: Wi-Fi |  | :heavy_check_mark: |                    |
  | ST_STM32F429I_DISCOVERY (B01) | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    |                    | :heavy_check_mark: |                    |
  | ST_NUCLEO64_F091RC      | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    |                    | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    |                    |                    |                    |
  | ST_STM32F769I_DISCOVERY | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: | :heavy_check_mark: |
  | ORGPAL_PALTHREE | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: |                    |
  | ORGPAL_PALX | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    | :heavy_check_mark: |                    |
  | SL_STK3701A_REVB | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |  | :heavy_check_mark: |  | :heavy_check_mark: | :heavy_check_mark: |  |                    |  |  |
  | SL_STK3701A | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |  | :heavy_check_mark: |  | :heavy_check_mark: | :heavy_check_mark: |  |                    |  |  |
  | TI_CC1352R1_LAUNCHXL    | :heavy_check_mark: |  |  |  |  |                    |                    |                    |  |                    |  |                    |                    |                    |
  | TI_CC3220SF_LAUNCHXL    | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |                    |                    |                    | :heavy_check_mark: |                    | :heavy_check_mark: |                    |                    |                    |
  | NXP_MIMXRT1060_EVK           | :heavy_check_mark: |  |  |  |  |  | :heavy_check_mark:  |                    | :heavy_check_mark: |                    | :heavy_check_mark: |                    | :heavy_check_mark: |                    |
</details>

<details>
  <summary>本库包括</summary>

  * **nanoFramework** 核心板块
    * [CLR](src/CLR)
    * [HAL](src/HAL)
    * [PAL](src/PAL)
  * CMSIS OS 移植
    * [ChibiOS](targets/ChibiOS)
      * 板卡移植
        * [Mikrobus QUAIL](targets/ChibiOS/MBN_QUAIL)
        * [ST NUCLEO64 F091RC](targets/ChibiOS/ST_NUCLEO64_F091RC)
        * [ST STM32F429I DISCOVERY](targets/ChibiOS/ST_STM32F429I_DISCOVERY)
        * [ST STM32F769I DISCOVERY](targets/ChibiOS/ST_STM32F769I_DISCOVERY)
        * [Wilderness Labs Netduino3 WiFi](targets/ChibiOS/NETDIUNO3_WIFI)
      * ChibiOS 增强 **nanoFramework**
        * [STM32 1.Wire driver](targets/ChibiOS/_nf-overlay/os/hal/src/stm32_onewire)
        * [STM32 CRC32 driver](targets/ChibiOS/_nf-overlay/os/hal/src/stm32_crc)
        * [STM32 Flash driver](targets/ChibiOS/_nf-overlay/os/hal/src/stm32_flash)
        * [STM32 Flexible Memory Controller driver](targets/ChibiOS/_nf-overlay/os/hal/src/stm32_fsmc)
        * [STM32 Random number generator driver](targets/ChibiOS/_nf-overlay/os/hal/src/stm32_rng)
  * FreeRTOS 移植
    * [ESP32_REV0](targets/ESP32/ESP32_REV0)
    * [NXP_MIMXRT1060_EVK](targets/FreeRTOS/NXP/NXP_MIMXRT1060_EVK)
  * Azure RTOS 移植
    * [Silabs Giant Gecko EVB](targets/AzureRTOS/Silabs/SL_STK3701A)
  * TI SimpleLink 移植
    * [TI CC1352R1_LAUNCHXL](targets/TI_SimpleLink/TI_CC1352R1_LAUNCHXL)
    * [TI CC3220SF_LAUNCHXL](targets/TI_SimpleLink/TI_CC3220SF_LAUNCHXL)
  * 其它系统移植
    * [Win32 OS (test project only at this time)](targets/os/win32)
  * [CMake 编译文件](CMake)
</details>

## 文档反馈

有关文档、提供反馈、问题以及如何做出贡献的信息，请参阅 [Home repo](https://github.com/nanoframework/Home).

加入我们的讨论社区 [here](https://discord.gg/gCyBu8T).

## Credits

本项目贡献者可在 [CONTRIBUTORS](https://github.com/nanoframework/Home/blob/main/CONTRIBUTORS.md) 中找到。

## 授权

**nanoFramework** 解析器基于 [MIT license](LICENSE.md) 授权。

## 行为准则

本项目采用了 [Contributor Covenant](CODE_OF_CONDUCT.md) 规范来阐明社区预期行为。
