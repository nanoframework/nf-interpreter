# MCUboot Flash Layout — esp32c5 (RISC-V RV32IMC, WiFi + BT LE + 802.15.4)

MCUboot replaces the ESP-IDF second-stage bootloader binary. The partition table selects the MCUboot variant CSV.

**CLR slot size**: 2240k (0x240000) — larger than most other ESP32 variants  
**4 MB**: not available — dual-slot infeasible (2×2240k = 4.4 MB > 4 MB flash)

## Available MCUboot partition tables

| File                                  | Flash | Strategy        | Slots      | Swap status | Config | deploy_0+1 each |
|---------------------------------------|-------|-----------------|-----------|-------------|--------|-----------------|
| `partitions_nanoclr_mcuboot_8mb.csv`  | 8 MB  | swap-using-move | 2× 2240k  | 32k         | 24k    | ~1.7 MB         |
| `partitions_nanoclr_mcuboot_16mb.csv` | 16 MB | swap-using-move | 2× 2240k  | 32k         | 24k    | ~5.7 MB         |

## Layout details

`CONFIG_PARTITION_TABLE_OFFSET=0x10000`; MCUboot binary at `0x1000`; primary slot at `0x20000`.

### 8 MB

| Partition    | Type | Subtype  | Offset     | Size   |
|--------------|------|----------|-----------|--------|
| nvs          | data | nvs      | 0x009000  | 24k    |
| phy_init     | data | phy      | 0x00F000  | 4k     |
| primary      | app  | ota_0    | 0x020000  | 2240k  |
| secondary    | app  | ota_1    | 0x260000  | 2240k  |
| swap_status  | data | 0x85     | 0x4A0000  | 32k    |
| config       | data | littlefs | 0x4A8000  | 24k    |
| deploy_0     | data | 0x84     | 0x4AE000  | ~1.7MB |
| deploy_1     | data | 0x84     | 0x657000  | ~1.7MB |

### 16 MB

Same structure as 8 MB; deploy_0+1 each increase to ~5.7 MB (0x5A9000).

## Notes

- **IMAGE_NUMBER=2**: Image 0 = nanoCLR (primary/secondary OTA slots); Image 1 = deployment area (deploy_0 primary + deploy_1 secondary). Both images are MCUboot-signed and managed independently.
