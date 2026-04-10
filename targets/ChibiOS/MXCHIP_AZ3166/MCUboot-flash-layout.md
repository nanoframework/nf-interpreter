# MCUboot Flash Layout — MXCHIP_AZ3166

MCUboot replaces nanoBooter at the same base address. All CLR and deployment regions are unchanged.

## Flash map (STM32F412xG, 1 MB internal)

### Release build

| Region        | Address    | Size |
|---------------|-----------|------|
| MCUboot       | 0x08000000 | 32k  |
| CLR (image_0) | 0x08008000 | 224k |
| deployment    | 0x08040000 | 768k |

### Debug build

| Region        | Address    | Size |
|---------------|-----------|------|
| MCUboot       | 0x08000000 | 32k  |
| CLR (image_0) | 0x08008000 | 608k |
| deployment    | 0x080A0000 | 384k |

> No config block on this target (config region has len = 0 in linker).

## MCUboot configuration

- **Upgrade strategy**: TBD (Phase 2+)
- **Primary slot (image_0)**: internal STM32F412xG flash
- **Secondary slot (image_1)**: 2 MB SPI flash on EMW3166 module — **shared with WiFi firmware**; cannot be used as raw secondary slot without coordination with WICED stack
- **IFU support status**: Phase 2+ investigation required before secondary slot design can be finalised
