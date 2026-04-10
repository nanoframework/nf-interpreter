# MCUboot Flash Layout — esp32s2 (Xtensa LX7, WiFi only)

MCUboot replaces the ESP-IDF second-stage bootloader binary. The partition table selects the MCUboot variant CSV.

**CLR slot size**: 1664k (0x1A0000)  
**2 MB**: not available — dual-slot infeasible (2×1664k = 3.3 MB > 2 MB flash)

## Available MCUboot partition tables

| File                                  | Flash | Strategy        | Slots      | Swap status | Config | Deploy   |
|---------------------------------------|-------|-----------------|-----------|-------------|--------|----------|
| `partitions_nanoclr_mcuboot_4mb.csv`  | 4 MB  | swap-using-move | 2× 1664k  | —           | 24k    | ~616k    |
| `partitions_nanoclr_mcuboot_8mb.csv`  | 8 MB  | swap-using-move | 2× 1664k  | 32k         | 24k    | ~4.6 MB  |
| `partitions_nanoclr_mcuboot_16mb.csv` | 16 MB | swap-using-move | 2× 1664k  | 32k         | 24k    | ~12.6 MB |

## Layout details

`CONFIG_PARTITION_TABLE_OFFSET=0x10000`; MCUboot binary at `0x1000`; primary slot at `0x20000`.

### 4 MB

| Partition   | Type | Subtype  | Offset     | Size   |
|-------------|------|----------|-----------|--------|
| nvs         | data | nvs      | 0x009000  | 24k    |
| phy_init    | data | phy      | 0x00F000  | 4k     |
| primary     | app  | ota_0    | 0x020000  | 1664k  |
| secondary   | app  | ota_1    | 0x1C0000  | 1664k  |
| config      | data | littlefs | 0x360000  | 24k    |
| deploy      | data | 0x84     | 0x366000  | 616k   |

### 8 MB

| Partition    | Type | Subtype  | Offset     | Size   |
|--------------|------|----------|-----------|--------|
| nvs          | data | nvs      | 0x009000  | 24k    |
| phy_init     | data | phy      | 0x00F000  | 4k     |
| primary      | app  | ota_0    | 0x020000  | 1664k  |
| secondary    | app  | ota_1    | 0x1C0000  | 1664k  |
| swap_status  | data | 0x85     | 0x360000  | 32k    |
| config       | data | littlefs | 0x368000  | 24k    |
| deploy       | data | 0x84     | 0x36E000  | ~4.6MB |

### 16 MB

Same structure as 8 MB; deploy size increases to ~12.6 MB (0xC92000).
