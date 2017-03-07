# CLR Managed heap definition

**About this document**

This document describes how the CLR manged heap is defined in a ChibiOS target.

These configurations are located in two linker files: 
- [rules.ld](../../targets/CMSIS-OS/ChibiOS/common/rules.ld) (common to all ChibiOS targets **except** the ones with F7 series that have to use [rules_STM32F7xx.ld](../../targets/CMSIS-OS/ChibiOS/common/rules_STM32F7xx.ld) instead)
- the target linker file provided for nanoCLR at the target board folder, e.g. [STM32F091xC.ld](../../targets/CMSIS-OS/ChibiOS/ST_NUCLEO_F091RC/nanoCLR/STM32F091xC.ld)


## Managed heap location and size

The CLR managed heap can be located at any RAM address space available in the target board. Either internal or external.

It will be placed (considering the RAM region defined) after the region containing the initialized variables (if any is assigned to that RAM region) and before the CRT heap (if this one is assigned to that same RAM region).

This empowers the target board developer with the maximum possible flexibility regarding here to locate the CLR managed heap and the size of it.


### Definition the CLR managed heap location

The location of the CLR managed heap is set in in target linker file provided for nanoCLR at the target board folder, e.g. [STM32F091xC.ld](../../targets/CMSIS-OS/ChibiOS/ST_NUCLEO_F091RC/nanoCLR/STM32F091xC.ld)

Near the end of the file, locate this line `REGION_ALIAS("CLR_MANAGED_HEAP_RAM", ram0);`. On the example here the CLR manged heap location is being set at _ram0_ region. The RAM regions are defined on that same file, at the beginning. Please check the ChibiOS documentation for details on how to define the RAM regions.


### Definition of the CLR managed heap size

The size of the CLR managed heap is set in CMake file of the target board, e.g. [CMakeLists.txt](../../targets/CMSIS-OS/ChibiOS/ST_NUCLEO_F091RC/CMakeLists.txt).
Locate the line containing `--defsym=__clr_managed_heap_size__=0x7000`. On the example here the size of CLR manged heap is being set to 0x7000.

When defining this size you need to take into account several factors:
- the total available size of the region here it's being placed
- if there are initialized variables assigned to this region how much space of ot they are taking
- if the CRT heap is to be located in this region what will be the size left for it and if that is enough

The linker will only be able to check if there is enough room for all these and it will complain if there isn't. But it can not check if the CRT heap is large enough for the running requirements. That is up to the target board developer.
For a detailed overview on the final memory map you may want to check the _nanoCLR.map_ that will be located at the build folder after a successful build. Look for the region called `.clr_managed_heap` to see the final address mapping for it.
