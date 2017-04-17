# Tweaking cmake-variants.TEMPLATE.json

## Table of contents ##

- [What is it](#what-is-it)
- [How to use it](#how-to-use-it)
- [Brief description](#brief-description)
- [Content explained](#content-explained)
- [Working example](#working-example)

**About this document**

This document describes how to use and modify the **cmake-variants.TEMPLATE.json** file to suit your needs.

_Note : this first revision of the document is focusing on using the ChibiOS RTOS. Other RTOSes will follow._

## What is it

This file is a template containing a minimal set of configurations examples needed to build nanoFramework for you board.

Its content describes what kind of build you will get, which toolchain(s) you will use, which MCU is on the board, and some other options that will be described later in this document.

## How to use it

First, you have to either rename it or copy it to **cmake-variants.json**. Then, you will have to modify the different settings to match your environment.


## Brief description

There are two sets of parameters that need to be present in this file :
- the build type : debug or release, for example
- the board's environment : MCU, toolchain, RTOS

You should not have to modify the *BuildType* section unless you have good reasons to do so. However, the *Linkage* section is the one you will have to take care of.

For each *board* you want a build, you will have to create a dedicated section in this *Linkage* area, describing precisely the MCU, the toolchain, the build type and the RTOS you will use.

## Content explained
Now, we will explain each line of the *linkage* section. A text in **bold** is an information that you will have to provide.

**_Note : Mind the forward slash ('/') in paths strings !_**

- "**OPTION1_NAME_HERE**"
	- Replace this text with a distinctive option name. e.g. *"STM32F429_Disco"*
- "oneWordSummary$": "**<summary-here>**"
	- Replace the *<summary-here>* text with one word describing shortly your board. e.g. "F429Disco"
- "description$": "**<description-here>**"
	- This is a more complete description of the configuration
- "TOOLCHAIN_PREFIX" : "**<path-to-gcc-toolchain-mind-the-forward-slash>**"
	- This is the path to your gcc toolchain compiler. Use forward slashes and do not provide executable name here
- "TARGET_CHIP" : "**<target-chip-with-official-vendor-reference>**"
	- For STM32 MCUs only : represents the complete official reference of the MCU
- "TARGET_SERIES" : "**STM32F7xx**"
	- For STM32 MCUs only : represents the target series (STM32F4XX, STM32L4XX, and so on)
- "USE_FPU" : "**TRUE**"
	- TRUE or FALSE : Enables or disables the use of the FPU unit, if present.
- "RTOS" : "**<one-of-valid-rtos-options>**"
	- Defines the RTOS that will be used to build nanoFramework. It can be CHIBIOS, FREERTOS or MBEDOS. Currently, only CHIBIOS is supported.
- "CHIBIOS_SOURCE" : "**<path-to-chibios-source-mind-the-forward-slash>**"
	- Path to an optionnal local installation of ChibiOS source files. If no path is given, then CMake will download the sources from the official repository when needed
- "CHIBIOS_VERSION" : "**<N.N.N>**"
	- Version of the local/distant distribution of ChibiOS that will be used
- "CHIBIOS_BOARD" : "**<valid-chibios-board-name-from-boards-collection>**"
	- Name of your board, chosen from the available boards collection that can be found in the \os\hal\boards folder of the ChibiOS installation (or distant repository)
- "FREERTOS_VERSION" : "**<N.N.N>**"
	- Version of the local/distant distribution of FREERTOS that will be used
- "FREERTOS_SOURCE" : "**<path-to-freertos-source-mind-the-forward-slash>**"
	- Path to an optionnal local installation of FREERTOS source files. If no path is given, then CMake will download the sources from the official repository when needed
- "MBEDOS_SOURCE" : "**<path-to-mbedos-source-mind-the-forward-slash>**"
	- Path to an optionnal local installation of MBEDOS source files. If no path is given, then CMake will download the sources from the official repository when needed
- "MBED_TARGET" : "**<valid-target-name-from-targets-json-file>**"
	- ????

## Working example
The followinf linkage section is a real example used to build nanoFramework for the MBN Quail board. It is using the minimal mandatory informations :

```
"MBNQUAIL":
	{
		"oneWordSummary$": "QUAIL",
        "description$": "MBN Quail",
        "settings":
		{
            "TOOLCHAIN_PREFIX" : "C:/Program Files (x86)/GNU Tools ARM Embedded/5.4 2016q3",
            "TARGET_CHIP" : "STM32F427VIT",
			"TARGET_SERIES" : "STM32F4xx",
            "USE_FPU" : "TRUE",
            "RTOS" : "CHIBIOS", 
            "CHIBIOS_VERSION" : "16.1.7",
            "CHIBIOS_SOURCE" : "C:/dev/ChibiOS_16.1.7",
            "CHIBIOS_BOARD" : "MBN_QUAIL"
        },
        "buildType": "Debug"
      },
```
	  
