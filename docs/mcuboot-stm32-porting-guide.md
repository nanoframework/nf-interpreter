# MCUboot STM32 Porting Guide

Step-by-step guide for adding MCUboot support to a new ChibiOS STM32F7 target in nf-interpreter.

Existing board implementations can serve as references:
- **ORGPAL_PALTHREE** — external SPI flash (AT25SF641) secondary slot, optional SD card fallback
- **ORGPAL_PALX** — external QSPI flash (W25Q512) secondary slot
- **ST_STM32F769I_DISCOVERY** — SD card FatFs secondary slot (deferred)

---

## 1. Overview

MCUboot replaces nanoBooter as the first-stage bootloader. The result is two independent firmware binaries built per target:

| Binary | Role | Size |
|---|---|---|
| `nanoBooter_MCUboot.elf` | MCUboot bootloader | ≤ 32 or 64 kB (see §5) |
| `nanoCLR.elf` | .NET nanoFramework CLR | fills primary slot |

MCUboot validates image signatures, performs firmware upgrades (swap-using-offset strategy), and then hands off to the CLR. The CLR binary is signed with `imgtool` as part of the build.

MCUboot manages two images:
- **Image 0** — nanoCLR runtime
- **Image 1** — managed deployment (application code)

Each image has a **primary slot** (internal flash, where the running image lives) and a **secondary slot** (external storage, where the upgrade candidate is staged).

---

## 2. Two-Binary Model

```
Internal flash:
  [0x08000000] MCUboot bootloader   (sectors 0-N)
  [0x080xxxxx] Config block          (1 sector, HAL-managed)
  [0x080xxxxx] Image 0 primary slot  (nanoCLR — signed)
  [0x081xxxxx] Image 1 primary slot  (deployment — signed)

External storage (SPI / QSPI / SD card):
  [offset 0x000000] Image 0 secondary slot (CLR upgrade candidate)
  [offset 0xxxxxxx] Image 1 secondary slot (deploy upgrade candidate)
```

Execution flow on boot:
1. MCUboot starts (Reset_Handler → `main()`)
2. `halInit()` + `chSysInit()` initialise ChibiOS
3. External storage initialised (`mcuboot_ext_flash_init()` or `mcuboot_sdcard_init()`)
4. `boot_go()` validates signatures and performs any pending upgrade
5. `do_boot()` jumps to the selected CLR image

---

## 3. Prerequisites

- ChibiOS with the target's board support (HAL drivers, `board.c`, `mcuconf.h`)
- External storage driver (SPI, QSPI, or SDMMC) integrated into ChibiOS HAL
- FatFs for SD card secondary slots (provided by ChibiOS community)
- `imgtool` from the MCUboot project (`pip install imgtool`) for image signing
- Signing key pair (Ed25519 or RSA); a development key is provided in the MCUboot source tree

---

## 4. Directory Structure

Create the following directory for the new board:

```
targets/ChibiOS/<BOARD>/
├── MCUboot/
│   ├── chconf.h                  # ChibiOS RT kernel config for bootloader
│   ├── halconf.h                 # ChibiOS HAL config for bootloader
│   ├── halconf_nf.h              # nf-overlay HAL extensions
│   ├── mcuconf.h                 # MCU peripheral config (clocks, SPI, SDC, etc.)
│   ├── mcuconf_nf.h              # nf-overlay MCU extensions
│   ├── mcuboot_config.h          # MCUboot tuning parameters (§7)
│   ├── mcuboot_flash_layout.h    # Slot address/size constants (§6)
│   ├── mcuboot_flash_map_boot.c  # flash_area_* porting layer (bootloader context)
│   ├── mcuboot_detect_pin.c      # boot_serial_detect_pin() + mcuboot_uart_init()
│   ├── mcuboot_sdcard_boot.c     # (optional) SD card init for FatFs secondary slots
│   ├── mcuboot_stm32f7xx_<board>.ld  # Linker script (§8)
│   ├── mcuboot_target.cmake      # CMake target definition (§11)
│   ├── MCUboot-flash-layout.md   # Flash layout documentation
│   └── target_platform.h        # Stub for FatFs (HAL_USE_SDC / HAL_USBH_USE_MSD)
```

The following files are **shared** across all ChibiOS MCUboot targets and must not be copied:

```
targets/ChibiOS/_mcuboot/
├── mcuboot_main.c               # Bootloader entry point
├── mcuboot_serial_port.c        # Serial recovery stub
├── mcuboot_serial_port.h
├── mcuboot_board_iface.h        # Board interface declarations
├── mcuboot_fatfs_flash_area.c   # FatFs virtual flash area driver
├── mcuboot_fatfs_flash_area.h
├── fatfs_diskio_boot.c          # FatFs diskio glue for ChibiOS SDC
├── stm32f7_flash_bare.c         # Bare-metal STM32F7 internal flash driver
├── stm32f7_flash_bare.h
├── stm32_f7xx_flash.h
├── startup_stm32f7.s            # ChibiOS crt0-based startup
├── system_stm32f7xx.c
├── nf_config.h
├── nanoHAL_v2.h
├── nanoHAL_Watchdog.h
└── hal_community_stub.c
```

---

## 5. Flash Layout Design

### 5.1 Internal flash structure

The MCU's internal flash is divided into **erase sectors** whose number and size are fixed by the silicon. The layout is not uniform: many STM32 devices (and others) use small sectors near the base address and progressively larger sectors deeper in flash. This asymmetry is deliberate — small sectors at the start allow fine-grained placement of the bootloader and any adjacent metadata blocks without wasting large flash regions on small payloads.

The flash map **must respect sector boundaries**. Every region boundary — bootloader end, config block, primary slot start — must coincide with the start of an erase sector. There is no sub-sector addressing for erase operations.

Consult the MCU reference manual and the ChibiOS board support layer (or HAL driver) to obtain the authoritative sector map for the target device.

### 5.2 Bootloader partition

MCUboot occupies the first N sectors of internal flash, starting at the reset vector base address. The partition size is not a fixed choice — it is driven by what the bootloader binary actually requires:

- Bare minimum: just MCUboot core and the internal flash driver.
- More space is needed when the bootloader must include additional stacks at build time (FatFs for SD card secondary slots, a SPI or QSPI driver, USB CDC, etc.).

The bootloader partition must be sized to a whole sector boundary that is at or beyond the largest expected binary. Allocating one or two extra small sectors as headroom avoids forced rework if the binary grows slightly. The existing targets illustrate two points on this spectrum — see their `MCUboot-flash-layout.md` for reference.

### 5.3 Config block (optional)

Some targets reserve one internal sector immediately after the bootloader partition for board configuration data managed by the HAL. This block is entirely outside MCUboot's concern:

- It must **not** be included in any MCUboot `flash_area`. MCUboot must never enumerate, erase, or write to it.
- The primary image slot (IMG0_PRI) starts at the sector immediately after the config block.
- The slot boundary constants in `mcuboot_flash_layout.h` must use the correct start address, accounting for the config sector.

Whether a config block exists is purely a target design decision. If the target has no need for a dedicated config sector the bootloader partition is immediately followed by IMG0_PRI.

### 5.4 Primary image slots

The remaining internal flash is divided between the two primary slots:

- **IMG0_PRI** — nanoCLR runtime binary (signed). Occupies a contiguous range of sectors starting right after the bootloader (and config, if present).
- **IMG1_PRI** — deployment image (managed application, signed). Typically placed in a second flash bank or at a higher address.

Both slots must start and end on sector boundaries. Their sizes must exactly equal the corresponding secondary slot sizes (MCUboot requirement).

**Sector size influence on slot boundaries:** When primary slots span sectors of mixed sizes (as is common on devices with small header sectors and large code sectors), the sector geometry determines the achievable slot sizes. The slot must end at the end of a sector — it cannot be trimmed to an arbitrary byte count. Design the layout by summing sector sizes from the chosen start address until the desired capacity is reached or exceeded.

### 5.5 Secondary slot storage

Secondary slots hold upgrade candidates on external storage (SPI/QSPI flash or SD card). The erase unit of the external device constrains the secondary slot size in the same way internal sector size constrains primary slots: the secondary slot size must be a whole multiple of the external erase unit.

This creates a constraint between primary and secondary: both must be the same size, so the primary slot must also be a whole multiple of the external erase unit. If the natural internal-flash sector boundary gives a primary size that is not divisible by the external erase unit, prefer a smaller external erase unit (many SPI flash devices support 4 kB sub-sector erase in addition to 64 kB block erase) rather than adjusting the primary slot boundary.

See §9 for details on each secondary slot backend.

### 5.6 Layout validation

Once the layout is decided, encode it in `mcuboot_flash_layout.h` (see §6) and add compile-time `static_assert` checks. Validate:

- IMG0_PRI does not overlap the config block or the bootloader partition.
- IMG0_PRI + IMG0_PRI_SIZE does not exceed IMG1_PRI start.
- IMG0_SEC + IMG0_SEC_SIZE does not exceed IMG1_SEC start (on the external device).
- IMG0_PRI_SIZE == IMG0_SEC_SIZE and IMG1_PRI_SIZE == IMG1_SEC_SIZE.
- Both secondary slot sizes are multiples of the external erase unit.

Document the final layout in `MCUboot-flash-layout.md` alongside the target source files. See the existing targets for examples.

---

## 6. `mcuboot_flash_layout.h`

This header is the single source of truth for slot addresses and sizes. It is included by both the MCUboot bootloader context (`mcuboot_flash_map_boot.c`) and the nanoCLR context (`mcuboot_flash_map.c`). All values are target-specific — derive them from the sector geometry and layout decisions in §5.

```c
// MCUboot bootloader slot
#define NF_MCUBOOT_SLOT_BOOTLOADER_OFF      <base>          // always 0x08000000
#define NF_MCUBOOT_SLOT_BOOTLOADER_SIZE     (<N>U * 1024U)  // size dictated by binary requirements (see §5.2)

// Config block (optional — omit if target has no config sector)
// This region is HAL-managed and must not be part of any MCUboot flash_area.
#define NF_MCUBOOT_CONFIG_OFF               <address>
#define NF_MCUBOOT_CONFIG_SIZE              (<N>U * 1024U)

// Image 0 primary (nanoCLR)
// Starts at the first sector after MCUboot (and config, if present).
#define NF_MCUBOOT_SLOT_IMG0_PRI_OFF        <address>
#define NF_MCUBOOT_SLOT_IMG0_PRI_SIZE       (<N>U * 1024U)

// Image 0 secondary (CLR upgrade on external storage)
#define NF_MCUBOOT_SLOT_IMG0_SEC_OFF        <offset>        // offset within the external device
#define NF_MCUBOOT_SLOT_IMG0_SEC_SIZE       (<N>U * 1024U)  // must equal IMG0_PRI

// Image 1 primary (deployment)
#define NF_MCUBOOT_SLOT_IMG1_PRI_OFF        <address>
#define NF_MCUBOOT_SLOT_IMG1_PRI_SIZE       (<N>U * 1024U)

// Image 1 secondary (deployment upgrade on external storage)
#define NF_MCUBOOT_SLOT_IMG1_SEC_OFF        <offset>
#define NF_MCUBOOT_SLOT_IMG1_SEC_SIZE       (<N>U * 1024U)  // must equal IMG1_PRI
```

Always add `static_assert` boundary checks to catch layout errors at compile time:

```c
static_assert(NF_MCUBOOT_SLOT_IMG0_PRI_OFF + NF_MCUBOOT_SLOT_IMG0_PRI_SIZE
              <= NF_MCUBOOT_SLOT_IMG1_PRI_OFF,
              "CLR primary overflows into deploy primary");
static_assert(NF_MCUBOOT_SLOT_IMG0_SEC_OFF + NF_MCUBOOT_SLOT_IMG0_SEC_SIZE
              <= NF_MCUBOOT_SLOT_IMG1_SEC_OFF,
              "CLR secondary overflows into deploy secondary");
```

See the existing targets' `mcuboot_flash_layout.h` for complete worked examples.

---

## 7. `mcuboot_config.h`

Defines MCUboot compile-time tuning parameters for the board.

```c
// STM32F7 FLASHv2: minimum write unit is 4 bytes (FLASH_CR_PSIZE_WORD).
#define MCUBOOT_FLASH_WRITE_ALIGNMENT   4U

// Erase unit of the secondary slot device:
//   SPI/QSPI 64 kB block erase:  64U * 1024U
//   FatFs virtual sector:         4U * 1024U
#define MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE  (64U * 1024U)

// Must match --header-size argument to imgtool sign.
#define MCUBOOT_IMAGE_HEADER_SIZE       0x200U

// Maximum sectors across any single slot.
// = largest slot size / MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE
// Example: IMG1_SEC = 1024 kB / 64 kB = 16
#define MCUBOOT_MAX_IMG_SECTORS         16U
```

`MCUBOOT_MAX_IMG_SECTORS` is the most common mistake: it must be large enough for the largest slot on the worst-case erase unit. For FatFs virtual sectors (4 kB), a 1024 kB slot requires 256 sectors.

---

## 8. Linker Script

The MCUboot linker script defines only the bootloader flash region and the RAM layout. It delegates code placement rules to the shared ChibiOS `rules_*.ld` files.

Key decisions:

1. **`flash0`** — set `org` to the MCUboot base address (`0x08000000`) and `len` to the bootloader partition size (32 kB or 64 kB).
2. **`flash1`** — ITCM alias of `flash0`; `org` = `0x00200000 + offset` (same offset as `flash0`).
3. **`config`, `deployment`** — set to `org = 0x00000000, len = 0` (bootloader does not use these).
4. **`ram3`** — DTCM-RAM; expand to full 128 kB (`org = 0x20000000, len = 128k`) since there is no boot clipboard in the MCUboot bootloader.

```
MEMORY
{
    flash0 (rx) : org = 0x08000000, len = 64k   /* MCUboot bootloader (sectors 0-1) */
    flash1 (rx) : org = 0x00200000, len = 64k   /* ITCM alias */
    /* ... other flash/ram regions ... */
    ram3   (wx) : org = 0x20000000, len = 128k  /* DTCM-RAM — full, no boot clipboard */
}
```

Then include the shared rules and any board-specific sections (`.nocache`, `.eth`):

```
INCLUDE rules_stacks.ld
/* ... .nocache / .eth SECTIONS ... */
INCLUDE rules_code.ld
INCLUDE rules_data.ld
INCLUDE rules_memory.ld
```

---

## 9. Secondary Slot Options

Three secondary slot backends are supported. Choose the one appropriate for the target's external storage.

**SPI flash** — secondary slots backed by a SPI NOR flash device accessed via the ChibiOS `SPI` HAL driver. `mcuboot_ext_flash_init()` initialises the SPI bus and the device. Read/write/erase are routed through the device-specific driver. Set `MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE` to the device's erase unit (typically 4 kB for sub-sector erase or 64 kB for block erase). See ORGPAL_PALTHREE (AT25SF641 via SPID1) as a reference.

**QSPI flash** — same as SPI flash but accessed via the STM32 QUADSPI peripheral. Set `MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE` to match the device's erase unit. See ORGPAL_PALX (W25Q512) as a reference.

**SD card via FatFs** — secondary slot images are stored as files on an SD card. Enabled by the `NF_FEATURE_MCUBOOT_HAS_SDCARD` Kconfig flag.

- Files: `/mcuboot/img0_sec.bin` (Image 0) and `/mcuboot/img1_sec.bin` (Image 1)
- `mcuboot_sdcard_init()` in `mcuboot_sdcard_boot.c` mounts the SD card via `sdcStart()` + `f_mount()`
- The shared `mcuboot_fatfs_flash_area.c` (in `targets/ChibiOS/_mcuboot/`) implements the flash area read/write/erase operations using FatFs file I/O
- Set `MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE = 4U * 1024U` (virtual sector granularity)
- Add `mcuboot_sdcard_boot.c` to `mcuboot_target.cmake` under the `NF_FEATURE_MCUBOOT_HAS_SDCARD` guard

When `mcuboot_ext_flash_init()` returns -1 and no SD card is configured, MCUboot boots the primary slot directly without attempting an upgrade (graceful degradation). See ST_STM32F769I_DISCOVERY and ORGPAL_PALTHREE as references.

---

## 10. Serial Recovery

MCUboot can enter a serial recovery mode when a detection pin is held at power-on.

### 10.1 Detection pin (`mcuboot_detect_pin.c`)

Implement `boot_serial_detect_pin()` to sample the boot button GPIO. The function must return `true` when the button is pressed. MCUboot waits `MCUBOOT_SERIAL_DETECT_DELAY` milliseconds (default 100 ms) before calling this function — no additional debounce is needed.

The specific GPIO port, pin number, and active polarity depend on the target board. Consult the board schematic and see the existing targets' `mcuboot_detect_pin.c` for reference.

### 10.2 UART initialisation (`mcuboot_uart_init()`)

The shared `mcuboot_serial_port.c` provides a weak default `mcuboot_uart_init()` that does nothing. Override it in `mcuboot_detect_pin.c` for boards that use UART for recovery. Configure the UART peripheral (port, pins, baud rate, alternate function) to match the board's intended recovery channel. See the existing targets for reference.

> **Note:** The full SMP serial recovery stack is a placeholder (`boot_serial_start()` loops forever). Recovery currently requires a JTAG reset. Full SMP implementation is deferred.

---

## 11. CMake Integration

### 11.1 `mcuboot_target.cmake`

Create `targets/ChibiOS/<BOARD>/MCUboot/mcuboot_target.cmake`:

```cmake
set(MCUBOOT_EXTRA_SOURCES
    ${CMAKE_SOURCE_DIR}/targets/ChibiOS/<BOARD>/board.c
    ${CMAKE_SOURCE_DIR}/targets/ChibiOS/<BOARD>/MCUboot/mcuboot_flash_map_boot.c
    ${CMAKE_SOURCE_DIR}/targets/ChibiOS/<BOARD>/MCUboot/mcuboot_detect_pin.c
)

if(NF_FEATURE_MCUBOOT_HAS_SDCARD)
    list(APPEND MCUBOOT_EXTRA_SOURCES
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/<BOARD>/MCUboot/mcuboot_sdcard_boot.c
    )
endif()

nf_setup_mcuboot_target_build(

    LINKER_FILE
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/<BOARD>/MCUboot/mcuboot_stm32f7xx_<board>.ld

    EXTRA_SOURCES
        ${MCUBOOT_EXTRA_SOURCES}

    EXTRA_INCLUDES
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/<BOARD>
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/<BOARD>/MCUboot

    COMPILE_DEFINITIONS
        MCUBOOT_USE_TINYCRYPT=1

    EXTRA_COMPILE_OPTIONS
        $<$<CONFIG:Release>:-Os>
        $<$<CONFIG:Debug>:-Os -g3>
)
```

### 11.2 CMake preset

Add a preset entry in `targets/ChibiOS/CMakePresets.json` for the MCUboot build, inheriting from the board's base preset and overriding the bootloader source.

---

## 12. Image Signing with `imgtool`

The nanoCLR binary must be signed before it can be validated by MCUboot. The build system calls `imgtool sign` as a post-build step.

Basic command:

```bash
imgtool sign \
    --key <signing_key.pem> \
    --header-size 0x200 \
    --align 4 \
    --version 1.0.0 \
    --slot-size <IMG0_PRI_SIZE_IN_BYTES> \
    nanoCLR.bin \
    nanoCLR_signed.bin
```

Key parameters:

| Parameter | Value | Notes |
|---|---|---|
| `--header-size` | `0x200` | must match `MCUBOOT_IMAGE_HEADER_SIZE` |
| `--align` | `4` | must match `MCUBOOT_FLASH_WRITE_ALIGNMENT` |
| `--slot-size` | IMG0 primary size in bytes | must match `NF_MCUBOOT_SLOT_IMG0_PRI_SIZE` exactly |
| `--version` | semver string | MCUboot compares versions during upgrade decision |

The signing key for production firmware must be kept secure. For development, use the test key from the MCUboot source tree (`root-ed25519.pem`).

---

## 13. Testing Checklist

Before declaring a new board complete:

- [ ] MCUboot binary fits within the bootloader partition (check `.map` file)
- [ ] `static_assert` checks in `mcuboot_flash_layout.h` pass (compile-time)
- [ ] `boot_serial_detect_pin()` returns correct value for button pressed/released
- [ ] External storage initialises successfully (check `mcuboot_ext_flash_init()` return value via debugger)
- [ ] A signed nanoCLR image boots correctly (`boot_go()` succeeds, `do_boot()` jumps to CLR)
- [ ] Upgrade works: place signed upgrade image in secondary slot, reboot, verify MCUboot swaps images
- [ ] After upgrade: reboot again, verify MCUboot confirms the new image (prevents rollback to old version)
- [ ] Serial recovery detection pin correctly triggers recovery mode when held at power-on
- [ ] `MCUboot-flash-layout.md` is up to date with the actual slot constants
