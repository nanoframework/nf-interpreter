# MCUboot Flash Layout — ST_STM32F769I_DISCOVERY

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

| Region        | Address    | Size  |
|---------------|-----------|-------|
| MCUboot       | 0x08000000 | 32k   |
| config        | 0x08008000 | 32k   |
| CLR (image_0) | 0x08010000 | 960k  |
| deployment    | 0x08100000 | 1024k |

## MCUboot configuration

- **Upgrade strategy**: swap-using-move
- **Primary slot (image_0)**: internal STM32F76xx flash
- **Secondary slot (image_1)**: SD card (FatFs) — or may leverage dual-bank capability of STM32F769
- **Scratch / swap status**: on SD card or second bank (Phase 1 design detail)
