# RP_PICO2_W_RP2350 — Raspberry Pi Pico 2 W

This target is the **Raspberry Pi Pico 2 W** board, which uses the **RP2350** microcontroller (dual-core Arm Cortex-M33 @ 150 MHz) with the **Infineon CYW43439** WiFi+Bluetooth module.

## Key Hardware

| Feature | Details |
|---------|---------|
| MCU | RP2350 (dual-core Cortex-M33 @ 150 MHz) |
| SRAM | 520 KB |
| Flash | 4 MB QSPI XIP |
| WiFi | 802.11n 2.4 GHz (CYW43439, same chip as Pico W) |
| Bluetooth | Bluetooth 5.2 (CYW43439) |

## WiFi Chip

The Pico 2 W uses the same **Infineon CYW43439** wireless chip as the original Pico W. It is connected to the RP2350 via a PIO-based SPI interface:

- **GP23**: `WL_ON` — CYW43 power enable
- **GP24**: `WL_D` — SPI data (SDIO)
- **GP25**: `WL_CS` — SPI chip select
- **GP29**: `WL_CLK` / VSYS monitor (ADC) — SPI clock line shared with VSYS monitor (ADC)

> **Note**: GP25 is **not** the LED on Pico 2 W. The on-board LED is connected to `WL_GPIO0` on the CYW43439. Do not toggle GP25 as an LED.

## Flash Layout

| Region | Address | Size |
|--------|---------|------|
| nanoBooter | `0x10000000` | 48 KB |
| Config block | `0x1000C000` | 32 KB |
| nanoCLR | `0x10014000` | 1536 KB |
| Deployment | `0x10194000` | 2352 KB |
| LittleFS | `0x103E0000` | 128 KB |

## Differences from RP_PICO2_RP2350

This WiFi target adds:
- WiFi/networking APIs (`CONFIG_API_SYSTEM_DEVICE_WIFI`, `CONFIG_API_SYSTEM_NET`)
- mbedTLS security stack (`CONFIG_NF_SECURITY_MBEDTLS`)
- I/O filesystem API (`CONFIG_API_SYSTEM_IO_FILESYSTEM`) and LittleFS storage
- CYW43439 driver (PIO-based SPI, fetched via CMake FetchContent)
- Larger nanoCLR flash partition to fit WiFi + TLS native code
- lwIP WiFi options (`target_lwipopts.h`, `target_lwip_sntp_opts.h`)
- Larger CLRStartupThread stack (8192 bytes) for TLS operations
