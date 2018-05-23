# How to Build, Flash and Debug the ESP32 nanoCLR on Windows using Visual Code


## Table of contents

- [Prerequisites](#prerequisites)
- [Setting up the build environment for ESP32](#Setting up the build environment for ESP32)
- [Set up Visual Code](#Set up Visual Code)
- [Build the nanoCLR](#Build the nanoCLR)
- [Flash nanoCLR to ESP32](#Flash nanoCLR to ESP32)
- [Start with a Hello World C# application](#Start with a Hello World C# application)
- [Debug the nanoCLR](#Debug the nanoCLR)

**About this document**

This document describes how to build the required images for **nanoFramework** for ESP32.
The build is based on CMake tool to ease the development in all major platforms.


## Prerequisites

You'll need:

- [Visual Studio Code](http://code.visualstudio.com/)
- [CMake](https://cmake.org/) (Minimum required version is 3.7)
- A build system for CMake to generate the build files to. 
  + If you have Visual Studio (full version) you can use the included NMake.
- [Ninja](https://github.com/ninja-build/ninja/releases/download/v1.8.2/ninja-win.zip). This is lightweight build system, designed for speed and it works on Windows and Linux machines. See [here](cmake/ninja-build.md) how to setup Ninja to build **nanoFramework**.
- [Python 3.6.5](https://www.python.org/ftp/python/3.6.5/python-3.6.5.exe) Required for uploading the nanoCLR to the ESP32.
- [OpenOCD](https://github.com/espressif/openocd-esp32/releases/download/v0.10.0-esp32-20180418/openocd-esp32-win32-0.10.0-esp32-20180418.zip) For on chip debugging of the nanoCLR


## Setting up the build environment for ESP32

To save time on building the nanoCLR and to avoid having to create a CMakeLists.txt project for the ESP32 IDF files, the ESP32 IDF libraries are prebuilt using the Esp32 Msys32 environment then used for linking in the CMake build of nanoCLR.
This has already been done and the libraries can be just be downloaded.

1. Create a directory structure such as the following:

   - `C:\Esp32_Tools`
   - `C:\Esp32_Tools\libs`

2. Download the pre-built libs zip from [here](https://bintray.com/nfbot/internal-build-tools/download_file?file_path=IDF_libs.zip)
and extract it into `C:\Esp32_Tools\libs`.

3. Download the v3.0rc1 IDF source zip file from [here](https://github.com/espressif/esp-idf/releases/download/v3.0-rc1/esp-idf-v3.0-rc1.zip) and extract it into `C:\Esp32_Tools` so you get `C:\ESP32_Tools\esp-idf-v3.0-rc1\components` etc.

4. Download the Esp32 toolchain from [here](https://dl.espressif.com/dl/xtensa-esp32-elf-win32-1.22.0-75-gbaf03c2-5.2.0.zip) and extract it into `C:\Esp32_Tools` so you get `C:\Esp32_Tools\xtensa-esp32-elf`.

5. Extract OpenOCD into `C:\Esp32_Tools` so you get `C:\Esp32_Tools\openocd-esp32`.


## **nanoFramework** GitHub repo

If you intend to change the nanoCLR for ESP32 and create Pull Requests then you will need to fork the [nanoFramework/nf-interpreter](https://github.com/nanoFramework/nf-interpreter) to your own GitHub repo and clone the forked GitHub repo to your Windows system using an Git client such as the [GitHub Desktop application](https://desktop.github.com/).

You should use the _develop_ branch for mainstream development or the _develop-network_ branch to work with the networking features which is currently a work in progress.


## Set up CMake

1. Download the latest stable version from [here](https://cmake.org/download/) and install it.


## Set up Visual Code

1. Install the extensions:

    - "C/C++" extension by Microsoft.
    - "CMake" language support for Visual Studio Code by twxs.
    - "CMake tools" Extended CMake support in Visual Studio code by vector-of-bool (Must be version 0.10.4 as version 0.11 is currently not supported).
            
        To install an older version of a VS extension you have to hack it like this:
        - In Visual Studio Code goto settings -> settings and set "extensions.autoUpdate" to false.
        - Download version 0.10.4 from [here](https://ms-vscode.gallery.vsassets.io/_apis/public/gallery/publisher/vector-of-bool/extension/cmake-tools/0.10.4/assetbyname/Microsoft.VisualStudio.Services.VSIXPackage).
        - Rename the download into a file with ".vsix" extension.
        - In Visual Studio Code goto Extension click the "..." and select "Install form VSIX" and select the file that you've just renamed.

2. Set up the `CMake-variants.json` in root directory of your local nanoFramework/nf-interpreter clone.
    
    There is a template file called `cmake-variants.TEMPLATE.json` that can be renamed and set up to the following:

    Be aware of the forward slashes in the paths. The TOOLCHAIN_PREFIX should be set to the directory where the xtensa-esp32-elf is the subdirectory. If you follow the paths in this guide then you can use this as it is:
 
```
  "linkage": { 
      "Esp32_nanoCLR": { 
        "oneWordSummary$": "nanoCLR", 
        "description$": "Build nanoCLR for ESP32 environment", 
        "settings": { 
            "TOOLCHAIN_PREFIX" : "C:/ESP32_Tools", 
            "TARGET_SERIES" : "ESP32", 
            "RTOS" : "FREERTOS", 
            "ESP32_IDF_PATH" : "C:/ESP32_Tools/esp-idf-v3.0-rc1", 
            "ESP32_LIBS_PATH" : "C:/ESP32_Tools/libs", 
            "NF_BUILD_RTM" : "OFF", 
            "NF_WP_TRACE_ERRORS" : "ON", 
            "NF_WP_TRACE_HEADERS" : "ON", 
            "NF_WP_TRACE_STATE" : "ON", 
            "NF_WP_TRACE_NODATA" : "OFF", 
            "NF_WP_TRACE_ALL" : "OFF", 
            "NF_WP_IMPLEMENTS_CRC32" : "ON", 
            "NF_FEATURE_DEBUGGER" : "ON", 
            "NF_FEATURE_RTC" : "ON", 
            "NF_FEATURE_USE_APPDOMAINS" : "OFF", 
            "NF_FEATURE_USE_NETWORKING" : "OFF", 
            "API_Windows.Devices.Gpio" : "ON", 
            "API_Windows.Devices.Spi" : "ON", 
            "API_Windows.Devices.I2c" : "ON", 
            "API_Windows.Devices.Adc" : "ON", 
            "API_Windows.Devices.Pwm" : "ON", 
            "API_Windows.Devices.Wifi": "ON", 
            "API_System.Net" : "OFF", 
            "API_Windows.Networking.Sockets" : "OFF" 
          } 
      }, 
```

3. Create a `./.vscode/tasks.json` from `/.vscode/tasks.TEMPLATE-ESP32.json`.

    For flashing the nanoCLR into the ESP32 or to erase the flash of the ESP32 you will need this in the `tasks.json` file.
    Adjust the COM port and the path to nanoFramework build directory (**!!mind the forward slashes!!**) to your needs.

```
{ 
    "version": "2.0.0", 
	    "tasks": [ 
        { 
            "taskName": "Flash nanoCLR COM6", 
            "type": "shell", 
            "command": "python C:/ESP32_Tools/esp-idf-v3.0-rc1/components/esptool_py/esptool/esptool.py --chip esp32 --port \"COM6\" --baud 115200 --before \"default_reset\" --after \"hard_reset\" write_flash -z --flash_mode \"dio\" --flash_freq \"40m\" --flash_size detect 0x1000 C:/ESP32_Tools/libs/bootloader.bin 0x10000 <path-to-nanoFramework-build-directory-mind-the-forward-slashes>/nanoCLR.bin 0x8000 <path-to-nanoFramework-build-directory-mind-the-forward-slashes>/partitions_4mb.bin", 
            "presentation": { 
                "reveal": "always", 
                "panel": "shared" 
            }, 
            "problemMatcher": [] 
        }, 
        { 
            "taskName": "Erase flash COM6", 
            "type": "shell", 
            "command": "python C:/ESP32_Tools/esp-idf-v3.0-rc1/components/esptool_py/esptool/esptool.py --chip esp32 --port \"COM6\" --baud 115200 --before \"default_reset\" --after \"hard_reset\" erase_flash", 
            "presentation": { 
                "reveal": "always", 
                "panel": "shared" 
            }, 
            "problemMatcher": [] 
        } 
    ] 
} 
```


## Set up Ninja

1. Extract the exe into `C:\Esp32_Tools` and add the `C:\Esp32_Tools` directory to your path variable.


## Set up Python

1. Install it and then install the serial driver for python from the command line: 
```
python -m pip install pyserial
```


## Build nanoCLR

1. To enter a command into Visual Studio Code use the key combination Ctrl+Shift+P.

2. Enter the command 
    ```
    CMake: Set build type
    ```
    and set it to the Debug build type. Currently only this build type is working.

3. Press F7, click on Build in the Status bar or enter the command 
    ```
    CMake: Build
    ```

4. If you get no error you will have in the build directory the files `nanoCLR.bin` and `partitions_4mb.bin`.

5. The third file that gets flashed into the ESP32 is the `C:/ESP32_Tools/libs/bootloader.bin`.


## Flash nanoCLR into ESP32

1. Connect your development board to the computer port that you've setup in `tasks.json`.

2. Bring your board into download mode by holding down the GPIO0 pin to GND or holding down the respective button during power up.

3. In Visual Studio Code enter the command 
    ```
    Tasks: Run task
    ```
    and if you flash the board for the first time 

    ```
    Erase flash
    ```
    and then 
    ```
    Flash nanoCLR
    ```


## Start with a 'Hello World' C# application

1. Watch the video tutorial [here](https://youtu.be/iZdN2GmefXI) and follow the step that should be done in Visual Studio 2017 Community Editon. Skip the steps that describing uploading the nanoCLR into the STM32 Nucleo board.


## Debugging nanoCLR

If you want to debug the nanoCLR on the ESP32 chip you can use the Olimex ARM-USB-OCD-H JTAG debugging adapter. The following setup is specific for the Olimex ARM-USB-OCD-H but you can use other configuration files to get it working with other JTAG adapters.

Create a `./.vscode/launch.json` from `/.vscode/launch.TEMPLATE-ESP32.json`.

Edit the file and adjust the absolute path to the build folder (**!!mind the forward slashes!!**) to your needs.

```
{
	"name": "ESP32 nanoCLR - Olimex ARM-USB-OCD-H",
	"type": "cppdbg",
	"request": "launch",
	"MIMode": "gdb",
	"miDebuggerPath": "C:/Esp32_Tools/xtensa-esp32-elf/bin/xtensa-esp32-elf-gdb.exe",
	"stopAtEntry":true,
	"program": "<absolute-path-to-the-build-folder-mind-the-forward-slashes>/targets/FreeRTOS/ESP32_DevKitC/nanoCLR.elf",

	"setupCommands": [
		{"text": "set logging on"},
		{"text": "target extended-remote localhost:3333"},
		{"text": "file <absolute-path-to-the-build-folder-mind-the-forward-slashes>/targets/FreeRTOS/ESP32_DevKitC/nanoCLR.elf"},
		{"text": "monitor reset halt"},
		{"text": "thb app_main"},
		{"text": "x $a1=0"}
	],

	"launchCompleteCommand": "exec-run",
	"debugServerPath": "C:/Esp32_Tools/openocd-esp32/bin/openocd.exe",
	"debugServerArgs": "-s \"C:/Esp32_Tools/openocd-esp32/share/openocd/scripts/\" -f interface/ftdi/olimex-arm-usb-ocd-h.cfg -f target/esp32.cfg -c \"adapter_khz 3000\" " ,
	"serverStarted": "Info : .*Tensilica.*0x1.",
	"filterStderr": true,
	"externalConsole": true,
	"cwd": "${cwd}",

	"logging": {
		"trace": true,
		"traceResponse": true,
		"engineLogging": true,
		"programOutput": true,
		"exceptions": true,
		"moduleLoad": true
	}
},
```

You can now debug nanoCLR on the ESP32 by pressing F5 in Visual Studio Code.


## Notes on JTAG debugging on ESP32

The Esp32 only has 2 hardware breakpoints.

As code is dynamically loaded unless the method has an `IRAM_ATTR` attribute any breakpoints set up at the start will cause an error when you try to debug (Unable to set breakpoint). When launched the debugger will normally stop at the main task. Its not possible to set a break point on code that is not yet loaded so either step down to a point that it is loaded or temporarily set the method with the IRAM_ATTR attribute.

For more information on JTAG debugging see [Espressif documentaion](http://esp-idf.readthedocs.io/en/latest/api-guides/jtag-debugging/).
