# MCUboot Flash Layout — ST_STM32F769I_DISCOVERY

MCUboot replaces nanoBooter at the same base address. The MCUboot build uses a **unified internal flash layout** (same slot boundaries for both debug and release builds). Legacy nanoBooter builds are unchanged.

> **⚠ Secondary slots deferred:** SD card FatFs integration for secondary slots is not yet complete. `mcuboot_ext_flash_init()` returns -1 unconditionally; `NF_FEATURE_MCUBOOT_HAS_SDCARD` is the build flag that gates it. MCUboot currently boots the primary slot directly without attempting any upgrade.

## Internal flash map (STM32F769NI, 2 MB)

| Region | Address | Size | Sector(s) | Notes |
|---|---|---|---|---|
| MCUboot | `0x08000000` | 64 kB | 0–1 | replaces nanoBooter; expanded from 32 kB to accommodate FatFs |
| Config block | `0x08010000` | 32 kB | **2** | HAL-managed; **outside** any MCUboot slot |
| *unused* | `0x08018000` | 160 kB | **3–4** | **stranded** — see "Logical sectors" below |
| Image 0 primary slot (CLR) | `0x08040000` | **1280 kB** | 5 × 256 kB logical sectors; **1024 kB usable** (one LS reserved for the swap trailer) |
| Image 1 primary (deploy) | `0x08180000` | **512 kB** | 2 × 256 kB logical sectors; **256 kB usable** |
| *(end of flash)* | `0x08200000` | — | — | |

> **Config block placement:** Sector 2 (`0x08010000`, 32 kB) sits between the MCUboot bootloader and the CLR primary slot. It is not part of any MCUboot-managed flash area — MCUboot never enumerates, erases, or writes to it. It keeps its pre-rework address, so existing device configuration survives the layout change.

### Logical sectors (`MCUBOOT_SWAP_USING_OFFSET`)

`CONFIG_NF_MCUBOOT_LOGICAL_SECTOR_SIZE = 0x40000` (**256 kB**). The swap algorithm erases and copies one logical sector at a time, and `boot_erase_region()` snaps every request to the port's real erase-page geometry. So **every logical-sector boundary in a primary slot must coincide with a real STM32F7 erase-page boundary**.

The STM32F769 dual-bank erase-page geometry (shared `stm32_f7xx_flash.h`) is: per bank 4 × 32 kB, 1 × 128 kB, 3 × 256 kB. Every 256 kB-aligned address at or above `0x08040000` is a real erase-page boundary, and each 256 kB window erases to exactly 256 kB (the small-sector run at `0x08100000`–`0x08140000` — 4 × 32 kB + 1 × 128 kB — sums to one clean 256 kB logical sector). Therefore:

* The primary slots start at `0x08040000` / `0x08180000` and are whole multiples of 256 kB.
* Sectors 3–4 (`0x08018000`, 32 kB + 128 kB) **cannot** start a 256 kB logical sector, so they are left unused.

The previous 32 kB logical sector (CLR primary at `0x08018000` / 928 kB) split the 128 kB and 256 kB physical pages and corrupts image 0 on the first real swap-over-populated-slot.

### Usable image size

MCUboot rounds the swap trailer up to a whole logical sector, so the largest image a slot can hold is **`(logical_sectors − 1) × 256 kB`**:

| Slot | Physical | Logical sectors | Usable | `imgtool --slot-size` |
|---|---|---|---|---|
| Image 0 primary (CLR) | 1280 kB | 5 | **1024 kB** | `0x100000` |
| Image 1 primary (deploy) | 512 kB | 2 | **256 kB** | `0x40000` |

> **LS split:** identical to ORGPAL_PALTHREE. The Discovery nanoCLR is large (NET + crypto + `System.IO.Hashing` + `nanoFramework.Graphics` / DSI + CAN + OneWire) and fit the old 928 kB slot, so it needs all 5 logical sectors (1024 kB usable, ~100 kB headroom). That leaves the deploy slot at 2 LS / 256 kB usable — down from 1024 kB in the pre-rework layout. Confirm 256 kB usable is adequate for the intended deploy payload; the only way to grow it is to cut CLR features to free a logical sector.

## External secondary slots (SD card, FatFs — DEFERRED)

| Area | `fa_id` | File path on SD card | Size | Notes |
|---|---|---|---|---|
| Image 0 secondary (CLR) | `FLASH_AREA_IMAGE_0_SECONDARY` | `/mcuboot/img0_sec.bin` | **1536 kB** | CLR upgrade candidate; primary (1280 kB) + 1 logical sector (256 kB) |
| Image 1 secondary (deploy) | `FLASH_AREA_IMAGE_1_SECONDARY` | `/mcuboot/img1_sec.bin` | **768 kB** | deploy upgrade candidate; primary (512 kB) + 1 logical sector (256 kB) |

- Virtual sector size: 32 kB (`MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE`) — only the chunk granularity MCUboot iterates the file at; `fatfs_flash_area_*` takes a plain byte offset + length. Set to 32 kB to match ORGPAL_PALTHREE / ORGPAL_PALX and keep the enumerated virtual-sector count well inside `MCUBOOT_MAX_IMG_SECTORS`.
- SD card interface: SDMMC1 (ChibiOS `SDCD1`)
- FatFs integration gated on `NF_FEATURE_MCUBOOT_HAS_SDCARD` Kconfig flag
- Until that path is wired, `mcuboot_ext_flash_init()` returns -1 and MCUboot skips secondary slot checks

## MCUboot configuration

| Parameter | Value | Notes |
|---|---|---|
| Upgrade strategy | `MCUBOOT_SWAP_USING_OFFSET` | no scratch area required |
| `MCUBOOT_IMAGE_NUMBER` | 2 | Image 0 = nanoCLR, Image 1 = deployment |
| `MCUBOOT_LOGICAL_SECTOR_SIZE` | `0x40000` (256 kB) | swap erase/copy unit; must align to real erase pages in every primary slot |
| `MCUBOOT_FLASH_WRITE_ALIGNMENT` | 4 bytes | STM32F7 FLASHv2 word-write minimum |
| `MCUBOOT_IMAGE_HEADER_SIZE` | `0x400` (1024 B) | `CONFIG_NF_MCUBOOT_HEADER_SIZE`; must match `--header-size` in `imgtool sign` |
| `MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE` | 32 kB | virtual chunk size for FatFs file-backed slots |
| `MCUBOOT_ERASE_PROGRESSIVELY` | enabled (via swap-using-offset, shared config) | serial-recovery upload erases the slot sector-by-sector as chunks arrive |
| `MCUBOOT_MAX_IMG_SECTORS` | 64 (generic default when logical sectors are configured) | largest slot = image 0 secondary, 1536 kB ÷ 256 kB = 6 logical sectors |

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
| CLR code start | `0x08010000` (sector 2) | `0x08040400` (slot base `0x08040000` + header `0x400`) — sectors 3–4 unused |
| Deploy slot | 1280 kB @ `0x080C0000` | 512 kB @ `0x08180000` (256 kB usable after swap trailer) |
| Upgrade mechanism | manual flash | MCUboot swap-using-offset (secondary DEFERRED) |
