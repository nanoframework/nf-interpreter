# CMake file for building ChibiOS from sources

**About this document**

This document describes the purpose and workflow for the CMake configuration files to build ChibiOS from the repository sources.
Building ChibiOS from sources might be needed when debugging a **nanoFramework** feature that interacts with it.


# Purpose

The purpose of the configuration files collections is to create a CMake package for ChibiOS and build it.


# Reasoning

The sources from ChibiOS can be downloaded from their GitHub mirror repository or, if already available in the build machine, can be copied to the build folder.
When invoking CMake these options are passed specifying ```RTOS=CHIBIRTOS``` and ```CHIBI_SOURCE=path-to-the-local-repository-folder```.

_Note: when specifying the location of a local clone make sure that the correct branch is checked out._

# Workflow

ChibiOS HAL is based on _boards_. There collection of supported boards and the respective configurations live in hal/boards directory. 

After successfully finding the board directory CMake tries to figure out the vendor and the series for later use. Please check the code at [FindCHIBIOS.cmake](../../CMake/Modules/FindCHIBIOS.cmake) for details.
(NOTE: the current code has been validated for STM boards only)

The _FindCHIBIOS.cmake_ includes the specifics for the target series and the respective GCC options.
The file naming logic is:
- CHIBIOS_**STM32F0xx**_sources.cmake: common source files for the series (with the series name in bold)
- CHIBIOS_**STM32F0xx**_GCC_options.cmake: GCC options for the series (with the series name in bold)

When adding a new vendor/series/board follow these general guidelines:
1. When in doubt try to follow the make files of the repo. They'll give you all the details that you need in order to replicate that in the CMake files.
2. Check if the series file exists. If not, create it and add the source files and include directories.
3. Check if the series list (e.g. STM32_F0xx_BOARDS) contains the board name. If not add the board there or, if the series list doesn't exist at all, add it along with the respective logic.
4. Check if the linker file name is listed in the series file. If not, add them.
