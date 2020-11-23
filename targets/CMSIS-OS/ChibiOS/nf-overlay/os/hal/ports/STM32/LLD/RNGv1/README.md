# RNG v1

This driver supports all STM32 series except for the STM32F0 series.
The low level driver code is taken or heavily inspired in the STCube MX HAL drivers from STMicroelectronics.
The ChibiOS driver follows the 'standard' template driver model.

**NOTE:** unlike all the other drivers and because most STM32 chips (**Except the STM32F0 series**) have a RNG unit, the default for this driver is to be included.
If for some reason the developer chooses NOT to include support, they have to add `#define HAL_USE_STM32_RNG == FALSE` preferably in the file halconf_nf.h.
