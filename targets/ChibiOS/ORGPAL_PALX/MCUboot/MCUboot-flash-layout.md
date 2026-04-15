# MCUboot Flash Layout — ORGPAL_PALX

MCUboot replaces nanoBooter at the same base address. PALX uses identical linker scripts for debug and release builds. The MCUboot internal flash layout is identical to the current nanoBooter release layout — **no linker script changes are required**.

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

- **Upgrade strategy**: swap-using-offset (primary in internal flash, secondary on W25Q512 external QSPI)
- **IMAGE_NUMBER**: 2 — Image 0 = nanoCLR, Image 1 = deployment
- **Primary slots**: internal STM32F76xx flash (as above)
- **Secondary slots**: W25Q512 64 MB QSPI (`hal_lfs_read_0 / hal_lfs_prog_0 / hal_lfs_erase_0`)
  - Erase unit for MCUboot: 64 kB blocks (`W25Q512_BLOCK_SIZE = 0x10000`)
  - Page size: 256 B
- **`MCUBOOT_MAX_IMG_SECTORS`**: 20 (largest slot = Image 1 secondary = 20 × 64 kB blocks)
- **Scratch area**: not required for swap-using-offset
