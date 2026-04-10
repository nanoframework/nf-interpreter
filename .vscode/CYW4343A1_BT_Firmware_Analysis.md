# CYW4343A1 (Murata 1DX) Bluetooth Firmware — Reverse Engineering Analysis

## Firmware Identity

| Property | Value |
|----------|-------|
| **File** | `cyw43_btfw_4343A1.h` |
| **Array** | `cyw43_btfw_4343A1[]` |
| **Size** | 35,182 bytes |
| **Chip** | CYW4343A1 |
| **Chip Revision** | A1 (0x12A1) |
| **Transport** | UART at 37.4 MHz |
| **Version** | `BCM4343A1 UART 37.4 MHz wlbga_ref wiced` |
| **Target Module** | Murata Type 1DX |
| **Package** | WLBGA (Wafer-Level Ball Grid Array) |
| **Format** | HCI command stream (FC4C Write RAM + FC4E Launch RAM) |
| **BT Spec** | Bluetooth 4.1 |

## Binary Layout

```
Offset    Size     Content
──────────────────────────────────────────────────────
0x0000    3+70     HCI cmd FC4C: Write BRCMcfgS (addr 0x00211810)
0x0049    3+255    HCI cmd FC4C: Write BRCMcfgD start (addr 0x00211852)
0x014B    3+255    HCI cmd FC4C: Continue BRCMcfgD
...       ...      (138 total FC4C commands)
0x8967    3+4      HCI cmd FC4E: Launch RAM (addr 0xFFFFFFFF)
0x896E    —        End of firmware
```

**Total**: 138 Write RAM commands + 1 Launch RAM command = 139 HCI commands

## Format Details — HCI Command Stream

The 4343A1 firmware is wrapped in **HCI Vendor-Specific commands** for UART-based download:

### HCI Command FC4C — Write RAM
```
Byte 0:    0x4C        Opcode low byte
Byte 1:    0xFC        Opcode high byte (OGF=0x3F, OCF=0x4C)
Byte 2:    length      Parameter length (20-255 bytes)
Bytes 3-6: address     32-bit LE target RAM address
Bytes 7+:  data        Payload to write at that address
```

Each FC4C command writes a chunk of data (up to 251 bytes of payload) to a specific RAM address. The 138 commands form one contiguous write starting at address **0x00211810**.

### HCI Command FC4E — Launch RAM
```
Bytes 0-2: 4E FC 04    Opcode + length
Bytes 3-6: FF FF FF FF Address (0xFFFFFFFF = default entry point)
```

The launch address 0xFFFFFFFF tells the chip to use its default firmware entry point rather than a specific address.

## Reassembled Memory Image

All 138 FC4C commands reassemble into a single contiguous block:

| Start Address | End Address | Size | Content |
|---------------|-------------|------|---------|
| 0x00211810 | 0x00219DB1 | 34,209 bytes | Complete firmware (BRCMcfgS + BRCMcfgD) |

This is **5× larger** than the CYW43439 firmware, reflecting the CYW4343A1's older chip revision that requires significantly more ROM patches.

## BRCMcfgS — Static Configuration (54 bytes)

```
Hex:  32 00 00 00 01 01 04 18 92 00 00 00 03 06 AC 1F
      12 A1 43 43 00 01 1C 52 18 21 00 00 00 00 00 00
      00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
      00 00 00 FE 00 00
```

### Decoded Fields

| Offset | Bytes | Value | Interpretation |
|--------|-------|-------|----------------|
| 0 | `32 00 00 00` | 50 | Inner section length |
| 4 | `01 01` | 1.1 | Config format version |
| 6 | `04` | 4 | Transport type |
| 7 | `18` | — | Config flags |
| 8-11 | `92 00 00 00` | 0x92 | Crystal oscillator config |
| 14-15 | `AC 1F` | 0x1FAC | LPO frequency config |
| 16-17 | `12 A1` | — | **Chip revision: A1** (differs from 43439's A2) |
| 18-19 | `43 43` | "CC" | **Chip family: CYW4343** |
| 23-26 | `52 18 21 00` | 0x00211852 | **BRCMcfgD load address** |
| 50 | `FE` | — | End marker |

### Key Differences from CYW43439

1. **Chip revision**: A1 (0x12A1) vs A2 (0x12A2)
2. **BRCMcfgD address**: 0x00211852 vs 0x0021F000 — loaded lower in RAM
3. **No signature bytes**: No trailing `FB F0` — firmware is unsigned

## BRCMcfgD — Dynamic Configuration / Patchram (34,131 bytes)

### 1. Version String

At offset 6 within the section:
```
"(BCM4343A1 UART 37.4 MHz wlbga_ref wiced"
```

Note the leading `(` character and the `BCM` prefix (Broadcom's original naming before Cypress/Infineon rebranding). The `wiced` suffix indicates this was built with Cypress WICED SDK.

### 2. Register Write Table

**~33 register writes** to the `0x006xxxxx` peripheral register space — significantly more than the CYW43439's 22 writes. The A1 revision requires more hardware configuration due to different silicon errata.

### 3. TX Power Calibration Tables

Two TX power table regions identified:

**Table format** — Groups of power values indexed by channel:
```
Region 1 (offset ~1977):
  C977885565   (power entries for early channels)
  LEABC@<;;<   (mid channels)
  \SRRRK GGGG  (higher channels)
  jc__dWSRRS   (high-rate entries)
  |qnnqa]\\]   (highest rates)

Region 2 (duplicate/BLE variant):
  LLLLL_____lllll  (primary rates)
  C9778LEABC\SRRR  (mixed channels)
```

The power values are generally **lower** than the CYW43439, reflecting:
- Different antenna matching for the Murata 1DX module
- Potentially different regulatory targets
- Older silicon characteristics

### 4. Patchram Code — Extensive

This firmware has **~211 patchram address references** and **16 identified patchram table entries**, making it the most heavily patched of the three variants.

### Patchram Table (ROM → RAM redirects)

| ROM Address | RAM Address | Purpose (estimated) |
|-------------|-------------|---------------------|
| 0x0000BED7 | 0x00210A34 | Early boot / init fix |
| 0x000D6708 | 0x0020294B | HCI command handler patch |
| 0x0000BCBD | 0x002031B4 | Link manager fix |
| 0x0000BD70 | 0x002027F0 | Connection setup patch |
| 0x0000BD70 | 0x00204118 | Connection setup (alternate) |
| 0x000D5E50 | 0x00202704 | Baseband controller fix |
| 0x0000BD10 | 0x0020080C | Scheduler/timing fix |
| 0x0000E7FB | 0x00201EA0 | Power management patch |

**Note**: Address 0x0000BD70 appears twice with different RAM targets — this suggests a conditional or versioned patch where different code paths get different fixes.

### Patchram Address Ranges

```
ROM patches:   0x0000BCBD – 0x000D6708  (ROM code being fixed)
RAM code:      0x00200804 – 0x00210A34  (replacement code in RAM)
RAM total:     ~34KB spanning 0x00200000–0x00211000
```

The ROM address range `0x000Dxxxx` indicates patches into the **HCI/Baseband controller** area of the ROM, while `0x0000Bxxx–0x0000Exxx` patches are in the **Link Manager / Link Controller** area.

### 5. Code Analysis

Due to the large size, this firmware contains substantial amounts of Thumb-2 code. Key patterns identified:

**Function prologue/epilogue patterns**: Standard ARM Cortex-M3 conventions:
```arm
push {r4, r5, r6, lr}    ; Save callee-saved regs + return address
... function body ...
pop  {r4, r5, r6, pc}    ; Restore and return
```

**Register access patterns**: Memory-mapped I/O reads/writes:
```arm
ldr  r0, [pc, #offset]   ; Load register base address from literal pool
ldr  r1, [r0, #offset]   ; Read register
orr  r1, r1, #mask       ; Modify bits
str  r1, [r0, #offset]   ; Write back
```

**HCI event construction**: Building response packets:
```arm
movs r0, #0x0E            ; HCI Command Complete event code
strb r0, [r1, #0]         ; Store event code
... parameter assembly ...
```

## Firmware Download Protocol (UART)

The UART download sequence for the CYW4343A1:

1. **Host sends HCI_Reset** (opcode 0x0C03) to the chip
2. **Host sends Download Minidriver** (FC2E) — not present in this file (handled by driver)
3. **Host sends 138 Write RAM commands** (FC4C) — each writes up to 251 bytes, chip ACKs each
4. **Host sends Launch RAM** (FC4E) with address 0xFFFFFFFF
5. **Chip executes firmware** from the patchram entry point
6. **Host waits** for HCI event indicating successful boot

Each FC4C command follows the HCI command-response pattern:
```
Host → Chip:  01 4C FC [len] [addr32] [data...]
Chip → Host:  04 0E 04 01 4C FC 00      (Command Complete, status=0)
```

## Why So Much Larger?

At 35,182 bytes vs 6,970 bytes for the CYW43439, the 4343A1 firmware is **5× larger** because:

1. **Older ROM revision**: The A1 silicon has more bugs requiring patchram fixes (~211 vs ~8 patch references)
2. **More extensive radio calibration**: The Murata 1DX module needs different antenna tuning
3. **UART transport overhead**: Each data chunk wrapped in 3-byte HCI header = ~414 bytes of framing overhead
4. **No firmware signing**: No signature verification = entire firmware must be loaded (no partial/delta updates)
5. **WICED SDK origin**: Built with Cypress WICED SDK which may include additional features/fixes

## Comparison with CYW43439

| Aspect | CYW4343A1 (1DX) | CYW43439 (Pico W) |
|--------|------------------|--------------------|
| Size | 35,182 B | 6,970 B |
| Chip Rev | A1 | A2 |
| Transport | UART | SDIO |
| Format | HCI command stream | Signed direct |
| Patchram entries | ~16+ pairs | ~8 references |
| Register writes | ~33 | ~22 |
| Signing | Unsigned | Signed |
| BRCMcfgD address | 0x00211852 | 0x0021F000 |
| Launch address | 0xFFFFFFFF (default) | N/A (direct) |
