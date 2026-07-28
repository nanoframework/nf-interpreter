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

| Area | `fa_id` | Offset on W25Q512 | Size | 32 kB blocks | Notes |
|---|---|---|---|---|---|
| Image 0 secondary (CLR) | `FLASH_AREA_IMAGE_0_SECONDARY` | `0x000000` | 736 kB | 23 | Upgrade candidate for Image 0; primary (704 kB) + 1 erase block (32 kB) |
| Image 1 secondary (deploy) | `FLASH_AREA_IMAGE_1_SECONDARY` | `0x0B8000` | 1312 kB | 41 | Upgrade candidate for Image 1; primary (1280 kB) + 1 erase block (32 kB) |
| LittleFS FS0 | — | `0x200000` | 62 MB | — | Boundary moved by +64 kB (the two erase-block bumps above). **`LFS0_BLOCK_COUNT` in `target_littlefs.h` is currently hardcoded to the whole chip (`W25Q512_FLASH_SIZE / W25Q512_SECTOR_SIZE` = 16384) and the driver applies no base offset — LittleFS FS0 does not actually honor this boundary today; see open issue below.** |

## MCUboot configuration

| Parameter | Value | Notes |
|---|---|---|
| Upgrade strategy | `MCUBOOT_SWAP_USING_OFFSET` | no scratch area required |
| `MCUBOOT_IMAGE_NUMBER` | 2 | Image 0 = nanoCLR, Image 1 = deployment |
| `MCUBOOT_FLASH_WRITE_ALIGNMENT` | 4 bytes | STM32F7 FLASHv2 word-write minimum |
| `MCUBOOT_IMAGE_HEADER_SIZE` | `0x200` (512 B) | must match `--header-size` in `imgtool sign` |
| `MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE` | 32 kB | W25Q512 block erase (0x52) |
| `MCUBOOT_MAX_IMG_SECTORS` | 41 | Image 1 secondary: 1312 kB ÷ 32 kB = 41 blocks |

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
