# MCUboot Upgrade Strategy — Per-Board Decision Table

This document records the MCUboot upgrade strategy selected for each nanoFramework
target, with rationale. The strategy is determined at design time based on available
flash, external storage hardware, and sector geometry.

---

## Upgrade Strategy Options

| Strategy | Description | Rollback | Requirements |
|---|---|---|---|
| **swap-using-move** | MCUboot swaps primary and secondary slots by moving sectors. No separate scratch area needed. Both slots must be on the same flash device. | Yes | Equal-size slots on the same device |
| **swap-using-offset** | MCUboot swaps a primary slot (internal flash) with a secondary slot on a different device (external storage). Swap status stored at end of primary slot. | Yes | External storage for secondary slot |
| **overwrite-only** | MCUboot overwrites the primary slot with the secondary slot contents. Simpler but no rollback. | No | Equal-size slots; any device combination |
| **Excluded / Not viable** | Target is not supported for MCUboot-managed IFU. Uses a vendor-native update mechanism, has insufficient flash, or has no external storage. | N/A | — |

---

## Decision Table

### ESP32 Family (ESP-IDF)

ESP32 targets have no nanoBooter. The ESP-IDF second-stage bootloader sits at a fixed
low address; MCUboot replaces it for IFU-capable builds. Secondary slots are in internal
flash (same chip as primary).

| Target / Flash Size | Upgrade Strategy | Secondary Slot | Rationale |
|---|---|---|---|
| **ESP32 — 4 MB** | **swap-using-move** | Internal flash | Default case; 4 MB provides enough headroom for MCUboot (128 KB), two equal slots, config, and deployment. No scratch area needed with move strategy. |
| **ESP32 — 8 MB** | **swap-using-move** | Internal flash | Ample flash; same strategy as 4 MB with larger slot and deployment sizes. |
| **ESP32 — 16 MB** | **swap-using-move** | Internal flash | Same as 8 MB pattern; extra space goes to deployment area. |
| **ESP32 — 32 MB** | **swap-using-move** | Internal flash | Available on ESP32-S3 and ESP32-P4 only; large deployment region. |
| **ESP32 — 2 MB** | **overwrite-only** | Internal flash | Tight budget: cannot afford the swap status/scratch overhead. No rollback, but the device can still receive IFU updates. Affected variants: ESP32, ESP32-C3, ESP32-H2 on 2 MB modules. |

> **Variant applicability:** All eight ESP32 chip variants (ESP32, ESP32-S2, ESP32-S3,
> ESP32-C3, ESP32-C5, ESP32-C6, ESP32-H2, ESP32-P4) follow the flash-size rules above.
> The 2 MB strategy applies only to the specific chip variants that ship on 2 MB modules.

> **Infeasible combinations:** ESP32-C3/H2 on 2 MB and ESP32-C5/C6 on 4 MB are
> documented as known constraints in the partition CSV files.

---

### STM32 Family (ChibiOS)

STM32 targets have nanoBooter today. MCUboot replaces nanoBooter at the same base
address. The **primary slot (nanoCLR) stays in internal MCU flash**. The secondary slot
lives on **external storage** — this design decision allows STM32 targets to keep their
full internal flash deployment region without sacrificing it for a second application
slot.

Supported external storage media:

| Media | Driver in nf-interpreter | Sector / erase unit for MCUboot |
|---|---|---|
| (Q)SPI NOR flash | `hal_lfs_read/prog/erase` (LittleFS) | 64 kB blocks (hardware erase unit) |
| SD card | FatFs | 4 kB virtual (file-backed; `flash_area_erase` fills with 0xFF) |
| USB MSD | FatFs | 4 kB virtual (same as SD card) |

> **FatFs bootloader size risk:** FatFs integration may push the MCUboot binary beyond the
> 64 KB budget. This must be validated in Phase 1 before committing to SD card or USB MSD.
> The (Q)SPI + LittleFS path is the lowest-risk first implementation.

#### STM32 Target Decision Table

| Target | SoC | Internal Flash | External Storage | Strategy | Secondary Slot Location | Rationale |
|---|---|---|---|---|---|---|
| **ORGPAL_PALTHREE** | STM32F76xx | 2 MB | AT25SF641 8 MB (SPI1) | **swap-using-offset** | AT25SF641 @ offset 0x000000 | Primary in internal flash, secondary on external SPI. Full rollback capability. No internal flash consumed for upgrade slot; deployment region at 1024 kB (unified layout). |
| **ORGPAL_PALX** | STM32F76xx | 2 MB | W25Q512 64 MB (QSPI) | **swap-using-offset** | W25Q512 @ offset 0x000000 | Same principle as PALTHREE. W25Q512 has 64 MB; MCUboot secondary slots consume < 2 MB, leaving > 62 MB for LittleFS. |
| **ST_STM32F769I_DISCOVERY** | STM32F76xx | 2 MB | SD card (FatFs) | **swap-using-offset** | SD card files `/mcuboot/img0_sec.bin`, `/mcuboot/img1_sec.bin` | SD card provides unlimited secondary slot space. FatFs virtual erase (fill 0xFF) used. Bootloader size with FatFs must be validated in Phase 1. |
| **MXCHIP_AZ3166** | STM32F412 | 1 MB | SPI flash (onboard) | **swap-using-offset** | External SPI flash | With secondary slot on external SPI, the 1 MB internal flash is no longer tight. Full primary slot and deployment region preserved. |
| **ST_STM32F429I_DISCOVERY** | STM32F429 | 2 MB | None available | **EXCLUDED** | N/A | No external storage hardware on this board. Internal flash is fully allocated; non-uniform sector layout makes dual-slot impractical without external storage. Continues using nanoBooter + Wire Protocol / JTAG updates. |
| **ST_NUCLEO64_F091RC** | STM32F091 | 256 KB | None | **NOT VIABLE** | N/A | Total flash (256 KB) is insufficient to fit MCUboot (~20 KB minimum) plus two application slots plus deployment area. Explicitly excluded from MCUboot support. Continues using nanoBooter + Wire Protocol / JTAG updates. |

---

### Other Platforms

| Target | Platform | Strategy | Rationale |
|---|---|---|---|
| **TI_CC3220SF** | TI SimpleLink | **EXCLUDED** | Vendor-native update mechanism (TI proprietary). MCUboot is not applicable. |
| **TI_CC3135R1** | TI SimpleLink | **EXCLUDED** | Same as CC3220SF — vendor-native update mechanism; no MCUboot support. |
| **NXP MIMXRT1060** | FreeRTOS / FlexSPI | **EXCLUDED** | No sponsor commitment for this target in the IFU project scope. May be revisited in a future phase. |

---

## Flash Sector Geometry Notes

### Why sector geometry matters

MCUboot's swap algorithms operate at the granularity of **flash erase sectors**. The
`flash_area_get_sectors()` function must report accurate sector geometry for each slot.
`MCUBOOT_MAX_IMG_SECTORS` must be set large enough to cover the sector count of the
largest slot across all images.

### STM32F76xx non-uniform internal flash

STM32F76xx bank 1 has a non-uniform sector layout:

| Sectors | Size each | Address range (bank 1) |
|---|---|---|
| 0–3 | 32 kB | 0x08000000–0x0800FFFF |
| 4 | 128 kB | 0x08010000–0x0802FFFF |
| 5–7 | 256 kB each | 0x08030000–0x080BFFFF |

Bank 2 mirrors bank 1 (same size pattern, base address 0x08100000).

The MCUboot unified layout places the Image 0 primary slot from `0x08010000` (960 kB),
spanning sectors 4–7 of bank 1. The `flash_area_get_sectors()` implementation in
`flash_map_stm32.c` must report the real geometry for these mixed-size sectors.
`MCUBOOT_MAX_IMG_SECTORS` is set per-target based on the largest slot sector count.

### External storage sectors for STM32

The secondary slot lives on external storage. MCUboot sees it as a flat byte range with
a uniform erase unit:

| Target | External Storage | MCUboot Erase Unit | `MCUBOOT_MAX_IMG_SECTORS` |
|---|---|---|---|
| ORGPAL_PALTHREE | AT25SF641 (SPI) | 64 kB | **16** (Image 1 secondary = 16 × 64 kB blocks) |
| ORGPAL_PALX | W25Q512 (QSPI) | 64 kB | **20** (Image 1 secondary = 20 × 64 kB blocks) |
| ST_STM32F769I | SD card (FatFs) | 4 kB (virtual) | **256** (Image 1 secondary = 256 × 4 kB) |

The default `MCUBOOT_MAX_IMG_SECTORS 512` in `mcuboot_config.h` covers all targets.
Per-target overrides in `mcuboot_stm32_config.h` can reduce this to save RAM (MCUboot
allocates one `flash_sector` struct per sector per slot).

---

## Summary Matrix

| Target | MCUboot Viable | Strategy | Phase |
|---|---|---|---|
| ESP32 — 4 MB | ✅ Yes | swap-using-move | Phase 2 |
| ESP32 — 8 / 16 / 32 MB | ✅ Yes | swap-using-move | Phase 7 |
| ESP32 — 2 MB | ✅ Yes (limited) | overwrite-only | Phase 7 |
| ORGPAL_PALTHREE | ✅ Yes | swap-using-offset | **Phase 1 MVP** |
| ORGPAL_PALX | ✅ Yes | swap-using-offset | Phase 7 |
| ST_STM32F769I_DISCOVERY | ✅ Yes | swap-using-offset | Phase 7 |
| MXCHIP_AZ3166 | ✅ Yes | swap-using-offset | Phase 7 |
| ST_STM32F429I_DISCOVERY | ❌ Excluded | — | Never |
| ST_NUCLEO64_F091RC | ❌ Not viable | — | Never |
| TI_CC3220SF | ❌ Excluded | — | Never |
| TI_CC3135R1 | ❌ Excluded | — | Never |
| NXP MIMXRT1060 | ❌ Excluded (scope) | — | TBD |
