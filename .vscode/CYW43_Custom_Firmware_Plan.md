# CYW43 Custom Firmware — Project Plan

## Objective

Build a custom Bluetooth firmware for the CYW4343A2 chip (as used on the Raspberry Pi Pico W and Murata 1YN module) that enables full control over TX power, radio calibration, and patchram configuration.

## Deliverables

1. **Python toolchain** — Extract, parse, modify, and repack CYW43 BT firmware blobs
2. **Custom BT firmware blob** — Modified CYW4343A2 firmware with configurable TX power and radio parameters
3. **Custom NVRAM configuration** — Board-specific WiFi radio tuning
4. **Integration patch** — Modifications to cyw43-driver to use custom firmware
5. **Validation test suite** — HCI-level tests to verify firmware loads and operates correctly

---

## Community Reference Repositories

The following third-party repositories were reviewed and inform this plan:

### [peterharperuk/cyw43-firmware](https://github.com/peterharperuk/cyw43-firmware) — **HIGH VALUE**

Peter Harper (Raspberry Pi engineer) created the **canonical firmware build pipeline**. Key discoveries:

- **`make_firmware/Makefile`** — Complete Make-based pipeline that converts source binaries into the `.h` firmware headers used by cyw43-driver. This is the reference implementation for the build we need.
- **`bt_firmware/main.c`** — A C program that reads `brcm_patchram_buf[]` (Intel HEX-like records from Broadcom) and outputs raw binary. The BT firmware source (`cybt_firmware_43439.c`) contains the patchram data as `:` (colon) prefixed hex records — essentially an **Intel HEX variant** with data count, address, type, and payload. This reveals the upstream format before HCI wrapping.
- **`bt_firmware/cybt_firmware_43439.c`** — The "source" BT firmware for the 43439 with the `brcm_patch_version` string and `brcm_patchram_buf[]` in Intel HEX format. Also includes a separate A1 firmware in `a1_firmware/`.
- **Compression pipeline** — Uses `miniz` (gzip) to compress firmware blobs. Headers include `CYW43_RESOURCE_ATTRIBUTE` macro, and the Pico SDK supports both compressed and uncompressed firmware loading via `cyw43_firmware_funcs_t` vtable.
- **Flash firmware storage** — `flash_firmware.c` + `cyw43_flash_firmware_defs.h` define a flash page layout (`cyw43_flash_firmware_details_t`) for storing WiFi FW + NVRAM + BT FW in external flash instead of embedding in the binary. Saves ~150–230KB of program space.
- **`nvram_settings/`** — Contains both Pico W (`wifi_nvram_43439.h`) and Murata 1DX (`wifi_nvram_1dx.h`) NVRAM configs with `.raw` binary equivalents.
- **`test_flash_firmware/`** — Shows how to override `cyw43_get_firmware_funcs` to load from flash with streaming decompression.

**Impact on Plan**: Phase 1 can be drastically simplified — instead of writing a C header parser from scratch, we can use the existing Intel HEX → raw binary pipeline as a reference. The compression support opens up a new delivery option (gzip-compressed firmware in flash).

### [embassy-rs/embassy/cyw43-firmware](https://github.com/embassy-rs/embassy/tree/main/cyw43-firmware) — **MEDIUM VALUE**

Embassy (Rust embedded async framework) maintains raw `.bin` firmware files extracted from the cyw43-driver headers:

- **Raw binary format** — `43439A0.bin` (WiFi), `43439A0_btfw.bin` (BT), `43439A0_clm.bin` (CLM) are ready-to-use binary blobs, no C header parsing needed.
- **NVRAM as binary** — `nvram_rp2040.bin` and `nvram_sterling_lwb+.bin` are null-terminated key=value NVRAM configs in raw binary form.
- **`write_nvrams.rs`** — Rust script that generates NVRAM binary files. Contains the **complete Sterling LWB+ NVRAM config** — a different CYW43439 module with different PA calibration (`pa2ga0=-155,6912,-779`), lower max power (`maxp2ga0=78`), 26MHz crystal (`xtalfreq=26000`), UART transport (`muxenab=0x1`), disabled BT coex (`btc_mode=0`), antenna diversity (`bt_default_ant=0`), and energy detect thresholds. This is a real-world example of NVRAM customization for a different module.
- **Firmware sourced from** `georgerobotics/cyw43-driver` commit `a1dc885`, firmware version 7.95.49/7.95.62.

**Impact on Plan**: The Sterling LWB+ NVRAM provides a second reference config for Phase 3. The raw `.bin` files can serve as test inputs for our toolchain without needing to parse C headers first.

### [tabemann/cyw43-firmware](https://github.com/tabemann/cyw43-firmware) — **LOW VALUE**

Used by zeptoforth (a Forth for ARM). Contains only the WiFi firmware (`43439A0.bin` + `43439A0_clm.bin`) in raw binary form. No BT firmware. Firmware sourced from [Infineon/wifi-host-driver](https://github.com/Infineon/wifi-host-driver/tree/master/WiFi_Host_Driver/resources/firmware/COMPONENT_43439), synced with version 7.95.62.

**Impact on Plan**: Confirms the Infineon WHAD repo as an additional firmware source. The `COMPONENT_43439` path in the Infineon repo may contain newer firmware versions than the georgerobotics source.

### [utahrobotics/utah-lunabotics-2026/embedded-legacy/cyw43-firmware](https://github.com/utahrobotics/utah-lunabotics-2026/tree/main/embedded-legacy/cyw43-firmware) — **LOW VALUE**

Utah Robotics lunabotics competition project. Contains only `43439A0.bin` (WiFi) and `43439A0_clm.bin` — same raw binary format as embassy-rs. No BT firmware, no NVRAM, no tools. Recently updated (~1 week ago for "backwards compatibility"). Likely derived from embassy-rs or tabemann.

**Impact on Plan**: None beyond confirming the raw binary approach is popular in the embedded Rust ecosystem.

---

## Phase 0: Environment Setup

**Duration**: 1 day  
**Status**: Not started

- [ ] **0.1** Set up development environment with Python 3.10+, ARM GCC cross-compiler
- [ ] **0.2** Clone [georgerobotics/cyw43-driver](https://github.com/georgerobotics/cyw43-driver) repository
- [ ] **0.3** Clone [peterharperuk/cyw43-firmware](https://github.com/peterharperuk/cyw43-firmware) — reference build pipeline with `make_firmware/` tooling, Intel HEX source files, gzip compression, and flash storage support
- [ ] **0.4** Download raw `.bin` firmware files from [embassy-rs/embassy/cyw43-firmware](https://github.com/embassy-rs/embassy/tree/main/cyw43-firmware) — pre-extracted binaries (WiFi, BT, CLM, NVRAM) usable without C header parsing
- [ ] **0.5** Obtain hardware: Raspberry Pi Pico W (or board with CYW4343A2) + BLE sniffer
- [ ] **0.6** Set up serial/UART access to the CYW4343x (optional but highly recommended for debugging)
- [ ] **0.7** Install BT test tools: `hcitool`, `btmon`, nRF Connect (phone), or similar BLE analyzer
- [ ] **0.8** Obtain RF test equipment access if modifying power levels: spectrum analyzer, power meter, RF cable
- [ ] **0.9** Check [Infineon/wifi-host-driver](https://github.com/Infineon/wifi-host-driver/tree/master/WiFi_Host_Driver/resources/firmware/COMPONENT_43439) for newer firmware versions than the georgerobotics source

### Exit Criteria
- Can build and flash unmodified Pico W firmware
- Can see BLE advertisements from unmodified firmware on a phone/sniffer
- Have all reference firmware sources (C headers, Intel HEX, and raw binaries) available locally

---

## Phase 1: Firmware Extraction Toolchain

**Duration**: 1–2 days (reduced — leverage existing peterharperuk pipeline)  
**Status**: Not started  
**Depends on**: Phase 0

### Existing Assets to Leverage

- **peterharperuk `bt_firmware/main.c`** — Reference C program that converts Broadcom Intel HEX patchram records (`brcm_patchram_buf[]`) to raw binary via `xxd -r -p`. We can rewrite this in Python for our toolchain.
- **peterharperuk `make_firmware/Makefile`** — Shows the complete: Intel HEX → raw binary → `xxd -i` → C header → optional gzip pipeline. Our Python toolchain replaces this.
- **embassy-rs raw `.bin` files** — `43439A0_btfw.bin` and `nvram_rp2040.bin` can serve as immediate test inputs, bypassing C header parsing entirely for validation.
- **peterharperuk `cybt_firmware_43439.c`** — Reveals the upstream BT firmware format is Intel HEX records with `:` prefix, data count, 16-bit address, type byte, and payload. This is the format Broadcom/Infineon distributes internally.

### Tasks

- [ ] **1.1** Build C header parser — extract hex byte arrays from `.h` files
  - Input: `cyw43_btfw_1yn.h`, `cyw43_btfw_43439.h`, `cyw43_btfw_4343A1.h`
  - Output: Raw binary blobs
  - **Shortcut**: Validate against embassy-rs `.bin` files for the 43439

- [ ] **1.2** Build Intel HEX record parser (new — from peterharperuk insight)
  - Parse `:` prefixed records from `brcm_patchram_buf[]` format
  - Extract data count, address, type, and payload per record
  - This is the "upstream" format — closer to what Broadcom originally generates
  - Reference: `peterharperuk/make_firmware/bt_firmware/main.c`

- [ ] **1.3** Build HCI command decoder — parse FC4C/FC4E command streams
  - Decode opcode, parameter length, payload
  - Reassemble memory regions from FC4C write commands
  - Handle both 20-byte (1YN) and 255-byte (4343A1) chunk sizes

- [ ] **1.4** Build BRCMcfg section parser
  - Locate `BRCMcfgS` and `BRCMcfgD` magic markers
  - Extract section content from reassembled memory image
  - Parse length fields and content boundaries

- [ ] **1.5** Build structure annotator
  - Identify version string location and length
  - Map register write table entries (addresses in `0x006xxxxx`)
  - Map TX power table locations (calibration data patterns)
  - Map patchram code locations (ARM Thumb-2 code at `0x002xxxxx`)
  - Map patchram redirect table entries (at `0x000D0xxx`)

- [ ] **1.6** Build verification tool — round-trip test
  - Parse original firmware → memory regions → repack to HCI stream
  - Verify the output is byte-for-byte identical to the input
  - Cross-validate: our C header extraction vs. embassy-rs `.bin` files must match
  - Cross-validate: our Intel HEX parser vs. peterharperuk `main.c` output must match

### Exit Criteria
- All three firmware files parse without error
- Round-trip test passes for 1YN and 4343A1 firmware
- Cross-validation against embassy-rs `.bin` and peterharperuk pipeline matches
- Structure annotation produces annotated hex dump with labeled sections

### Artifacts
- `tools/cyw43_fw_parser.py` — Parser module (supports C header, Intel HEX, and raw binary inputs)
- `tools/cyw43_fw_annotate.py` — Structure annotation / dump tool
- `tests/test_roundtrip.py` — Round-trip verification tests

---

## Phase 2: Firmware Modification Engine

**Duration**: 3–4 days  
**Status**: Not started  
**Depends on**: Phase 1

### Tasks

- [ ] **2.1** Version string modifier
  - Replace version string in-place (pad with nulls if shorter)
  - Validate new string fits within allocated space
  - Update any length fields that reference the string

- [ ] **2.2** Register write table editor
  - List all register writes with addresses and current values
  - Allow modifying value for a given register address
  - Allow adding new register writes (if space permits in BRCMcfgD)
  - Known register map documentation:
    - `0x00600160–0x00600184`: Radio front-end config
    - `0x006001xx`: Baseband / modem registers
    - `0x006002xx`: PHY / RF calibration

- [ ] **2.3** TX power table editor
  - Parse power tables into structured format: `[channel][rate] = power_level`
  - Allow modification per-channel, per-rate
  - Allow global scaling (e.g., reduce all by 3 dB)
  - Validate values are in range (0–127, quarter-dBm units)
  - Support both tables (Classic BT and BLE)

- [ ] **2.4** BRCMcfgS modifier
  - Transport type byte
  - Chip/module variant ID bytes
  - BRCMcfgD load address

- [ ] **2.5** Firmware re-packer
  - Memory regions → HCI FC4C command stream (configurable chunk size)
  - Add FC4E Launch RAM command with appropriate address
  - Option: output in signed/direct format (for SDIO — without actual signature)

- [ ] **2.6** C header generator
  - Convert binary to C hex array header file
  - Include size define macros (`CYW43_BT_FW_LEN`, `CYW43_BT_FW_ADDR`, `CYW43_RAW_BT_FW_LEN`)
  - Apply `CYW43_RESOURCE_ATTRIBUTE` macro to array declaration (matches peterharperuk format)
  - Include comment with version and generation timestamp

- [ ] **2.7** Gzip compression support (from peterharperuk pipeline)
  - Compress raw firmware with gzip/zlib (the Pico SDK supports decompression via `tinf` library)
  - Generate both `.h` (uncompressed) and `.gz.h` (compressed) header variants
  - The compressed variant uses the streaming API (`cyw43_start_compressed_firmware` / `cyw43_read_compressed_firmware`)
  - Reference: `peterharperuk/make_firmware/Makefile` patterns `%.gz` and `%.gz.premeta.h`

- [ ] **2.8** Raw binary output for Rust/Embassy targets
  - Output raw `.bin` file (no C header wrapping) for use with embassy-rs and other non-C frameworks
  - Match the format of `43439A0_btfw.bin` from embassy-rs

### Exit Criteria  
- Can modify version string, regenerate, and verify only those bytes changed
- Can scale power table by known factor and verify in annotated dump
- Can change a register value and verify in annotated dump
- Generated C header compiles without warning

### Artifacts
- `tools/cyw43_fw_modify.py` — Modification engine
- `tools/cyw43_fw_build.py` — Complete build script (parse → modify → pack → header)
- `configs/custom_board.json` — JSON configuration file for custom firmware parameters

---

## Phase 3: NVRAM Configuration

**Duration**: 1–2 days  
**Status**: Not started  
**Depends on**: Phase 0

### Reference Configurations Available

Three known-good NVRAM configs to use as references:
1. **Pico W / CYW43439** — `wifi_nvram_43439.h` (georgerobotics & peterharperuk): SDIO, `muxenab=0x100`, `btc_mode=1`, `pa2ga0=-168,6649,-778`, `maxp2ga0=84` (21dBm), 37.4MHz xtal
2. **Sterling LWB+** — `nvram_sterling_lwb+.bin` (embassy-rs `write_nvrams.rs`): UART, `muxenab=0x1`, `btc_mode=0`, `pa2ga0=-155,6912,-779`, `maxp2ga0=78` (19.5dBm), **26MHz xtal**, `bt_default_ant=0`, energy detect thresholds (`edonthd20l=-72`, `edoffthd20ul=-78`). A real-world example of customization for a different module.
3. **Murata 1DX / CYW4343A1** — `wifi_nvram_1dx.h` (peterharperuk & georgerobotics): UART, `muxenab=0x11`, `btc_mode=1`, different PA calibration

### Tasks

- [ ] **3.1** Create template NVRAM generator
  - Python script that generates NVRAM in **both** C header (`.h`) and raw binary (`.bin`) formats
  - C header: null-terminated string concatenation format matching `wifi_nvram_43439.h`
  - Raw binary: null-separated key=value pairs with double-null terminator (matching embassy-rs `.bin` format)
  - Validates parameter ranges and types
  - Warns about missing critical parameters

- [ ] **3.2** Create board-specific NVRAM config
  - Start from `wifi_nvram_43439.h` (Pico W) as base
  - Use Sterling LWB+ config as comparison for what parameters to customize
  - Document every parameter with purpose and valid range
  - Set board-specific values:
    - `boardtype`, `boardrev`
    - `pa2ga0` (requires measurement — use Pico W values as placeholder)
    - `maxp2ga0` (target regulatory limit)
    - `muxenab` (transport: `0x100` for SDIO, `0x1` for UART, `0x11` for UART+WAKE)
    - `btc_mode` (coexistence: 0=off, 1=on)
    - `xtalfreq` (37400 for Pico W, 26000 for Sterling LWB+)

- [ ] **3.3** Validate NVRAM works with unmodified WiFi firmware
  - Flash Pico W with custom NVRAM + stock WiFi firmware
  - Verify WiFi connects and operates normally

### Exit Criteria
- Custom NVRAM loads without error
- WiFi connects to AP with custom NVRAM
- NVRAM generator produces valid C header AND raw binary outputs
- Generated binary matches embassy-rs format when using Pico W config

### Artifacts
- `tools/nvram_generator.py` — NVRAM config → C header + raw binary converter
- `configs/nvram_custom_board.yaml` — Board NVRAM configuration
- `firmware/wifi_nvram_custom.h` — Generated NVRAM C header
- `firmware/nvram_custom.bin` — Generated NVRAM raw binary (for Rust/Embassy targets)

---

## Phase 4: Firmware Integration & Loading

**Duration**: 2–3 days  
**Status**: Not started  
**Depends on**: Phase 2, Phase 3

### Integration Approaches (choose one or more)

1. **Embedded in binary (default)** — Replace `.h` header files in cyw43-driver/firmware/. Standard approach, firmware compiled into the application. Uses ~230KB flash.
2. **Compressed embedded** — Use gzip-compressed `.gz.h` headers (from peterharperuk pipeline). Saves flash at cost of RAM + CPU during decompression. Requires `CYW43_ENABLE_FIRMWARE_COMPRESSION=1` and the `tinf` decompression library.
3. **External flash** — Store WiFi FW + NVRAM + BT FW in a dedicated flash region using the `cyw43_flash_firmware_details_t` layout from peterharperuk. Saves ~150–230KB of program flash. Requires a separate "flash firmware" utility to write the blobs. Firmware loaded via `cyw43_get_firmware_funcs_flash()`.
4. **Raw binary for Rust** — Supply `.bin` files for embassy-rs or similar Rust frameworks.

### Tasks

- [ ] **4.1** Integrate custom BT firmware with cyw43-driver
  - Replace `cyw43_btfw_43439.h` or add conditional include
  - Verify the BT loading path accepts the modified firmware
  - Test both unsigned (HCI stream) and signed (direct) formats

- [ ] **4.2** Test firmware loading sequence
  - Monitor SDIO/SPI bus during firmware download (logic analyzer if available)
  - Verify BRCMcfgS is written to correct address
  - Verify BRCMcfgD is written to correct address
  - Verify Launch RAM command is sent
  - Verify chip responds with HCI ready event

- [ ] **4.3** Investigate signature bypass (if targeting SDIO)
  - Test: does the CYW43439 accept unsigned firmware over SDIO?
  - Test: does the `wb43439A0` combo firmware bypass BT signature?
  - Test: can BRCMcfgS `FB F0` bytes be omitted/zeroed?
  - Document results

- [ ] **4.4** Integrate custom NVRAM
  - Replace NVRAM header in build
  - Verify WiFi + BT both work with custom NVRAM + custom BT firmware

- [ ] **4.5** Flash-based firmware loading (optional, from peterharperuk)
  - Implement `cyw43_flash_firmware_details_t` flash page layout
  - Write "flash firmware" utility that programs WiFi FW + NVRAM + BT FW to flash
  - Override `cyw43_get_firmware_funcs` to use `cyw43_get_firmware_funcs_flash()`
  - Set `CYW43_FIRMWARE_FLASH_PAGE` to correct flash offset
  - Verify firmware loads from flash on cold boot
  - Reference: `peterharperuk/flash_firmware/flash_firmware.c` and `test_flash_firmware/flash_firmware_details.c`

- [ ] **4.6** Compressed firmware loading (optional, from peterharperuk)
  - Enable `CYW43_ENABLE_FIRMWARE_COMPRESSION`
  - Use gzip-compressed headers from Phase 2 task 2.7
  - Verify decompression works correctly during firmware download
  - Measure boot time impact vs. uncompressed

### Exit Criteria
- Custom firmware loads successfully on target hardware
- Chip responds to HCI_Reset command
- Read_Local_Version returns expected version string
- WiFi and BT both operational simultaneously
- (If flash path chosen) Firmware loads from flash on cold boot

### Artifacts
- Modified `cyw43_configport.h` or equivalent build config
- Test log showing successful firmware load and HCI communication
- (Optional) Flash firmware utility binary
- (Optional) Compressed firmware headers

---

## Phase 5: Validation & Testing

**Duration**: 3–5 days  
**Status**: Not started  
**Depends on**: Phase 4

### Tasks

- [ ] **5.1** HCI-level functional tests
  - HCI_Reset → Command Complete
  - HCI_Read_Local_Version_Information → correct version
  - HCI_Read_BD_ADDR → valid address
  - HCI_Read_Buffer_Size → valid ACL/SCO buffer info
  - HCI_LE_Set_Advertising_Parameters → success
  - HCI_LE_Set_Advertising_Data → success
  - HCI_LE_Set_Advertise_Enable → success + visible on scanner

- [ ] **5.2** BLE connection test
  - Advertise from CYW43
  - Connect from phone/other device
  - Exchange GATT data
  - Verify connection stability (1 hour sustained)

- [ ] **5.3** Classic BT test (if applicable)
  - Inquiry scan
  - Page scan / discoverable
  - SPP or A2DP connection (depending on stack)

- [ ] **5.4** Coexistence test
  - WiFi AP mode + BLE advertising simultaneously
  - WiFi STA + BLE connected simultaneously
  - Measure throughput degradation vs. standalone

- [ ] **5.5** RF validation (if power/antenna changed)
  - Conducted TX power per channel (all 14 BT channels)
  - Receiver sensitivity (BER at -70 dBm, -80 dBm, -90 dBm)
  - Adjacent channel rejection
  - Compare against original firmware baseline

- [ ] **5.6** Stress / stability test
  - 24-hour continuous BLE advertising + connection cycling
  - Repeated firmware load/unload cycles (100x)
  - Power cycling with firmware reload

### Exit Criteria
- All HCI functional tests pass
- BLE connection sustained for 1 hour
- WiFi+BT coexistence operates without crashes
- If RF modified: TX power within ±2 dB of target across all channels

### Artifacts
- `tests/test_hci_functional.py` — HCI test suite
- `tests/test_ble_connection.py` — BLE connection test
- `docs/test_results.md` — Test results documentation

---

## Phase 6: Documentation & Packaging

**Duration**: 1–2 days  
**Status**: Not started  
**Depends on**: Phase 5

### Tasks

- [ ] **6.1** Package all tools into a single `cyw43-custom-fw` repository/directory
  - `tools/` — All Python scripts
  - `configs/` — Board configuration files (JSON/YAML)
  - `firmware/` — Generated firmware headers
  - `tests/` — Test scripts
  - `docs/` — All documentation

- [ ] **6.2** Write user guide
  - How to customize for a new board (step-by-step)
  - Configuration parameter reference
  - Troubleshooting guide

- [ ] **6.3** Create CI pipeline (optional)
  - Automated: parse → modify → pack → verify round-trip
  - Run on PR to catch regressions

- [ ] **6.4** Archive known-good configurations
  - Original 1YN as reference baseline
  - Your custom board configuration
  - Test results for each

### Exit Criteria
- A new engineer can follow the guide to create firmware for a different board
- All tools run without error on clean Python 3.10+ environment
- Configuration files are documented and validated

---

## Risk Register

| Risk | Impact | Likelihood | Mitigation |
|------|--------|-----------|------------|
| SDIO signature blocks custom firmware on Pico W | High | Medium | Use UART path; investigate combo FW bypass; test unsigned load |
| TX power calibration requires expensive RF equipment | Medium | High | Use original power tables initially; only calibrate if changing antenna |
| Patchram code corruption causes BT stack crash | High | Low | Never modify patchram sections; verify with round-trip tests |
| ROM dump needed for advanced patching | Medium | Medium | Can read ROM via SDIO backplane once chip is running with stock FW |
| Regulatory non-compliance if increasing TX power | High | Medium | Start at lower power than original; get FCC/CE pre-scan before shipping |
| CYW4343A1 patchram slots exhausted (no room for custom patches) | Medium | High (A1 only) | Target A2 silicon which has 10+ free slots |
| Intel HEX format has undocumented quirks | Low | Medium | Cross-validate against peterharperuk `main.c` output and embassy-rs `.bin` files |
| Compressed firmware decompression fails on target | Medium | Low | Always maintain uncompressed path as fallback; test with `tinf` library on host first |
| Flash firmware layout conflicts with application flash usage | Medium | Medium | Calculate flash requirements carefully; use `PICO_FLASH_BANK_STORAGE_OFFSET` as reference |
| Newer Infineon firmware breaks assumptions | Medium | Low | Pin to known-good version (7.95.49); check Infineon WHAD repo for changelogs |

---

## Timeline Summary

```
Phase 0: Environment Setup         ████                         (1 day)
Phase 1: Extraction Toolchain      ████████                     (1-2 days, reduced via reuse)
Phase 2: Modification Engine       ████████████████             (3-4 days)
Phase 3: NVRAM Configuration       ████████                     (1-2 days, parallel with P1-2)
Phase 4: Integration & Loading     ████████████████             (2-4 days, +1 if flash/compress)
Phase 5: Validation & Testing      ████████████████████         (3-5 days)
Phase 6: Documentation             ████████                     (1-2 days)
                                   ─────────────────────────────
                                   Total: 12-20 working days
```

Phases 1–2 and Phase 3 can run in parallel.  
Phase 1 reduced from 2–3 days to 1–2 days by leveraging peterharperuk and embassy-rs.  
Phase 4 may extend by 1 day if implementing flash-based or compressed loading.  
Phase 5 duration depends on RF testing requirements.

---

## Decision Log

| Date | Decision | Rationale |
|------|----------|-----------|
| — | Use 1YN firmware as base | Unsigned, smallest, same A2 silicon as Pico W |
| — | Python for toolchain | Rapid prototyping, struct library, no build dependencies |
| — | 16-byte HCI chunk size | Matches 1YN format, maximum UART compatibility |
| — | Keep all patchram entries unmodified | ROM bug fixes are essential; no reason to change |
| — | Custom NVRAM as separate deliverable | Independent from BT FW; no signing issues |
| — | Leverage peterharperuk build pipeline as reference | Proven-working toolchain by RPi engineer; reduces Phase 1 by ~1 day |
| — | Support both C header and raw binary output | C headers for Pico SDK/cyw43-driver; raw `.bin` for Rust/Embassy ecosystem |
| — | Include Intel HEX parser in toolchain | Upstream Broadcom format found in `cybt_firmware_43439.c`; enables working closer to source |
| — | Use Sterling LWB+ NVRAM as second reference | Real-world customization example with different xtal, power, transport — validates our NVRAM generator |
| — | Defer flash-based loading to Phase 4 optional task | Saves ~150–230KB flash but adds complexity; not needed for initial bring-up |
| — | Pin firmware to version 7.95.49 (WiFi) / 001.003.016.0065 (BT) | Known-good baseline matching Pico W stock firmware |
