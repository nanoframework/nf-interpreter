# MCUboot Flash Layout — ST_STM32F769I_DISCOVERY

MCUboot replaces nanoBooter at the same base address. The MCUboot unified layout is identical to the **F769I debug nanoBooter layout** — so for debug builds nothing changes. Release deployment shrinks from 1280 kB to 1024 kB in MCUboot builds; legacy nanoBooter release builds are unchanged.

## Internal flash map (STM32F76xx, 2 MB)

| Region | Address | Size | Sector(s) | Notes |
|---|---|---|---|---|
| MCUboot | `0x08000000` | 32 kB | 0 | replaces nanoBooter |
| config | `0x08008000` | 32 kB | 1 | same as debug nanoBooter |
| Image 0 primary (CLR) | `0x08010000` | 960 kB | 2–7 (bank 1) | same as debug nanoBooter |
| Image 1 primary (deploy) | `0x08100000` | 1024 kB | bank 2 | same as debug nanoBooter; 256 kB less than release nanoBooter |

## External secondary slots (SD card via FatFs)

> **Risk:** FatFs integration may push the MCUboot binary beyond the 64 kB budget. This must be size-tested in Phase 1 before committing to SD card strategy. Alternative: use STM32F769 dual-bank for Image 0 secondary only (but Image 1 then has no secondary, limiting it to overwrite-only).

Secondary slots are managed as FatFs files. Virtual erase unit = 4 kB (FAT cluster). `flash_area_erase` fills the target region with 0xFF via `fwrite`. Files must be pre-created before MCUboot's first upgrade attempt.

| Area | `fa_id` | File path on SD card | Size | Virtual sector | Notes |
|---|---|---|---|---|---|
| Image 0 secondary (CLR) | `FLASH_AREA_IMAGE_0_SECONDARY` | `/mcuboot/img0_sec.bin` | 960 kB | 4 kB | 240 virtual sectors |
| Image 1 secondary (deploy) | `FLASH_AREA_IMAGE_1_SECONDARY` | `/mcuboot/img1_sec.bin` | 1024 kB | 4 kB | 256 virtual sectors |

## MCUboot configuration

- **Upgrade strategy**: swap-using-offset (primary in internal flash, secondary on SD card via FatFs)
- **IMAGE_NUMBER**: 2 — Image 0 = nanoCLR, Image 1 = deployment
- **Primary slots**: internal STM32F76xx flash (as above)
- **Secondary slots**: SD card FatFs files (requires FatFs driver integration in MCUboot porting layer)
- **`MCUBOOT_MAX_IMG_SECTORS`**: 256 (Image 1 secondary = 1024 kB / 4 kB virtual sectors)
- **Scratch area**: not required for swap-using-offset
- **Phase 1 validation required**: MCUboot binary size with FatFs must be ≤ 32 kB (sector 0 only) or ≤ 64 kB (with sector 1 reserved for MCUboot)
