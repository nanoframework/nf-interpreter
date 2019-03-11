@echo off
REM BuildNF - Build nanoframework for various targets
REM Pass in a name for the build, also used for the subdirectory (nf-interpreter\build\name)
REM Label must appear, and be set for build options, see examples
REM (A spaghetti code example by DAV 5MAR19!)
REM BEWARE of trailing whitespace after line continuation characer ^ - they will BREAK things!
setlocal

:: List of available targets - keep in sync with defined configurations below!
set targets=STM32Disco STM32Nucleo ESP32_S ESP32_L STM32Disco_yaml STM32Nucleo_yaml

:: default target
set target=help
if %1.==. goto :check_input

:check_opts
if /I %1==-n set nobuild=-n& shift /1 & goto :check_opts
if /I %1==-d set rmbuild=-d& shift /1 & goto :check_opts

if %1.==. goto :check_input
set target=%1

:check_input
if /I %target%==list goto :list
if /I %target%==help goto :help
if /I %target%==all goto :all

for %%G in (%targets%) do (if /I "%target%"=="%%G" goto :match)
:nomatch
echo:
echo ============ Unknown target %target%
goto :help

:match
echo We have a match with: %target%
::goto :eof

:set_path
set _mypath=%~dp0
call :setshortpath %_mypath%
pushd %shortpath%
if [%rmbuild%]==[-d] rd /Q /S %shortpath%build\%target%
md %shortpath%build\%target% 2> nul
cd %shortpath%build\%target%
echo Building target %target% in %shortpath%build\%target%

set bt=%target:~0,3%
echo Build type = %bt%

if /I NOT %bt%==STM goto :try_esp
	REM Check if we have ChibiOS source set in environment.
	if NOT %CHIBIOS_SOURCE%.==. goto :have_chibios
	set CHIBIOS_SOURCE=""
	REM Hopefully set in environment? Else set here (can be ""). Here we try in the same directory as the nf-interpreter sources
	if exist %_mypath%..\ChibiOS set CHIBIOS_SOURCE="%_mypath%..\ChibiOS"
	:have_chibios
	if %CHIBIOS_SOURCE%=="" echo Local Chibios source not set in environment or at %_mypath%..\ChibiOS
	echo set up for ChibiOS
	set stm_vars=-DTOOLCHAIN_PREFIX:PATH=%GNU_GCC_TOOLCHAIN_PATH% -DCHIBIOS_SOURCE:PATH=%CHIBIOS_SOURCE%
:try_esp

if /I %bt%==ESP (
	REM Check if we have Espressif IDF? We could test for installed tools and fail if not found...
	echo set up for Espressif IDF
	REM We are using environment variables here - we could use hardcoded "C:/ESP32_TOOLS" etc instead
	REM Set common vars for ESP32 builds
	set esp_vars=-DEXECUTABLE_OUTPUT_PATH:PATH=%shortpath%build\%target% -DTARGET_SERIES:STRING=ESP32 -DRTOS:STRING=FREERTOS_ESP32 -DESP32_IDF_PATH:PATH=%ESP32_IDF_PATH% -DESP32_LIBS_PATH:PATH=%ESP32_LIBS_PATH%
)
::goto :eof
call :%target%
if %errorlevel% equ 1 goto :lblerror
goto end

REM Start Ninja build (optional, comment out if not required)
:build
if %errorlevel% neq 0 goto :configerror
echo =============== Cmake configure successful ===============
REM Add ninja.bat to build directory to assist command-line build on short path
pushd %shortpath%build\%target%
echo >ninja.bat @echo off
echo >>ninja.bat REM Run command in short-path version of this directory
echo >>ninja.bat setlocal
echo >>ninja.bat pushd %shortpath%build\%target%
echo >>ninja.bat ninja.exe
echo >>ninja.bat popd
popd
if [%nobuild%]==[-n] goto :endsub
echo =============== Commencing Ninja build     ===============
cd %shortpath%build\%target%
ninja.exe
if %errorlevel% neq 0 goto :builderror
goto :endsub

:endsub
::exit
popd
goto :eof

:end
:: Clear %errorlevel%
call set _a=a > null
endlocal
popd
::exit
goto :eof

:: ======= Start Build Definitions =======

:: === STM32 Discovery
:STM32Disco
cmake %stm_vars%^
 -DCHIBIOS_BOARD:STRING=ST_STM32F769I_DISCOVERY^
 -DTARGET_SERIES:STRING=STM32F7xx^
 -DNF_FEATURE_HAS_CONFIG_BLOCK:BOOL=TRUE^
 -DNF_FEATURE_DEBUGGER:BOOL=TRUE^
 -DNF_FEATURE_RTC:BOOL=ON^
 -DNF_FEATURE_HAS_SDCARD:BOOL=ON^
 -DNF_FEATURE_HAS_USB_MSD:BOOL=OFF^
 -DAPI_Hardware.Stm32:BOOL=ON^
 -DAPI_System.Math:BOOL=OFF^
 -DAPI_System.Net:BOOL=OFF^
 -DAPI_Windows.Devices.Adc:BOOL=ON^
 -DAPI_Windows.Devices.Gpio:BOOL=ON^
 -DAPI_Windows.Devices.I2c:BOOL=ON^
 -DAPI_Windows.Devices.Pwm:BOOL=ON^
 -DAPI_Windows.Devices.SerialCommunication:BOOL=ON^
 -DAPI_Windows.Devices.Spi:BOOL=ON^
 -DAPI_Windows.Networking.Sockets:BOOL=OFF^
 -DAPI_nanoFramework.Devices.Can:BOOL=OFF^
 -DAPI_nanoFramework.Devices.OneWire:BOOL=OFF^
 -DAPI_nanoFramework.Networking.Sntp:BOOL=OFF^
 -G Ninja %shortpath%
 
goto :build

:: === STM32 Nucleo
:STM32Nucleo
cmake %stm_vars%^
 -DCHIBIOS_BOARD:STRING=ST_NUCLEO64_F091RC^
 -DTARGET_SERIES:STRING=STM32F0xx^
 -DNF_FEATURE_HAS_CONFIG_BLOCK:BOOL=TRUE^
 -DNF_FEATURE_DEBUGGER:BOOL=TRUE^
 -DAPI_Windows.Devices.Gpio:BOOL=ON^
 -DNF_FEATURE_RTC:BOOL=ON^
 -G Ninja %shortpath%

goto :build


:: === ESP32 WROOM32 - Small Example
:ESP32_S
cmake %esp_vars%%^
 -DESP32_BOARD:STRING=ESP32_WROOM_32^
 -DBUILD_VERSION:STRING="0.9.99.999"^
 -DUSE_NETWORKING_OPTION:BOOL=TRUE^
 -DNF_FEATURE_HAS_CONFIG_BLOCK:BOOL=TRUE^
 -DNF_FEATURE_DEBUGGER:BOOL=TRUE^
 -DAPI_Windows.Devices.Gpio:BOOL=ON^
 -DNF_FEATURE_RTC=ON^
 -DNF_NETWORKING_SNTP=OFF^
 -G Ninja %shortpath%
 
goto :build

:: === ESP32 WROOM32 - Large Example
:ESP32_L
cmake %esp_vars%%^
 -DESP32_BOARD:STRING=ESP32_WROOM_32^
 -DBUILD_VERSION:STRING="0.9.99.999"^
 -DUSE_NETWORKING_OPTION:BOOL=TRUE^
 -DNF_FEATURE_HAS_CONFIG_BLOCK:BOOL=TRUE^
 -DNF_FEATURE_DEBUGGER:BOOL=TRUE^
 -DAPI_Windows.Devices.Gpio=ON^
 -DNF_FEATURE_RTC=ON^
 -DNF_NETWORKING_SNTP=OFF^
 -DAPI_Hardware.Esp32:BOOL=ON^
 -DAPI_Hardware.Stm32:BOOL=OFF^
 -DAPI_System.Net:BOOL=ON^
 -DAPI_Windows.Devices.Adc:BOOL=ON^
 -DAPI_Windows.Devices.Gpio:BOOL=ON^
 -DAPI_Windows.Devices.I2c:BOOL=ON^
 -DAPI_Windows.Devices.Pwm:BOOL=ON^
 -DAPI_Windows.Devices.SerialCommunication:BOOL=ON^
 -DAPI_Windows.Devices.Spi:BOOL=ON^
 -DAPI_Windows.Networking.Sockets:BOOL=OFF^
 -DAPI_Windows.Storage:BOOL=OFF^
 -DAPI_Windows.Devices.Wifi:BOOL=ON^
 -DAPI_nanoFramework.Devices.Can:BOOL=OFF^
 -DAPI_nanoFramework.Devices.OneWire:BOOL=ON^
 -G Ninja %shortpath%
 
goto :build

:: === STM32 Discovery -yaml settings
:STM32Disco_yaml
cmake %stm_vars%^
 -DCHIBIOS_BOARD:STRING=ST_STM32F769I_DISCOVERY^
 -DTARGET_SERIES:STRING=STM32F7xx^
  DRTOS=CHIBIOS^
 -DSUPPORT_ANY_BASE_CONVERSION=ON^
 -DNF_FEATURE_DEBUGGER=ON -DSWO_OUTPUT=ON -DNF_FEATURE_RTC=ON -DNF_FEATURE_HAS_SDCARD=ON -DAPI_System.Math=ON -DAPI_Hardware.Stm32=ON -DNF_FEATURE_HAS_CONFIG_BLOCK=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON -DAPI_System.Net=ON -DNF_SECURITY_MBEDTLS=ON -DAPI_nanoFramework.Devices.OneWire=ON -DAPI_nanoFramework.Devices.Can=ON -DAPI_Windows.Storage=ON^
  -G Ninja %shortpath%

goto :build

 -DNF_FEATURE_HAS_CONFIG_BLOCK:BOOL=TRUE^
 -DNF_FEATURE_DEBUGGER:BOOL=TRUE^
 -DNF_FEATURE_RTC:BOOL=ON^
 -DNF_FEATURE_HAS_SDCARD:BOOL=ON^
 -DNF_FEATURE_HAS_USB_MSD:BOOL=OFF^
 -DAPI_Hardware.Stm32:BOOL=ON^
 -DAPI_System.Math:BOOL=OFF^
 -DAPI_System.Net:BOOL=OFF^
 -DAPI_Windows.Devices.Adc:BOOL=ON^
 -DAPI_Windows.Devices.Gpio:BOOL=ON^
 -DAPI_Windows.Devices.I2c:BOOL=ON^
 -DAPI_Windows.Devices.Pwm:BOOL=ON^
 -DAPI_Windows.Devices.SerialCommunication:BOOL=ON^
 -DAPI_Windows.Devices.Spi:BOOL=ON^
 -DAPI_Windows.Networking.Sockets:BOOL=OFF^
 -DAPI_nanoFramework.Devices.Can:BOOL=OFF^
 -DAPI_nanoFramework.Devices.OneWire:BOOL=OFF^
 -DAPI_nanoFramework.Networking.Sntp:BOOL=OFF^
 -G Ninja %shortpath%
 
goto :build

:: === STM32 Nucleo - yaml settings
:STM32Nucleo_yaml
rem set GNU_GCC_TOOLCHAIN_PATH=C:\GNU_Tools_ARM_Embedded\5-2016-q3-update
set stm_vars=-DTOOLCHAIN_PREFIX:PATH=%GNU_GCC_TOOLCHAIN_PATH%/../5-2016-q3-update -DCHIBIOS_SOURCE:PATH=%CHIBIOS_SOURCE%

cmake %stm_vars%^
 -DCHIBIOS_BOARD:STRING=ST_NUCLEO64_F091RC^
 -DTARGET_SERIES:STRING=STM32F0xx^
 -DRTOS=CHIBIOS -DNF_FEATURE_DEBUGGER=ON -DNF_FEATURE_RTC=ON -DUSE_RNG=OFF -DNF_PLATFORM_NO_CLR_TRACE=ON -DNF_CLR_NO_IL_INLINE=ON -DAPI_Hardware.Stm32=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON^
 -G Ninja %shortpath%

goto :build
      ST_NUCLEO64_F091RC:
        BoardName: ST_NUCLEO64_F091RC
        BuildOptions: -DTARGET_SERIES=STM32F0xx -DRTOS=CHIBIOS -DNF_FEATURE_DEBUGGER=ON -DNF_FEATURE_RTC=ON -DUSE_RNG=OFF -DNF_PLATFORM_NO_CLR_TRACE=ON -DNF_CLR_NO_IL_INLINE=ON -DAPI_Hardware.Stm32=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON
        GccArm_Version: 5-2016-q3-update
        NeedsDFU: false
:: ======= End Build Definitions =======

:: ============ Utility commands =====

:: === List available targets
:list
echo Available Targets:
for %%G in (%targets%) do echo %%G 
goto :eof

:: === Help
:help
echo:
echo Usage: %0 [opt] Target
echo:
echo opt = -d    Delete build directory before configuring (clean start, lose CMake Cache)
echo opt = -n    No ninja build - just configure with CMake
echo Target = all to build all targets
echo:
echo Command line tool to build nf-interpreter for specific target configuration
echo First 3 letters of target name define build type (eg STM or ESP)
echo Add targets as required
echo:
goto :list


:: === Build all targets
:all
for %%G in (%targets%) do call %0 %nobuild% %rmbuild% %%G
goto :eof

::============== Error handlers =======

:lblerror
REM mshta javascript:alert("Error: Unknown label %1");close();
echo:
echo ==========================================================
echo ==== No Build Definition for: %target%
echo ==========================================================
echo:
goto :end

:configerror
REM mshta javascript:alert("Error: Build Error %1");close();
echo:
echo ==========================================================
echo ==== Cmake configuration failed for %target%
echo ==========================================================
echo:
goto :end

:builderror
REM mshta javascript:alert("Error: Build Error %1");close();
echo:
echo ==========================================================
echo ==== Ninja build failed for %target%
echo ==========================================================
echo:
goto :end

REM  ==========================================================
REM Utility Functions
REM
REM Call to set a short path to try avoid overflowing 250 char file path limit
:setshortpath
set "mypath=%~1"
set shortpath=%mypath%
rem echo mypath = %_mypath%

REM Are we already on short path?
set t=%mypath:~0,3%
if /I %t%==%mypath% goto :haveshortpath

REM Not on short path -  is there already a subst for it?
FOR /F "tokens=1,3" %%G IN ('subst') DO (
	if /I "%%H\"=="%mypath%" (
		set sdg=%%G
		goto :foundsubst
	)
)
REM No subst short path found - create one
call :freedrive driveletter && goto :cont
echo ERROR: No free drive letter found, staying on original path
goto :haveshortpath

:cont
echo Found free drive letter: %driveletter%
pushd %mypath%
subst %driveletter% .
popd
set shortpath=%driveletter%\
echo Created new  subst for %mypath% on %driveletter%
echo You can remove it with subst %driveletter% /D
goto :haveshortpath

:foundsubst
set shortpath=%sdg:~0,3%
echo Using existing subst for %mypath% on %shortpath%
goto :haveshortpath

:haveshortpath
echo Using short path %shortpath% for %mypath%
REM So now we have a short path if it was possible
goto :eof

rem Finds a free drive letter.
rem
rem Parameters:
rem     %1 = Output variable name.
rem
rem Example:
rem     call :freedrive mydriveletter && goto :cont
rem     echo ERROR: No free drive letter found.
rem     goto :EOF
rem     :cont
rem     echo Found drive letter: %mydriveletter%
:freedrive
setlocal EnableDelayedExpansion
set exitcode=0
set "output_var=%~1"
for %%i in (B,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z) do (
    set "drive=%%i:"
    rem If 'subst' fails, the drive letter is already in use.
    rem This way we can even detect optical drives that have a drive
    rem letter but no media in them, a case that goes undetected when
    rem using 'if exist'.
	subst !drive! %SystemDrive%\ >nul
    if !errorlevel! == 0 (
        subst !drive! /d >nul
        goto :freedrive0
    )
)
set exitcode=1
set drive=
:freedrive0
endlocal & set "%output_var%=%drive%" & exit /b %exitcode%

:exit

:eof
