# MCUboot Flash Layout — ORGPAL_PALTHREE

MCUboot replaces nanoBooter at the same base address. The MCUboot build uses a **unified internal flash layout** (same slot boundaries for both debug and release builds). Legacy nanoBooter builds are unchanged.

## Internal flash map (STM32F76xx, 2 MB)

| Region | Address | Size | Notes |
|---|---|---|---|
| MCUboot | `0x08000000` | **96 kB** | replaces nanoBooter |
| Config block | `0x08018000` | 32 kB | HAL-managed; **outside** any MCUboot slot |
| *unused* | `0x08020000` | 128 kB | **stranded** — see "Logical sectors" below |
| Image 0 primary slot (CLR) | `0x08040000` | **1280 kB** | 5 × 256 kB logical sectors; **1024 kB usable** (one LS reserved for the swap trailer) |
| Image 1 primary (deploy) | `0x08180000` | **512 kB** | 2 × 256 kB logical sectors; **256 kB usable** |
| *(end of flash)* | `0x08200000` | — | — |

> **Config block placement:** Sector 3 (`0x08018000`, 32 kB) sits between the MCUboot bootloader and the CLR primary slot. It is not part of any MCUboot-managed flash area - MCUboot never enumerates, erases, or writes to it.

### Logical sectors (`MCUBOOT_SWAP_USING_OFFSET`)

`CONFIG_NF_MCUBOOT_LOGICAL_SECTOR_SIZE = 0x40000` (**256 kB**). The swap algorithm erases and copies one logical sector at a time, and `boot_erase_region()` snaps every request to the port's real erase-page geometry. So **every logical-sector boundary in a primary slot must coincide with a real STM32F7 erase-page boundary**.

The STM32F769 erase-page geometry is: 4 × 32 kB (`0x08000000`), 1 × 128 kB (`0x08020000`), then 256 kB pages from `0x08040000` on. Therefore:

* The primary slots start at `0x08040000` / `0x08180000` and are whole multiples of 256 kB, so each 256 kB logical sector maps onto exactly one 256 kB physical page.
* Sector 4 (`0x08020000`, 128 kB) **cannot** start a 256 kB logical sector — the next boundary (`0x08060000`) would land mid-page — so it is left unused.

A 32 kB logical sector (the previous value, with the CLR primary at `0x08020000` / 896 kB) split the 128 kB and 256 kB physical pages and corrupted image 0 on the first real swap-over-populated-slot. Bootstrap installs into an empty primary were unaffected because that path erases the whole slot up front, then copies once.

### Usable image size

MCUboot rounds the swap trailer up to a whole logical sector, so the largest image a slot can hold is **`(logical_sectors − 1) × 256 kB`**:

| Slot | Physical | Logical sectors | Usable | `imgtool --slot-size` |
|---|---|---|---|---|
| Image 0 primary (CLR) | 1280 kB | 5 | **1024 kB** | `0x100000` |
| Image 1 primary (deploy) | 512 kB | 2 | **256 kB** | `0x40000` |

## External secondary slots (AT25SF641, SPI1, 8 MB)

| Area | `fa_id` | Offset on AT25SF641 | Size | 32 kB blocks | Notes |
|---|---|---|---|---|---|
| Image 0 secondary (CLR) | `FLASH_AREA_IMAGE_0_SECONDARY` | `0x000000` | **1536 kB** | 48 | primary (1280 kB) + 1 logical sector (256 kB) |
| Image 1 secondary (deploy) | `FLASH_AREA_IMAGE_1_SECONDARY` | `0x180000` | **768 kB** | 24 | primary (512 kB) + 1 logical sector (256 kB) |
| LittleFS FS0 | — | `0x240000` | ≈ 5.75 MB | 1472 | `LFS0_BASE_OFFSET` in `target_littlefs.h` derives from `NF_MCUBOOT_SLOT_IMG1_SEC_OFF + NF_MCUBOOT_SLOT_IMG1_SEC_SIZE`. |

> **LittleFS FS0 moved** from `0x1F8000` to `0x240000` versus the pre-MCUboot-rework layout. Any existing FS0 contents are invalidated — reformat FS0 on a device that had the previous layout.

W25Q128 QSPI (16 MB) — unchanged, still fully allocated to LittleFS FS1.

## MCUboot configuration

| Parameter | Value | Notes |
|---|---|---|
| Upgrade strategy | `MCUBOOT_SWAP_USING_OFFSET` | no scratch area required |
| `MCUBOOT_IMAGE_NUMBER` | 2 | Image 0 = nanoCLR, Image 1 = deployment |
| `MCUBOOT_LOGICAL_SECTOR_SIZE` | `0x40000` (256 kB) | swap erase/copy unit; must align to real erase pages in every primary slot |
| `MCUBOOT_FLASH_WRITE_ALIGNMENT` | 4 bytes | STM32F7 FLASHv2 word-write minimum (internal); 1 byte on the AT25SF641 |
| `MCUBOOT_IMAGE_HEADER_SIZE` | `0x400` (1024 B) | `CONFIG_NF_MCUBOOT_HEADER_SIZE`; must match `--header-size` in `imgtool sign` |
| `MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE` | 32 kB | AT25SF641 block erase (0x52); `MCUBOOT_LOGICAL_SECTOR_SIZE` and all secondary offsets/sizes are whole multiples of it. |
| `MCUBOOT_ERASE_PROGRESSIVELY` | enabled (via swap-using-offset) | serial-recovery upload erases the slot sector-by-sector as chunks arrive |
| `MCUBOOT_MAX_IMG_SECTORS` | 64 | largest slot = image 0 secondary, 1536 kB ÷ 256 kB = 6 logical sectors |

## Serial recovery

| Item | Value |
|---|---|
| Detection pin | GPIOK7 (active-LOW, external pull-up) |
| Detection delay | 100 ms (`MCUBOOT_SERIAL_DETECT_DELAY`) |
| UART | None — wire protocol uses USB CDC (SDU1); USART1 pins occupied by USB OTG |

## Legacy nanoBooter comparison

| | nanoBooter (release) | MCUboot |
|---|---|---|
| Bootloader | 32 kB (sector 0) | 96 kB (sectors 0–2) |
| Config | 32 kB @ `0x08008000` (sector 1) | 32 kB @ `0x08018000` (sector 3) |
| CLR code start | `0x08010000` (sector 2) | `0x08040000` — sector 4 (`0x08020000`) unused |
| Deploy slot | 1280 kB @ `0x080C0000` | 512 kB @ `0x08180000` (256 kB usable) |
| Upgrade mechanism | manual flash | MCUboot swap-using-offset |
