# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

[CmdletBinding(SupportsShouldProcess = $true)]
param (
    [Parameter(HelpMessage = "Please enter the target name [e.g. ESP32_WROOM_32 or ST_STM32F429I_DISCOVERY].")][string]$TargetBoard,
    [Parameter(HelpMessage = "Enter the path to the folder where the tools should be installed.")][string]$Path = "C:\nftools"
)

# create build folder
mkdir -Force "$PSScriptRoot\build" | Out-Null

# move to build folder
Set-Location "build" | Out-Null

"Running CMake build preparation..." | Write-Host -ForegroundColor White

If ($TargetBoard -eq "ORGPAL_PALTHREE" -or
    $TargetBoard -eq "ST_NUCLEO64_F091RC" -or
    $TargetBoard -eq "ST_STM32F429I_DISCOVERY" -or
    $TargetBoard -eq "ST_STM32F769I_DISCOVERY") {

    # run the install tools to have the build vars filled
    Invoke-Expression "$PSScriptRoot\install-scripts\install-stm32-tools.ps1 -Path $Path"

    If ($TargetBoard -eq "ORGPAL_PALTHREE") {

        $cmakeOptions = @"
-DTOOL_HEX2DFU_PREFIX=$:env:HEX2DFU_PATH
-DTARGET_SERIES=STM32F7xx
-DRTOS=ChibiOS
-DCHIBIOS_CONTRIB_REQUIRED=ON
-DSTM32_CUBE_PACKAGE_REQUIRED=ON
-DSUPPORT_ANY_BASE_CONVERSION=ON
-DNF_FEATURE_DEBUGGER=ON
-DNF_FEATURE_RTC=ON
-DNF_FEATURE_HAS_USB_MSD=ON
-DNF_FEATURE_HAS_SDCARD=ON
-DNF_FEATURE_USE_SPIFFS=ON
-DAPI_System.Math=ON
-DAPI_Hardware.Stm32=ON
-DNF_FEATURE_HAS_CONFIG_BLOCK=ON
-DAPI_Windows.Devices.Gpio=ON -DAPI_System.Device.Gpio=ON
-DAPI_Windows.Devices.Spi=ON -DAPI_System.Device.Spi=ON
-DAPI_Windows.Devices.I2c=ON -DAPI_System.Device.I2c=ON
-DAPI_Windows.Devices.Pwm=ON -DAPI_System.Device.Pwm=ON
-DAPI_Windows.Devices.SerialCommunication=ON -DAPI_System.IO.Ports=ON
-DAPI_Windows.Devices.Adc=ON -DAPI_System.Device.Adc=ON
-DAPI_System.Device.Dac=ON
-DAPI_System.Net=ON
-DNF_SECURITY_MBEDTLS=ON
-DAPI_nanoFramework.ResourceManager=ON
-DAPI_nanoFramework.System.Collections=ON
-DAPI_nanoFramework.System.Text=ON
"@
    }
    elseif ($TargetBoard -eq "ST_NUCLEO64_F091RC") {
    
    $cmakeOptions = @"
-DTOOL_HEX2DFU_PREFIX=$:env:HEX2DFU_PATH
-DTARGET_SERIES=STM32F0xx
-DRTOS=ChibiOS
-DNF_FEATURE_DEBUGGER=ON
-DNF_FEATURE_RTC=ON
-DUSE_RNG=OFF
-DNF_PLATFORM_NO_CLR_TRACE=ON
-DNF_CLR_NO_IL_INLINE=ON
-DAPI_Hardware.Stm32=ON
-DAPI_Windows.Devices.Gpio=ON -DAPI_System.Device.Gpio=ON
-DAPI_Windows.Devices.Spi=ON -DAPI_System.Device.Spi=ON
-DAPI_Windows.Devices.I2c=ON -DAPI_System.Device.I2c=ON
-DAPI_Windows.Devices.Pwm=ON -DAPI_System.Device.Pwm=ON
-DAPI_Windows.Devices.SerialCommunication=ON -DAPI_System.IO.Ports=ON
-DAPI_nanoFramework.System.Text=ON
"@
    }
    elseif ($TargetBoard -eq "ST_STM32F429I_DISCOVERY") {

        $cmakeOptions = @"
-DTARGET_SERIES=STM32F4xx
-DRTOS=ChibiOS
-DSUPPORT_ANY_BASE_CONVERSION=ON
-DNF_FEATURE_DEBUGGER=ON
-DNF_FEATURE_RTC=ON
-DAPI_System.Math=ON
-DAPI_Hardware.Stm32=ON
-DAPI_Windows.Devices.Gpio=ON -DAPI_System.Device.Gpio=ON
-DAPI_Windows.Devices.Spi=ON -DAPI_System.Device.Spi=ON
-DAPI_Windows.Devices.I2c=ON -DAPI_System.Device.I2c=ON
-DAPI_Windows.Devices.Pwm=ON -DAPI_System.Device.Pwm=ON
-DAPI_Windows.Devices.SerialCommunication=ON -DAPI_System.IO.Ports=ON
-DAPI_Windows.Devices.Adc=ON -DAPI_System.Device.Adc=ON
-DAPI_nanoFramework.Device.OneWire=ON
-DAPI_nanoFramework.Devices.Can=ON
-DAPI_nanoFramework.ResourceManager=ON
-DAPI_nanoFramework.System.Collections=ON
-DAPI_nanoFramework.System.Text=ON
"@
    }
    elseif ($TargetBoard -eq "ST_STM32F769I_DISCOVERY") {

        $cmakeOptions = @"
-DTARGET_SERIES=STM32F7xx
-DRTOS=ChibiOS
-DSUPPORT_ANY_BASE_CONVERSION=ON
-DNF_FEATURE_DEBUGGER=ON
-DSWO_OUTPUT=ON
-DNF_FEATURE_RTC=ON
-DNF_FEATURE_HAS_SDCARD=ON
-DAPI_System.Math=ON
-DAPI_Hardware.Stm32=ON
-DNF_FEATURE_HAS_CONFIG_BLOCK=ON
-DAPI_Windows.Devices.Gpio=ON -DAPI_System.Device.Gpio=ON
-DAPI_Windows.Devices.Spi=ON -DAPI_System.Device.Spi=ON
-DAPI_Windows.Devices.I2c=ON -DAPI_System.Device.I2c=ON
-DAPI_Windows.Devices.Pwm=ON -DAPI_System.Device.Pwm=ON
-DAPI_Windows.Devices.SerialCommunication=ON -DAPI_System.IO.Ports=ON
-DAPI_Windows.Devices.Adc=ON -DAPI_System.Device.Adc=ON
-DAPI_System.Device.Dac=ON
-DAPI_System.Net=ON
-DNF_SECURITY_MBEDTLS=ON
-DAPI_nanoFramework.Device.OneWire=ON
-DAPI_nanoFramework.Devices.Can=ON
-DAPI_System.IO.FileSystem=ON
-DAPI_nanoFramework.ResourceManager=ON
-DAPI_nanoFramework.System.Collections=ON
-DAPI_nanoFramework.System.Text=ON
-DAPI_nanoFramework.Graphics=ON
-DGRAPHICS_MEMORY=Graphics_Memory.cpp
-DGRAPHICS_DISPLAY=Otm8009a_DSI_Video_Mode.cpp
-DGRAPHICS_DISPLAY_INTERFACE=DSI_To_Display_Video_Mode.cpp
-DTOUCHPANEL_DEVICE=ft6x06_I2C.cpp
-DTOUCHPANEL_INTERFACE=I2C_To_TouchPanel.cpp
"@
    }
    
    # CMake prep
    $cmakePrep = @"
-G Ninja 
-DCMAKE_TOOLCHAIN_FILE="CMake/toolchain.arm-none-eabi.cmake"
-DTOOLCHAIN_PREFIX="$env:GNU_GCC_TOOLCHAIN_PATH"
-DCMAKE_BUILD_TYPE=Debug
-DBUILD_VERSION=9.9.99.99
-DTARGET_BOARD=$TargetBoard
$cmakeOptions
..
"@
}
elseif ($TargetBoard -eq "ESP32_WROOM_32") {

    # run the install tools
    Invoke-Expression "$PSScriptRoot\install-scripts\install-esp32-tools.ps1 -Path $Path"

    $cmakeOptions = @"
-DTARGET_SERIES=ESP32
-DRTOS=ESP32
-DNF_FEATURE_DEBUGGER=ON
-DNF_FEATURE_RTC=ON
-DNF_FEATURE_HAS_CONFIG_BLOCK=ON
-DNF_FEATURE_HAS_SDCARD=ON
-DAPI_System.IO.FileSystem=ON
-DAPI_System.Math=ON
-DAPI_Windows.Devices.Gpio=ON
-DAPI_System.Device.Gpio=ON
-DAPI_Windows.Devices.Spi=ON
-DAPI_System.Device.Spi=ON
-DAPI_Windows.Devices.I2c=ON
-DAPI_System.Device.I2c=ON
-DAPI_Windows.Devices.Pwm=ON
-DAPI_Windows.Devices.SerialCommunication=ON
-DAPI_System.IO.Ports=ON 
-DAPI_Windows.Devices.Adc=ON
-DAPI_System.Net=ON
-DAPI_System.Device.WiFi=ON
-DNF_SECURITY_MBEDTLS=ON
-DAPI_Hardware.Esp32=ON
-DSUPPORT_ANY_BASE_CONVERSION=ON
-DAPI_nanoFramework.Device.OneWire=ON
-DAPI_nanoFramework.ResourceManager=ON
-DAPI_nanoFramework.System.Collections=ON
-DAPI_nanoFramework.System.Text=ON
-DAPI_nanoFramework.Hardware.Esp32.Rmt=ON
-DAPI_System.Device.Dac=ON
"@
    
    # CMake prep
    $cmakePrep = @"
-G Ninja
"-DCMAKE_TOOLCHAIN_FILE="CMake/toolchain.xtensa-esp32-elf.cmake""
-DCMAKE_BUILD_TYPE=Debug
-DBUILD_VERSION=9.99.999
-DTARGET_BOARD=$TargetBoard
-DTARGET_NAME=$TargetBoard
$cmakeOptions
..
"@
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
        $cmakeOptions = @"
-DTARGET_SERIES=CC13X2
-DRTOS=TI_SimpleLink
-DRADIO_FREQUENCY=915
-DSUPPORT_ANY_BASE_CONVERSION=OFF
-DNF_FEATURE_DEBUGGER=ON
-DNF_FEATURE_RTC=ON
-DNF_FEATURE_WATCHDOG=OFF
-DAPI_Windows.Devices.Gpio=ON -DAPI_System.Device.Gpio=ON
-DAPI_Windows.Devices.Spi=OFF
-DAPI_Windows.Devices.I2c=OFF
-DAPI_Windows.Devices.Pwm=OFF
-DAPI_Windows.Devices.SerialCommunication=OFF -DAPI_System.IO.Ports=OFF
-DAPI_Windows.Devices.Adc=ON -DAPI_System.Device.Adc=ON
-DAPI_nanoFramework.TI.EasyLink=ON
-DAPI_nanoFramework.Hardware.TI=ON
"@
    }
    
    # CMake prep
    $cmakePrep = @"
-G Ninja 
-DCMAKE_TOOLCHAIN_FILE="CMake/toolchain.arm-none-eabi.cmake"
-DTOOLCHAIN_PREFIX="$env:GNU_GCC_TOOLCHAIN_PATH"
-DCMAKE_BUILD_TYPE=Debug
-DBUILD_VERSION=9.99.999
-DTARGET_BOARD=$TargetBoard $cmakeOptions
..
"@
}
elseif ($TargetBoard -eq "NXP_MIMXRT1060_EVK") {
    # TODO
    Write-Error "Build not supported becuase the build system for this target is being reworked"

    Exit 1
}
elseif ($TargetBoard -eq "GHI_FEZ_CERB40_NF" -or
    $TargetBoard -eq "NETDUINO3_WIFI" -or
    $TargetBoard -eq "I2M_ELECTRON_NF" -or
    $TargetBoard -eq "I2M_OXYGEN_NF" -or
    $TargetBoard -eq "ST_NUCLEO64_F401RE_NF" -or
    $TargetBoard -eq "ST_NUCLEO64_F411RE_NF" -or
    $TargetBoard -eq "ST_STM32F411_DISCOVERY" -or
    $TargetBoard -eq "ST_NUCLEO144_F412ZG_NF" -or
    $TargetBoard -eq "ST_NUCLEO144_F746ZG" -or
    $TargetBoard -eq "ST_STM32F4_DISCOVERY" -or
    $TargetBoard -eq "ST_NUCLEO144_F439ZI" -or
    $TargetBoard -eq "MBN_QUAIL") {

    # run the install tools, OK to reuse the STM32
    Invoke-Expression "$PSScriptRoot\install-scripts\install-stm32-tools.ps1 -Path $Path"

    # community targets with STM32
    If ($TargetBoard -eq "GHI_FEZ_CERB40_NF") {
        $cmakeOptions = @"
-DTARGET_SERIES=STM32F4xx
-DRTOS=ChibiOS
-DNF_FEATURE_DEBUGGER=ON
-DNF_FEATURE_RTC=ON
-DAPI_Windows.Devices.Gpio=ON -DAPI_System.Device.Gpio=ON
-DAPI_Windows.Devices.Spi=ON -DAPI_System.Device.Spi=ON
-DAPI_Windows.Devices.I2c=ON -DAPI_System.Device.I2c=ON
-DAPI_Windows.Devices.Pwm=ON -DAPI_System.Device.Pwm=ON
-DAPI_Windows.Devices.SerialCommunication=ON -DAPI_System.IO.Ports=ON
-DAPI_Windows.Devices.Adc=ON -DAPI_System.Device.Adc=ON
"@
    }
    elseif ($TargetBoard -eq "NETDUINO3_WIFI") {
        $cmakeOptions = @"
-DTARGET_SERIES=STM32F4xx
-DRTOS=ChibiOS
-DSUPPORT_ANY_BASE_CONVERSION=ON
-DNF_FEATURE_DEBUGGER=ON
-DNF_FEATURE_RTC=ON
-DNF_FEATURE_HAS_SDCARD=ON
-DAPI_System.Math=ON
-DAPI_Hardware.Stm32=ON
-DAPI_Windows.Devices.Gpio=ON -DAPI_System.Device.Gpio=ON
-DAPI_Windows.Devices.Spi=ON -DAPI_System.Device.Spi=ON
-DAPI_Windows.Devices.I2c=ON -DAPI_System.Device.I2c=ON
-DAPI_Windows.Devices.Pwm=ON -DAPI_System.Device.Pwm=ON
-DAPI_Windows.Devices.SerialCommunication=ON -DAPI_System.IO.Ports=ON
-DAPI_Windows.Devices.Adc=ON -DAPI_System.Device.Adc=ON
-DAPI_System.Device.Dac=OFF
-DAPI_nanoFramework.Device.OneWire=ON
-DAPI_nanoFramework.ResourceManager=ON
-DAPI_nanoFramework.System.Collections=ON
-DAPI_nanoFramework.System.Text=ON
"@
    }
    elseif ($TargetBoard -eq "I2M_ELECTRON_NF") {
        $cmakeOptions = @"
-DTARGET_SERIES=STM32F4xx
-DRTOS=ChibiOS
-DNF_FEATURE_DEBUGGER=ON
-DUSE_RNG=OFF
-DNF_FEATURE_RTC=ON
-DAPI_Windows.Devices.Gpio=ON -DAPI_System.Device.Gpio=ON
-DAPI_Windows.Devices.Spi=ON -DAPI_System.Device.Spi=ON
-DAPI_Windows.Devices.I2c=ON -DAPI_System.Device.I2c=ON
-DAPI_Windows.Devices.Pwm=ON -DAPI_System.Device.Pwm=ON
-DAPI_Windows.Devices.SerialCommunication=ON -DAPI_System.IO.Ports=ON
-DAPI_Windows.Devices.Adc=ON -DAPI_System.Device.Adc=ON
"@
    }
    elseif ($TargetBoard -eq "I2M_OXYGEN_NF") {
        $cmakeOptions = @"
-DTARGET_SERIES=STM32F4xx
-DRTOS=ChibiOS
-DNF_FEATURE_DEBUGGER=ON
-DUSE_RNG=OFF
-DNF_FEATURE_RTC=ON
-DAPI_Windows.Devices.Gpio=ON -DAPI_System.Device.Gpio=ON
-DAPI_Windows.Devices.Spi=ON -DAPI_System.Device.Spi=ON
-DAPI_Windows.Devices.I2c=ON -DAPI_System.Device.I2c=ON
-DAPI_Windows.Devices.Pwm=ON -DAPI_System.Device.Pwm=ON
-DAPI_Windows.Devices.SerialCommunication=ON -DAPI_System.IO.Ports=ON
-DAPI_Windows.Devices.Adc=ON -DAPI_System.Device.Adc=ON
-DAPI_System.Math=ON
-DAPI_Hardware.Stm32=ON
"@
    }
    elseif ($TargetBoard -eq "ST_NUCLEO64_F401RE_NF") {
        $cmakeOptions = @"
-DTARGET_SERIES=STM32F4xx
-DNF_FEATURE_DEBUGGER=ON
-DUSE_RNG=OFF
-DSWO_OUTPUT=ON
-DNF_FEATURE_RTC=ON
-DAPI_Windows.Devices.Gpio=ON
-DAPI_Windows.Devices.Spi=ON
-DAPI_Windows.Devices.I2c=ON
-DAPI_Windows.Devices.Pwm=ON
-DAPI_Windows.Devices.SerialCommunication=ON
-DAPI_Windows.Devices.Adc=ON
-DAPI_nanoFramework.Device.OneWire=ON
"@
    }
    elseif ($TargetBoard -eq "ST_NUCLEO64_F411RE_NF") {
        $cmakeOptions = @"
-DTARGET_SERIES=STM32F4xx
-DRTOS=ChibiOS
-DNF_FEATURE_DEBUGGER=ON
-DUSE_RNG=OFF
-DSWO_OUTPUT=ON
-DNF_FEATURE_RTC=ON
-DAPI_Windows.Devices.Gpio=ON -DAPI_System.Device.Gpio=ON
-DAPI_Windows.Devices.Spi=ON -DAPI_System.Device.Spi=ON
-DAPI_Windows.Devices.I2c=ON -DAPI_System.Device.I2c=ON
-DAPI_Windows.Devices.Pwm=ON -DAPI_System.Device.Pwm=ON
-DAPI_Windows.Devices.SerialCommunication=ON -DAPI_System.IO.Ports=ON
-DAPI_Windows.Devices.Adc=ON -DAPI_System.Device.Adc=ON
-DAPI_nanoFramework.System.Collections=ON
-DAPI_nanoFramework.ResourceManager=ON
-DAPI_nanoFramework.System.Text=ON
-DAPI_System.Math=ON
-DAPI_Hardware.Stm32=ON
"@
    }
    elseif ($TargetBoard -eq "ST_STM32F411_DISCOVERY") {
        $cmakeOptions = @"
-DTARGET_SERIES=STM32F4xx
-DRTOS=ChibiOS
-DNF_FEATURE_DEBUGGER=ON
-DUSE_RNG=OFF
-DSWO_OUTPUT=ON
-DNF_FEATURE_RTC=ON
-DAPI_Windows.Devices.Gpio=ON -DAPI_System.Device.Gpio=ON
-DAPI_Windows.Devices.Spi=ON -DAPI_System.Device.Spi=ON
-DAPI_Windows.Devices.I2c=ON -DAPI_System.Device.I2c=ON
-DAPI_Windows.Devices.Pwm=ON -DAPI_System.Device.Pwm=ON
-DAPI_Windows.Devices.SerialCommunication=ON -DAPI_System.IO.Ports=ON
-DAPI_Windows.Devices.Adc=ON -DAPI_System.Device.Adc=ON
-DAPI_nanoFramework.System.Collections=ON
-DAPI_nanoFramework.ResourceManager=ON
-DAPI_nanoFramework.System.Text=ON
"@
    }
    elseif ($TargetBoard -eq "ST_NUCLEO144_F412ZG_NF") {
        $cmakeOptions = @"
-DTARGET_SERIES=STM32F4xx
-DRTOS=ChibiOS
-DNF_FEATURE_DEBUGGER=ON
-DSWO_OUTPUT=ON
-DNF_FEATURE_RTC=ON
-DAPI_Windows.Devices.Gpio=ON -DAPI_System.Device.Gpio=ON
-DAPI_Windows.Devices.Spi=ON -DAPI_System.Device.Spi=ON
-DAPI_Windows.Devices.I2c=ON -DAPI_System.Device.I2c=ON
-DAPI_Windows.Devices.Pwm=ON -DAPI_System.Device.Pwm=ON
-DAPI_Windows.Devices.SerialCommunication=ON -DAPI_System.IO.Ports=ON
-DAPI_Windows.Devices.Adc=ON -DAPI_System.Device.Adc=ON
"@
    }
    elseif ($TargetBoard -eq "ST_NUCLEO144_F746ZG") {
        $cmakeOptions = @"
-DTARGET_SERIES=STM32F7xx
-DRTOS=ChibiOS
-DNF_FEATURE_DEBUGGER=ON
-DSWO_OUTPUT=ON
-DNF_FEATURE_RTC=ON
-DNF_FEATURE_HAS_CONFIG_BLOCK=ON
-DAPI_System.Math=ON
-DAPI_Hardware.Stm32=ON
-DAPI_Windows.Devices.Gpio=ON -DAPI_System.Device.Gpio=ON
-DAPI_Windows.Devices.Spi=ON -DAPI_System.Device.Spi=ON
-DAPI_Windows.Devices.I2c=ON -DAPI_System.Device.I2c=ON
-DAPI_Windows.Devices.Pwm=ON -DAPI_System.Device.Pwm=ON
-DAPI_Windows.Devices.SerialCommunication=ON -DAPI_System.IO.Ports=ON
-DAPI_Windows.Devices.Adc=ON -DAPI_System.Device.Adc=ON
-DAPI_System.Net=ON
-DNF_SECURITY_MBEDTLS=ON
-DAPI_nanoFramework.Device.OneWire=ON
-DAPI_nanoFramework.System.Collections=ON
-DAPI_nanoFramework.ResourceManager=ON
-DAPI_nanoFramework.System.Text=ON
"@
    }
    elseif ($TargetBoard -eq "ST_STM32F4_DISCOVERY") {
        $cmakeOptions = @"
-DTARGET_SERIES=STM32F4xx
-DRTOS=ChibiOS
-DSUPPORT_ANY_BASE_CONVERSION=ON
-DNF_FEATURE_DEBUGGER=ON
-DSWO_OUTPUT=ON
-DNF_FEATURE_RTC=ON
-DAPI_System.Math=ON
-DAPI_Hardware.Stm32=ON
-DAPI_Windows.Devices.Gpio=ON -DAPI_System.Device.Gpio=ON
-DAPI_Windows.Devices.Spi=ON -DAPI_System.Device.Spi=ON
-DAPI_Windows.Devices.I2c=ON -DAPI_System.Device.I2c=ON
-DAPI_Windows.Devices.Pwm=ON -DAPI_System.Device.Pwm=ON
-DAPI_Windows.Devices.SerialCommunication=ON -DAPI_System.IO.Ports=ON
-DAPI_Windows.Devices.Adc=ON -DAPI_System.Device.Adc=ON
-DAPI_nanoFramework.Device.OneWire=ON
-DAPI_nanoFramework.Devices.Can=ON
"@
    }
    elseif ($TargetBoard -eq "ST_NUCLEO144_F439ZI") {
        $cmakeOptions = @"
-DTARGET_SERIES=STM32F4xx
-DRTOS=ChibiOS
-DNF_FEATURE_DEBUGGER=ON
-DSWO_OUTPUT=ON
-DNF_FEATURE_RTC=ON
-DNF_FEATURE_HAS_CONFIG_BLOCK=ON
-DAPI_System.Math=ON
-DAPI_Hardware.Stm32=ON
-DAPI_Windows.Devices.Gpio=ON -DAPI_System.Device.Gpio=ON
-DAPI_Windows.Devices.Spi=ON -DAPI_System.Device.Spi=ON
-DAPI_Windows.Devices.I2c=ON -DAPI_System.Device.I2c=ON
-DAPI_Windows.Devices.Pwm=ON -DAPI_System.Device.Pwm=ON
-DAPI_Windows.Devices.SerialCommunication=ON -DAPI_System.IO.Ports=ON
-DAPI_Windows.Devices.Adc=ON -DAPI_System.Device.Adc=ON
-DAPI_System.Net=ON
-DNF_SECURITY_MBEDTLS=ON
"@
    }
    elseif ( $TargetBoard -eq "MBN_QUAIL" ) {

        $cmakeOptions = @"
-DTARGET_SERIES=STM32F4xx
-DRTOS=ChibiOS
-DSUPPORT_ANY_BASE_CONVERSION=ON
-DNF_FEATURE_DEBUGGER=ON
-DNF_FEATURE_RTC=ON
-DNF_FEATURE_HAS_SDCARD=ON
-DAPI_System.Math=ON
-DAPI_Hardware.Stm32=ON
-DAPI_Windows.Devices.Gpio=ON -DAPI_System.Device.Gpio=ON
-DAPI_Windows.Devices.Spi=ON -DAPI_System.Device.Spi=ON
-DAPI_Windows.Devices.I2c=ON -DAPI_System.Device.I2c=ON
-DAPI_Windows.Devices.Pwm=ON -DAPI_System.Device.Pwm=ON
-DAPI_Windows.Devices.SerialCommunication=ON -DAPI_System.IO.Ports=ON
-DAPI_nanoFramework.Device.OneWire=ON
-DAPI_Windows.Storage=ON
-DAPI_nanoFramework.ResourceManager=ON
-DAPI_nanoFramework.System.Collections=ON
-DAPI_nanoFramework.System.Text=ON
"@
    }
    
    # CMake prep
    $cmakePrep = @"
-G Ninja
-DCMAKE_TOOLCHAIN_FILE="CMake/toolchain.arm-none-eabi.cmake"
-DTOOLCHAIN_PREFIX="$env:GNU_GCC_TOOLCHAIN_PATH"
-DCMAKE_BUILD_TYPE=Debug
-DBUILD_VERSION=9.9.99.99
-DTARGET_BOARD=$TargetBoard
$cmakeOptions
..
"@
}
elseif ($TargetBoard -eq "TI_CC1352P1_LAUNCHXL") {
    # community targets with TI CC13xx
    $cmakeOptions = @"
-DTARGET_SERIES=CC13x2_26x2
-DRTOS=TI_SIMPLELINK
-DSUPPORT_ANY_BASE_CONVERSION=OFF
-DNF_FEATURE_DEBUGGER=ON
-DNF_FEATURE_RTC=ON 
-DNF_FEATURE_WATCHDOG=OFF
-DAPI_Windows.Devices.Gpio=ON
-DAPI_Windows.Devices.Spi=OFF
-DAPI_Windows.Devices.I2c=OFF
-DAPI_Windows.Devices.Pwm=OFF 
-DAPI_Windows.Devices.SerialCommunication=OFF
-DAPI_Windows.Devices.Adc=OFF
-DAPI_nanoFramework.TI.EasyLink=ON
"@
    
    # CMake prep
    $cmakePrep = @"
-G Ninja
-DCMAKE_TOOLCHAIN_FILE="CMake/toolchain.arm-none-eabi.cmake"
-DTOOLCHAIN_PREFIX="$env:GNU_GCC_TOOLCHAIN_PATH"
-DCMAKE_BUILD_TYPE=Debug
-DBUILD_VERSION=9.9.99.99
-DTARGET_BOARD=$TargetBoard
$cmakeOptions
..
"@
}
else {
    Write-Error "Unknown target name."
}

# get path to the cmake executable
$cmake = (Get-Command "cmake.exe" -ErrorAction SilentlyContinue)

# output CMake command
$cmakePrep = $cmakePrep -replace "`r`n",' '
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

# SIG # Begin signature block
# MIIeIgYJKoZIhvcNAQcCoIIeEzCCHg8CAQExDzANBglghkgBZQMEAgEFADB5Bgor
# BgEEAYI3AgEEoGswaTA0BgorBgEEAYI3AgEeMCYCAwEAAAQQH8w7YFlLCE63JNLG
# KX7zUQIBAAIBAAIBAAIBAAIBADAxMA0GCWCGSAFlAwQCAQUABCDS5v6EV3zBPlA0
# OQ6q19fyt8uE6tAronK4YFXe0ms6T6CCDg8wggPFMIICraADAgECAhACrFwmagtA
# m48LefKuRiV3MA0GCSqGSIb3DQEBBQUAMGwxCzAJBgNVBAYTAlVTMRUwEwYDVQQK
# EwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5jb20xKzApBgNV
# BAMTIkRpZ2lDZXJ0IEhpZ2ggQXNzdXJhbmNlIEVWIFJvb3QgQ0EwHhcNMDYxMTEw
# MDAwMDAwWhcNMzExMTEwMDAwMDAwWjBsMQswCQYDVQQGEwJVUzEVMBMGA1UEChMM
# RGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3d3cuZGlnaWNlcnQuY29tMSswKQYDVQQD
# EyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5jZSBFViBSb290IENBMIIBIjANBgkqhkiG
# 9w0BAQEFAAOCAQ8AMIIBCgKCAQEAxszlc+b71LvlLS0ypt/lgT/JzSVJtnEqw9WU
# NGeiChywX2mmQLHEt7KP0JikqUFZOtPclNY823Q4pErMTSWC90qlUxI47vNJbXGR
# fmO2q6Zfw6SE+E9iUb74xezbOJLjBuUIkQzEKEFV+8taiRV+ceg1v01yCT2+OjhQ
# W3cxG42zxyRFmqesbQAUWgS3uhPrUQqYQUEiTmVhh4FBUKZ5XIneGUpX1S7mXRxT
# LH6YzRoGFqRoc9A0BBNcoXHTWnxV215k4TeHMFYE5RG0KYAS8Xk5iKICEXwnZreI
# t3jyygqoOKsKZMK/Zl2VhMGhJR6HXRpQCyASzEG7bgtROLhLywIDAQABo2MwYTAO
# BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUsT7DaQP4
# v0cB1JgmGggC72NkK8MwHwYDVR0jBBgwFoAUsT7DaQP4v0cB1JgmGggC72NkK8Mw
# DQYJKoZIhvcNAQEFBQADggEBABwaBpfc15yfPIhmBghXIdshR/gqZ6q/GDJ2QBBX
# wYrzetkRZY41+p78RbWe2UwxS7iR6EMsjrN4ztvjU3lx1uUhlAHaVYeaJGT2imbM
# 3pw3zag0sWmbI8ieeCIrcEPjVUcxYRnvWMWFL04w9qAxFiPI5+JlFjPLvxoboD34
# yl6LMYtgCIktDAZcUrfE+QqY0RVfnxK+fDZjOL1EpH/kJisKxJdpDemM4sAQV7jI
# dhKRVfJIadi8KgJbD0TUIDHb9LpwJl2QYJ68SxcJL7TLHkNoyQcnwdJc9+ohuWgS
# nDycv578gFybY83sR6olJ2egN/MAgn1U16n46S4To3foH0owggSRMIIDeaADAgEC
# AhAHsEGNpR4UjDMbvN63E4MjMA0GCSqGSIb3DQEBCwUAMGwxCzAJBgNVBAYTAlVT
# MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
# b20xKzApBgNVBAMTIkRpZ2lDZXJ0IEhpZ2ggQXNzdXJhbmNlIEVWIFJvb3QgQ0Ew
# HhcNMTgwNDI3MTI0MTU5WhcNMjgwNDI3MTI0MTU5WjBaMQswCQYDVQQGEwJVUzEY
# MBYGA1UEChMPLk5FVCBGb3VuZGF0aW9uMTEwLwYDVQQDEyguTkVUIEZvdW5kYXRp
# b24gUHJvamVjdHMgQ29kZSBTaWduaW5nIENBMIIBIjANBgkqhkiG9w0BAQEFAAOC
# AQ8AMIIBCgKCAQEAwQqv4aI0CI20XeYqTTZmyoxsSQgcCBGQnXnufbuDLhAB6GoT
# NB7HuEhNSS8ftV+6yq8GztBzYAJ0lALdBjWypMfL451/84AO5ZiZB3V7MB2uxgWo
# cV1ekDduU9bm1Q48jmR4SVkLItC+oQO/FIA2SBudVZUvYKeCJS5Ri9ibV7La4oo7
# BJChFiP8uR+v3OU33dgm5BBhWmth4oTyq22zCfP3NO6gBWEIPFR5S+KcefUTYmn2
# o7IvhvxzJsMCrNH1bxhwOyMl+DQcdWiVPuJBKDOO/hAKIxBG4i6ryQYBaKdhDgaA
# NSCik0UgZasz8Qgl8n0A73+dISPumD8L/4mdywIDAQABo4IBPzCCATswHQYDVR0O
# BBYEFMtck66Im/5Db1ZQUgJtePys4bFaMB8GA1UdIwQYMBaAFLE+w2kD+L9HAdSY
# JhoIAu9jZCvDMA4GA1UdDwEB/wQEAwIBhjATBgNVHSUEDDAKBggrBgEFBQcDAzAS
# BgNVHRMBAf8ECDAGAQH/AgEAMDQGCCsGAQUFBwEBBCgwJjAkBggrBgEFBQcwAYYY
# aHR0cDovL29jc3AuZGlnaWNlcnQuY29tMEsGA1UdHwREMEIwQKA+oDyGOmh0dHA6
# Ly9jcmwzLmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEhpZ2hBc3N1cmFuY2VFVlJvb3RD
# QS5jcmwwPQYDVR0gBDYwNDAyBgRVHSAAMCowKAYIKwYBBQUHAgEWHGh0dHBzOi8v
# d3d3LmRpZ2ljZXJ0LmNvbS9DUFMwDQYJKoZIhvcNAQELBQADggEBALNGxKTz6gq6
# clMF01GjC3RmJ/ZAoK1V7rwkqOkY3JDl++v1F4KrFWEzS8MbZsI/p4W31Eketazo
# Nxy23RT0zDsvJrwEC3R+/MRdkB7aTecsYmMeMHgtUrl3xEO3FubnQ0kKEU/HBCTd
# hR14GsQEccQQE6grFVlglrew+FzehWUu3SUQEp9t+iWpX/KfviDWx0H1azilMX15
# lzJUxK7kCzmflrk5jCOCjKqhOdGJoQqstmwP+07qXO18bcCzEC908P+TYkh0z9gV
# rlj7tyW9K9zPVPJZsLRaBp/QjMcH65o9Y1hD1uWtFQYmbEYkT1K9tuXHtQYx1Rpf
# /dC8Nbl4iukwggWtMIIElaADAgECAhAM/wF08NAk2CbHMbcUImY5MA0GCSqGSIb3
# DQEBCwUAMFoxCzAJBgNVBAYTAlVTMRgwFgYDVQQKEw8uTkVUIEZvdW5kYXRpb24x
# MTAvBgNVBAMTKC5ORVQgRm91bmRhdGlvbiBQcm9qZWN0cyBDb2RlIFNpZ25pbmcg
# Q0EwHhcNMjAxMTI2MDAwMDAwWhcNMjMxMTMwMjM1OTU5WjCBqDEUMBIGA1UEBRML
# NjAzIDM4OSAwNjgxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpXYXNoaW5ndG9uMRAw
# DgYDVQQHEwdSZWRtb25kMS0wKwYDVQQKEyQuTkVUIG5hbm9GcmFtZXdvcmsgKC5O
# RVQgRm91bmRhdGlvbikxLTArBgNVBAMTJC5ORVQgbmFub0ZyYW1ld29yayAoLk5F
# VCBGb3VuZGF0aW9uKTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALxP
# 0PKK+4q46QcvKvyo9IcuRJSSYhctdYtq6O4OXHQlcauUOOGteoSheB/Aynzt5OQ+
# v+IOg2eb6JS090E9DyXVJKzs34fCoHqIZNOpdyxcSAVYgM1sYnFurTFI175THg/e
# son1ARyiqxYDaPlMcjJ5hiIid5xDlGMVFzU4WvlpMXCtjZp4bQOGVl78smTXwMvz
# iqmfFOsuv1MtGvhpcxl7k5M4FA0BksnliRMmCMvXQliLniXwcZNy3xbTtFnqXXIa
# gYQHEduPyoLhOd8Cit/TAyzHTPS/XEQOd7QUnz2v3XOx8P3dEpCT6ioPiyq9y9v+
# NV22SWeMSAxWocL5xkMCAwEAAaOCAh4wggIaMB8GA1UdIwQYMBaAFMtck66Im/5D
# b1ZQUgJtePys4bFaMB0GA1UdDgQWBBR/5rPX9+2cdJnzQUdaNJwIoH7JUjA0BgNV
# HREELTAroCkGCCsGAQUFBwgDoB0wGwwZVVMtV0FTSElOR1RPTi02MDMgMzg5IDA2
# ODAOBgNVHQ8BAf8EBAMCB4AwEwYDVR0lBAwwCgYIKwYBBQUHAwMwgZkGA1UdHwSB
# kTCBjjBFoEOgQYY/aHR0cDovL2NybDMuZGlnaWNlcnQuY29tL05FVEZvdW5kYXRp
# b25Qcm9qZWN0c0NvZGVTaWduaW5nQ0EuY3JsMEWgQ6BBhj9odHRwOi8vY3JsNC5k
# aWdpY2VydC5jb20vTkVURm91bmRhdGlvblByb2plY3RzQ29kZVNpZ25pbmdDQS5j
# cmwwTAYDVR0gBEUwQzA3BglghkgBhv1sAwEwKjAoBggrBgEFBQcCARYcaHR0cHM6
# Ly93d3cuZGlnaWNlcnQuY29tL0NQUzAIBgZngQwBBAEwgYQGCCsGAQUFBwEBBHgw
# djAkBggrBgEFBQcwAYYYaHR0cDovL29jc3AuZGlnaWNlcnQuY29tME4GCCsGAQUF
# BzAChkJodHRwOi8vY2FjZXJ0cy5kaWdpY2VydC5jb20vTkVURm91bmRhdGlvblBy
# b2plY3RzQ29kZVNpZ25pbmdDQS5jcnQwDAYDVR0TAQH/BAIwADANBgkqhkiG9w0B
# AQsFAAOCAQEAV1W9PfOR6rsxNB3gU0w5dPm+kp/DqCFBV24BB9CJx1dMWh8AijYm
# qyUpQ5n3S6x4lIU8KeFgC253LQA5wF6OjDoj86t/fohrsBcUbU9XsJ+AqscyEZYR
# fbrFicrII5VQXMus77/h7JfCAxMy4IKym0IOPEA+4wo1+mGNyGzsdTd4fqLibuUB
# SFhQry8tS8JFAnil8J6F9WK3GvJn6gZhbavPZr442KUsb0EomhYmni25kaotNrmQ
# D7Q+k2GMyx7DtgKF86uIbyfSoMavS4Yf9N7hVXmLeTeGrC5GqqcyDfe+reWOPDU6
# EIEZMcWHkoyvJNRFXACjvNV4MK6u282mMjGCD2kwgg9lAgEBMG4wWjELMAkGA1UE
# BhMCVVMxGDAWBgNVBAoTDy5ORVQgRm91bmRhdGlvbjExMC8GA1UEAxMoLk5FVCBG
# b3VuZGF0aW9uIFByb2plY3RzIENvZGUgU2lnbmluZyBDQQIQDP8BdPDQJNgmxzG3
# FCJmOTANBglghkgBZQMEAgEFAKCBhDAYBgorBgEEAYI3AgEMMQowCKACgAChAoAA
# MBkGCSqGSIb3DQEJAzEMBgorBgEEAYI3AgEEMBwGCisGAQQBgjcCAQsxDjAMBgor
# BgEEAYI3AgEVMC8GCSqGSIb3DQEJBDEiBCBxI6r+8jb+gMrS7/kV9R5vL9Qy06lX
# 4/uTY0eMAImvmTANBgkqhkiG9w0BAQEFAASCAQCAuN+Gy0MJwIVRhcIyWSKQFe/7
# V86ARhn1cRYo9GMcs7jPje9b3qNePhOJMlJBiGyIvIqleJcnc1HXAE07jZEYpELj
# Hck9S9gNO6/jfHuchnsRWV6HVbEQx3Vr3f1fejKP7goxdlA7OFqUI9FZFBElGbSh
# joF+F6Tqo+S0fMz557R4HpJepB5dQYTQq0/Sy0StlFBtbhBAwsIg3BdmqQUDOmCt
# J8QhWm73cB/7eKERkgk12kZk8PVa9Pty8b82MomZVD5okl4g0SsR87GzzdDzWWnv
# pWRZYXY2uvgr0NIksk9JGPyUOJLAnl6hvX9aKgwkaGolZEn4yfjozXlzSeRmoYIN
# RTCCDUEGCisGAQQBgjcDAwExgg0xMIINLQYJKoZIhvcNAQcCoIINHjCCDRoCAQMx
# DzANBglghkgBZQMEAgEFADB4BgsqhkiG9w0BCRABBKBpBGcwZQIBAQYJYIZIAYb9
# bAcBMDEwDQYJYIZIAWUDBAIBBQAEIPECX7IvQK5M76qk7J1j+xCMO75TDVhdu+4c
# kWgGQVzJAhEAvPEURUjrIMxHcF0lz/IJexgPMjAyMTAzMTcxNTAxMDNaoIIKNzCC
# BP4wggPmoAMCAQICEA1CSuC+Ooj/YEAhzhQA8N0wDQYJKoZIhvcNAQELBQAwcjEL
# MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3
# LmRpZ2ljZXJ0LmNvbTExMC8GA1UEAxMoRGlnaUNlcnQgU0hBMiBBc3N1cmVkIElE
# IFRpbWVzdGFtcGluZyBDQTAeFw0yMTAxMDEwMDAwMDBaFw0zMTAxMDYwMDAwMDBa
# MEgxCzAJBgNVBAYTAlVTMRcwFQYDVQQKEw5EaWdpQ2VydCwgSW5jLjEgMB4GA1UE
# AxMXRGlnaUNlcnQgVGltZXN0YW1wIDIwMjEwggEiMA0GCSqGSIb3DQEBAQUAA4IB
# DwAwggEKAoIBAQDC5mGEZ8WK9Q0IpEXKY2tR1zoRQr0KdXVNlLQMULUmEP4dyG+R
# awyW5xpcSO9E5b+bYc0VkWJauP9nC5xj/TZqgfop+N0rcIXeAhjzeG28ffnHbQk9
# vmp2h+mKvfiEXR52yeTGdnY6U9HR01o2j8aj4S8bOrdh1nPsTm0zinxdRS1LsVDm
# QTo3VobckyON91Al6GTm3dOPL1e1hyDrDo4s1SPa9E14RuMDgzEpSlwMMYpKjIjF
# 9zBa+RSvFV9sQ0kJ/SYjU/aNY+gaq1uxHTDCm2mCtNv8VlS8H6GHq756WwogL0sJ
# yZWnjbL61mOLTqVyHO6fegFz+BnW/g1JhL0BAgMBAAGjggG4MIIBtDAOBgNVHQ8B
# Af8EBAMCB4AwDAYDVR0TAQH/BAIwADAWBgNVHSUBAf8EDDAKBggrBgEFBQcDCDBB
# BgNVHSAEOjA4MDYGCWCGSAGG/WwHATApMCcGCCsGAQUFBwIBFhtodHRwOi8vd3d3
# LmRpZ2ljZXJ0LmNvbS9DUFMwHwYDVR0jBBgwFoAU9LbhIB3+Ka7S5GGlsqIlssgX
# NW4wHQYDVR0OBBYEFDZEho6kurBmvrwoLR1ENt3janq8MHEGA1UdHwRqMGgwMqAw
# oC6GLGh0dHA6Ly9jcmwzLmRpZ2ljZXJ0LmNvbS9zaGEyLWFzc3VyZWQtdHMuY3Js
# MDKgMKAuhixodHRwOi8vY3JsNC5kaWdpY2VydC5jb20vc2hhMi1hc3N1cmVkLXRz
# LmNybDCBhQYIKwYBBQUHAQEEeTB3MCQGCCsGAQUFBzABhhhodHRwOi8vb2NzcC5k
# aWdpY2VydC5jb20wTwYIKwYBBQUHMAKGQ2h0dHA6Ly9jYWNlcnRzLmRpZ2ljZXJ0
# LmNvbS9EaWdpQ2VydFNIQTJBc3N1cmVkSURUaW1lc3RhbXBpbmdDQS5jcnQwDQYJ
# KoZIhvcNAQELBQADggEBAEgc3LXpmiO85xrnIA6OZ0b9QnJRdAojR6OrktIlxHBZ
# vhSg5SeBpU0UFRkHefDRBMOG2Tu9/kQCZk3taaQP9rhwz2Lo9VFKeHk2eie38+dS
# n5On7UOee+e03UEiifuHokYDTvz0/rdkd2NfI1Jpg4L6GlPtkMyNoRdzDfTzZTlw
# S/Oc1np72gy8PTLQG8v1Yfx1CAB2vIEO+MDhXM/EEXLnG2RJ2CKadRVC9S0yOIHa
# 9GCiurRS+1zgYSQlT7LfySmoc0NR2r1j1h9bm/cuG08THfdKDXF+l7f0P4TrweOj
# SaH6zqe/Vs+6WXZhiV9+p7SOZ3j5NpjhyyjaW4emii8wggUxMIIEGaADAgECAhAK
# oSXW1jIbfkHkBdo2l8IVMA0GCSqGSIb3DQEBCwUAMGUxCzAJBgNVBAYTAlVTMRUw
# EwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5jb20x
# JDAiBgNVBAMTG0RpZ2lDZXJ0IEFzc3VyZWQgSUQgUm9vdCBDQTAeFw0xNjAxMDcx
# MjAwMDBaFw0zMTAxMDcxMjAwMDBaMHIxCzAJBgNVBAYTAlVTMRUwEwYDVQQKEwxE
# aWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5jb20xMTAvBgNVBAMT
# KERpZ2lDZXJ0IFNIQTIgQXNzdXJlZCBJRCBUaW1lc3RhbXBpbmcgQ0EwggEiMA0G
# CSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC90DLuS82Pf92puoKZxTlUKFe2I0rE
# DgdFM1EQfdD5fU1ofue2oPSNs4jkl79jIZCYvxO8V9PD4X4I1moUADj3Lh477sym
# 9jJZ/l9lP+Cb6+NGRwYaVX4LJ37AovWg4N4iPw7/fpX786O6Ij4YrBHk8JkDbTuF
# fAnT7l3ImgtU46gJcWvgzyIQD3XPcXJOCq3fQDpct1HhoXkUxk0kIzBdvOw8YGqs
# LwfM/fDqR9mIUF79Zm5WYScpiYRR5oLnRlD9lCosp+R1PrqYD4R/nzEU1q3V8mTL
# ex4F0IQZchfxFwbvPc3WTe8GQv2iUypPhR3EHTyvz9qsEPXdrKzpVv+TAgMBAAGj
# ggHOMIIByjAdBgNVHQ4EFgQU9LbhIB3+Ka7S5GGlsqIlssgXNW4wHwYDVR0jBBgw
# FoAUReuir/SSy4IxLVGLp6chnfNtyA8wEgYDVR0TAQH/BAgwBgEB/wIBADAOBgNV
# HQ8BAf8EBAMCAYYwEwYDVR0lBAwwCgYIKwYBBQUHAwgweQYIKwYBBQUHAQEEbTBr
# MCQGCCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdpY2VydC5jb20wQwYIKwYBBQUH
# MAKGN2h0dHA6Ly9jYWNlcnRzLmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEFzc3VyZWRJ
# RFJvb3RDQS5jcnQwgYEGA1UdHwR6MHgwOqA4oDaGNGh0dHA6Ly9jcmw0LmRpZ2lj
# ZXJ0LmNvbS9EaWdpQ2VydEFzc3VyZWRJRFJvb3RDQS5jcmwwOqA4oDaGNGh0dHA6
# Ly9jcmwzLmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEFzc3VyZWRJRFJvb3RDQS5jcmww
# UAYDVR0gBEkwRzA4BgpghkgBhv1sAAIEMCowKAYIKwYBBQUHAgEWHGh0dHBzOi8v
# d3d3LmRpZ2ljZXJ0LmNvbS9DUFMwCwYJYIZIAYb9bAcBMA0GCSqGSIb3DQEBCwUA
# A4IBAQBxlRLpUYdWac3v3dp8qmN6s3jPBjdAhO9LhL/KzwMC/cWnww4gQiyvd/Mr
# HwwhWiq3BTQdaq6Z+CeiZr8JqmDfdqQ6kw/4stHYfBli6F6CJR7Euhx7LCHi1lss
# FDVDBGiy23UC4HLHmNY8ZOUfSBAYX4k4YU1iRiSHY4yRUiyvKYnleB/WCxSlgNcS
# R3CzddWThZN+tpJn+1Nhiaj1a5bA9FhpDXzIAbG5KHW3mWOFIoxhynmUfln8jA/j
# b7UBJrZspe6HUSHkWGCbugwtK22ixH67xCUrRwIIfEmuE7bhfEJCKMYYVs9BNLZm
# XbZ0e/VWMyIvIjayS6JKldj1po5SMYICTTCCAkkCAQEwgYYwcjELMAkGA1UEBhMC
# VVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3LmRpZ2ljZXJ0
# LmNvbTExMC8GA1UEAxMoRGlnaUNlcnQgU0hBMiBBc3N1cmVkIElEIFRpbWVzdGFt
# cGluZyBDQQIQDUJK4L46iP9gQCHOFADw3TANBglghkgBZQMEAgEFAKCBmDAaBgkq
# hkiG9w0BCQMxDQYLKoZIhvcNAQkQAQQwHAYJKoZIhvcNAQkFMQ8XDTIxMDMxNzE1
# MDEwM1owKwYLKoZIhvcNAQkQAgwxHDAaMBgwFgQU4deCqOGRvu9ryhaRtaq0lKYk
# m/MwLwYJKoZIhvcNAQkEMSIEIL3DuKPVHvDOe0CCBzcl2YwglRCaemu0lRK+fNG6
# WUMdMA0GCSqGSIb3DQEBAQUABIIBALBZ5HPr3tiEbf1YbZBbqwZpsAHOy27IbsAa
# 6P/xQ/IAPOAtpx9nY3YR/oSEeLtVfhCAvKsWMmgNCOSnmeMhuL+32Am4ZgMhmYqj
# 7zlhw+HCgVCMkTsJSyUbI8Zw0DLUWl7e8Z5bC0L/a/EuLnjrtZej8KUx8+ZdCHiN
# cPz9zaPTPYkTAZLbWnPnVxAeeZeTCmHolpHzMXVtTZmjlcFBjlCxIRepUtDOprka
# LhUkuL99mPOeE+kkQK8cYWbx/qTLs2WfjhXq/mR7FpzU1LRkEF/qUPXyDuam6IVC
# YydWqgyYeuMuuCqPPjDkzvCOSYfN8UL7bHGmDEj8iqMsfVOcRhY=
# SIG # End signature block
