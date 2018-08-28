#build matrx with target names and build options
$BuildMatrix = ("MBN_QUAIL", "-DTARGET_SERIES=STM32F4xx -DRTOS=CHIBIOS -DSUPPORT_ANY_BASE_CONVERSION=ON -DNF_FEATURE_DEBUGGER=ON -DNF_FEATURE_RTC=ON -DUSE_RNG=ON -DUSE_RNG=ON -DAPI_System.Math=ON -DAPI_Hardware.Stm32=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON", 'True'),
("ST_STM32F4_DISCOVERY", "-DTARGET_SERIES=STM32F4xx -DRTOS=CHIBIOS -DNF_FEATURE_DEBUGGER=ON -DSWO_OUTPUT=ON -DNF_FEATURE_RTC=ON -DUSE_RNG=ON -DUSE_RNG=ON -DAPI_System.Math=ON -DAPI_Hardware.Stm32=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON", 'False'),
("ST_STM32F429I_DISCOVERY", "-DTARGET_SERIES=STM32F4xx -DRTOS=CHIBIOS -DSUPPORT_ANY_BASE_CONVERSION=ON -DNF_FEATURE_DEBUGGER=ON -DUSE_RNG=ON -DSWO_OUTPUT=ON -DNF_FEATURE_RTC=ON -DUSE_RNG=ON -DAPI_System.Math=ON -DAPI_Hardware.Stm32=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON", 'False'),
("ST_NUCLEO64_F091RC", "-DTARGET_SERIES=STM32F0xx -DRTOS=CHIBIOS -DNF_FEATURE_DEBUGGER=ON -DNF_FEATURE_RTC=ON -DUSE_RNG=OFF -DNF_PLATFORM_NO_CLR_TRACE=ON -DNF_CLR_NO_IL_INLINE=ON -DAPI_Hardware.Stm32=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON", 'False'),
("ST_NUCLEO144_F746ZG", "-DTARGET_SERIES=STM32F7xx -DRTOS=CHIBIOS -DNF_FEATURE_DEBUGGER=ON -DSWO_OUTPUT=ON -DNF_FEATURE_RTC=ON -DUSE_RNG=ON -DNF_FEATURE_HAS_CONFIG_BLOCK=ON -DUSE_RNG=ON -DAPI_System.Math=ON -DAPI_Hardware.Stm32=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON -DAPI_System.Net=ON -DNF_SECURITY_MBEDTLS=ON", 'False'),
("ST_STM32F769I_DISCOVERY", "-DTARGET_SERIES=STM32F7xx -DRTOS=CHIBIOS -DSUPPORT_ANY_BASE_CONVERSION=ON -DNF_FEATURE_DEBUGGER=ON -DSWO_OUTPUT=ON -DNF_FEATURE_RTC=ON -DUSE_RNG=ON -DUSE_RNG=ON -DAPI_System.Math=ON -DAPI_Hardware.Stm32=ON -DNF_FEATURE_HAS_CONFIG_BLOCK=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON -DAPI_System.Net=ON -DNF_SECURITY_MBEDTLS=ON", 'False'),
("NETDUINO3_WIFI", "-DTARGET_SERIES=STM32F4xx -DRTOS=CHIBIOS -DSUPPORT_ANY_BASE_CONVERSION=ON -DNF_FEATURE_DEBUGGER=ON -DNF_FEATURE_RTC=ON -DUSE_RNG=ON -DAPI_System.Math=ON -DAPI_Hardware.Stm32=ON -DAPI_Windows.Devices.Gpio=ON -DAPI_Windows.Devices.Spi=ON -DAPI_Windows.Devices.I2c=ON -DAPI_Windows.Devices.Pwm=ON -DAPI_Windows.Devices.SerialCommunication=ON -DAPI_Windows.Devices.Adc=ON", 'True')

# get commit message
$commitMessage = "$env:APPVEYOR_REPO_COMMIT_MESSAGE" + " " + "$env:APPVEYOR_REPO_COMMIT_MESSAGE_EXTENDED"

# is there a target name in the commit message?
$targetCandidate = [regex]::Matches("$commitMessage",'[#]+\w+[#]').Value

if($targetCandidate -is [array])
{
    $global:BUILD_MATRIX = @(,@()) 

    ForEach($candidate in $targetCandidate)
    {
        # remove the leading and trailinig '#'
        $thisCandidate = $candidate -replace "#", ""

        # find if there is a target with this name
        ForEach($item in $BuildMatrix)
        {
            if(!$item[0].CompareTo($thisCandidate))
            {
                if($global:BUILD_MATRIX)
                {
                    $global:BUILD_MATRIX += , $item
                }
                else
                {
                    $global:BUILD_MATRIX = , $item
                }

                break;
            }
        }
    }
}
else
{
    if($targetCandidate)
    {
        # special case for #ALL_STM32# (build all targets)
        if(!$targetCandidate.CompareTo('#ALL_STM32#'))
        {
            # target to build is the board name
            $env:BOARD_NAME = "STM32"

            $global:BUILD_MATRIX = $BuildMatrix
        }
        else
        {
            # remove the leading and trailinig '#'
            $targetCandidate = $targetCandidate -replace "#", ""

            # find if there is a target with this name
            ForEach($item in $BuildMatrix)
            {
                if(!$item[0].CompareTo($targetCandidate))
                {
                    $env:BOARD_NAME = $item[0]
                    $env:BUILD_OPTIONS = $item[1]
                    $env:NEEDS_DFU = $item[2]
            
                    break;
                }
            }
        }
    }
    else
    {
        # default is to build all STM32 targets
        $env:BOARD_NAME = 'STM32'

        $global:BUILD_MATRIX = $BuildMatrix
    }
}

if($env:BOARD_NAME)
{
    if(!$env:BOARD_NAME.CompareTo('STM32'))
    {
        Write-Host "Build all STM32 targets"
    }
    else 
    {
        Write-Host "Target board is " $env:BOARD_NAME
    }
}
else 
{
    if($global:BUILD_MATRIX)
    {
        Write-Host "Build targets:"

        ForEach($item in $global:BUILD_MATRIX)
        {
            $env:BOARD_NAME += $item[0]+'|'
            Write-Host $item[0]
        }        
    }
    else
    {
        Write-Host "No STM32 target to build"
    }
}
