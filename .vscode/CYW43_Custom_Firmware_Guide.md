# How to Create Custom CYW43 Firmware

## Introduction

This document is a practical engineering guide for creating custom Bluetooth firmware for the Cypress/Infineon CYW4343x family of WiFi+Bluetooth combo chips. It is based on reverse engineering of the three official firmware variants in the [georgerobotics/cyw43-driver](https://github.com/georgerobotics/cyw43-driver) repository, combined with analysis of the host driver source code (`cyw43_ll.c`).

**Target audience**: Embedded engineers who need to customize BT radio behavior, power levels, or ROM patches for a custom board design based on the CYW4343x.

**Prerequisite reading**:
- [CYW43439 BT Firmware Analysis](CYW43439_BT_Firmware_Analysis.md)
- [CYW4343A2 1YN BT Firmware Analysis](CYW4343A2_1YN_BT_Firmware_Analysis.md)
- [CYW4343A1 BT Firmware Analysis](CYW4343A1_BT_Firmware_Analysis.md)
- [NVRAM Configuration Analysis](CYW43_NVRAM_Analysis.md)
- [Firmware Comparison](CYW43_Firmware_Comparison.md)

---

## 1. What Is "Custom Firmware" in This Context

The CYW4343x runs its Bluetooth stack primarily from **mask ROM** — code baked into the silicon at manufacturing time. The downloadable firmware is **not** a full BT stack; it is a **patch blob** consisting of:

| Component | Purpose | Customizable? |
|-----------|---------|---------------|
| BRCMcfgS | Static chip/transport config | Yes |
| Version string | Identity / logging | Yes |
| Register write table | Radio hardware configuration | **Yes — primary customization target** |
| TX power tables | Per-channel, per-rate power calibration | **Yes — primary customization target** |
| Patchram redirect table | ROM address → RAM address mappings | No (keep original) |
| Patchram code | Thumb-2 bug fix code | No (keep original) |

**You do NOT need to write a Bluetooth stack.** You only need to change the configuration data while preserving the essential ROM patches.

---

## 2. Critical Discovery: No Host-Side Signature Verification

Analysis of `cyw43_ll.c` in the cyw43-driver reveals:

### WiFi firmware validation
```c
static int cyw43_check_valid_chipset_firmware(cyw43_int_t *self, size_t len, uintptr_t source) {
    // get the last bit of the firmware, the last 800 bytes
    uint32_t fw_end = 800;
    const uint8_t *b = (const uint8_t *)source + len - fw_end;
    // get length of trailer
    fw_end -= 16; // skip DVID trailer
    uint32_t trail_len = b[fw_end - 2] | b[fw_end - 1] << 8;
    if (trail_len < 500 && b[fw_end - 3] == '\0') {
        for (int i = 80; i < (int)trail_len; ++i) {
            if (strncmp((const char *)&b[fw_end - 3 - i], "Version: ", 9) == 0) {
                // valid chipset firmware found
                return 0;
            }
        }
    }
    CYW43_WARN("could not find valid firmware\n");
    return CYW43_FAIL_FAST_CHECK(-CYW43_EIO);
}
```

The WiFi validation only looks for a `"Version: "` string in the last 800 bytes. **There is no cryptographic signature check in the host driver.**

### BT firmware loading

BT firmware is loaded via `cyw43_ll_write_backplane_mem()` — raw memory writes through the SDIO/SPI backplane. The host driver does not perform any validation or signature checking on the BT firmware blob at all.

### What about on-chip verification?

The CYW43439 BT firmware has `_dl_signed` in its version string and extra bytes (`FB F0`) at the end of BRCMcfgS. This hints at on-chip signature verification in the BT ROM bootloader. However:

1. The 1YN firmware (same CYW4343A2 silicon) is **unsigned** and works fine
2. The `_dl_signed` flag may be related to the **firmware download protocol** (SDIO vs UART), not the silicon
3. The extra bytes may be an authentication tag that's only checked when loaded via the SDIO BT download path

**Conclusion**: Custom firmware is viable, especially when using the UART/HCI download path or the 1YN-compatible loading method.

---

## 3. Choosing Your Base Firmware

| If your target is... | Use this base | Reason |
|---------------------|---------------|--------|
| CYW4343A2 over UART | `cyw43_btfw_1yn.h` | Unsigned, smallest, same silicon |
| CYW4343A2 over SDIO (Pico W) | `cyw43_btfw_1yn.h` + format conversion | Need to test if SDIO path accepts unsigned |
| CYW4343A1 (1DX module) | `cyw43_btfw_4343A1.h` | Only option for A1 silicon |
| CYW4343A2 over SDIO (verified signed) | `cyw43_btfw_43439.h` | Keep signed, modify only safe fields |

**Recommended starting point**: The **1YN firmware** (`cyw43_btfw_1yn.h`):
- 4,857 bytes — small enough to fully understand
- Unsigned — no signature obstacles
- Same CYW4343A2 chip as Pico W
- Identical TX power tables to the 43439 firmware
- HCI command format is well-documented

---

## 4. Firmware Binary Anatomy — Complete Map

### 4.1 HCI Command Stream Format (1YN / 4343A1)

The firmware is a series of HCI Vendor-Specific commands:

```
┌──────────────────────────────────────────────────┐
│  HCI Command: Write RAM (FC4C)                    │
│  ┌──────┬──────┬────────┬───────────────────────┐ │
│  │ 0x4C │ 0xFC │ length │ [4B addr] [data...]   │ │
│  └──────┴──────┴────────┴───────────────────────┘ │
│  ... repeat for all data chunks ...                │
│                                                    │
│  HCI Command: Launch RAM (FC4E)                    │
│  ┌──────┬──────┬────────┬───────────────────────┐ │
│  │ 0x4E │ 0xFC │ 0x04   │ 0xFF 0xFF 0xFF 0xFF   │ │
│  └──────┴──────┴────────┴───────────────────────┘ │
└──────────────────────────────────────────────────┘
```

### 4.2 Memory Regions Written

For the 1YN firmware, three memory regions are written:

```
Region 1: 0x000D0200 — 0x000D055C  (860 bytes)
  └── Patchram hardware redirect table
  └── DO NOT MODIFY — essential ROM bug fixes

Region 2: 0x0021E000 — 0x0021E042  (66 bytes)
  └── BRCMcfgS (static configuration)
  └── CAN MODIFY: transport type, variant ID

Region 3: 0x0021F000 — 0x0021F988  (2,440 bytes)
  └── BRCMcfgD (dynamic configuration + patchram code)
  └── CAN MODIFY: version string, register writes, TX power tables
  └── DO NOT MODIFY: patchram code patches
```

### 4.3 BRCMcfgD Internal Layout

```
Offset   Content                          Modifiable?
──────────────────────────────────────────────────────
0x0000   "BRCMcfgD" magic + length        No
0x000C   TLV header (tag 0x03, len)       No
0x000E   Version string offset            Update string
0x00xx   Version string bytes             Yes — change to your ID
0x00xx   Register write table             Yes — tune radio
0x00xx   TX power calibration table 1     Yes — adjust per board
0x00xx   TX power calibration table 2     Yes — adjust per board
0x00xx   Patchram code (Thumb-2)          No — ROM bug fixes
0x00xx   Additional config data           Depends on field
0x09xx   End                              —
```

---

## 5. Step-by-Step: Building Custom Firmware

### Step 1: Extract and Parse

```python
#!/usr/bin/env python3
"""Step 1: Extract the raw binary from the C header file."""
import re, struct

def extract_firmware(header_path):
    """Parse C hex array into raw bytes."""
    with open(header_path) as f:
        text = f.read()
    m = re.search(r'\{([^}]+)\}', text, re.DOTALL)
    hex_vals = re.findall(r'0x([0-9a-fA-F]{2})', m.group(1))
    return bytes(int(h, 16) for h in hex_vals)

def decode_hci_commands(data):
    """Decode HCI FC4C Write RAM commands into memory writes."""
    writes = {}  # addr -> bytearray
    i = 0
    while i + 2 < len(data):
        opcode = (data[i+1] << 8) | data[i]
        param_len = data[i+2]
        if i + 3 + param_len > len(data):
            break
        if opcode == 0xFC4C:
            payload = data[i+3 : i+3+param_len]
            addr = struct.unpack_from('<I', payload, 0)[0]
            chunk = payload[4:]
            # Accumulate into contiguous regions
            for existing_addr in list(writes.keys()):
                existing_end = existing_addr + len(writes[existing_addr])
                if addr == existing_end:
                    writes[existing_addr].extend(chunk)
                    addr = None
                    break
            if addr is not None:
                writes[addr] = bytearray(chunk)
        i += 3 + param_len
    return writes

# Usage:
raw = extract_firmware("cyw43_btfw_1yn.h")
regions = decode_hci_commands(raw)
for addr, data in sorted(regions.items()):
    print(f"  0x{addr:08X}: {len(data)} bytes")
    with open(f"region_{addr:08X}.bin", "wb") as f:
        f.write(data)
```

### Step 2: Locate Modifiable Sections

```python
def find_sections(data):
    """Find BRCMcfgS and BRCMcfgD sections within a memory region."""
    sections = {}
    for magic in [b'BRCMcfgS', b'BRCMcfgD']:
        idx = data.find(magic)
        if idx >= 0:
            sec_len = struct.unpack_from('<I', data, idx + 8)[0]
            sections[magic.decode()] = {
                'offset': idx,
                'header_len': 12,  # 8 magic + 4 length
                'declared_len': sec_len,
            }
    return sections

def find_version_string(data):
    """Find the firmware version string in BRCMcfgD."""
    # Version is typically after a TLV tag 0x03 with a sub-tag 0x15 or 0x03
    # Simpler: look for known prefixes
    for prefix in [b'UART ', b'SDIO ', b'BCM43']:
        idx = data.find(prefix)
        if idx >= 0:
            # Read until null terminator
            end = data.index(0, idx)
            return idx, data[idx:end].decode('ascii')
    return -1, None

def find_register_writes(data):
    """Find register write entries (addr in 0x006xxxxx range)."""
    writes = []
    for i in range(0, len(data) - 7, 4):
        addr = struct.unpack_from('<I', data, i)[0]
        if 0x00600000 <= addr <= 0x006FFFFF:
            val = struct.unpack_from('<I', data, i + 4)[0]
            writes.append({'offset': i, 'addr': addr, 'value': val})
    return writes

def find_power_tables(data):
    """Find TX power calibration tables.
    
    Power tables contain sequences of values 30-127 (positive dBm * 4 range)
    typically organized in groups of 10-14 values (one per channel).
    """
    tables = []
    # Look for the known byte pattern from our analysis
    # The tables have a distinctive pattern of gradually decreasing values
    for i in range(len(data) - 28):
        # Check for a sequence that looks like a power table row
        # (14 values, all in 30-127 range, mostly decreasing)
        vals = list(data[i:i+14])
        if all(30 <= v <= 127 for v in vals):
            # Check for rough monotonic decrease
            decreasing = sum(1 for j in range(13) if vals[j] >= vals[j+1])
            if decreasing >= 8:
                tables.append(i)
    # Group nearby candidates
    if tables:
        groups = [[tables[0]]]
        for t in tables[1:]:
            if t - groups[-1][-1] < 5:
                groups[-1].append(t)
            else:
                groups.append([t])
        return [(g[0], len(g) * 14) for g in groups]
    return []
```

### Step 3: Modify Target Fields

```python
def modify_version_string(data, old_version, new_version):
    """Replace the version string in place (must be same length or shorter)."""
    data = bytearray(data)
    old_bytes = old_version.encode('ascii')
    new_bytes = new_version.encode('ascii')
    
    if len(new_bytes) > len(old_bytes):
        raise ValueError(f"New version string ({len(new_bytes)}B) is longer "
                        f"than old ({len(old_bytes)}B). Must be same or shorter.")
    
    # Pad with nulls if shorter
    new_bytes = new_bytes + b'\x00' * (len(old_bytes) - len(new_bytes))
    
    idx = data.find(old_bytes)
    if idx < 0:
        raise ValueError("Old version string not found")
    
    data[idx:idx+len(old_bytes)] = new_bytes
    return bytes(data)

def modify_register_write(data, reg_addr, new_value):
    """Change the value written to a specific register address."""
    data = bytearray(data)
    for i in range(0, len(data) - 7, 4):
        addr = struct.unpack_from('<I', data, i)[0]
        if addr == reg_addr:
            struct.pack_into('<I', data, i + 4, new_value)
            return bytes(data)
    raise ValueError(f"Register 0x{reg_addr:08X} not found in register write table")

def modify_power_table(data, table_offset, channel, new_power):
    """Modify a TX power table entry.
    
    Args:
        data: firmware region bytes
        table_offset: start offset of the power table
        channel: channel number (0-13)
        new_power: new power level (in 0.25 dBm units, 0-127)
    """
    data = bytearray(data)
    if not 0 <= channel <= 13:
        raise ValueError("Channel must be 0-13")
    if not 0 <= new_power <= 127:
        raise ValueError("Power must be 0-127 (quarter-dBm units)")
    
    # Each row in the power table covers all 14 channels
    data[table_offset + channel] = new_power
    return bytes(data)

def scale_power_table(data, table_offset, table_length, scale_factor):
    """Scale all power values in a table by a factor (e.g., 0.8 for -20%).
    
    Useful for reducing TX power across the board for regulatory compliance.
    """
    data = bytearray(data)
    for i in range(table_offset, table_offset + table_length):
        if 30 <= data[i] <= 127:
            data[i] = max(0, min(127, int(data[i] * scale_factor)))
    return bytes(data)
```

### Step 4: Modify BRCMcfgS (if needed)

```python
def modify_cfgs_transport(cfgs_data, transport_type):
    """Change the transport type in BRCMcfgS.
    
    Known values:
        0x04 = Standard (SDIO/UART auto-detect?)
    """
    data = bytearray(cfgs_data)
    # Transport byte is at offset 6 within the section data (after 12-byte header)
    data[6] = transport_type
    return bytes(data)

def modify_cfgs_variant_id(cfgs_data, variant_bytes):
    """Change the chip/module variant ID bytes.
    
    Known values:
        0x12, 0xA2 = CYW4343A2 (Pico W / generic)
        0x12, 0xA1 = CYW4343A1 (Murata 1DX)
        0x2A, 0x0A = CYW4343A2 (Murata 1YN specific)
    """
    data = bytearray(cfgs_data)
    # Variant bytes are at offsets 16-17 within the section data
    data[16] = variant_bytes[0]
    data[17] = variant_bytes[1]
    return bytes(data)
```

### Step 5: Re-pack into HCI Command Stream

```python
def pack_hci_firmware(memory_regions, chunk_size=16):
    """Pack memory regions into HCI FC4C command stream.
    
    Args:
        memory_regions: dict of {address: bytes} for each region
        chunk_size: bytes per FC4C command (16 for 1YN compatibility, 
                    up to 251 for 4343A1 compatibility)
    """
    result = bytearray()
    
    for base_addr in sorted(memory_regions.keys()):
        data = memory_regions[base_addr]
        for offset in range(0, len(data), chunk_size):
            chunk = data[offset:offset + chunk_size]
            addr = base_addr + offset
            
            # FC4C Write RAM command
            payload = struct.pack('<I', addr) + chunk
            result.append(0x4C)                     # opcode low
            result.append(0xFC)                     # opcode high
            result.append(len(payload))             # param length
            result.extend(payload)
    
    # FC4E Launch RAM command (0xFFFFFFFF = default entry point)
    result.extend(b'\x4E\xFC\x04\xFF\xFF\xFF\xFF')
    
    return bytes(result)

def firmware_to_c_header(data, array_name, output_path):
    """Convert firmware bytes to a C header file."""
    with open(output_path, 'w') as f:
        f.write(f'// Custom CYW43 BT Firmware - Generated\n')
        f.write(f'// Size: {len(data)} bytes\n\n')
        f.write(f'static const unsigned char {array_name}[] = {{\n')
        
        for i in range(0, len(data), 12):
            chunk = data[i:i+12]
            hex_str = ', '.join(f'0x{b:02x}' for b in chunk)
            f.write(f'    {hex_str},\n')
        
        f.write(f'}};\n')
        f.write(f'#define {array_name.upper()}_LEN ({len(data)})\n')
    
    print(f"Written {len(data)} bytes to {output_path}")
```

### Step 6: Complete Build Script

```python
#!/usr/bin/env python3
"""
Complete custom CYW43 BT firmware builder.
Modify the CUSTOMIZATION section below for your board.
"""
import re, struct, sys

# ─── CUSTOMIZATION ── Change these for your board ───────────────
NEW_VERSION = "UART 4343A2 custom_board"   # Must be <= original length
TX_POWER_SCALE = 0.85                       # 85% of original (reduce for lower power)
CHUNK_SIZE = 16                             # 16 for 1YN compat, 251 for 4343A1
OUTPUT_FILE = "cyw43_btfw_custom.h"
ARRAY_NAME = "cyw43_btfw_custom"
BASE_FIRMWARE = "cyw43_btfw_1yn.h"          # Starting point

# Register overrides: {address: new_value}
# Only change registers you understand!
REGISTER_OVERRIDES = {
    # Example: 0x00600160: 0x12345678,
}
# ─── END CUSTOMIZATION ──────────────────────────────────────────

def extract_firmware(path):
    with open(path) as f:
        text = f.read()
    m = re.search(r'\{([^}]+)\}', text, re.DOTALL)
    hex_vals = re.findall(r'0x([0-9a-fA-F]{2})', m.group(1))
    return bytes(int(h, 16) for h in hex_vals)

def decode_hci(data):
    writes = {}
    i = 0
    while i + 2 < len(data):
        opcode = (data[i+1] << 8) | data[i]
        param_len = data[i+2]
        if i + 3 + param_len > len(data):
            break
        if opcode == 0xFC4C:
            payload = data[i+3 : i+3+param_len]
            addr = struct.unpack_from('<I', payload, 0)[0]
            chunk = payload[4:]
            merged = False
            for ea in list(writes.keys()):
                if addr == ea + len(writes[ea]):
                    writes[ea].extend(chunk)
                    merged = True
                    break
            if not merged:
                writes[addr] = bytearray(chunk)
        i += 3 + param_len
    return writes

def pack_hci(regions, chunk_size):
    result = bytearray()
    for addr in sorted(regions.keys()):
        data = regions[addr]
        for off in range(0, len(data), chunk_size):
            chunk = data[off:off+chunk_size]
            payload = struct.pack('<I', addr + off) + chunk
            result.extend(b'\x4c\xfc')
            result.append(len(payload))
            result.extend(payload)
    result.extend(b'\x4e\xfc\x04\xff\xff\xff\xff')
    return bytes(result)

def to_c_header(data, name, path):
    with open(path, 'w') as f:
        f.write(f'// Custom CYW43 BT Firmware\n// Size: {len(data)} bytes\n\n')
        f.write(f'static const unsigned char {name}[] = {{\n')
        for i in range(0, len(data), 12):
            c = data[i:i+12]
            f.write('    ' + ', '.join(f'0x{b:02x}' for b in c) + ',\n')
        f.write('};\n')
        f.write(f'#define {name.upper()}_LEN ({len(data)})\n')

def main():
    print(f"Loading base firmware: {BASE_FIRMWARE}")
    raw = extract_firmware(BASE_FIRMWARE)
    print(f"  Raw size: {len(raw)} bytes")
    
    regions = decode_hci(raw)
    print(f"  Memory regions: {len(regions)}")
    for addr, data in sorted(regions.items()):
        print(f"    0x{addr:08X}: {len(data)} bytes")
    
    # Find the BRCMcfgD region (largest region at 0x0021Fxxx)
    cfgd_addr = None
    for addr in regions:
        if 0x0021F000 <= addr <= 0x0021FFFF:
            cfgd_addr = addr
    
    if cfgd_addr is None:
        print("ERROR: Could not find BRCMcfgD region")
        sys.exit(1)
    
    cfgd = regions[cfgd_addr]
    print(f"\n  BRCMcfgD at 0x{cfgd_addr:08X}, {len(cfgd)} bytes")
    
    # Modify version string
    for prefix in [b'UART ', b'SDIO ', b'BCM43']:
        idx = cfgd.find(prefix)
        if idx >= 0:
            end = cfgd.index(0, idx)
            old_ver = cfgd[idx:end].decode('ascii')
            new_ver = NEW_VERSION.encode('ascii')
            if len(new_ver) > len(old_ver.encode('ascii')):
                print(f"  WARNING: New version string too long, truncating")
                new_ver = new_ver[:len(old_ver.encode('ascii'))]
            padded = new_ver + b'\x00' * (end - idx - len(new_ver))
            cfgd[idx:end] = padded
            print(f"  Version: '{old_ver}' -> '{NEW_VERSION}'")
            break
    
    # Apply register overrides
    for reg_addr, new_val in REGISTER_OVERRIDES.items():
        found = False
        for i in range(0, len(cfgd) - 7, 4):
            addr = struct.unpack_from('<I', cfgd, i)[0]
            if addr == reg_addr:
                old_val = struct.unpack_from('<I', cfgd, i + 4)[0]
                struct.pack_into('<I', cfgd, i + 4, new_val)
                print(f"  Reg 0x{reg_addr:08X}: 0x{old_val:08X} -> 0x{new_val:08X}")
                found = True
                break
        if not found:
            print(f"  WARNING: Register 0x{reg_addr:08X} not found")
    
    # Scale TX power tables
    if TX_POWER_SCALE != 1.0:
        modified = 0
        for i in range(len(cfgd)):
            if 40 <= cfgd[i] <= 127:
                # Heuristic: only scale bytes that are in consecutive runs
                # of power-table-like values
                neighbors = sum(1 for j in range(max(0,i-2), min(len(cfgd),i+3))
                               if 30 <= cfgd[j] <= 127)
                if neighbors >= 4:
                    cfgd[i] = max(20, min(127, int(cfgd[i] * TX_POWER_SCALE)))
                    modified += 1
        print(f"  Scaled {modified} power table bytes by {TX_POWER_SCALE}")
    
    # Re-pack
    print(f"\nRe-packing with chunk_size={CHUNK_SIZE}...")
    firmware = pack_hci(regions, CHUNK_SIZE)
    print(f"  Output size: {len(firmware)} bytes")
    
    to_c_header(firmware, ARRAY_NAME, OUTPUT_FILE)
    print(f"\nWritten to {OUTPUT_FILE}")
    print("Done.")

if __name__ == "__main__":
    main()
```

---

## 6. NVRAM Customization (WiFi Radio Tuning)

NVRAM customization is separate from BT firmware and much simpler — it's plain text.

### Creating a Custom NVRAM

```c
// custom_wifi_nvram.h
// Based on wifi_nvram_43439.h for Pico W

static const uint8_t wifi_nvram_4343[] CYW43_RESOURCE_ATTRIBUTE =
    "NVRAMRev=$Rev: custom_v1 $\x00"
    "manfid=0x2d0\x00"
    "prodid=0x0727\x00"
    "vendid=0x14e4\x00"
    "devid=0x43e2\x00"
    "boardtype=0x0900\x00"        // <-- YOUR board type ID
    "boardrev=0x1100\x00"
    "boardnum=22\x00"
    "macaddr=00:00:00:00:00:00\x00"  // Will be overridden by OTP/driver
    "sromrev=11\x00"
    "boardflags=0x00404001\x00"   // <-- Adjust for your board flags
    "boardflags3=0x04000000\x00"
    "xtalfreq=37400\x00"
    "nocrc=1\x00"
    "ag0=255\x00"
    "aa2g=1\x00"
    "ccode=ALL\x00"
    
    // PA calibration — MUST BE MEASURED FOR YOUR ANTENNA
    "pa0itssit=0x20\x00"
    "extpagain2g=0\x00"
    "pa2ga0=-168,7161,-820\x00"   // <-- Replace with your measured values
    "AvVmid_c0=0x0,0xc8\x00"
    "cckpwroffset0=5\x00"
    
    // TX power limits
    "maxp2ga0=74\x00"             // <-- Set to your regulatory limit
    "txpwrbckof=6\x00"
    "cckbw202gpo=0\x00"
    "legofdmbw202gpo=0x66111111\x00"  // <-- Rate-specific backoff
    "mcsbw202gpo=0x77711111\x00"
    "propbw202gpo=0xdd\x00"
    
    // Digital filters & PAPD
    "ofdmdigfilttype=18\x00"
    "ofdmdigfilttypebe=18\x00"
    "papdmode=1\x00"
    "papdvalidtest=1\x00"
    "pacalidx2g=45\x00"
    "papdepsoffset=-30\x00"
    "papdendidx=58\x00"
    
    // Transport — choose based on your interface
    "muxenab=0x100\x00"           // 0x100=SDIO, 0x10=GPIO, 0x11=UART+WAKE
    
    // BT coexistence
    "btc_mode=1\x00"              // 0=passive, 1=active
    
    // Spur mitigation
    "spurconfig=0x3\x00"
    "glitch_based_crsmin=1\x00"
    
    // Deadman timer
    "deadman_to=0xffffffff\x00"
    
    "\x00\x00";
```

### Key Parameters to Calibrate

1. **`pa2ga0`**: Three polynomial coefficients for PA linearization. Requires RF test equipment (spectrum analyzer + power meter) to measure your board's actual PA response curve.

2. **`maxp2ga0`**: Maximum TX power in quarter-dBm units. 84 = 21 dBm, 74 = 18.5 dBm. Must comply with FCC/CE/etc. limits for your product.

3. **`legofdmbw202gpo` / `mcsbw202gpo`**: Per-rate power backoff. Each nibble (4 bits) is the backoff for one rate group. Higher nibble = more backoff.

---

## 7. Integration with cyw43-driver

### Replacing the BT Firmware Blob

In the cyw43-driver, the BT firmware is loaded from the header file specified by `CYW43_CHIPSET_FIRMWARE_INCLUDE_FILE` or directly included. To use your custom firmware:

1. Place your generated header file in the `firmware/` directory
2. Modify the include to point to your file:
   ```c
   // In your build config or cyw43_configport.h
   #define CYW43_BLUETOOTH_FW_INCLUDE "cyw43_btfw_custom.h"
   ```
3. Or replace the contents of an existing firmware header file

### Replacing NVRAM

Similarly, the NVRAM file is included directly:
```c
// The driver includes one of:
//   wifi_nvram_43439.h
//   wifi_nvram_1dx.h  
//   wifi_nvram_1yn.h
```

Replace the contents of the appropriate file or redirect the include.

### Loading Sequence in the Driver

From `cyw43_ll.c`, the firmware loading order is:

```
1. cyw43_ll_bus_init()
   ├── Reset chip (disable WLAN ARM + SOCRAM cores)
   ├── Reset SOCRAM core
   ├── Disable SRAM_3 remap (4343x specific)
   ├── cyw43_check_valid_chipset_firmware()  ← WiFi FW validation
   │   └── Only checks for "Version: " string in trailer
   ├── cyw43_download_resource(0x00000000, WiFi_FW)  ← WiFi FW at addr 0
   ├── Write NVRAM at (512KB - 4 - nvram_len)  ← NVRAM at top of RAM
   ├── Reset WLAN ARM core
   ├── Wait for HT clock
   ├── cyw43_clm_load()  ← CLM blob via ioctl
   └── Configure ioctl params (bus, apsta, MAC)
   
2. BT firmware loaded separately (via cyw43_bluetooth.c)
   └── Uses cyw43_ll_write_backplane_mem() for SDIO
       or HCI commands for UART
```

---

## 8. Testing and Validation

### Minimum Viability Test

1. **Load firmware** — does the chip accept it without error?
2. **HCI Reset** — send HCI_Reset (0x0C03), get Command Complete event?
3. **Read Local Version** — send HCI_Read_Local_Version (0x1001), get valid response?
4. **BLE Advertising** — can you start BLE advertising and see it on a phone?
5. **Classic BT Inquiry** — can you perform an inquiry scan?

### RF Validation

If you changed TX power or PA parameters:

1. **Conducted power measurement** — attach RF cable, measure actual TX power per channel
2. **OTA (Over-The-Air) test** — measure EIRP with your antenna
3. **EVM measurement** — check modulation quality hasn't degraded
4. **Receiver sensitivity** — verify RX performance hasn't been affected
5. **Regulatory pre-scan** — run against FCC/CE/ISED limits

### What Can Go Wrong

| Symptom | Likely Cause | Fix |
|---------|-------------|-----|
| Chip doesn't respond after load | Corrupted patchram code | Revert to original patchram sections |
| HCI timeout | Wrong transport config in BRCMcfgS | Check transport byte and variant ID |
| Low TX power | Power table values too low | Increase power table entries |
| Poor range | PA polynomial mismatch | Re-calibrate `pa2ga0` for your antenna |
| BLE works, Classic doesn't | Missing patchram entry | Ensure all patchram entries preserved |
| WiFi interference | BTC mode wrong | Set correct `btc_mode` in NVRAM |
| Chip rejects firmware | Signature verification (SDIO only) | Use UART path or unsigned format |

---

## 9. Advanced: Adding New Patchram Entries

If you need to patch ROM functions (e.g., changing BT stack behavior), you need to:

1. **Identify the ROM function** to patch — requires Thumb-2 disassembly of ROM dump
2. **Write replacement code** in ARM Thumb-2 assembly
3. **Add a patchram table entry** at 0x000D0200 mapping ROM_addr → RAM_addr
4. **Add the code** to the BRCMcfgD region at the designated RAM address
5. **Ensure the RAM address** doesn't conflict with existing patches

This is advanced work requiring:
- ARM Cortex-M3 architecture knowledge
- Thumb-2 instruction set encoding
- Understanding of the CYW4343x ROM memory map
- A disassembly of the ROM (obtainable via backplane read after chip is running)

**Warning**: The patchram hardware has a limited number of slots (typically 16-32). The A1 silicon uses nearly all of them. The A2 silicon uses only ~5-8, leaving room for custom entries.

---

## 10. Legal and Regulatory Notes

1. **FCC/CE Compliance**: Any modification to TX power, frequency, or modulation parameters may require re-certification. The original Pico W's FCC grant covers only the unmodified firmware.

2. **Broadcom/Infineon Licensing**: The firmware blobs in cyw43-driver are distributed under specific license terms. Check the license before redistributing modified firmware.

3. **Module Certification**: If using a Murata 1DX/1YN module, the module's pre-certification assumes unmodified firmware. Changes may void the module's certification.

4. **Intentional Radiators**: In most jurisdictions, you cannot increase TX power beyond the certified levels without your own FCC/CE test report.
