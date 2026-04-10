# MCUboot Flash Layout — esp32p4 (Xtensa LX7 dual-core, high-performance)

MCUboot replaces the ESP-IDF second-stage bootloader binary. The partition table selects the MCUboot variant CSV.

**CLR slot size**: 1664k (0x1A0000)  
**2 MB**: not available — dual-slot infeasible (2×1664k = 3.3 MB > 2 MB flash)

## Available MCUboot partition tables

| File                                  | Flash | Strategy        | Slots      | Swap status | Config | deploy_0+1 each |
|---------------------------------------|-------|-----------------|-----------|-------------|--------|-----------------|
| `partitions_nanoclr_mcuboot_4mb.csv`  | 4 MB  | swap-using-move | 2× 1664k  | —           | 24k    | 308k            |
| `partitions_nanoclr_mcuboot_8mb.csv`  | 8 MB  | swap-using-move | 2× 1664k  | 32k         | 24k    | ~2.3 MB         |
| `partitions_nanoclr_mcuboot_16mb.csv` | 16 MB | swap-using-move | 2× 1664k  | 32k         | 24k    | ~6.2 MB         |
| `partitions_nanoclr_mcuboot_32mb.csv` | 32 MB | swap-using-move | 2× 1664k  | 32k         | 24k    | ~14.3 MB        |

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
| deploy_0    | data | 0x84     | 0x366000  | 308k   |
| deploy_1    | data | 0x84     | 0x3B3000  | 308k   |

### 8 MB

| Partition    | Type | Subtype  | Offset     | Size   |
|--------------|------|----------|-----------|--------|
| nvs          | data | nvs      | 0x009000  | 24k    |
| phy_init     | data | phy      | 0x00F000  | 4k     |
| primary      | app  | ota_0    | 0x020000  | 1664k  |
| secondary    | app  | ota_1    | 0x1C0000  | 1664k  |
| swap_status  | data | 0x85     | 0x360000  | 32k    |
| config       | data | littlefs | 0x368000  | 24k    |
| deploy_0     | data | 0x84     | 0x36E000  | ~2.3MB |
| deploy_1     | data | 0x84     | 0x5B7000  | ~2.3MB |

### 16 MB / 32 MB

Same structure as 8 MB; deploy_0+1 each increase to ~6.2 MB (16MB) or ~14.3 MB (32MB).

## Notes

- **IMAGE_NUMBER=2**: Image 0 = nanoCLR (primary/secondary OTA slots); Image 1 = deployment area (deploy_0 primary + deploy_1 secondary). Both images are MCUboot-signed and managed independently.
