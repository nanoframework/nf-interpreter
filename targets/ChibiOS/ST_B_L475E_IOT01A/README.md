# ST B-L475E-IOT01A — nanoFramework Target

## Overview

This target supports the **STMicroelectronics B-L475E-IOT01A** Discovery kit running ChibiOS/RT.

- **MCU:** STM32L475VG — ARM Cortex-M4
- **RTOS:** ChibiOS/RT
- **WiFi:** Inventek ISM43362 (ES-WIFI) module, connected over SPI and driven entirely through its AT command set (socket-proxy driver, no lwIP netif on this interface).

## Known Limitations

- **No TLS/HTTPS support:** the onboard WiFi module's own TLS stack is capped at TLS 1.0/1.1 by its firmware, so it cannot connect to modern servers that require TLS 1.2/1.3 (which is effectively all of them today). Running a software TLS stack (mbedTLS) on the MCU itself was also evaluated and is not feasible on this board due to flash/RAM constraints. As a result, **no TLS is supported at all** — only plain, unencrypted HTTP/sockets over WiFi are supported.
