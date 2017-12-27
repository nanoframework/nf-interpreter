
# Building **nanoframework** for **Espressif ESP32**

To build the NanoClr for Esp32 on your system you will need the latest nanoframework/nf-interpreter and the Espressif ESP-IDF installed.


## Installing the ESP32 toolchain and IDF

Using the ESP-IDF programming Guide follow the "Get Started" section to setup your environment, ESP32 toolchain and get the latest ESP-IDF repository.
[ESP-IDF programming Guide](https://esp-idf.readthedocs.io/en/latest/get-started/index.html)

Use the default location of ~/esp for ESP-IDF

### Windows 

  - Install MSYS2 environment including latest toolchain
  - Get ESP-IDF into ~/esp
  - Set up the IDF_PATH in your MSYS2 user profile

### Linux
  - Install toolchain
  - Get ESP-IDF
  - Set up the IDF_PATH in your user profile
 

Build the Get-started Blink sample in situ to provide the pre built IDF libraries for Nanoframwork build


	cd $HOME/esp/esp-idf/examples/Get-Started/blink
	make 

For *Windows* you will need to start a MSYS2 command prompt.  C:\msys32\mingw32.exe


## Setting up Visual Code

Using the cmake-variants.TEMPLATE_ESP32.json copy to your cmake-variants.json
Fill in the paths to the ESP-IDF and the TOOLCHAIN_PREFIX

	For Windows with a default install for MSYS2
		"TOOLCHAIN_PREFIX" : "C:/msys32/opt/xtensa-esp32-elf"
		"ESP32_IDF_PATH"   : "C:/msys32/home/<username>/esp/esp-idf"

	For Linux installs
		"TOOLCHAIN_PREFIX" : "/opt/xtensa-esp32-elf"
		"ESP32_IDF_PATH"   : "/home/<username>/esp/esp-idf"

	Where <username> is the name of the home directory where the ESP-IDF is installed.

### Flashing code to ESP32 board

The .vscode/task.TEMPLATE provides tasks for flashing the nanoClr to the ESP32 boards.

	Using the .vscode/tasks.TEMPLATE_ESP32.json copy to or update your existing   .vscode/tasks.json
	In the tasks.json is a task for flashing the Nanoclr, ESP bootloader and partition table to the ESP32 board. If you have more then
	one board type then have a task for each board with the correct COM port name filled in and the paths to the ESP-IDF folder 
	

### Launching Nanoclr in a debug environment ( .vscode/launch.TEMPLATE-ESP32.json )

For launching the NanoClr in a debug enviroment using openocd and gdb. 

From Visual Code on Windows the openocd.exe in the MSYS2 environment doesn't seem to run. For testing on windows I used the openocd port that comes with Sysprogs.com VisualGDB for Esp32. Maybe someone will know how to call the openocd in the espressif IDF so it works from Windows. The openocd config files refernced in the template file are for the Sysprogs port and are different names in the Espressif openocd port.  

Depending on what board you are using will depend on the Jtag device used. 

  * Espressif DevKitc a external Jtag is required such as Olimex USB-OCD-H
  * Espressif Wrover the jtag is built in ( I had some Windows Blue screens from the drivers using this board, probably version of drivers )

Set up the .vscode/launch.json using the .vscode/launch.TEMPLATE-ESP32.json template

	"miDebuggerPath" :  Path to GDB executable ( C:\msys32\opt\xtensa-esp32-elf\bin\xtensa-esp32-elf-gdb.exe )
	"debugServerPath":  Path to openocd executable ( C:/SysGCC/esp32/esp32-bsp/OpenOCD/bin/openocd.exe )
	"debugServerArgs":  Path to openocd scripts directory ( C:/SysGCC/esp32/esp32-bsp/OpenOCD/share/openocd/scripts/ )

### Notes on Jtag debugging on ESP32
As code is dynamically loaded unless the method has an IRAM_ATTR attribute any breakpoints set up at the start will cause an error when you try to start. 
When launched the debugger will normally stop at the main task. Its not possible to set a break point on code that is not yet loaded so either step into the code down to a point that it is loaded or temporarly set the method you want to debug with the IRAM_ATTR attribute.

## Building from the command prompt

If you are building from the command prompt, just go to your build directory and run CMake from there with the appropriate parameters. 
The following is an example on Windows:

cmake \
-DTOOLCHAIN_PREFIX="C:/msys32/opt/xtensa-esp32-elf" \
-DTARGET_SERIES=ESP32 \
-DRTOS="FREERTOS" \
-DESP32_IDF_PATH="C:/msys32/home/<username>/esp/esp-idf" \
-DNF_FEATURE_DEBUGGER=TRUE \
-DAPI_Windows.Devices.Gpio=ON \
-DAPI_Windows.Devices.Spi=ON \
-DAPI_Windows.Devices.I2c=ON \
-G "NMake Makefiles" ../ 

Set <username> to correct value for your system.






	





