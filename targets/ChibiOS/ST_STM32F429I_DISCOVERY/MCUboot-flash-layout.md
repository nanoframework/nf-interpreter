# MCUboot Flash Layout — ST_STM32F429I_DISCOVERY

MCUboot replaces nanoBooter at the same base address. All CLR and deployment regions are unchanged.  
Both release and debug builds are identical (corrected in commit 51c42210 — release linker had wrong deployment address).

## Flash map (STM32F429xI, 2 MB internal)

### Release and debug builds

| Region        | Address    | Size  |
|---------------|-----------|-------|
| MCUboot       | 0x08000000 | 32k   |
| CLR (image_0) | 0x08008000 | 608k  |
| deployment    | 0x080A0000 | 1408k |

> No config block on this target (config region has len = 0 in linker).

## MCUboot configuration

- **Upgrade strategy**: overwrite-only
- **Primary slot (image_0)**: internal STM32F429xI flash
- **Secondary slot (image_1)**: NONE — 2 MB internal flash is fully occupied; no external NVM on the discovery board
- **IFU support status**: overwrite-only update only; OR excluded from IFU — decision pending Phase 1
