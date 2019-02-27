REM Starts OpenOCD - designed to be called from GDB, itself started from Visual Studio launch.vs.json
REM If already running it will fail and exit, leaving the existing one to handle GDB requests
REM Template allows for multiple target interfaces and boards - add new ones as required
REM DAV 26FEB19
setlocal
set cmd=C:/Esp32_Tools/openocd-esp32/bin/openocd.exe -s C:/Esp32_Tools/openocd-esp32/share/openocd/scripts

if %1.==. goto ESP32_JLINK
call :%1
if %errorlevel% equ 1 goto :lblerror
goto end

REM Start OpenOCD
:start
start %cmd% %iface%
goto :eof

:end
endlocal
exit

:lblerror
mshta javascript:alert("Error: Unknown label %1");close();
goto end

REM Add additional interface/board combinations here, and add the label to the launch.vs.json file

:ESP32_JLINK
set iface=-f interface/jlink.cfg -f target/esp32.cfg -c "adapter_khz 3000"
goto start

:ESP32_OLIMEX
set iface=-f interface/ftdi/olimex-arm-usb-ocd-h.cfg -f target/esp32.cfg -c "adapter_khz 3000"
goto start

:ESP32_WROVER
set iface=-f interface/ftdi/esp32_devkitj_v1.cfg -f board/esp-wroom-32.cfg 
goto start

:STM32_STLINK
set cmd=C:/nanoFramework_Tools/Tools/openocd/bin/openocd.exe -s C:/nanoFramework_Tools/Tools/openocd/bin/scripts
set iface=-f interface/stlink-v2-1.cfg -f board/stm32f7discovery.cfg
goto start




