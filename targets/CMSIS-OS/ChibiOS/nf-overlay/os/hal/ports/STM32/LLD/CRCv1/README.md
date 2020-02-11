# CRC v1

This driver supports all STM32 series.
The low level driver code is taken or heavily inspired in the STCube MX HAL drivers from STMicroelectronics.
The ChibiOS driver follows the 'standard' template driver model.

**NOTE:** unlike all the other drivers and because all SMT32 chips (until now) have a CRC unit, the default for this driver is to be included.
If for some reason the developer chooses NOT to include support, they have to add `#define HAL_NF_USE_STM32_CRC == FALSE` preferably in the file halconf_nf.h.
