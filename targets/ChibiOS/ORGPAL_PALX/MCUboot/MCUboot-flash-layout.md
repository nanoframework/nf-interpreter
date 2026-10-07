# MCUboot Flash Layout — ORGPAL_PALX

MCUboot replaces nanoBooter at the same base address. PALX uses identical linker scripts for debug and release builds.

## Internal flash map (STM32F76xx, 2 MB)

| Region | Address | Size | Notes |
|---|---|---|---|
| MCUboot | `0x08000000` | 96 kB | sectors 0-2; USB CDC + QSPI HAL + USB MSD update import (USB host + FatFs) don't fit in 64 kB |
| Config block | `0x08018000` | 32 kB | sector 3; HAL-managed; **outside** any MCUboot slot |
| *unused* | `0x08020000` | 128 kB | sector 4 — **stranded**; see "Logical sectors" below |
| Image 0 primary slot (CLR) | `0x08040000` | **1024 kB** | 4 × 256 kB logical sectors; **768 kB usable** (one LS reserved for the swap trailer) |
| Image 1 primary (deploy) | `0x08140000` | **768 kB** | 3 × 256 kB logical sectors; **512 kB usable** |
| *(end of flash)* | `0x08200000` | — | — |

> **Config block placement:** Sector 3 (`0x08018000`, 32 kB) sits between the MCUboot bootloader and the CLR primary slot. It is not part of any MCUboot-managed flash area — MCUboot never enumerates, erases, or writes to it. It moved from sector 2 (`0x08010000`) when the bootloader grew from 64 kB to 96 kB for the USB MSD update import; a device flashed with the previous layout loses its configuration block.

### Logical sectors (`MCUBOOT_SWAP_USING_OFFSET`)

`CONFIG_NF_MCUBOOT_LOGICAL_SECTOR_SIZE = 0x40000` (**256 kB**). The swap algorithm erases and copies one logical sector at a time, and `boot_erase_region()` snaps every request to the port's real erase-page geometry. So **every logical-sector boundary in a primary slot must coincide with a real STM32F7 erase-page boundary**.

The STM32F769 dual-bank erase-page geometry (shared `stm32_f7xx_flash.h`) is: per bank 4 × 32 kB, 1 × 128 kB, 3 × 256 kB. Every 256 kB-aligned address at or above `0x08040000` is a real erase-page boundary, and each 256 kB window erases to exactly 256 kB (the small-sector run at `0x08100000`–`0x08140000` — 4 × 32 kB + 1 × 128 kB — sums to one clean 256 kB logical sector). Therefore:

* The primary slots start at `0x08040000` / `0x08140000` and are whole multiples of 256 kB.
* Sector 4 (`0x08020000`, 128 kB) **cannot** start a 256 kB logical sector, so it is left unused (sector 3 holds the config block).

The previous 32 kB logical sector (CLR primary at `0x08018000` / 672 kB) split the 128 kB and 256 kB physical pages and corrupts image 0 on the first real swap-over-populated-slot.

### Usable image size

MCUboot rounds the swap trailer up to a whole logical sector, so the largest image a slot can hold is **`(logical_sectors − 1) × 256 kB`**:

| Slot | Physical | Logical sectors | Usable | `imgtool --slot-size` |
|---|---|---|---|---|
| Image 0 primary (CLR) | 1024 kB | 4 | **768 kB** | `0xC0000` |
| Image 1 primary (deploy) | 768 kB | 3 | **512 kB** | `0x80000` |

> **LS split vs. ORGPAL_PALTHREE:** PALTHREE gives the CLR 5 LS (1024 kB usable) and the deploy slot 2 LS (256 kB usable) because its signed nanoCLR is ~825 kB (networking + crypto). PALX's nanoCLR has neither namespace and fit the old 672 kB slot, so PALX keeps the CLR at 4 LS (768 kB usable, ~100 kB headroom) and hands the extra logical sector to the deploy slot (3 LS, 512 kB usable) — closer to PALX's historical 1280 kB deploy allocation.

## External secondary slots (W25Q512, QSPI, 64 MB)

> **Note:** PALX has a single W25Q512 QSPI device (no separate SPI flash). The MCUboot port for PALX requires a QSPI driver in the porting layer.

| Area | `fa_id` | Offset on W25Q512 | Size | 32 kB blocks | Notes |
|---|---|---|---|---|---|
| Image 0 secondary (CLR) | `FLASH_AREA_IMAGE_0_SECONDARY` | `0x000000` | **1280 kB** | 40 | primary (1024 kB) + 1 logical sector (256 kB) |
| Image 1 secondary (deploy) | `FLASH_AREA_IMAGE_1_SECONDARY` | `0x140000` | **1024 kB** | 32 | primary (768 kB) + 1 logical sector (256 kB) |
| LittleFS FS0 | — | `0x240000` | ≈ 61.75 MB | — | `LFS0_BASE_OFFSET` in `target_littlefs.h` derives from `NF_MCUBOOT_SLOT_IMG1_SEC_OFF + NF_MCUBOOT_SLOT_IMG1_SEC_SIZE`. |

> **LittleFS FS0 moved** from `0x200000` to `0x240000` versus the pre-rework layout. Any existing FS0 contents are invalidated — reformat FS0 on a device that had the previous layout.

## MCUboot configuration

| Parameter | Value | Notes |
|---|---|---|
| Upgrade strategy | `MCUBOOT_SWAP_USING_OFFSET` | no scratch area required |
| `MCUBOOT_IMAGE_NUMBER` | 2 | Image 0 = nanoCLR, Image 1 = deployment |
| `MCUBOOT_LOGICAL_SECTOR_SIZE` | `0x40000` (256 kB) | swap erase/copy unit; must align to real erase pages in every primary slot |
| `MCUBOOT_HEADER_SIZE` | `0x400` (1024 B) | `CONFIG_NF_MCUBOOT_HEADER_SIZE`; must match `--header-size` in `imgtool sign`; larger than the usual `0x200` because ChibiOS aligns `.vectors` to a 1024-byte boundary by default |
| `MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE` | 32 kB | W25Q512 block erase (0x52); `MCUBOOT_LOGICAL_SECTOR_SIZE` and all secondary offsets/sizes are whole multiples of it |
| `MCUBOOT_ERASE_PROGRESSIVELY` | enabled (via swap-using-offset, shared config) | serial-recovery upload erases the slot sector-by-sector as chunks arrive |
| `MCUBOOT_MAX_IMG_SECTORS` | 64 (generic default when logical sectors are configured) | largest slot = image 0 secondary, 1280 kB ÷ 256 kB = 5 logical sectors |

## Update media (USB MSD)

With `NF_FEATURE_MCUBOOT_HAS_USB_MSD` enabled (default in `defconfig`), before `boot_go()` the bootloader enumerates a USB mass storage device on OTG_HS (`USBHD2`, FatFs volume `E:`) and imports `nanoCLR-*.bin` / `nanoDeployment-*.bin` from its root directory into the matching W25Q512 secondary slot for a test swap (see `MCUboot/docs/upgrade-strategy.md`). Board side: `mcuboot_media_boot.c`, engine in `MCUboot/common/MCUboot_media_import.c`. The stick stays a regular storage volume for the application; it is not a slot. PALX has no SD card, so USB MSD is the only update medium.

| Item | Value |
|---|---|
| Files (root of the volume) | `nanoCLR-*.bin` → image 0, `nanoDeployment-*.bin` → image 1 (e.g. `nanoCLR-1.2.3.bin`); two separate signed files (`imgtool sign --header-size 0x400 --pad-header --slot-size <usable>`), no bundle. Several matches → the highest image version wins |
| Placement | one logical sector in (`+0x40000`) as `MCUBOOT_SWAP_USING_OFFSET` requires; max file size = usable image size (768 kB / 512 kB) |
| Image identity | the SHA-256 TLV of the image (verified against the bytes in flash), **not** the version number: a rebuilt image with an unchanged version is still imported. Files without a SHA-256 TLV are ignored |
| After import | copy in flash hashed against the file's SHA-256 (mismatch → slot erased, file not tagged), slot marked pending (**test** swap; the app confirms), file kept, `<file>.used` gets a line `<STM32 UID hex> <version> <size> <sha256 first 16 hex>` |
| Skipped (tagged) | the primary already holds the identical image · the identical image is already pending |
| Skipped | `.used` already lists this device for that exact file **and** the primary holds a valid image (an erased/corrupt/random primary ignores the marker, so a re-flashed production board is updated again) · header/size invalid |
| Deferred (not tagged, retried next boot) | MCUboot swap interrupted by a reset · revert in progress · unconfirmed test image in primary with an intact image to revert to (MCUboot reverts first) |
| Replaced | an unconfirmed test image in primary with **no** valid image to revert to (MCUboot would otherwise revert to garbage) · a pending image that is invalid or different from the file |
| Log | every outcome is one `media import: image N: …` line with its reason, plus the candidate's version, size and SHA-256 prefix |
| Image 1 note | the deployment block storage spans the whole image 1 primary, header included: a raw (non-MCUboot) deployment from the debugger leaves no valid image there, so a stick left plugged in re-imports its `nanoDeployment-*.bin` on the next boot |
| USB bring-up budget | nothing attached: gives up after `MCUBOOT_USB_MSD_ATTACH_TIMEOUT_MS` (1 s, judged on the live `HPRT.PCSTS` bit); device attached: up to `MCUBOOT_USB_MSD_ENUM_TIMEOUT_MS` (6 s) for enumeration + LUN ready. Outcome is logged either way |

Signature verification is MCUboot's: a bad signature is caught by `boot_go()` and the secondary slot erased; the `.used` line prevents the same file from being staged again on this device (so an image that fails its test boot and is reverted is not retried in a loop).

## Serial recovery

| Item | Value |
|---|---|
| Detection pin | GPIOK7 (active-LOW, external pull-up) |
| Detection delay | 100 ms (`MCUBOOT_SERIAL_DETECT_DELAY`) |
| UART | None — wire protocol uses USB CDC (SDU1); USART1 pins occupied by USB OTG |

## Legacy nanoBooter comparison

| | nanoBooter (release) | MCUboot |
|---|---|---|
| Bootloader | 32 kB (sector 0) | 96 kB (sectors 0-2) — **expanded** |
| Config | 32 kB @ `0x08008000` | 32 kB @ `0x08018000` — **moved** |
| CLR code start | `0x08010000` | `0x08040400` (slot base `0x08040000` + header `0x400`) — sector 4 unused |
| Deploy slot | 1280 kB @ `0x080C0000` | 768 kB @ `0x08140000` (512 kB usable after swap trailer) |
| Upgrade mechanism | manual flash | MCUboot swap-using-offset |
