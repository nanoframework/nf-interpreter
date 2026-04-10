# CYW43 Firmware — Complete Comparison & Custom Firmware Guide

## Table of Contents

1. [Firmware Inventory](#firmware-inventory)
2. [Bluetooth Firmware Comparison](#bluetooth-firmware-comparison)
3. [WiFi Firmware Overview](#wifi-firmware-overview)
4. [NVRAM Configuration Summary](#nvram-configuration-summary)
5. [Architecture Deep Dive](#architecture-deep-dive)
6. [Key Learnings](#key-learnings)
7. [Creating Custom Firmware](#creating-custom-firmware)

---

## Firmware Inventory

### Bluetooth Firmware Files

| File | Chip | Rev | Transport | Size | Format | Signed | Module |
|------|------|-----|-----------|------|--------|--------|--------|
| `cyw43_btfw_43439.h` | CYW4343A2 | A2 | SDIO 37MHz | 6,970 B | Direct/signed | **Yes** | Pico W |
| `cyw43_btfw_4343A1.h` | CYW4343A1 | A1 | UART 37.4MHz | 35,182 B | HCI cmd stream | No | Murata 1DX |
| `cyw43_btfw_1yn.h` | CYW4343A2 | A2 | UART | 4,857 B | HCI cmd stream | No | Murata 1YN |

### WiFi Firmware Files

| File | Chip | FW Version | WiFi FW Size | CLM Size | Total | Notes |
|------|------|------------|-------------|----------|-------|-------|
| `w43439A0_7_95_49_00_combined.h` | CYW43439 | 7.95.49 | 224,190 B | 984 B | ~225 KB | Pico W standard |
| `w43439_sdio_1yn_7_95_59_combined.h` | CYW43439 | 7.95.59 | 228,914 B | 4,752 B | ~234 KB | Murata 1YN |
| `wb43439A0_7_95_49_00_combined.h` | CYW43439 | 7.95.49 | 231,077 B | 984 B | ~232 KB | **WiFi+BT combo** |
| `w4343WA1_7_45_98_102_combined.h` | CYW4343WA1 | 7.45.98.102 | 421,331 B | 7,222 B | ~429 KB | Murata 1DX (latest) |
| `w4343WA1_7_45_98_50_combined.h` | CYW4343WA1 | 7.45.98.50 | 383,110 B | 7,222 B | ~390 KB | Murata 1DX (older) |

### NVRAM Configuration Files

| File | Module | Transport Focus | Key Differences |
|------|--------|-----------------|-----------------|
| `wifi_nvram_43439.h` | Pico W | SDIO | maxp=84, btc_mode=1, muxenab=0x100 |
| `wifi_nvram_1dx.h` | Murata 1DX | GPIO/UART | maxp=74, swdiv_en=1, muxenab=0x10 |
| `wifi_nvram_1yn.h` | Murata 1YN | UART+WAKE | maxp=74, btc_mode=0, muxenab=0x11 |

---

## Bluetooth Firmware Comparison

### Size & Complexity

```
                    CYW4343A1 (1DX)    ████████████████████████████████████ 35,182 B
                    CYW43439 (Pico W)  ███████ 6,970 B
                    CYW4343A2 (1YN)    █████ 4,857 B
```

The CYW4343A1 is **5-7× larger** than both CYW4343A2 variants because:
- **Older ROM (A1)** needs far more patchram fixes
- **Different architecture** — the A2 silicon fixed many A1 bugs at the mask level
- The two A2 firmwares (43439 and 1YN) are similar in size despite different transports

### Binary Format Comparison

| Aspect | CYW43439 (Pico W) | CYW4343A1 (1DX) | CYW4343A2 (1YN) |
|--------|-------------------|------------------|------------------|
| **Envelope** | Version string + raw sections | HCI FC4C commands (255B chunks) | HCI FC4C commands (20B chunks) |
| **Chunk size** | N/A (direct) | Up to 251 bytes data | 16 bytes data |
| **# of commands** | 0 (direct write) | 138 FC4C + 1 FC4E | 212 FC4C + 1 FC4E |
| **Launch cmd** | N/A | FC4E @ 0xFFFFFFFF | FC4E @ 0xFFFFFFFF |
| **Signing** | Yes (`_dl_signed`) | No | No |

### BRCMcfgS — Static Config Comparison

All three share the same 50-byte internal structure with a common prefix:

```
Common: 32 00 00 00 01 01 04 18 92 00 00 00 03 06 AC 1F
                                                         ^^ ^^ Chip-specific from here
43439:  12 A2 43 43 00 01 1C 00 F0 21 00 ... FE 00 00 FB F0 00 00
4343A1: 12 A1 43 43 00 01 1C 52 18 21 00 ... FE 00 00
1YN:    2A 0A 43 43 00 01 1C 00 F0 21 00 ... FE 00 00
```

| Field | CYW43439 | CYW4343A1 | CYW4343A2 1YN |
|-------|----------|-----------|---------------|
| Chip revision | 0x12A2 (A2) | 0x12A1 (A1) | 0x2A0A (1YN) |
| Chip family | 0x4343 | 0x4343 | 0x4343 |
| BRCMcfgD addr | 0x0021F000 | 0x00211852 | 0x0021F000 |
| Signature | Yes (FB F0) | No | No |

**Key insight**: The 43439 and 1YN share the same BRCMcfgD load address (0x0021F000), confirming they run on the same silicon. The 4343A1 uses a different address (0x00211852) because its patchram layout is fundamentally different.

### BRCMcfgD — Dynamic Config Comparison

| Content | CYW43439 | CYW4343A1 | CYW4343A2 1YN |
|---------|----------|-----------|---------------|
| **Size** | 6,802 B | 34,131 B | 2,428 B |
| **Version string** | "SDIO 37_4M wlbga_BU RPI" | "(BCM4343A1 UART 37.4 MHz wlbga_ref wiced" | "UART 4343A2 wlbga_BU" |
| **Register writes** | ~22 | ~33 | ~19 |
| **Patchram refs** | ~8 | ~211 | ~5 |
| **Patchram pairs** | (embedded) | 16+ (explicit table) | (via 0x000D region) |
| **TX power tables** | 3 regions | 2 regions | 3 regions |
| **Memory regions** | 1 contiguous | 1 contiguous (34KB) | 3 non-contiguous |

### TX Power Tables — The Shared DNA

The CYW43439 and 1YN firmwares have **identical TX power calibration tables**:

```
Both:   E=4/.83-*'   NJ=97@:3.-   _XMGCKE=75   mjYSRWQG@>   yh`]aZPHF
```

The CYW4343A1 has **different power tables**:

```
A1:     C977885565   LEABC@<;;<   \SRRRK GGGG   jc__dWSRRS   |qnnqa]\\]
```

This confirms:
- **Power tables are chip-revision-specific, not board-specific** (both A2 variants share them)
- **Board-specific power tuning is in NVRAM**, not in BT firmware
- The A1 has lower/different power characteristics reflecting older silicon

### Patchram Architecture Differences

| Aspect | CYW4343A1 | CYW4343A2 (both) |
|--------|-----------|-------------------|
| ROM patch count | ~211 addresses | ~5-8 addresses |
| ROM address range | 0x0000Bxxx–0x000D6xxx | 0x00200xxx–0x00280xxx |
| RAM code range | 0x00200804–0x00210A34 | 0x00209Cxx–0x0021Fxxx |
| Patch table | Explicit ROM→RAM pairs | Implicit / via 0x000D registers |
| Total RAM used | ~34 KB | ~2-7 KB |

The A2 silicon moved most fix targets to higher ROM addresses (0x002xxxxx range), suggesting a reorganized ROM layout that is much more stable.

---

## WiFi Firmware Overview

### Structure

All WiFi firmwares share the same binary structure:

```
Offset 0x0000: ARM Cortex-M3 vector table (32 × 4-byte entries)
               - Entry 0: Initial SP
               - Entry 1: Reset handler
               - Entries 2-31: Exception/interrupt handlers

Offset 0x0080: Padding (zeros)

Offset 0x00C0: "DBPP" magic (0x44 0x42 0x50 0x50)
               Broadcom firmware container header

Offset 0x00C8: Function pointer / offset tables
               Internal firmware subsystem entry points

Offset ~0x0200: Actual firmware code begins
               ARM Thumb-2 code for the WiFi MAC/PHY stack

[CYW43_WIFI_FW_LEN]: End of WiFi firmware
[padded to 512B]: Zero padding

[CYW43_WIFI_FW_LEN padded]: CLM (Country Locale Matrix) blob
               Regulatory/channel configuration per country
```

### Vector Table Comparison

| FW Version | Reset Vector | Default Handler | Architecture |
|------------|-------------|-----------------|--------------|
| CYW43439 v7.95.49 | 0x00001465 | 0x00001391 | ARM Cortex-M3 |
| CYW43439 v7.95.59 (1YN) | 0x00001465 | 0x00001391 | ARM Cortex-M3 |
| CYW4343WA1 v7.45.98.102 | 0x000021ED | 0x00002119 | ARM Cortex-M3 |
| CYW4343WA1 v7.45.98.50 | 0x000021B1 | 0x000020DD | ARM Cortex-M3 |
| wb43439 v7.95.49 (BT combo) | 0x00001471 | 0x0000139D | ARM Cortex-M3 |

**Observations**:
- CYW43439 variants (v7.95.x) share nearly identical vector tables — same code base, minor patches
- CYW4343WA1 has handlers at much higher addresses (~0x2000 vs ~0x1400) — different/larger firmware
- The combo "wb" firmware has slightly different vectors from the standard "w" version — BT stack additions

### CLM (Country Locale Matrix)

| FW File | CLM Size | CLM Source |
|---------|----------|------------|
| Pico W (43439) | 984 B | `43439_raspberrypi_picow_v5_220624.clm_blob` |
| 1YN | 4,752 B | `cyfmac43439-sdio.1YN.clm_blob` |
| wb43439 (combo) | 984 B | Same as Pico W |
| WA1 (both versions) | 7,222 B | (unnamed) |

The CLM sizes vary dramatically:
- **Pico W**: 984 B — minimal CLM, likely supports fewer regions
- **1YN**: 4,752 B — Murata's regulatory data covers more countries
- **WA1**: 7,222 B — most comprehensive regulatory database

### WiFi FW Size Progression

```
FW Version | Size      | Notes
──────────────────────────────────────────
7.45.98.50 | 383,110 B | CYW4343WA1 older
7.45.98.102| 421,331 B | CYW4343WA1 newer (+38KB patches)
7.95.49    | 224,190 B | CYW43439 base
7.95.49 bt | 231,077 B | CYW43439 WiFi+BT combo (+7KB BT)
7.95.59    | 228,914 B | CYW43439 1YN variant (+5KB)
```

The v7.95.x series (CYW43439) is ~40% smaller than v7.45.x (CYW4343WA1) — significant code optimization between chip generations.

---

## NVRAM Configuration Summary

> See [CYW43_NVRAM_Analysis.md](CYW43_NVRAM_Analysis.md) for the detailed parameter-by-parameter comparison.

### Critical Parameters for Custom Boards

| Parameter | What It Controls | Must Customize? |
|-----------|-----------------|-----------------|
| `pa2ga0` | PA linearization (3 coefficients) | **Yes** — antenna-specific |
| `maxp2ga0` | Max TX power (quarter-dBm) | **Yes** — regulatory |
| `boardflags` | Hardware feature enables | **Yes** — board-specific |
| `muxenab` | Pin muxing (SDIO vs UART) | **Yes** — transport-specific |
| `btc_mode` | BT coexistence mode | **Yes** — combo chip config |
| `legofdmbw202gpo` | Per-rate power backoff | Recommended |
| `mcsbw202gpo` | MCS rate power backoff | Recommended |
| `swdiv_en/gpio` | Antenna diversity | If applicable |
| `boardtype/rev` | Board identity | Should update |

### Parameters That Are Safe to Copy

| Parameter | Value (all boards) | Safe? |
|-----------|-------------------|-------|
| `vendid` | 0x14e4 | Yes — always Broadcom |
| `devid` | 0x43e2 | Yes — CYW4343x family |
| `sromrev` | 11 | Yes — format version |
| `xtalfreq` | 37400 | Yes — standard crystal |
| `nocrc` | 1 | Yes — performance setting |
| `ccode` | ALL | Yes — default country |
| `ofdmdigfilttype` | 18 | Yes — digital filter |
| `papdmode` | 1 | Yes — standard PAPD |

---

## Architecture Deep Dive

### CYW4343x Chip Architecture

```
┌──────────────────────────────────────────────┐
│                CYW4343x SoC                   │
│                                               │
│  ┌─────────────┐    ┌──────────────────────┐  │
│  │  ARM CM3    │    │   802.11 MAC/PHY     │  │
│  │  (BT+WiFi)  │    │   RF Frontend        │  │
│  │             │    │   ADC/DAC            │  │
│  │  ROM (fixed)│    │                      │  │
│  │  RAM (patch)│    └──────────────────────┘  │
│  │             │                              │
│  │  Patchram   │    ┌──────────────────────┐  │
│  │  Hardware:  │    │  SDIO Interface       │  │
│  │  ROM→RAM    │    │  UART Interface       │  │
│  │  redirect   │    │  GPIO / Pin Mux       │  │
│  └─────────────┘    └──────────────────────┘  │
│                                               │
│  ┌─────────────────────────────────────────┐  │
│  │         Shared SRAM (~512KB)             │  │
│  │  0x00200000: Patchram code area          │  │
│  │  0x000D0000: Patchram config registers   │  │
│  │  0x00600000: Peripheral registers        │  │
│  └─────────────────────────────────────────┘  │
└──────────────────────────────────────────────┘
```

### Firmware Loading Sequence

```
Host                                    CYW4343x
 │                                        │
 │  ─── Reset (GPIO or SDIO cmd) ───────> │
 │                                        │ ROM bootloader starts
 │                                        │
 │  ──── WiFi FW (224KB via SDIO) ──────> │ → Internal SRAM
 │  ──── CLM blob (appended) ───────────> │ → Internal SRAM
 │                                        │
 │  ──── NVRAM config (text) ───────────> │ → Parsed into registers
 │                                        │
 │  ──── BT FW (BRCMcfgS + BRCMcfgD) ──> │ → Patchram + registers
 │                                        │
 │  ──── Launch command ────────────────> │ → Execute from RAM
 │                                        │
 │  <─── HCI Ready event ──────────────── │
 │  <─── WiFi Ready ──────────────────── │
```

### Patchram Mechanism

The patchram is a **hardware feature** of the CYW4343x that allows ROM code to be overridden:

1. The chip has a set of **patchram registers** at 0x000D0000-0x000D0FFF
2. Each register pair contains: `(ROM_address_to_intercept, RAM_address_of_replacement)`
3. When the CPU fetches an instruction from a matched ROM address, the hardware **transparently redirects** to the RAM address
4. This happens at the instruction fetch level — zero cycle overhead after the redirect table is loaded

**Number of patchram slots**: Limited (typically 16-32 entries on CYW4343x). This is why:
- A1 firmware uses ~16 explicit pairs (likely hitting the hardware limit)
- A2 firmware uses only ~5-8 (the improved ROM needs fewer patches)

---

## Key Learnings

### 1. The Firmware Is Mostly Data, Not Code

BT firmware is **not** a full OS/stack — it's a patch blob containing:
- **80-90%**: Configuration data (register values, power tables, calibration)
- **10-20%**: Actual code patches (Thumb-2 ROM fixes)
- The main BT stack runs from **ROM** — the firmware only patches bugs and configures radio

### 2. A2 Silicon Is Much More Mature Than A1

| Metric | A1 (CYW4343A1) | A2 (CYW4343A2/43439) |
|--------|----------------|----------------------|
| BT FW size | 35 KB | 5-7 KB |
| Patchram entries | ~16 (likely maxed out) | ~5-8 |
| Register writes | ~33 | ~19-22 |
| WiFi FW size | 383-421 KB | 224-231 KB |

The A2 revision fixed so many ROM bugs that the patch requirements dropped by ~85%.

### 3. Transport Determines Format, Not Content

The same CYW4343A2 chip produces two different firmware formats:
- **SDIO (Pico W)**: Direct memory writes, signed format, no HCI wrapping
- **UART (1YN)**: HCI vendor commands (FC4C/FC4E), unsigned, same core content

The actual patchram code and power tables are **identical** between 43439 and 1YN.

### 4. Signing Is SDIO-Specific

Only the CYW43439 (SDIO) firmware is signed. The UART-based firmwares (4343A1 and 1YN) are **unsigned**. This suggests:
- The SDIO bootloader has signature verification; the UART bootloader does not
- Or: the Pico W specifically enables secure boot, while Murata modules don't
- Custom firmware over UART should work without signature issues

### 5. NVRAM Is Independent from BT Firmware

NVRAM controls WiFi radio configuration and is loaded separately from BT firmware. They don't interact directly (except `btc_mode` for coexistence). You can customize NVRAM without touching BT firmware and vice versa.

### 6. WiFi Firmware Is Much Larger and More Complex

WiFi FW (224-421 KB) dwarfs BT FW (5-35 KB). WiFi firmware is a complete ARM firmware image with vector table, real-time OS, full 802.11 MAC/PHY stack. It runs on the same ARM Cortex-M3 core. Reverse engineering WiFi firmware is a much larger undertaking.

### 7. CLM Controls Regulatory Compliance

The CLM (Country Locale Matrix) blob determines:
- Which channels are available per country
- Maximum TX power per channel per country
- DFS (Dynamic Frequency Selection) requirements
- Channel bandwidth restrictions

Custom CLM = custom regulatory behavior. The Pico W CLM (984 B) is minimal; the Murata 1YN CLM (4,752 B) covers more regions.

---

## Creating Custom Firmware

### Approach 1: Custom BT Firmware (Easiest)

**Goal**: Modify BT radio behavior, power levels, or patch ROM code.

**Base firmware**: Use `cyw43_btfw_1yn.h` as the starting point because:
- Same A2 silicon as Pico W
- Unsigned (no signature to bypass)
- Smallest/simplest variant
- Well-understood UART HCI command format

**Steps**:

1. **Extract the binary data** from the C header file
2. **Decode the HCI FC4C commands** to get the memory image  
3. **Modify the desired sections**:
   - **BRCMcfgS**: Change transport type (UART/SDIO), chip variant bytes
   - **TX Power Tables**: Adjust per-channel, per-rate power levels
   - **Register Writes**: Modify hardware register values for radio tuning
   - **Patchram Code**: Add/modify Thumb-2 code patches (requires disassembly knowledge)
4. **Re-encode** into the target format:
   - For UART: Wrap in HCI FC4C commands + FC4E launch
   - For SDIO: Direct format with version string prefix
5. **Convert back to C header** using `xxd -i` or equivalent

**Critical constraint**: If targeting the Pico W (CYW43439 over SDIO), you must either:
- Produce a validly signed firmware (requires the signing key — likely not available)
- Modify the host driver to skip signature verification (if the check is in the driver, not ROM)
- Use UART transport instead (requires hardware modification)

### Approach 2: Custom NVRAM (Simplest)

**Goal**: Tune WiFi radio for a custom board design.

**Steps**:

1. Copy the closest matching NVRAM file
2. Modify parameters:
   - `pa2ga0`: Run PA calibration on your board's antenna setup
   - `maxp2ga0`: Set to your regulatory limit
   - `boardflags`: Configure to match your board's hardware
   - `muxenab`: Set for your transport
   - Power backoff values: Tune for EVM compliance
3. Change array name if needed and recompile

**No firmware signing issues** — NVRAM is plain text processed by the driver.

### Approach 3: Custom WiFi+BT Composite (Advanced)

**Goal**: Full custom firmware for a new board design.

**Requirements**:
1. Custom NVRAM (Approach 2)
2. Custom BT firmware:
   - Start from 1YN base
   - Adjust BRCMcfgS for your transport and module
   - Keep A2 patchram code/table unchanged (ROM bug fixes are universal)
   - Calibrate TX power tables for your antenna
   - Re-wrap in appropriate format (HCI for UART, direct for SDIO)
3. WiFi firmware: Use one of the existing combined files as-is (modifying WiFi firmware requires Broadcom WICED SDK).
4. Custom CLM: Generate from Broadcom's CLM tools if you need specific regulatory support.

### Tool: Converting Between Formats

```python
# Convert HCI command stream (1YN format) to raw memory image
def hci_to_raw(data):
    """Extract memory writes from HCI FC4C command stream."""
    writes = {}
    i = 0
    while i + 2 < len(data):
        opcode = (data[i+1] << 8) | data[i]
        param_len = data[i+2]
        if opcode == 0xFC4C and i + 3 + param_len <= len(data):
            payload = data[i+3:i+3+param_len]
            addr = int.from_bytes(payload[:4], 'little')
            chunk = payload[4:]
            writes[addr] = chunk
        i += 3 + param_len
    return writes

# Convert raw memory image to HCI command stream
def raw_to_hci(writes, chunk_size=16):
    """Pack memory writes into HCI FC4C commands."""
    result = bytearray()
    for addr in sorted(writes):
        data = writes[addr]
        for offset in range(0, len(data), chunk_size):
            chunk = data[offset:offset+chunk_size]
            target_addr = addr + offset
            payload = target_addr.to_bytes(4, 'little') + chunk
            result.append(0x4C)  # opcode low
            result.append(0xFC)  # opcode high
            result.append(len(payload))  # param length
            result.extend(payload)
    # Add Launch RAM command
    result.extend(b'\x4E\xFC\x04\xFF\xFF\xFF\xFF')
    return bytes(result)

# Convert to signed direct format (for SDIO/Pico W - signature issue!)
def raw_to_signed(version_string, cfgs_data, cfgd_data):
    """Create signed direct format. WARNING: Signature not implemented."""
    result = bytearray()
    vs = version_string.encode('ascii') + b'\x00'
    result.append(len(vs))
    result.extend(vs)
    result.append(0x00)  # record count (0 = use section format)
    # ... pad to BRCMcfgS offset ...
    result.extend(b'BRCMcfgS')
    result.extend(b'\x00\x00\x00\x00')  # length
    result.extend(cfgs_data)
    result.extend(b'BRCMcfgD')
    result.extend(b'\x00\x00\x00\x00')  # length
    result.extend(cfgd_data)
    # NOTE: Would need to add signature here for CYW43439 SDIO
    return bytes(result)
```

### Signing Considerations

The CYW43439 SDIO firmware signature is the main barrier to custom firmware on the Pico W. Options:

1. **Find the signing key**: Unlikely — Broadcom/Infineon guards these closely
2. **Disable verification in host driver**: If the cyw43-driver code performs the check, it can be patched out. However, if the CYW43439 ROM itself verifies the signature, this won't work.
3. **Use UART transport**: The 1YN firmware proves CYW4343A2 works fine over UART without signing. If your board has UART access to the CYW43439, you can use the HCI command format.
4. **Use the unsigned combo firmware**: The `wb43439A0` WiFi+BT combined firmware may handle BT differently — worth investigating if it bypasses the BT firmware signature check by integrating BT patchram into the WiFi firmware.

---

## Related Analysis Documents

- [CYW43439 BT Firmware Analysis](CYW43439_BT_Firmware_Analysis.md) — Detailed Pico W BT firmware reverse engineering
- [CYW4343A1 BT Firmware Analysis](CYW4343A1_BT_Firmware_Analysis.md) — Detailed Murata 1DX BT firmware reverse engineering
- [CYW4343A2 1YN BT Firmware Analysis](CYW4343A2_1YN_BT_Firmware_Analysis.md) — Detailed Murata 1YN BT firmware reverse engineering
- [NVRAM Configuration Analysis](CYW43_NVRAM_Analysis.md) — Parameter-by-parameter NVRAM comparison
