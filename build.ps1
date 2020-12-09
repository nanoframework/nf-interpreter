# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

[CmdletBinding(SupportsShouldProcess = $true)]
param (
    [Parameter(HelpMessage = "Please enter the target name [e.g. ESP32_WROOM_32 or ST_STM32F429I_DISCOVERY or NETDUINO3_WIFI].")][string]$TargetBoard,
    [Parameter(HelpMessage = "Enter the path to the folder where the tools should be installed.")][string]$Path = "C:\nftools"
)

# create build folder
mkdir -Force "$PSScriptRoot\build" | Out-Null

# move to build folder
Set-Location "build" | Out-Null

"Running CMake build preparation..." | Write-Host -ForegroundColor White

If ($TargetBoard -eq "MBN_QUAIL" -or 
    $TargetBoard -eq "NETDUINO3_WIFI" -or
    $TargetBoard -eq "ORGPAL_PALTHREE" -or
    $TargetBoard -eq "ST_NUCLEO64_F091RC" -or
    $TargetBoard -eq "ST_STM32F429I_DISCOVERY" -or
    $TargetBoard -eq "ST_STM32F769I_DISCOVERY") {

    # run the install tools to have the build vars filled
    Invoke-Expression "$PSScriptRoot\install-scripts\install-stm32-tools.ps1 -Path $Path"

    If ( $TargetBoard -eq "MBN_QUAIL" ) {

        $cmakeOptions = " -DTOOL_HEX2DFU_PREFIX=$:env:HEX2DFU_PATH -DTARGET_SERIES=STM32F4xx -DRTOS=CHIBIOS -DSUPPORT_ANY_BASE_CONVERSION=ON -DNF_FEATURE_DEBUGGER=ON -DNF_FEATURE_RTC=ON -DAPI_System.Math=ON -DAPI_Hardware.Stm32=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_nanoFramework.Devices.OneWire=ON -DAPI_nanoFramework.ResourceManager=ON -DAPI_nanoFramework.System.Collections=ON -DAPI_nanoFramework.System.Text=ON"
    }
    elseif ($TargetBoard -eq "NETDUINO3_WIFI") {

        $cmakeOptions = " -DTOOL_HEX2DFU_PREFIX=$:env:HEX2DFU_PATH -DTARGET_SERIES=STM32F4xx -DRTOS=CHIBIOS -DSUPPORT_ANY_BASE_CONVERSION=ON -DNF_FEATURE_DEBUGGER=ON -DNF_FEATURE_RTC=ON -DNF_FEATURE_HAS_SDCARD=ON -DAPI_System.Math=ON -DAPI_Hardware.Stm32=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON -DAPI_System.Device.Dac=OFF -DAPI_nanoFramework.Devices.OneWire=ON -DAPI_Windows.Storage=ON -DAPI_nanoFramework.ResourceManager=ON -DAPI_nanoFramework.System.Collections=ON -DAPI_nanoFramework.System.Text=ON"
    }
    elseif ($TargetBoard -eq "ORGPAL_PALTHREE") {

        $cmakeOptions = " -DTOOL_HEX2DFU_PREFIX=$:env:HEX2DFU_PATH -DTARGET_SERIES=STM32F7xx -DRTOS=CHIBIOS -DCHIBIOS_CONTRIB_REQUIRED=ON -DSTM32_CUBE_PACKAGE_REQUIRED=ON -DSUPPORT_ANY_BASE_CONVERSION=ON -DNF_FEATURE_DEBUGGER=ON -DSWO_OUTPUT=ON -DNF_FEATURE_RTC=ON -DNF_FEATURE_HAS_USB_MSD=ON -DNF_FEATURE_HAS_SDCARD=ON -DNF_FEATURE_USE_SPIFFS=ON -DAPI_System.Math=ON -DAPI_Hardware.Stm32=ON -DNF_FEATURE_HAS_CONFIG_BLOCK=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON -DAPI_System.Device.Dac=ON -DAPI_System.Net=ON -DNF_SECURITY_MBEDTLS=ON -DAPI_Windows.Storage=ON -DAPI_nanoFramework.ResourceManager=ON -DAPI_nanoFramework.System.Collections=ON -DAPI_nanoFramework.System.Text=ON"
    }
    elseif ($TargetBoard -eq "ST_NUCLEO64_F091RC") {
        Write-Error "Build not supported because this target requires a different GCC toolchain version"
        
        Exit 1
    }
    elseif ($TargetBoard -eq "ST_STM32F429I_DISCOVERY") {

        $cmakeOptions = "-DTARGET_SERIES=STM32F4xx -DRTOS=CHIBIOS -DSUPPORT_ANY_BASE_CONVERSION=ON -DNF_FEATURE_DEBUGGER=ON -DSWO_OUTPUT=ON -DNF_FEATURE_RTC=ON -DAPI_System.Math=ON -DAPI_Hardware.Stm32=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON -DAPI_nanoFramework.Devices.OneWire=ON -DAPI_nanoFramework.Devices.Can=ON -DAPI_nanoFramework.ResourceManager=ON -DAPI_nanoFramework.System.Collections=ON -DAPI_nanoFramework.System.Text=ON"
    }
    elseif ($TargetBoard -eq "ST_STM32F769I_DISCOVERY") {

        $cmakeOptions = " -DTARGET_SERIES=STM32F7xx -DRTOS=CHIBIOS -DSUPPORT_ANY_BASE_CONVERSION=ON -DNF_FEATURE_DEBUGGER=ON -DSWO_OUTPUT=ON -DNF_FEATURE_RTC=ON -DNF_FEATURE_HAS_SDCARD=ON -DAPI_System.Math=ON -DAPI_Hardware.Stm32=ON -DNF_FEATURE_HAS_CONFIG_BLOCK=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON -DAPI_System.Device.Dac=ON -DAPI_System.Net=ON -DNF_SECURITY_MBEDTLS=ON -DAPI_nanoFramework.Devices.OneWire=ON -DAPI_nanoFramework.Devices.Can=ON -DAPI_Windows.Storage=ON -DAPI_nanoFramework.ResourceManager=ON -DAPI_nanoFramework.System.Collections=ON -DAPI_nanoFramework.System.Text=ON"
    }

    # CMake prep
    $cmakePrep = " -G Ninja -DCMAKE_TOOLCHAIN_FILE=""CMake/toolchain.arm-none-eabi.cmake"" -DTOOLCHAIN_PREFIX=""$env:GNU_GCC_TOOLCHAIN_PATH"" -DCMAKE_BUILD_TYPE=Debug -DBUILD_VERSION=9.9.99.99 -DTARGET_BOARD=$TargetBoard $cmakeOptions .."
}
elseif ($TargetBoard -eq "ESP32_WROOM_32") {

    # run the install tools
    Invoke-Expression "$PSScriptRoot\install-scripts\install-esp32-tools.ps1 -Path $Path"

    $cmakeOptions = " -DTARGET_SERIES=ESP32 -DRTOS=FREERTOS_ESP32 -DNF_FEATURE_DEBUGGER=ON -DNF_FEATURE_RTC=ON -DNF_FEATURE_HAS_CONFIG_BLOCK=ON -DNF_FEATURE_HAS_SDCARD=OFF -DAPI_System.Math=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON -DAPI_System.Net=ON -DAPI_Windows.Devices.Wifi=ON -DAPI_Windows.Storage=ON -DNF_SECURITY_MBEDTLS=ON -DAPI_Hardware.Esp32=ON -DSUPPORT_ANY_BASE_CONVERSION=ON -DAPI_nanoFramework.Devices.OneWire=ON -DAPI_nanoFramework.ResourceManager=ON -DAPI_nanoFramework.System.Collections=ON -DAPI_nanoFramework.System.Text=ON"

    # CMake prep
    $cmakePrep = " -G Ninja -DCMAKE_TOOLCHAIN_FILE=""CMake/toolchain.xtensa-esp32-elf.cmake"" -DTOOLCHAIN_PREFIX=""$env:ESP32_TOOLCHAIN_PATH"" -DESP32_IDF_PATH=""$env:ESP32_IDF_PATH"" -DESP32_LIBS_PATH=""$env:ESP32_LIBS_PATH"" -DCMAKE_BUILD_TYPE=Debug -DBUILD_VERSION=9.99.999 -DTARGET_BOARD=$TargetBoard $cmakeOptions .."
}
elseif ($TargetBoard -eq "TI_CC3220SF_LAUNCHXL") {
    # TODO    
    Write-Error "Build not supported becuase the build system for this target is being reworked"

    Exit 1
}
elseif ($TargetBoard -eq "TI_CC1352R1_LAUNCHXL") {

    # run the install tools, OK to reuse the STM32
    Invoke-Expression "$PSScriptRoot\install-scripts\install-stm32-tools.ps1 -Path $Path"

    If ( $TargetBoard -eq "TI_CC1352R1_LAUNCHXL" ) {
        $cmakeOptions = " -DTARGET_SERIES=CC13x2_26x2 -DRTOS=TI_SIMPLELINK -DSUPPORT_ANY_BASE_CONVERSION=OFF -DNF_FEATURE_DEBUGGER=ON -DNF_FEATURE_RTC=ON -DNF_FEATURE_WATCHDOG=OFF -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=OFF -DAPI_Windows.Devices.I2c=OFF -DAPI_Windows.Devices.Pwm=OFF -DAPI_Windows.Devices.SerialCommunication=OFF -DAPI_Windows.Devices.Adc=OFF -DAPI_nanoFramework.TI.EasyLink=ON"
    }

    # CMake prep
    $cmakePrep = " -G Ninja -DCMAKE_TOOLCHAIN_FILE=""CMake/toolchain.arm-none-eabi.cmake"" -DTOOLCHAIN_PREFIX=""$env:GNU_GCC_TOOLCHAIN_PATH"" -DCMAKE_BUILD_TYPE=Debug -DBUILD_VERSION=9.99.999 -DTARGET_BOARD=$TargetBoard $cmakeOptions .." 
}
elseif ($TargetBoard -eq "NXP_MIMXRT1060_EVK") {
    # TODO
    Write-Error "Build not supported becuase the build system for this target is being reworked"

    Exit 1
}
elseif ($TargetBoard -eq "GHI_FEZ_CERB40_NF" -or 
    $TargetBoard -eq "I2M_ELECTRON_NF" -or
    $TargetBoard -eq "I2M_OXYGEN_NF" -or
    $TargetBoard -eq "ST_NUCLEO64_F401RE_NF" -or
    $TargetBoard -eq "ST_NUCLEO64_F411RE_NF" -or
    $TargetBoard -eq "ST_STM32F411_DISCOVERY" -or
    $TargetBoard -eq "ST_NUCLEO144_F412ZG_NF" -or
    $TargetBoard -eq "ST_NUCLEO144_F746ZG" -or
    $TargetBoard -eq "ST_STM32F4_DISCOVERY" -or
    $TargetBoard -eq "ST_NUCLEO144_F439ZI") {

    # run the install tools, OK to reuse the STM32
    Invoke-Expression "$PSScriptRoot\install-scripts\install-stm32-tools.ps1 -Path $Path"

    # community targets with STM32
    If ($TargetBoard -eq "GHI_FEZ_CERB40_NF") {
        $cmakeOptions = " -DTARGET_SERIES=STM32F4xx -DNF_FEATURE_DEBUGGER=ON -DNF_FEATURE_RTC=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON"
    }
    elseif ($TargetBoard -eq "I2M_ELECTRON_NF") {
        $cmakeOptions = " -DTARGET_SERIES=STM32F4xx -DNF_FEATURE_DEBUGGER=ON -DUSE_RNG=OFF -DNF_FEATURE_RTC=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON"
    }
    elseif ($TargetBoard -eq "I2M_OXYGEN_NF") {
        $cmakeOptions = " -DTARGET_SERIES=STM32F4xx -DNF_FEATURE_DEBUGGER=ON -DUSE_RNG=OFF -DNF_FEATURE_RTC=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON"
    }
    elseif ($TargetBoard -eq "ST_NUCLEO64_F401RE_NF") {
        $cmakeOptions = " -DTARGET_SERIES=STM32F4xx -DNF_FEATURE_DEBUGGER=ON -DUSE_RNG=OFF -DSWO_OUTPUT=ON -DNF_FEATURE_RTC=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON -DAPI_nanoFramework.Devices.OneWire=ON"
    }
    elseif ($TargetBoard -eq "ST_NUCLEO64_F411RE_NF") {
        $cmakeOptions = " -DTARGET_SERIES=STM32F4xx -DNF_FEATURE_DEBUGGER=ON -DUSE_RNG=OFF -DSWO_OUTPUT=ON -DNF_FEATURE_RTC=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON -DAPI_nanoFramework.System.Collections=ON -DAPI_nanoFramework.ResourceManager=ON -DAPI_nanoFramework.System.Text=ON"
    }
    elseif ($TargetBoard -eq "ST_STM32F411_DISCOVERY") {
        $cmakeOptions = " -DTARGET_SERIES=STM32F4xx -DNF_FEATURE_DEBUGGER=ON -DUSE_RNG=OFF -DSWO_OUTPUT=ON -DNF_FEATURE_RTC=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON -DAPI_nanoFramework.System.Collections=ON -DAPI_nanoFramework.ResourceManager=ON -DAPI_nanoFramework.System.Text=ON"
    }
    elseif ($TargetBoard -eq "ST_NUCLEO144_F412ZG_NF") {
        $cmakeOptions = " -DTARGET_SERIES=STM32F4xx -DNF_FEATURE_DEBUGGER=ON -DSWO_OUTPUT=ON -DNF_FEATURE_RTC=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=OFF -DAPI_Windows.Devices.Adc=ON"
    }
    elseif ($TargetBoard -eq "ST_NUCLEO144_F746ZG") {
        $cmakeOptions = " -DTARGET_SERIES=STM32F7xx -DRTOS=CHIBIOS -DNF_FEATURE_DEBUGGER=ON -DSWO_OUTPUT=ON -DNF_FEATURE_RTC=ON -DNF_FEATURE_HAS_CONFIG_BLOCK=ON -DAPI_System.Math=ON -DAPI_Hardware.Stm32=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON -DAPI_System.Net=ON -DNF_SECURITY_MBEDTLS=ON -DAPI_nanoFramework.Devices.OneWire=ON"
    }
    elseif ($TargetBoard -eq "ST_STM32F4_DISCOVERY") {
        $cmakeOptions = " -DTARGET_SERIES=STM32F4xx -DRTOS=CHIBIOS -DSUPPORT_ANY_BASE_CONVERSION=ON -DNF_FEATURE_DEBUGGER=ON -DSWO_OUTPUT=ON -DNF_FEATURE_RTC=ON -DAPI_System.Math=ON -DAPI_Hardware.Stm32=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON -DAPI_nanoFramework.Devices.OneWire=ON -DAPI_nanoFramework.Devices.Can=ON"
    }
    elseif ($TargetBoard -eq "ST_NUCLEO144_F439ZI") {
        $cmakeOptions = " -DTARGET_SERIES=STM32F4xx -DRTOS=CHIBIOS -DNF_FEATURE_DEBUGGER=ON -DSWO_OUTPUT=ON -DNF_FEATURE_RTC=ON -DNF_FEATURE_HAS_CONFIG_BLOCK=ON -DAPI_System.Math=ON -DAPI_Hardware.Stm32=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON -DAPI_System.Net=ON -DNF_SECURITY_MBEDTLS=ON"
    }

    # CMake prep
    $cmakePrep = " -G Ninja -DCMAKE_TOOLCHAIN_FILE=""CMake/toolchain.arm-none-eabi.cmake"" -DTOOLCHAIN_PREFIX=""$env:GNU_GCC_TOOLCHAIN_PATH"" -DCMAKE_BUILD_TYPE=Debug -DBUILD_VERSION=9.9.99.99 -DTARGET_BOARD=$TargetBoard $cmakeOptions .."
}
elseif ($TargetBoard -eq "TI_CC1352P1_LAUNCHXL") {
    # community targets with TI CC13xx
    $cmakeOptions = " -DTARGET_SERIES=CC13x2_26x2 -DRTOS=TI_SIMPLELINK -DSUPPORT_ANY_BASE_CONVERSION=OFF -DNF_FEATURE_DEBUGGER=ON -DNF_FEATURE_RTC=ON -DNF_FEATURE_WATCHDOG=OFF -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=OFF -DAPI_Windows.Devices.I2c=OFF -DAPI_Windows.Devices.Pwm=OFF -DAPI_Windows.Devices.SerialCommunication=OFF -DAPI_Windows.Devices.Adc=OFF -DAPI_nanoFramework.TI.EasyLink=ON"

    # CMake prep
    $cmakePrep = " -G Ninja -DCMAKE_TOOLCHAIN_FILE=""CMake/toolchain.arm-none-eabi.cmake"" -DTOOLCHAIN_PREFIX=""$env:GNU_GCC_TOOLCHAIN_PATH"" -DCMAKE_BUILD_TYPE=Debug -DBUILD_VERSION=9.9.99.99 -DTARGET_BOARD=$TargetBoard $cmakeOptions .."
}
else {
    Write-Error "Unknown target name."
}

# get path to the cmake executable
$cmake = (Get-Command "cmake.exe" -ErrorAction SilentlyContinue)

# output CMake command
"Just in case you're curious on what the call to CMake is, here's the command line for preparation:" | Write-Host -ForegroundColor White
Write-Host "'cmake $cmakePrep'" | Write-Host -ForegroundColor White

# start a process with the call to CMake
# dev note: this seems to be the best approach to calling CMake from Powershell because of the complications with properly passing the arguments
# also CMake may write to stdErr and PSCore treats that as an error
$buildProcess = Start-Process -ErrorAction Continue -NoNewWindow -Wait -PassThru -FilePath $cmake.Path -ArgumentList $cmakePrep
$buildProcess.WaitForExit();
if ($buildProcess.ExitCode -ne 0) {
    "CMake preparation failed. Check the error message above." | Write-Host -ForegroundColor Red
    exit 1
}

# need to move up from the build directory
Set-Location ".." | Out-Null

# CMake build
$cmakeBuild = " --build build --target all --config Debug"

# output CMake command
"Just in case you're curious on what the call to CMake is, here's the command line for the build:" | Write-Host -ForegroundColor White
Write-Host "'cmake $cmakeBuild'" | Write-Host -ForegroundColor White

$buildProcess = Start-Process -ErrorAction Continue -NoNewWindow -Wait -PassThru -FilePath $cmake.Path -ArgumentList $cmakeBuild
$buildProcess.WaitForExit();
if ($buildProcess.ExitCode -ne 0) {
    "CMake build failed. Check the error message above." | Write-Host -ForegroundColor Red
    exit 1
}

"Build completed." | Write-Host -ForegroundColor Green
"Firmware images (nanoBooter or/and nanoCLR) available in 'build' folder." | Write-Host -ForegroundColor White

<#
.SYNOPSIS
    Performs the build of a nanoFramework target, installing all the required tools and libraries.
.DESCRIPTION
    Power Shell Script to build of a nanoFramework target, installing all the required tools and libraries. 
.PARAMETER Target
	The name of teh target to build [e.g. ESP32_WROOM_32 or ST_STM32F429I_DISCOVERY or NETDUINO3_WIFI].
.EXAMPLE
   .\build.ps1 -Target ST_STM32F429I_DISCOVERY
.NOTES
    The script installs all the required tools and libraries on default locations.
#>
