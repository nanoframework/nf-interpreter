# MCUboot Flash Layout — esp32h2 (RISC-V RV32IMC, BT LE + 802.15.4, no WiFi)

MCUboot replaces the ESP-IDF second-stage bootloader binary. The partition table selects the MCUboot variant CSV.

**CLR slot size**: 1728k (0x1B0000)  
**2 MB**: not available — dual-slot infeasible (2×1728k = 3.4 MB > 2 MB flash)  
**8 MB / 16 MB**: not commonly available for this SoC; 4 MB is the practical upper limit

## Available MCUboot partition tables

| File                                 | Flash | Strategy        | Slots      | Swap status | Config | Deploy  |
|--------------------------------------|-------|-----------------|-----------|-------------|--------|---------|
| `partitions_nanoclr_mcuboot_4mb.csv` | 4 MB  | swap-using-move | 2× 1728k  | —           | 24k    | ~488k   |

> No swap_status partition on 4 MB — flash budget is tight (only 488k remains for deployment).

## Layout details

`CONFIG_PARTITION_TABLE_OFFSET=0x10000`; MCUboot binary at `0x1000`; primary slot at `0x20000`.

### 4 MB

| Partition   | Type | Subtype  | Offset     | Size   |
|-------------|------|----------|-----------|--------|
| nvs         | data | nvs      | 0x009000  | 24k    |
| phy_init    | data | phy      | 0x00F000  | 4k     |
| primary     | app  | ota_0    | 0x020000  | 1728k  |
| secondary   | app  | ota_1    | 0x1D0000  | 1728k  |
| config      | data | littlefs | 0x380000  | 24k    |
| deploy      | data | 0x84     | 0x386000  | 488k   |

## Notes

- Existing non-MCUboot CSVs incorrectly label factory size as 1664k; actual size is **1728k** (confirmed from partition CSV offsets).
- esp32h2 has no WiFi; `phy_init` is retained for Bluetooth/802.15.4 PHY calibration data.
- The existing 4 MB non-MCUboot CSV uses `spiffs` for config; MCUboot variant uses `littlefs`.
