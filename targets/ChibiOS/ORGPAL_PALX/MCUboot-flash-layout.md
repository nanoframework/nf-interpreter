# MCUboot Flash Layout — ORGPAL_PALX

MCUboot replaces nanoBooter at the same base address. All CLR, config, and deployment regions are unchanged.  
Both release and debug builds use the same linker layout (identical linker files).

## Flash map (STM32F76xx, 2 MB internal)

### Release and debug builds

| Region        | Address    | Size  |
|---------------|-----------|-------|
| MCUboot       | 0x08000000 | 32k   |
| config        | 0x08008000 | 32k   |
| CLR (image_0) | 0x08010000 | 704k  |
| deployment    | 0x080C0000 | 1280k |

## MCUboot configuration

- **Upgrade strategy**: swap-using-move
- **Primary slot (image_0)**: internal STM32F76xx flash
- **Secondary slot (image_1)**: external SPI/QSPI flash (see `target_littlefs.h` for geometry)
- **Scratch / swap status**: on external flash (Phase 1 design detail)
