# CYW43 NVRAM Configuration — Analysis & Comparison

## Overview

NVRAM (Non-Volatile RAM) configuration files contain **board-level WiFi/BT radio parameters** in human-readable key=value format. These are loaded by the WiFi driver at initialization to configure the radio for a specific board/module design.

Unlike BT firmware (binary patchram code), NVRAM files are **plain text** describing radio calibration, antenna settings, power limits, and coexistence parameters. They are **critical for** regulatory compliance, radio performance, and board-specific tuning.

## Files Analyzed

| File | Module | Source |
|------|--------|--------|
| `wifi_nvram_43439.h` | Raspberry Pi Pico W (CYW43439) | Generated from `cyw943439wlpth_rev1_0.txt` |
| `wifi_nvram_1dx.h` | Murata Type 1DX (CYW4343A1) | From `Type1DX_Final_nvram2.txt` |
| `wifi_nvram_1yn.h` | Murata Type 1YN (CYW43439/A2) | From `cyfmac43439-sdio.1YN.txt` (Murata FMAC repo) |

All three use the same C array name: `wifi_nvram_4343[]` with `CYW43_RESOURCE_ATTRIBUTE`.

## Parameter Comparison

### Chip & Board Identity

| Parameter | Pico W (43439) | Murata 1DX | Murata 1YN |
|-----------|---------------|------------|------------|
| `manfid` | 0x2d0 | 0x2d0 | 0x2d0 |
| `prodid` | 0x0727 | **0x0726** | 0x0727 |
| `vendid` | 0x14e4 | 0x14e4 | 0x14e4 |
| `devid` | 0x43e2 | 0x43e2 | 0x43e2 |
| `boardtype` | 0x0887 | **0x0726** | 0x0887 |
| `boardrev` | 0x1100 | **0x1202** | **0x1101** |
| `boardnum` | 22 | 22 | 22 |
| `sromrev` | 11 | 11 | 11 |

**Key findings**:
- `vendid=0x14e4` is Broadcom's PCI vendor ID across all three
- `devid=0x43e2` identifies all as CYW4343x family
- `prodid` differs for 1DX (0x0726 vs 0x0727) — different product variant
- `boardtype` differs for 1DX (0x0726 vs 0x0887) — different board design
- `boardrev`: 1DX is rev 0x1202, 1YN is 0x1101, Pico W is 0x1100

### Board Flags

| Parameter | Pico W (43439) | Murata 1DX | Murata 1YN |
|-----------|---------------|------------|------------|
| `boardflags` | 0x00404001 | **0x00404201** | **0x00404001** (configurable) |
| `boardflags3` | **0x04000000** | **0x08000000** | **0x08000000** |

Board flags are bitmasks controlling hardware features:
- Bit 0 (0x01): BFL_BTC2WIRE — 2-wire BT coexistence
- Bit 9 (0x200): BFL_NOPLLDOWN — keep PLL running
- Bit 14 (0x4000): Related to PA configuration
- Bit 22 (0x400000): WiFi/BT combo enable

**Pico W vs 1DX**: The 1DX has bit 9 set (0x200 → `0x00404201`), keeping the PLL running — important for UART-based BT coexistence.

**boardflags3**: Pico W uses 0x04000000, while 1DX and 1YN use 0x08000000 — different bits controlling advanced features. The 1YN header notes this is configurable with `0x00404201` as an alternative.

### Crystal & Clock

| Parameter | Pico W (43439) | Murata 1DX | Murata 1YN |
|-----------|---------------|------------|------------|
| `xtalfreq` | 37400 | 37400 | 37400 |
| `nocrc` | 1 | 1 | 1 |

All three use a **37.4 MHz crystal** — standard for CYW4343x.

### Antenna Configuration

| Parameter | Pico W (43439) | Murata 1DX | Murata 1YN |
|-----------|---------------|------------|------------|
| `ag0` | 255 | **0** | 255 |
| `aa2g` | 1 | 1 | 1 |
| `ccode` | ALL | ALL | ALL |
| `extpagain2g` | 0 | 0 | 0 |
| `swdiv_en` | *(absent)* | **1** | **1** |
| `swdiv_gpio` | *(absent)* | **1** | **1** |

**Critical difference**: The 1DX and 1YN enable **software antenna diversity** (`swdiv_en=1`) and use GPIO 1 for antenna switching. The Pico W does not — it has a single PCB trace antenna.

- `ag0=255`: No specific antenna gain value (auto)
- `ag0=0`: Default antenna selection (1DX)
- `aa2g=1`: Single antenna for 2.4 GHz

### PA (Power Amplifier) Calibration

| Parameter | Pico W (43439) | Murata 1DX | Murata 1YN |
|-----------|---------------|------------|------------|
| `pa2ga0` | **-168,7161,-820** | **-145,6667,-751** | **-168,6777,-789** |
| `AvVmid_c0` | 0x0,0xc8 | 0x0,0xc8 | 0x0,0xc8 |
| `cckpwroffset0` | **5** | **2** | **5** |
| `pa0itssit` | 0x20 | *(commented out)* | 0x20 |

The `pa2ga0` parameter contains three polynomial coefficients for the PA linearization model:
- **Pico W**: (-168, 7161, -820) — highest gain coefficient (7161), most loss (-820)
- **Murata 1DX**: (-145, 6667, -751) — lower gain, less aggressive compensation
- **Murata 1YN**: (-168, 6777, -789) — similar to Pico W but slightly different gain

These differences reflect the **antenna/PCB matching network** of each board:
- Pico W: Onboard PCB antenna with specific impedance characteristics
- 1DX: Murata module with integrated antenna, different matching
- 1YN: Murata module, values closer to Pico W (same chip revision)

### TX Power Limits

| Parameter | Pico W (43439) | Murata 1DX | Murata 1YN |
|-----------|---------------|------------|------------|
| `maxp2ga0` | **84** | **74** | **74** |
| `txpwrbckof` | 6 | *(commented out)* | 6 |
| `cckbw202gpo` | 0 | 0 | 0 |
| `legofdmbw202gpo` | **0x66111111** | **0x88888888** | **0x88888888** |
| `mcsbw202gpo` | **0x77711111** | **0xaaaaaaaa** | **0xaaaaaaaa** |
| `propbw202gpo` | 0xdd | 0xdd | 0xdd |

**maxp2ga0**: Maximum TX power — Pico W allows **84** (21 dBm) while both Murata modules are capped at **74** (18.5 dBm). The Pico W runs at higher power, possibly due to its less efficient PCB antenna needing more TX power to achieve equivalent EIRP.

**Power offsets** (`gpo` parameters): These are per-rate power backoff values:
- Pico W: `0x66111111` / `0x77711111` — less backoff for lower rates
- 1DX/1YN: `0x88888888` / `0xaaaaaaaa` — more aggressive backoff across all rates

This means the **Murata modules reduce power more aggressively at higher data rates** — likely for better EVM (Error Vector Magnitude) compliance with their antenna designs.

### OFDM & PAPD Configuration

| Parameter | Pico W (43439) | Murata 1DX | Murata 1YN |
|-----------|---------------|------------|------------|
| `ofdmdigfilttype` | 18 | 18 | 18 |
| `ofdmdigfilttypebe` | 18 | 18 | 18 |
| `papdmode` | 1 | 1 | 1 |
| `papdvalidtest` | 1 | 1 | 1 |
| `pacalidx2g` | **45** | **48** | **45** |
| `papdepsoffset` | -30 | **-22** | -30 |
| `papdendidx` | 58 | 58 | 58 |

All use PAPD (Pre-distortion for PA Distortion) mode 1 with the same digital filter type 18. The `pacalidx2g` (PA calibration index) differs for 1DX (48 vs 45) — different operating point for its antenna.

### BT Coexistence

| Parameter | Pico W (43439) | Murata 1DX | Murata 1YN |
|-----------|---------------|------------|------------|
| `btc_mode` | **1** | *(absent/commented)* | **0** |
| `bt_default_ant` | *(absent)* | *(absent)* | **0** |

**BT coexistence mode**:
- Pico W: `btc_mode=1` — active BT coexistence enabled (needed for SDIO combo operation)
- 1YN: `btc_mode=0` — passive coexistence (BT on separate UART)
- 1DX: Not configured — handled differently

### LTE Coexistence (LTECX)

All three configure LTE coexistence pins:

| Parameter | Pico W (43439) | Murata 1DX | Murata 1YN |
|-----------|---------------|------------|------------|
| `ltecxmux` | 0 | *(absent)* | 0 |
| `ltecxpadnum` | 0x0102 | *(absent)* | 0x0102 |
| `ltecxfnsel` | 0x44 | *(absent)* | 0x44 |
| `ltecxgcigpio` | 0x01 | *(absent)* | 0x01 |

The 1DX omits LTE coexistence — it's an older module that didn't need this feature.

### Host Interface

| Parameter | Pico W (43439) | Murata 1DX | Murata 1YN |
|-----------|---------------|------------|------------|
| `muxenab` | **0x100** | **0x10** | **0x11** |

The `muxenab` parameter controls pin multiplexing:
- **0x01**: UART enable
- **0x10**: HOST WAKE interrupt enable
- **0x100**: SDIO-specific mux configuration
- **0x11**: UART + HOST WAKE (1YN uses both)

This directly reflects the transport:
- Pico W uses SDIO → `0x100`
- 1DX uses basic GPIO → `0x10`
- 1YN uses UART + wake → `0x11`

### Miscellaneous

| Parameter | Pico W (43439) | Murata 1DX | Murata 1YN |
|-----------|---------------|------------|------------|
| `deadman_to` | 0xffffffff | *(absent)* | 0xffffffff |
| `spurconfig` | 0x3 | *(commented)* | 0x3 |
| `glitch_based_crsmin` | 1 | *(absent)* | 1 |
| `tempbased_duty_cycle_en` | *(absent)* | *(absent)* | **0** |

The 1YN explicitly disables temperature-based duty cycling — may be unnecessary for its thermal design.

## Parameters Unique to Each Board

### Pico W Only
- Higher max power (84 vs 74)
- BTC mode 1 (active coexistence)
- SDIO mux configuration

### 1DX Only
- Software antenna diversity (swdiv_en/swdiv_gpio)
- Different board type (0x0726)
- Commented-out BTC and spur parameters
- Higher PA calibration index (48)

### 1YN Only
- Explicit BTC mode 0
- Default antenna selection
- Temperature duty cycle disabled
- Combined UART + HOST WAKE mux
- [Configurable boardflags noted in source]

## Implications for Custom Firmware

### What NVRAM Controls
- **Radio TX power and linearity** — PA polynomial, max power, per-rate backoff
- **Antenna diversity** — whether to use and how to switch antennas
- **Coexistence** — WiFi/BT and WiFi/LTE interference management
- **Board identity** — product/board IDs for driver matching
- **Pin muxing** — which physical pins are used for what function

### Creating Custom NVRAM
1. **Start from the closest matching board** (Pico W for SDIO, 1YN for UART+A2, 1DX for UART+A1)
2. **Calibrate PA parameters** (`pa2ga0`) for your actual antenna — requires RF test equipment
3. **Set power limits** (`maxp2ga0`, `*gpo`) for your regulatory requirements
4. **Configure antenna diversity** if your board has antenna switching
5. **Set muxenab** for your transport (SDIO vs UART) and wake configuration
6. **Set btc_mode** based on how WiFi/BT coexistence works on your board
7. **Update board identity** (boardtype, boardrev) for your design revision

### What NVRAM Does NOT Control
- ROM patches (that's in the BT firmware blob)
- BT-specific radio configuration (in BRCMcfgD TX power tables)
- Firmware signing/authentication
- HCI transport layer behavior
