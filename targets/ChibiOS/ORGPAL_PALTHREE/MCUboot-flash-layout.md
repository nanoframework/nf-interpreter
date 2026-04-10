# MCUboot Flash Layout — ORGPAL_PALTHREE

MCUboot replaces nanoBooter at the same base address. The MCUboot build uses a **unified internal flash layout** (same slot boundaries for both debug and release builds). Legacy nanoBooter builds are unchanged.

## Internal flash map (STM32F76xx, 2 MB)

| Region | Address | Size | Sector(s) | Notes |
|---|---|---|---|---|
| MCUboot | `0x08000000` | 32 kB | 0 | replaces nanoBooter |
| config | `0x08008000` | 32 kB | 1 | address unified (was 0x08010000 in debug nanoBooter) |
| Image 0 primary (CLR) | `0x08010000` | 960 kB | 2–7 (bank 1) | fits debug CLR binary (≤ 928 kB) ✓ |
| Image 1 primary (deploy) | `0x08100000` | 1024 kB | bank 2 | 256 kB less than release nanoBooter (1280 kB → 1024 kB) |

> **Deploy slot note:** 1024 kB is the same as the F76xx debug nanoBooter deploy slot. Legacy nanoBooter release builds retain 1280 kB.

## External secondary slots (AT25SF641, SPI1, 8 MB)

| Area | `fa_id` | Offset on AT25SF641 | Size | 64 kB blocks | Notes |
|---|---|---|---|---|---|
| Image 0 secondary (CLR) | `FLASH_AREA_IMAGE_0_SECONDARY` | `0x000000` | 960 kB | 15 | Upgrade candidate for Image 0 |
| Image 1 secondary (deploy) | `FLASH_AREA_IMAGE_1_SECONDARY` | `0x0F0000` | 1024 kB | 16 | Upgrade candidate for Image 1 |
| LittleFS FS0 | — | `0x1F0000` | 6208 kB (≈ 6 MB) | 97 | `LFS0_BLOCK_COUNT` changes from 2048 → 1552 (4 kB blocks) |

W25Q128 QSPI (16 MB) — unchanged, still fully allocated to LittleFS FS1.

## MCUboot configuration

- **Upgrade strategy**: swap-using-offset (primary in internal flash, secondary on AT25SF641 external SPI)
- **IMAGE_NUMBER**: 2 — Image 0 = nanoCLR, Image 1 = deployment
- **Primary slots**: internal STM32F76xx flash (as above)
- **Secondary slots**: AT25SF641 8 MB SPI1 (`hal_lfs_read_0 / hal_lfs_prog_0 / hal_lfs_erase_0`)
  - Erase unit for MCUboot: 64 kB blocks (`AT25SF641_SECTOR_SIZE = 0x10000`)
  - Page size: 256 B
- **`MCUBOOT_MAX_IMG_SECTORS`**: 16 (largest slot = Image 1 secondary = 16 × 64 kB blocks)
- **Scratch area**: not required for swap-using-offset
