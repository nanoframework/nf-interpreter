# MCUboot Flash Layout — ST_STM32F769I_DISCOVERY

MCUboot replaces nanoBooter at the same base address. The MCUboot build uses a **unified internal flash layout** (same slot boundaries for both debug and release builds). Legacy nanoBooter builds are unchanged.

> **⚠ Secondary slots deferred:** SD card FatFs integration for secondary slots is not yet complete. `mcuboot_ext_flash_init()` returns -1 unconditionally; `NF_FEATURE_MCUBOOT_HAS_SDCARD` is the build flag that will enable it. MCUboot currently boots the primary slot directly without attempting any upgrade.

## Internal flash map (STM32F769NI, 2 MB)

| Region | Address | Size | Sector(s) | Notes |
|---|---|---|---|---|
| MCUboot | `0x08000000` | 64 kB | 0–1 | replaces nanoBooter; expanded from 32 kB to accommodate FatFs |
| Config block | `0x08010000` | 32 kB | **2** | HAL-managed; **outside** any MCUboot slot |
| Image 0 primary slot (CLR) | `0x08018000` | **928 kB** | **3–7 (bank 1)** | MCUboot-managed slot; MCUboot header at slot start (`0x08018000`) |
| Image 1 primary (deploy) | `0x08100000` | 1024 kB | bank 2 | |

> **Config block placement:** Sector 2 (0x08010000, 32 kB) sits between the MCUboot bootloader and the CLR primary slot. It is not part of any MCUboot-managed flash area. MCUboot never enumerates, erases, or writes to sector 2.

### STM32F769 sector geometry reference (bank 1)

| Sector | Address | Size |
|---|---|---|
| 0 | `0x08000000` | 32 kB |
| 1 | `0x08008000` | 32 kB |
| 2 | `0x08010000` | 32 kB |
| 3 | `0x08018000` | 32 kB |
| 4 | `0x08020000` | 128 kB |
| 5 | `0x08040000` | 256 kB |
| 6 | `0x08080000` | 256 kB |
| 7 | `0x080C0000` | 256 kB |

Bank 2 (0x08100000–0x081FFFFF, 1024 kB) has the same sector geometry.

## External secondary slots (SD card, FatFs — DEFERRED)

| Area | `fa_id` | File path on SD card | Size | Notes |
|---|---|---|---|---|
| Image 0 secondary (CLR) | `FLASH_AREA_IMAGE_0_SECONDARY` | `/mcuboot/img0_sec.bin` | 928 kB | CLR upgrade candidate |
| Image 1 secondary (deploy) | `FLASH_AREA_IMAGE_1_SECONDARY` | `/mcuboot/img1_sec.bin` | 1024 kB | deploy upgrade candidate |

- Virtual sector size: 4 kB (FatFs cluster granularity)
- SD card interface: SDMMC1 (ChibiOS `SDCD1`)
- FatFs integration gated on `NF_FEATURE_MCUBOOT_HAS_SDCARD` Kconfig flag
- Until the flag is enabled, `mcuboot_ext_flash_init()` returns -1 and MCUboot skips secondary slot checks

## MCUboot configuration

| Parameter | Value | Notes |
|---|---|---|
| Upgrade strategy | `MCUBOOT_SWAP_USING_OFFSET` | no scratch area required |
| `MCUBOOT_IMAGE_NUMBER` | 2 | Image 0 = nanoCLR, Image 1 = deployment |
| `MCUBOOT_FLASH_WRITE_ALIGNMENT` | 4 bytes | STM32F7 FLASHv2 word-write minimum |
| `MCUBOOT_IMAGE_HEADER_SIZE` | `0x200` (512 B) | must match `--header-size` in `imgtool sign` |
| `MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE` | 4 kB | virtual sector for FatFs file-backed slots |
| `MCUBOOT_MAX_IMG_SECTORS` | 256 | Image 1 secondary: 1024 kB ÷ 4 kB = 256 virtual sectors |

## Serial recovery

| Item | Value |
|---|---|
| Detection pin | PA0 — USER button (active-HIGH, external pull-down) |
| Detection delay | 100 ms (`MCUBOOT_SERIAL_DETECT_DELAY`) |
| UART | USART1 — PA9 (TX) / PA10 (RX), AF7, 115 200 baud, 8N1 |
| Init function | `mcuboot_uart_init()` in `MCUboot/mcuboot_detect_pin.c` |

## Legacy nanoBooter comparison

| | nanoBooter (release) | MCUboot |
|---|---|---|
| Bootloader | 32 kB (sector 0) | **64 kB (sectors 0–1)** |
| Config | 32 kB @ `0x08008000` (sector 1) | 32 kB @ `0x08010000` (sector 2) |
| CLR code start | `0x08010000` (sector 2) | `0x08018000` (sector 3) |
| Deploy slot | 1280 kB @ `0x080C0000` | 1024 kB @ `0x08100000` (bank 2) |
| Upgrade mechanism | manual flash | MCUboot swap-using-offset (secondary DEFERRED) |
