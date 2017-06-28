# CLR Managed heap definition

**About this document**

This document describes how the CLR manged heap is defined as a ChibiOS target.

For STM32 based devices:
The configurations are spread between two linker files: 
- [rules.ld](../../targets/CMSIS-OS/ChibiOS/common/rules.ld) (which is common to all STM32 based ChibiOS targets **except** the F7 series that use [rules_STM32F7xx.ld](../../targets/CMSIS-OS/ChibiOS/common/rules_STM32F7xx.ld) instead)
- the target linker file provided for the nanoCLR in the target board folder, e.g. [STM32F091xC.ld](../../targets/CMSIS-OS/ChibiOS/ST_NUCLEO_F091RC/nanoCLR/STM32F091xC.ld)


## Managed heap location and size

The CLR managed heap can be located on the target board at any RAM address where space available. Either internal or external.

It will be placed (considering the RAM region defined) after the region containing the initialized variables (if any is assigned to that RAM region) and before the CRT heap (if this is assigned to the same RAM region).

This empowers developers to create new target boards with maximum flexibility of where to locate the CLR managed heap and its size.


### Definition the CLR managed heap location

The location of the CLR managed heap is set in in target linker file provided for nanoCLR in the target boards folder, e.g. [STM32F091xC.ld](../../targets/CMSIS-OS/ChibiOS/ST_NUCLEO_F091RC/nanoCLR/STM32F091xC.ld)

For exaple the line (usually toward the end of the file) will contain something similar to `REGION_ALIAS("CLR_MANAGED_HEAP_RAM", ram0);`. The example stated here defines CLR manged heap location as being set in the _ram0_ region. The RAM regions are defined on that same file, at the beginning. For further information, please check the ChibiOS documentation for details on how to define further RAM regions.


### Definition of the CLR managed heap size

The size of the CLR managed heap is set in the CMake file of the target board, e.g. [CMakeLists.txt](../../targets/CMSIS-OS/ChibiOS/ST_NUCLEO_F091RC/CMakeLists.txt).
For example the line will contain something similar to `--defsym=__clr_managed_heap_size__=0x7000`. In the example stated here the size of CLR manged heap is being set to 0x7000.

When defining the size you need to take into account several factors:
- the total available size of the region where it's being placed
- if there are initialized variables assigned to this region how much space they are taking
- if the CRT heap is located in this region and the size left for it is enough

The linker is only able to determine whether there is enough room for all of these factors and it will only complain if there isn't. However it can not determine if the CRT heap is large enough for the running requirements. That is up to the target board developer.
For a detailed overview on the final memory map you may want to check the _nanoCLR.map_ that will be located in the build folder after a successful build. Look for the region called `.clr_managed_heap` to see the final address mapping for it.
