# Build the ESP32 IDF Libraries

Using the instruction of the espressif website
https://esp-idf.readthedocs.io/en/latest/get-started/windows-setup.html

Download the complete Msys2 enviroment and toolchain and unzip to c:\msys2

Download the required ESP IDF into the nanoClr build default location c:\Esp32_tools\esp-idf-v3.1
https://github.com/espressif/esp-idf/releases/download/v3.0/esp-idf-v3.1.zip

Set up your Windows enviroment with the IDF_PATH=c:\Esp32_tools\esp-idf-v3.1

Start Msys command shell C:\msys32\mingw32.exe
 cd /c/esp32_tools/esp-idf-v3.1/examples/get-started/blink/

 run:-
 make menuconfig

Change the following options:-
- Esp32 specific/Cpu frquency 240Mhz
- Esp32 specific/Initialize Task Watcho dog Timer in startup = OFF
- Esp32 specific/Main XTAL frequency = Autodetect
- LWIP SO RCVbuf
- Component config/Bluetooth enable

Save and Exit

Run *make* to build blink project

Exit msys2

*Copy the libraries to the Esp32_tools/lib-v3.1 directory*

Copy the nf-interpreter\targets\FreeRTOS\ESP32_DevKitC\CopyLibs.cmd to the  c:\esp32_tools\esp-idf-v3.1\examples\get-started\blink directory
Open windows command in same directory and run batch file.

This will create and copy all the libraries and bootloader.bin to the c:\esp32_tools\libs-v3.1 directory

copy the updated build/include/sdkconfig.h to the nf-interpreter\targets\FreeRTOS\ESP32_DevKitC directory

## Check build files

Check Appveyor.yml, install_esp32-libs.ps1, install_esp32-idf.ps1 have correct paths

 















