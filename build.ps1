[CmdletBinding(SupportsShouldProcess = $true)]
param (
[Parameter(HelpMessage="Please enter the target name [e.g. ESP32 or ST_STM32F429I_DISCOVERY or NETDUINO3_WIFI].",Position=1)][string]$Target
)

# install CMake
Invoke-Expression -Command .\install-scripts\install-cmake.ps1

# create build folder
md -Force "$PSScriptRoot\build" | Out-Null

# move to build folder
cd "build" | Out-Null

Write-Host "Running CMake build preparation..."

If( $Target -eq "MBN_QUAIL" -or 
    $Target -eq "NETDUINO3_WIFI" -or
    $Target -eq "ST_NUCLEO64_F091RC" -or
    $Target -eq "ST_STM32F429I_DISCOVERY" -or
    $Target -eq "ST_STM32F769I_DISCOVERY"
)
{
    # install STM32 tools
    Invoke-Expression -Command .\install-scripts\install-stm32-tools.ps1

    If( $Target -eq "MBN_QUAIL" )
    {
        $cmakeOptions = "-DTOOL_HEX2DFU_PREFIX=$:env:HEX2DFU_PATH -DTARGET_SERIES=STM32F4xx -DRTOS=CHIBIOS -DSUPPORT_ANY_BASE_CONVERSION=ON -DNF_FEATURE_DEBUGGER=ON -DNF_FEATURE_RTC=ON -DAPI_System.Math=ON -DAPI_Hardware.Stm32=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_nanoFramework.Devices.OneWire=ON -DAPI_nanoFramework.ResourceManager=ON -DAPI_nanoFramework.System.Collections=ON -DAPI_nanoFramework.System.Text=ON"
    }
    elseif ($Target -eq "NETDUINO3_WIFI")
    {
        $cmakeOptions = "-DTOOL_HEX2DFU_PREFIX=$:env:HEX2DFU_PATH -DTARGET_SERIES=STM32F4xx -DRTOS=CHIBIOS -DSUPPORT_ANY_BASE_CONVERSION=ON -DNF_FEATURE_DEBUGGER=ON -DNF_FEATURE_RTC=ON -DNF_FEATURE_HAS_SDCARD=ON -DAPI_System.Math=ON -DAPI_Hardware.Stm32=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON -DAPI_System.Devices.Dac=OFF -DAPI_nanoFramework.Devices.OneWire=ON -DAPI_Windows.Storage=ON -DAPI_nanoFramework.ResourceManager=ON -DAPI_nanoFramework.System.Collections=ON -DAPI_nanoFramework.System.Text=ON"
    }
    elseif ($Target -eq "ST_NUCLEO64_F091RC")
    {
        Write-Error "Build not supported becuase this target requires a different GCC toolchain version"
        
        Exit 1
    }
    elseif ($Target -eq "ST_STM32F429I_DISCOVERY")
    {
        $cmakeOptions = "-DTARGET_SERIES=STM32F4xx -DRTOS=CHIBIOS -DSUPPORT_ANY_BASE_CONVERSION=ON -DNF_FEATURE_DEBUGGER=ON -DSWO_OUTPUT=ON -DNF_FEATURE_RTC=ON -DAPI_System.Math=ON -DAPI_Hardware.Stm32=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON -DAPI_nanoFramework.Devices.OneWire=ON -DAPI_nanoFramework.Devices.Can=ON -DAPI_nanoFramework.ResourceManager=ON -DAPI_nanoFramework.System.Collections=ON -DAPI_nanoFramework.System.Text=ON"
    }
    elseif ($Target -eq "ST_STM32F769I_DISCOVERY")
    {
        $cmakeOptions = "-DTARGET_SERIES=STM32F7xx -DRTOS=CHIBIOS -DSUPPORT_ANY_BASE_CONVERSION=ON -DNF_FEATURE_DEBUGGER=ON -DSWO_OUTPUT=ON -DNF_FEATURE_RTC=ON -DNF_FEATURE_HAS_SDCARD=ON -DAPI_System.Math=ON -DAPI_Hardware.Stm32=ON -DNF_FEATURE_HAS_CONFIG_BLOCK=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON -DAPI_System.Devices.Dac=ON -DAPI_System.Net=ON -DNF_SECURITY_MBEDTLS=ON -DAPI_nanoFramework.Devices.OneWire=ON -DAPI_nanoFramework.Devices.Can=ON -DAPI_Windows.Storage=ON -DAPI_nanoFramework.ResourceManager=ON -DAPI_nanoFramework.System.Collections=ON -DAPI_nanoFramework.System.Text=ON"
    }

    # CMake prep
    cmake "-G Ninja -DTOOLCHAIN_PREFIX=$env:GNU_GCC_TOOLCHAIN_PATH -DCMAKE_BUILD_TYPE=Debug -DBUILD_VERSION=9.9.99.99 -DCHIBIOS_BOARD=$Target $cmakeOptions .."
}
elseif ($Target -eq "ESP32_WROOM_32") 
{
    # install ESP32 tools
    Invoke-Expression -Command .\install-scripts\install-esp32-tools.ps1

    $cmakeOptions = "-DTARGET_SERIES=ESP32 -DRTOS=FREERTOS_ESP32 -DNF_FEATURE_DEBUGGER=ON -DNF_FEATURE_RTC=ON -DNF_FEATURE_HAS_CONFIG_BLOCK=ON -DNF_FEATURE_HAS_SDCARD=ON -DAPI_System.Math=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON -DAPI_System.Net=ON -DAPI_Windows.Devices.Wifi=ON -DAPI_Windows.Storage=ON -DNF_SECURITY_MBEDTLS=ON -DAPI_Hardware.Esp32=ON -DSUPPORT_ANY_BASE_CONVERSION=ON -DAPI_nanoFramework.Devices.OneWire=ON -DAPI_nanoFramework.ResourceManager=ON -DAPI_nanoFramework.System.Collections=ON -DAPI_nanoFramework.System.Text=ON"

    # CMake prep
    cmake "-G Ninja -DCMAKE_TOOLCHAIN_FILE=CMake/toolchain.xtensa-esp32-elf.cmake -DTOOLCHAIN_PREFIX=$env:ESP32_TOOLCHAIN_PATH -DESP32_IDF_PATH=$env:ESP32_IDF_PATH -DESP32_LIBS_PATH=$env:ESP32_LIBS_PATH -DCMAKE_BUILD_TYPE=Debug -DBUILD_VERSION=9.99.999 -DESP32_BOARD=$Target $cmakeOptions .."
}
elseif ($Target -eq "TI_CC3220SF_LAUNCHXL") 
{
    # TODO    
    Write-Error "Build not supported becuase the build system for this target is being reworked"

    Exit 1
}
elseif ($Target -eq "TI_CC1352R1_LAUNCHXL") 
{
    # install STM32 tools (OK to reuse)
    Invoke-Expression -Command .\install-scripts\install-stm32-tools.ps1

    If( $Target -eq "TI_CC1352R1_LAUNCHXL" )
    {
        $cmakeOptions = "-DTARGET_SERIES=CC13x2_26x2 -DRTOS=TI_SIMPLELINK -DSUPPORT_ANY_BASE_CONVERSION=OFF -DNF_FEATURE_DEBUGGER=ON -DNF_FEATURE_RTC=ON -DNF_FEATURE_WATCHDOG=OFF -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=OFF -DAPI_Windows.Devices.I2c=OFF -DAPI_Windows.Devices.Pwm=OFF -DAPI_Windows.Devices.SerialCommunication=OFF -DAPI_Windows.Devices.Adc=OFF -DAPI_nanoFramework.TI.EasyLink=ON"
    }

    # CMake prep
    cmake "-G Ninja -DCMAKE_TOOLCHAIN_FILE=CMake/toolchain.arm-none-eabi.cmake -DTOOLCHAIN_PREFIX=$env:GNU_GCC_TOOLCHAIN_PATH -DCMAKE_BUILD_TYPE=Debug -DBUILD_VERSION=9.99.999 -DTI_BOARD=$Target $cmakeOptions .."
}
elseif ($Target -eq "NXP_MIMXRT1060_EVK") 
{
    # TODO
    Write-Error "Build not supported becuase the build system for this target is being reworked"

    Exit 1
}
elseif ($Target -eq "GHI_FEZ_CERB40_NF" -or 
$Target -eq "I2M_ELECTRON_NF" -or
$Target -eq "I2M_OXYGEN_NF" -or
$Target -eq "ST_NUCLEO64_F401RE_NF" -or
$Target -eq "ST_NUCLEO64_F411RE_NF" -or
$Target -eq "ST_STM32F411_DISCOVERY" -or
$Target -eq "ST_NUCLEO144_F412ZG_NF" -or
$Target -eq "ST_NUCLEO144_F746ZG" -or
$Target -eq "ST_STM32F4_DISCOVERY" -or
$Target -eq "ST_NUCLEO144_F439ZI"
)
{
    # community targets with STM32
    If ($Target -eq "GHI_FEZ_CERB40_NF")
    {
        $cmakeOptions = "-DTARGET_SERIES=STM32F4xx -DNF_FEATURE_DEBUGGER=ON -DNF_FEATURE_RTC=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON"
    }
    elseif ($Target -eq "I2M_ELECTRON_NF")
    {
        $cmakeOptions = "-DTARGET_SERIES=STM32F4xx -DNF_FEATURE_DEBUGGER=ON -DUSE_RNG=OFF -DNF_FEATURE_RTC=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON"
    }
    elseif ($Target -eq "I2M_OXYGEN_NF")
    {
        $cmakeOptions = "-DTARGET_SERIES=STM32F4xx -DNF_FEATURE_DEBUGGER=ON -DUSE_RNG=OFF -DNF_FEATURE_RTC=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON"
    }
    elseif ($Target -eq "ST_NUCLEO64_F401RE_NF")
    {
        $cmakeOptions = "-DTARGET_SERIES=STM32F4xx -DNF_FEATURE_DEBUGGER=ON -DUSE_RNG=OFF -DSWO_OUTPUT=ON -DNF_FEATURE_RTC=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON -DAPI_nanoFramework.Devices.OneWire=ON"
    }
    elseif ($Target -eq "ST_NUCLEO64_F411RE_NF")
    {
        $cmakeOptions = "-DTARGET_SERIES=STM32F4xx -DNF_FEATURE_DEBUGGER=ON -DUSE_RNG=OFF -DSWO_OUTPUT=ON -DNF_FEATURE_RTC=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON -DAPI_nanoFramework.System.Collections=ON -DAPI_nanoFramework.ResourceManager=ON -DAPI_nanoFramework.System.Text=ON"
    }
    elseif ($Target -eq "ST_STM32F411_DISCOVERY")
    {
        $cmakeOptions = "-DTARGET_SERIES=STM32F4xx -DNF_FEATURE_DEBUGGER=ON -DUSE_RNG=OFF -DSWO_OUTPUT=ON -DNF_FEATURE_RTC=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON -DAPI_nanoFramework.System.Collections=ON -DAPI_nanoFramework.ResourceManager=ON -DAPI_nanoFramework.System.Text=ON"
    }
    elseif ($Target -eq "ST_NUCLEO144_F412ZG_NF")
    {
        $cmakeOptions = "-DTARGET_SERIES=STM32F4xx -DNF_FEATURE_DEBUGGER=ON -DSWO_OUTPUT=ON -DNF_FEATURE_RTC=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=OFF -DAPI_Windows.Devices.Adc=ON"
    }
    elseif ($Target -eq "ST_NUCLEO144_F746ZG")
    {
        $cmakeOptions = "-DTARGET_SERIES=STM32F7xx -DRTOS=CHIBIOS -DNF_FEATURE_DEBUGGER=ON -DSWO_OUTPUT=ON -DNF_FEATURE_RTC=ON -DNF_FEATURE_HAS_CONFIG_BLOCK=ON -DAPI_System.Math=ON -DAPI_Hardware.Stm32=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON -DAPI_System.Net=ON -DNF_SECURITY_MBEDTLS=ON -DAPI_nanoFramework.Devices.OneWire=ON"
    }
    elseif ($Target -eq "ST_STM32F4_DISCOVERY")
    {
        $cmakeOptions = "-DTARGET_SERIES=STM32F4xx -DRTOS=CHIBIOS -DSUPPORT_ANY_BASE_CONVERSION=ON -DNF_FEATURE_DEBUGGER=ON -DSWO_OUTPUT=ON -DNF_FEATURE_RTC=ON -DAPI_System.Math=ON -DAPI_Hardware.Stm32=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON -DAPI_nanoFramework.Devices.OneWire=ON -DAPI_nanoFramework.Devices.Can=ON"
    }
    elseif ($Target -eq "ST_NUCLEO144_F439ZI")
    {
        $cmakeOptions = "-DTARGET_SERIES=STM32F4xx -DRTOS=CHIBIOS -DNF_FEATURE_DEBUGGER=ON -DSWO_OUTPUT=ON -DNF_FEATURE_RTC=ON -DNF_FEATURE_HAS_CONFIG_BLOCK=ON -DAPI_System.Math=ON -DAPI_Hardware.Stm32=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON -DAPI_System.Net=ON -DNF_SECURITY_MBEDTLS=ON"
    }

    # CMake prep
    cmake "-G Ninja -DTOOLCHAIN_PREFIX=$env:GNU_GCC_TOOLCHAIN_PATH -DCMAKE_BUILD_TYPE=Debug -DBUILD_VERSION=9.9.99.99 -DCHIBIOS_BOARD=$Target $cmakeOptions .."
}
elseif ($Target -eq "TI_CC1352P1_LAUNCHXL")
{
    # community targets with TI CC13xx
    $cmakeOptions = "-DTARGET_SERIES=CC13x2_26x2 -DRTOS=TI_SIMPLELINK -DSUPPORT_ANY_BASE_CONVERSION=OFF -DNF_FEATURE_DEBUGGER=ON -DNF_FEATURE_RTC=ON -DNF_FEATURE_WATCHDOG=OFF -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=OFF -DAPI_Windows.Devices.I2c=OFF -DAPI_Windows.Devices.Pwm=OFF -DAPI_Windows.Devices.SerialCommunication=OFF -DAPI_Windows.Devices.Adc=OFF -DAPI_nanoFramework.TI.EasyLink=ON"

    # CMake prep
    cmake "-G Ninja -DCMAKE_TOOLCHAIN_FILE=CMake/toolchain.arm-none-eabi.cmake -DTOOLCHAIN_PREFIX=$env:GNU_GCC_TOOLCHAIN_PATH -DCMAKE_BUILD_TYPE=Debug -DBUILD_VERSION=9.99.999 -DTI_BOARD=$Target $cmakeOptions .."
}
else 
{
    Write-Error "Unknown target name."    
}

Write-Host "Building with CMake..."

# CMake build
cmake "--build build --target all --config Debug"    

Write-Host "Build completed. Grab the fw images (nanoBooter or/and nanoCLR) from build folder."

<#
.SYNOPSIS
    Performs the build of a nanoFramework target, installing all the required tools and libraries.
.DESCRIPTION
    Power Shell Script to build of a nanoFramework target, installing all the required tools and libraries. 
.PARAMETER Target
	The name of teh target to build [e.g. ESP32 or ST_STM32F429I_DISCOVERY or NETDUINO3_WIFI].
.EXAMPLE
   .\buikd.ps1 -Target ESP32
.NOTES
    The script installs all the required tools and libraries on default locations.
#>
