# CYW4343A2 1YN (Murata 1YN) Bluetooth Firmware — Reverse Engineering Analysis

## Firmware Identity

| Property | Value |
|----------|-------|
| **File** | `cyw43_btfw_1yn.h` |
| **Source Comment** | `CYW4343A2_001.003.016.0031.0000.1YN.hcd` |
| **Array** | `cyw43_btfw_1yn[]` |
| **Size** | 4,857 bytes |
| **Chip** | CYW4343A2 (same silicon as CYW43439) |
| **Chip Revision** | A2 variant (0x2A0A in BRCMcfgS) |
| **Transport** | UART |
| **Version** | `UART 4343A2 wlbga_BU` |
| **Target Module** | Murata Type 1YN |
| **Package** | WLBGA |
| **Format** | HCI command stream (FC4C Write RAM + FC4E Launch RAM) |
| **BT Spec** | Bluetooth 5.0 (BLE + Classic) |

## Key Observation: Same Chip, Different Config

The 1YN firmware targets the **same CYW4343A2 silicon** as the CYW43439 (Pico W), but configured for:
- **UART transport** instead of SDIO
- **Murata 1YN module** antenna/PCB layout instead of Pico W
- **Earlier firmware version**: 001.003.016.**0031** vs 001.003.016.**0065** (Pico W)
- **Unsigned** format (no `_dl_signed` suffix)

This makes it the smallest firmware (4,857 bytes) — the A2 ROM needs few patches, and UART transport requires different but minimal radio tuning.

## Binary Layout

```
Offset    Size     Content
──────────────────────────────────────────────────────
0x0000    3+20     HCI cmd FC4C: Write BRCMcfgS start (addr 0x0021E000)
0x0017    3+20     HCI cmd FC4C: Continue BRCMcfgS
...       ...      (5 commands for BRCMcfgS region)
0x005C    3+6      HCI cmd FC4C: Final BRCMcfgS chunk
0x0065    3+20     HCI cmd FC4C: Write BRCMcfgD start (addr 0x0021F000)
...       ...      (remaining FC4C commands)
0x12F2    3+4      HCI cmd FC4E: Launch RAM (addr 0xFFFFFFFF)
0x12F9    —        End of firmware
```

**Total**: 212 Write RAM commands + 1 Launch RAM command = 213 HCI commands

## HCI Command Structure

The 1YN firmware uses a **fixed 20-byte payload** per FC4C command (unlike the 4343A1 which uses up to 255 bytes):

```
4C FC 14 [addr_lo] [addr_hi] [addr_mid] [addr_top] [16 bytes data]
```

This small chunk size (16 bytes of actual data) means **many more commands** (212 vs 138) despite carrying less total data. This may be a UART flow control consideration for lower-speed links.

## Reassembled Memory Image

The 212 FC4C commands write to **3 non-contiguous memory regions**:

| Start Address | End Address | Size | Content |
|---------------|-------------|------|---------|
| 0x000D0200 | 0x000D055C | 860 bytes | **Direct ROM patch area** |
| 0x0021E000 | 0x0021E042 | 66 bytes | **BRCMcfgS** |
| 0x0021F000 | 0x0021F988 | 2,440 bytes | **BRCMcfgD** |

### Notable: Direct ROM Patch Region

Unlike the other two firmwares, the 1YN writes **860 bytes directly to 0x000D0200** — this is in the ROM overlay/patch region. This suggests some patches are applied directly rather than through the standard patchram redirect mechanism.

The 0x000Dxxxx range is typically:
- **0x000D0000–0x000D0FFF**: Patchram configuration registers
- **0x000D0200+**: Patchram redirect table entries

So this 860-byte write likely configures the **patchram hardware redirect table** itself, programming which ROM addresses get redirected to RAM.

## BRCMcfgS — Static Configuration (54 bytes)

```
Hex:  32 00 00 00 01 01 04 18 92 00 00 00 03 06 AC 1F
      2A 0A 43 43 00 01 1C 00 F0 21 00 00 00 00 00 00
      00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
      00 00 00 FE 00 00
```

### Decoded Fields

| Offset | Bytes | Value | Interpretation |
|--------|-------|-------|----------------|
| 0 | `32 00 00 00` | 50 | Inner section length |
| 4 | `01 01` | 1.1 | Config format version |
| 6 | `04` | 4 | Transport type |
| 8-11 | `92 00 00 00` | 0x92 | Crystal oscillator config |
| 14-15 | `AC 1F` | 0x1FAC | LPO frequency config |
| 16-17 | `2A 0A` | — | **Chip/module variant ID** (different from both 43439 and 4343A1!) |
| 18-19 | `43 43` | "CC" | **Chip family: CYW4343** |
| 23-26 | `00 F0 21 00` | 0x0021F000 | **BRCMcfgD load address** (same as 43439) |
| 50 | `FE` | — | End marker |

### Variant ID: 0x2A0A

The chip variant bytes `2A 0A` are unique to the 1YN module, differing from:
- 43439 (Pico W): `12 A2`
- 4343A1 (1DX): `12 A1`

This may encode module-specific configuration (e.g., Murata 1YN-specific antenna diversity settings, package pin mappings).

## BRCMcfgD — Dynamic Configuration / Patchram (2,428 bytes)

### 1. Version String

At offset 7:
```
"UART 4343A2 wlbga_BU"
```

Shorter than the 43439's version string — no "Generic", "RPI", or "dl_signed" qualifiers.

### 2. Register Write Table

**~19 register writes** to `0x006xxxxx` — between the 43439's 22 and fewer than the 4343A1's 33. This makes sense: same A2 silicon as Pico W (fewer errata fixes needed) but different module (different radio tuning).

### 3. TX Power Calibration Tables

Three TX power table regions — the **same structure** as the CYW43439:

```
Region 1 (offset ~1410):
  E=4/.83-*'     (identical to CYW43439!)
  NJ=97@:3.-     (identical)
  _XMGCKE=75     (identical)
  mjYSRWQG@>     (identical)
  yh`]aZPHF      (identical)

Region 2 (offset ~1540):
  E=4/.83-*'     (duplicate, also identical to CYW43439)
  ...
```

**Critical finding**: The TX power tables are **identical** between the 1YN and CYW43439 firmwares! This confirms they use the same CYW4343A2 silicon with the same RF characteristics. The differences are only in transport and board-level configuration.

### 4. Patchram Code

**~5 patchram address references** — very few, consistent with the A2 silicon having a much more mature ROM than the A1.

### 5. Patchram vs Direct Writes

The 1YN uses a hybrid approach:
- **860 bytes at 0x000D0200**: Direct patchram table configuration
- **~2,440 bytes at 0x0021F000**: Standard patchram code + calibration

## Firmware Download Protocol (UART)

Same as 4343A1 but with smaller chunks:

1. **Host sends HCI_Reset** (0x0C03)
2. **Host sends Download Minidriver** (FC2E) — handled by driver
3. **Host sends 212 Write RAM commands** (FC4C) at 20-byte payloads each
4. **Host sends Launch RAM** (FC4E) @ 0xFFFFFFFF
5. **Chip executes** from default entry point

The smaller 20-byte payload size means:
- **More reliable over UART** (less risk of buffer overflow)
- **Slower download** (more protocol overhead per byte)
- **Better for flow control** (each command gets an ACK)

## Comparison with CYW43439 (Same Chip)

| Aspect | CYW4343A2 1YN | CYW43439 (Pico W) |
|--------|---------------|--------------------|
| Size | 4,857 B | 6,970 B |
| Chip Rev | A2 | A2 |
| FW Version | 001.003.016.0031 | 001.003.016.0065 |
| Transport | UART | SDIO |
| Format | HCI cmd stream (20B chunks) | Signed direct |
| Patchram refs | ~5 | ~8 |
| Register writes | ~19 | ~22 |
| TX Power tables | **Identical** to 43439 | Same |
| Signing | Unsigned | Signed |
| BRCMcfgD address | 0x0021F000 | 0x0021F000 (same!) |
| Memory regions | 3 (including 0x000D0xxx) | 1 (contiguous) |
| Extra | 860B patchram HW table | None visible |

## Implications for Custom Firmware

The 1YN firmware is the best candidate for understanding custom firmware creation because:

1. **Unsigned** — no signature verification to bypass
2. **Same silicon** as Pico W — patches and power tables are transferable
3. **UART transport** — easier to debug/intercept than SDIO
4. **Smallest size** — proves only minimal patches are truly needed for A2 silicon
5. **Older version** — shows what's essential vs what 43439 added in later versions

To create a custom firmware from the 1YN base, you would need:
- Keep the BRCMcfgS with your target configuration
- Keep the patchram table at 0x000D0200 (essential ROM fixes)
- Modify TX power tables for your antenna/board
- Modify register writes for your radio tuning
- Update the version string
- Wrap in HCI FC4C commands for UART, or use direct format for SDIO
