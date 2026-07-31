# MCUboot Flash Layout — ORGPAL_PALX

MCUboot replaces nanoBooter at the same base address. PALX uses identical linker scripts for debug and release builds.

## Internal flash map (STM32F76xx, 2 MB)

| Region | Address | Size | Sector(s) | Notes |
|---|---|---|---|---|
| MCUboot | `0x08000000` | 64 kB | 0-1 | USB CDC + QSPI HAL don't fit in 32 kB |
| config | `0x08010000` | 32 kB | 2 | |
| Image 0 primary (CLR) | `0x08018000` | 672 kB | 3–6 (bank 1) | vector table at slot base + `CONFIG_NF_MCUBOOT_HEADER_SIZE` (`0x400`) = `0x08018400` |
| Image 1 primary (deploy) | `0x080C0000` | 1280 kB | 7 (bank 1) + bank 2 | unchanged |

## External secondary slots (W25Q512, QSPI, 64 MB)

> **Note:** PALX has a single W25Q512 QSPI device (no separate SPI flash). The MCUboot port for PALX requires a QSPI driver in the porting layer.

| Area | `fa_id` | Offset on W25Q512 | Size | 32 kB blocks | Notes |
|---|---|---|---|---|---|
| Image 0 secondary (CLR) | `FLASH_AREA_IMAGE_0_SECONDARY` | `0x000000` | 704 kB | 22 | Upgrade candidate for Image 0; primary (672 kB) + 1 erase block (32 kB) |
| Image 1 secondary (deploy) | `FLASH_AREA_IMAGE_1_SECONDARY` | `0x0B8000` | 1312 kB | 41 | Upgrade candidate for Image 1; primary (1280 kB) + 1 erase block (32 kB). Offset unchanged by the image-0 correction above, leaving a harmless 32 kB unused gap ahead of it (`0x0B0000`–`0x0B8000`). |
| LittleFS FS0 | — | `0x200000` | 62 MB | — | `LFS0_BASE_OFFSET` in `target_littlefs.h` derives this offset from `NF_MCUBOOT_SLOT_IMG1_SEC_OFF + NF_MCUBOOT_SLOT_IMG1_SEC_SIZE` in `mcuboot_flash_layout.h`, and `hal_lfs_read_0`/`hal_lfs_prog_0`/`hal_lfs_erase_0`/`hal_lfs_erase_chip_0` in `target_littlefs.c` all add it to the block address. |

## MCUboot configuration

| Parameter | Value | Notes |
|---|---|---|
| Upgrade strategy | `MCUBOOT_SWAP_USING_OFFSET` | no scratch area required |
| `MCUBOOT_IMAGE_NUMBER` | 2 | Image 0 = nanoCLR, Image 1 = deployment |
| `MCUBOOT_HEADER_SIZE` | `0x400` (1024 B) | must match `--header-size` in `imgtool sign`; larger than the usual `0x200` because ChibiOS aligns `.vectors` to a 1024-byte boundary by default |
| `MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE` | 32 kB | W25Q512 block erase (0x52) |
| `MCUBOOT_MAX_IMG_SECTORS` | 41 (needs ≥ 41; generic header default is 64 when logical sectors are configured) | Image 1 secondary: 1312 kB ÷ 32 kB = 41 blocks |

## Serial recovery

| Item | Value |
|---|---|
| Detection pin | GPIOK7 (active-LOW, external pull-up) |
| Detection delay | 100 ms (`MCUBOOT_SERIAL_DETECT_DELAY`) |
| UART | None — wire protocol uses USB CDC (SDU1); USART1 pins occupied by USB OTG |

## Legacy nanoBooter comparison

| | nanoBooter (release) | MCUboot |
|---|---|---|
| Bootloader | 32 kB (sector 0) | 64 kB (sectors 0-1) — **expanded** |
| Config | 32 kB @ `0x08008000` | 32 kB @ `0x08010000` — **moved** |
| CLR code start | `0x08010000` | `0x08018400` (slot base `0x08018000` + header `0x400`) — **moved** |
| Deploy slot | 1280 kB @ `0x080C0000` | 1280 kB @ `0x080C0000` — **unchanged** |
| Upgrade mechanism | manual flash | MCUboot swap-using-offset |
