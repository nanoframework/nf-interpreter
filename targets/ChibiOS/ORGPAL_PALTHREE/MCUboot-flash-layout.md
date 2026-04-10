# MCUboot Flash Layout — ORGPAL_PALTHREE

MCUboot replaces nanoBooter at the same base address. All CLR, config, and deployment regions are unchanged.

## Flash map (STM32F76xx, 2 MB internal)

### Release build

| Region        | Address    | Size  |
|---------------|-----------|-------|
| MCUboot       | 0x08000000 | 32k   |
| config        | 0x08008000 | 32k   |
| CLR (image_0) | 0x08010000 | 704k  |
| deployment    | 0x080C0000 | 1280k |

### Debug build

nanoBooter debug was 64k; MCUboot stays 32k — sector 1 (0x08008000–0x08010000) becomes unused.

| Region        | Address    | Size   |
|---------------|-----------|--------|
| MCUboot       | 0x08000000 | 32k    |
| *(gap)*       | 0x08008000 | 32k    |
| config        | 0x08010000 | 32k    |
| CLR (image_0) | 0x08018000 | 1184k  |
| deployment    | 0x08140000 | 768k   |

## MCUboot configuration

- **Upgrade strategy**: swap-using-move
- **Primary slot (image_0)**: internal STM32F76xx flash
- **Secondary slot (image_1)**: AT25SF641 8 MB SPI1 external flash
  - Driver: `hal_lfs_read_0 / hal_lfs_prog_0 / hal_lfs_erase_0` (see `target_littlefs.c`)
  - Sector size: 4 kB; block size: 64 kB; page size: 256 B
- **Scratch / swap status**: on AT25SF641 (Phase 1 design detail)
