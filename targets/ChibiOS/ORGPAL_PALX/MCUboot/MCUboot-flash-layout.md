# MCUboot Flash Layout — ORGPAL_PALX

MCUboot replaces nanoBooter at the same base address. PALX uses identical linker scripts for debug and release builds. The MCUboot internal flash layout is identical to the current nanoBooter release layout — **no linker script changes are required**.

> **Note:** PALX was not expanded to 64 kB. The FatFs stack is not needed in the PALX MCUboot bootloader because secondary slots use the W25Q512 QSPI driver directly (not SD card FatFs). The 32 kB bootloader partition (sector 0 only) is sufficient.

## Internal flash map (STM32F76xx, 2 MB)

| Region | Address | Size | Sector(s) | Notes |
|---|---|---|---|---|
| MCUboot | `0x08000000` | 32 kB | 0 | replaces nanoBooter (same size) |
| config | `0x08008000` | 32 kB | 1 | unchanged |
| Image 0 primary (CLR) | `0x08010000` | 704 kB | 2–6 (bank 1) | unchanged |
| Image 1 primary (deploy) | `0x080C0000` | 1280 kB | 7 (bank 1) + bank 2 | unchanged |

## External secondary slots (W25Q512, QSPI, 64 MB)

> **Note:** PALX has a single W25Q512 QSPI device (no separate SPI flash). The MCUboot port for PALX requires a QSPI driver in the porting layer.

| Area | `fa_id` | Offset on W25Q512 | Size | 64 kB blocks | Notes |
|---|---|---|---|---|---|
| Image 0 secondary (CLR) | `FLASH_AREA_IMAGE_0_SECONDARY` | `0x000000` | 704 kB | 11 | Upgrade candidate for Image 0 |
| Image 1 secondary (deploy) | `FLASH_AREA_IMAGE_1_SECONDARY` | `0x0B0000` | 1280 kB | 20 | Upgrade candidate for Image 1 |
| LittleFS FS0 | — | `0x1F0000` | ≈ 62 MB | 993 | `LFS0_BLOCK_COUNT` changes from 16384 → 15872 (4 kB blocks) |

## MCUboot configuration

| Parameter | Value | Notes |
|---|---|---|
| Upgrade strategy | `MCUBOOT_SWAP_USING_OFFSET` | no scratch area required |
| `MCUBOOT_IMAGE_NUMBER` | 2 | Image 0 = nanoCLR, Image 1 = deployment |
| `MCUBOOT_FLASH_WRITE_ALIGNMENT` | 4 bytes | STM32F7 FLASHv2 word-write minimum |
| `MCUBOOT_IMAGE_HEADER_SIZE` | `0x200` (512 B) | must match `--header-size` in `imgtool sign` |
| `MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE` | 64 kB | W25Q512 block erase (0xD8) |
| `MCUBOOT_MAX_IMG_SECTORS` | 20 | Image 1 secondary: 1280 kB ÷ 64 kB = 20 blocks |

## Serial recovery

| Item | Value |
|---|---|
| Detection pin | GPIOK7 (active-LOW, external pull-up) |
| Detection delay | 100 ms (`MCUBOOT_SERIAL_DETECT_DELAY`) |
| UART | None — wire protocol uses USB CDC (SDU1); USART1 pins occupied by USB OTG |

## Legacy nanoBooter comparison

| | nanoBooter (release) | MCUboot |
|---|---|---|
| Bootloader | 32 kB (sector 0) | 32 kB (sector 0) — **unchanged** |
| Config | 32 kB @ `0x08008000` | 32 kB @ `0x08008000` — **unchanged** |
| CLR code start | `0x08010000` | `0x08010000` — **unchanged** |
| Deploy slot | 1280 kB @ `0x080C0000` | 1280 kB @ `0x080C0000` — **unchanged** |
| Upgrade mechanism | manual flash | MCUboot swap-using-offset |
