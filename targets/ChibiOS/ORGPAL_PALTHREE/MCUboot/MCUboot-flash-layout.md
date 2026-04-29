# MCUboot Flash Layout — ORGPAL_PALTHREE

MCUboot replaces nanoBooter at the same base address. The MCUboot build uses a **unified internal flash layout** (same slot boundaries for both debug and release builds). Legacy nanoBooter builds are unchanged.

## Internal flash map (STM32F76xx, 2 MB)

| Region | Address | Size | Sector(s) | Notes |
|---|---|---|---|---|
| MCUboot | `0x08000000` | **64 kB** | **0–1** | replaces nanoBooter; expanded from 32 kB to accommodate FatFs |
| Config block | `0x08010000` | 32 kB | **2** | HAL-managed; **outside** any MCUboot slot |
| Image 0 primary slot (CLR) | `0x08018000` | **928 kB** | **3–7 (bank 1)** | MCUboot-managed slot; MCUboot header at slot start (`0x08018000`) |
| Image 1 primary (deploy) | `0x08100000` | 1024 kB | bank 2 | MCUboot-managed deploy slot |

> **Config block placement:** Sector 2 (0x08010000, 32 kB) sits between the MCUboot bootloader and the CLR primary slot. It is not part of any MCUboot-managed flash area. MCUboot never enumerates, erases, or writes to sector 2.

> **Deploy slot note:** 1024 kB matches the F76xx nanoBooter debug deploy slot. Legacy nanoBooter release builds retain 1280 kB.

## External secondary slots (AT25SF641, SPI1, 8 MB)

| Area | `fa_id` | Offset on AT25SF641 | Size | 4 kB sub-sectors | Notes |
|---|---|---|---|---|---|
| Image 0 secondary (CLR) | `FLASH_AREA_IMAGE_0_SECONDARY` | `0x000000` | 928 kB | 232 | Upgrade candidate for Image 0 |
| Image 1 secondary (deploy) | `FLASH_AREA_IMAGE_1_SECONDARY` | `0x0E8000` | 1024 kB | 256 | Upgrade candidate for Image 1 |
| LittleFS FS0 | — | `0x1E8000` | 6240 kB (≈ 6 MB) | — | `LFS0_BLOCK_COUNT` changes from 2048 → 1560 (4 kB blocks) |

W25Q128 QSPI (16 MB) — unchanged, still fully allocated to LittleFS FS1.

## MCUboot configuration

| Parameter | Value | Notes |
|---|---|---|
| Upgrade strategy | `MCUBOOT_SWAP_USING_OFFSET` | no scratch area required |
| `MCUBOOT_IMAGE_NUMBER` | 2 | Image 0 = nanoCLR, Image 1 = deployment |
| `MCUBOOT_FLASH_WRITE_ALIGNMENT` | 4 bytes | STM32F7 FLASHv2 word-write minimum |
| `MCUBOOT_IMAGE_HEADER_SIZE` | `0x200` (512 B) | must match `--header-size` in `imgtool sign` |
| `MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE` | 4 kB | AT25SF641 sub-sector erase (0x20); 928 kB ÷ 4 kB = 232 sectors (not divisible by 64 kB) |
| `MCUBOOT_MAX_IMG_SECTORS` | 256 | Image 1 secondary: 1024 kB ÷ 4 kB = 256 sub-sectors |

## Serial recovery

| Item | Value |
|---|---|
| Detection pin | GPIOK7 (active-LOW, external pull-up) |
| Detection delay | 100 ms (`MCUBOOT_SERIAL_DETECT_DELAY`) |
| UART | None — wire protocol uses USB CDC (SDU1); USART1 pins occupied by USB OTG |

## Legacy nanoBooter comparison

| | nanoBooter (release) | MCUboot |
|---|---|---|
| Bootloader | 32 kB (sector 0) | **64 kB (sectors 0–1)** |
| Config | 32 kB @ `0x08008000` (sector 1) | 32 kB @ `0x08010000` (sector 2) |
| CLR code start | `0x08010000` (sector 2) | `0x08018000` (sector 3) |
| Deploy slot | 1280 kB @ `0x080C0000` | 1024 kB @ `0x08100000` (bank 2) |
| Upgrade mechanism | manual flash | MCUboot swap-using-offset |
