REM Starts OpenOCD - designed to be called from GDB, itself started from Visual Studio launch.vs.json
REM If already running it will fail and exit, leaving the existing one to handle GDB requests
REM Should be easy enough to expand to take a startup parameter, so we can use one file for multiple targets/configurations
REM DAV 25FEB19
start C:/Esp32_Tools/openocd-esp32/bin/openocd.exe -s C:/Esp32_Tools/openocd-esp32/share/openocd/scripts -f interface/jlink.cfg -f target/esp32.cfg -c "adapter_khz 3000"
