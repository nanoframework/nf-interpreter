# Instructions for building a **nano Framework** image

## Table of contents ##

- [Prerequisites](#Prerequisites)
- [Preparation](#Preparation)
- [Build a **nano Framework** image](#BuildNanoFrameworkImage)
- [**nano Framework** build deliverables](#NanoFrameworkBuildDeliverables)

**About this document**

This document describes how to build an image of **nano Framework** to be flashed in a SoC or MCU.
The build is based on CMake tool to ease the development in all major platforms.

<a name="Prerequisites"></a>
# Prerequisites

You'll need:
- [GNU ARM Embedded Toolchain](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads)
- [CMake](https://cmake.org/)

If you are using VS Code as your development platform we suggest that you use the CMake Tools extension. This will allow you to run the builds without leaving VS Code.
- [Visual Studio Code](http://code.visualstudio.com/)
- [CMake Tools](https://marketplace.visualstudio.com/items?itemName=vector-of-bool.cmake-tools)


<a name="Preparation"></a>
# Preparation

It's ***highly*** recommended that run the build outside the source tree. This prevents you from clutering the source tree with CMake artifacts, temporary files and like. 
In fact this is enforced and checked by the CMake script.

In case you need to clean up or start a fresh build all you have to do is simply delete the contents of the build directory.

As a suggestion we recommend that you create a directory named *build* in the repository root and run CMake from there.


<a name="BuildNanoFrameworkImage"></a>
# Build a **nano Framework** image

The build script accepts the following parameters (some of them are mandatory).
- TARGET_CHIP: this is the vendor reference to the chip that you are building the image for. At this time only STM32F4 series are supported. A valid reference here would be STM32F407VG.
- PACKAGE_VERSION: In case you are building for an STM32 chip this is the package version of the ST Cube package (with the CMSIS and HAL drivers) that you want to use. This will be downloaded from ST web site except if the package already exists in the ST Cube repository folder. A valid package number is required. E.g.: 1.3.0 or 1.41.2.  
- TOOLCHAIN: the toolchain to use in the build. The default (and only option at this time) is GCC.
- TOOLCHAIN_PREFIX: path to the install directory of the toolchain. E.g.: "E:/GNU_Tools_ARM_Embedded/5_4_2016q3". Mind the forward slash on the path for all platforms.
- CMAKE_BUILD_TYPE: build type (Debug, Release, etc). The default is Release.
- RTOS: specifies the RTOS to add to the image. This will download the appropriate files from the respective repository. Current valid RTOSes are FreeRTOS (FREERTOS) and mBed RTOS (MBEDRTOS).
- FREERTOS_VERSION: specifies the FreeRTOS version to grab the source files. It has to match one of the official versions from the FreeRTOS repository. If none is specified it will download the 'trunk' version.

You can specify any generator that is supported in the platform where you are building.
For more information on this check CMake documentation [here](https://cmake.org/cmake/help/v3.7/manual/cmake-generators.7.html?highlight=generator).

## Building from the command prompt

If you are building from the command prompt, just go to your *build* directory and run CMake from there with the appropriate parameters. 
The following is a working example:

```
cmake \
-DTOOLCHAIN_PREFIX="E:/GNU_Tools_ARM_Embedded/5_4_2016q3" \
-DTARGET_CHIP=STM32F407VG \
-DPACKAGE_VERSION=1.13.1 \
-RTOS=FREERTOS \
-FREERTOS_VERSION=9.0.0 \
-G "NMake Makefiles" ../ 
```

This will call CMake (on your *build* directory that is assumed to be under the repository root) specifing the location of the toolchain install, targeting STM32F407VG, asking for the ST Cube package version 1.13.1 to be used and that the build files suitable for NMake are to be generated.
After succesfull completion you'll have the build files ready to be used in the target build tool.

## Building from VS Code (using CMake Tools extension)

we've added the required files and configurations to help you launch your build from VS Code.
Follows a brief explanation on the files you might want to tweak.

- settings.json (inside .vscode folder) here you can change the generator that CMage uses to generate the build. The default is ```"cmake.generator": "NMake Makefiles"```.
- cmake-variants.json (at the repository root) here you can add several build flavors. You can even add variants to each one. Check the documentation extension [here](https://github.com/vector-of-bool/vscode-cmake-tools/blob/develop/docs/build_variants.md).

To launch the build in VS Code check the status bar at the bottom. Select the build flavor and then click the build buttom (or hit F7).


<a name="NanoFrameworkBuildDeliverables"></a>
# **nano Framework** build deliverables

After a sucesfull build you can find the **nano Framework** image files in the *build* directory. Those are:
- nanoFramework.bin (raw binary format)
- nanoFramework.hex (Intel hex format)
- nanoFramework.lst (source code intermixed with disassembly)
- nanoFramework.map (image map) 
