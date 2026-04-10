# MCUboot Flash Layout — esp32 (Xtensa LX6, original)

MCUboot replaces the ESP-IDF second-stage bootloader binary. The partition table selects the MCUboot variant CSV.

## Available MCUboot partition tables

| File                                  | Flash | Strategy         | Slots       | Swap status | Config | deploy_0+1 each |
|---------------------------------------|-------|------------------|-------------|-------------|--------|-----------------|
| `partitions_nanoclr_mcuboot_2mb.csv`  | 2 MB  | overwrite-only   | 2× 768k     | —           | 24k    | 212k            |
| `partitions_nanoclr_mcuboot_4mb.csv`  | 4 MB  | swap-using-move  | 2× 1600k    | —           | 24k    | 372k            |
| `partitions_nanoclr_mcuboot_8mb.csv`  | 8 MB  | swap-using-move  | 2× 2048k    | 32k         | 24k    | ~1.9 MB         |
| `partitions_nanoclr_mcuboot_16mb.csv` | 16 MB | swap-using-move  | 2× 2048k    | 32k         | 24k    | ~5.9 MB         |

## Layout details

### 2 MB — overwrite-only
`CONFIG_PARTITION_TABLE_OFFSET=0x8000` (tight — partition table at 32k)

| Partition   | Type | Subtype  | Offset     | Size  |
|-------------|------|----------|-----------|-------|
| nvs         | data | nvs      | 0x009000  | 24k   |
| phy_init    | data | phy      | 0x00F000  | 4k    |
| primary     | app  | ota_0    | 0x010000  | 768k  |
| secondary   | app  | ota_1    | 0x0D0000  | 768k  |
| config      | data | littlefs | 0x190000  | 24k   |
| deploy_0    | data | 0x84     | 0x196000  | 212k  |
| deploy_1    | data | 0x84     | 0x1CB000  | 212k  |

### 4 MB and above
`CONFIG_PARTITION_TABLE_OFFSET=0x10000`; primary slot starts at `0x20000`.

| Partition    | Type | Subtype  | Offset     | Size   | 4MB | 8MB | 16MB |
|--------------|------|----------|-----------|--------|-----|-----|------|
| nvs          | data | nvs      | 0x009000  | 24k    | ✓   | ✓   | ✓    |
| phy_init     | data | phy      | 0x00F000  | 4k     | ✓   | ✓   | ✓    |
| primary      | app  | ota_0    | 0x020000  | 1600k / 2048k | ✓ | ✓ | ✓ |
| secondary    | app  | ota_1    | varies    | same as primary | ✓ | ✓ | ✓ |
| swap_status  | data | 0x85     | varies    | 32k    | —   | ✓   | ✓    |
| config       | data | littlefs | varies    | 24k    | ✓   | ✓   | ✓    |
| deploy_0     | data | 0x84     | varies    | 372k / ~1.9MB / ~5.9MB each | ✓ | ✓ | ✓ |
| deploy_1     | data | 0x84     | varies    | same as deploy_0 | ✓ | ✓ | ✓ |

## Notes

- All variants require `CONFIG_BOOTLOADER_OFFSET_IN_FLASH=0x1000` (MCUboot binary at 0x1000).
- 2 MB uses 768k slots (factory CLR size for esp32 original); 4 MB uses 1600k slots; 8/16 MB use 2048k slots.
- **IMAGE_NUMBER=2**: Image 0 = nanoCLR (primary/secondary OTA slots); Image 1 = deployment area (deploy_0 primary + deploy_1 secondary). Both images are MCUboot-signed and managed independently.
