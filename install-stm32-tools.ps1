$BOARD_NAME = "STM32"

[Net.ServicePointManager]::SecurityProtocol = "tls12, tls11, tls"

Write-Host "Install Tools to build nanoCLR into default folders and configure json files ..."
$env:BOARD_NAME = $BOARD_NAME
$env:NANOCLR_COMPORT = $COMPORT

# check if build folder` already exists
$buildPathExists = Test-Path "$PSScriptRoot\build\" -ErrorAction SilentlyContinue
If($buildPathExists -eq $False)
{
	md $("$PSScriptRoot\build\" ) 

	Write-Host "Create build folder ..."
}

Write-Host "BOARD_NAME=" $env:BOARD_NAME
If([string]::IsNullOrEmpty($env:GNU_GCC_TOOLCHAIN_PATH) -or $force)
{
	$env:GNU_GCC_TOOLCHAIN_PATH='C:\GNU_Tools_ARM_Embedded\7-2018-q2-update'
	Write-Host ("Set User Environment GNU_GCC_TOOLCHAIN_PATH='"+$env:GNU_GCC_TOOLCHAIN_PATH+"'")
	[System.Environment]::SetEnvironmentVariable("GNU_GCC_TOOLCHAIN_PATH", $env:GNU_GCC_TOOLCHAIN_PATH, "User")
}
If([string]::IsNullOrEmpty($env:HEX2DFU_PATH) -or $force)
{
	$env:HEX2DFU_PATH= 'C:\mytools\hex2dfu'
	Write-Host ("Set User Environment HEX2DFU_PATH='"+$env:HEX2DFU_PATH+"'")
	[System.Environment]::SetEnvironmentVariable("HEX2DFU_PATH", $env:HEX2DFU_PATH, "User")
}
If([string]::IsNullOrEmpty($env:NINJA_PATH) -or $force)
{
	$env:NINJA_PATH= 'C:\mytools\ninja'
	Write-Host ("Set User Environment NINJA_PATH='"+$env:NINJA_PATH+"'")
	[System.Environment]::SetEnvironmentVariable("NINJA_PATH", $env:NINJA_PATH, "User")
}

# get build matrix
Invoke-Expression -Command .\get-stm32-targets-to-build.ps1

# install tools and utilities
Invoke-Expression -Command .\install-arm-gcc-toolchain.ps1
Invoke-Expression -Command .\install-ninja.ps1
Invoke-Expression -Command .\install-nf-hex2dfu.ps1

$test = [System.Environment]::GetEnvironmentVariable("NINJA_PATH", "User")
If($test -eq "")
{
	[System.Environment]::SetEnvironmentVariable("NINJA_PATH", $env:NINJA_PATH, "User")
}

<#
.SYNOPSIS
    Install the default tools to build nanoFramework for STM32 and setup build environemnt.
.DESCRIPTION
    Power Shell Script to install the default tools to build nano Framework, including setting the machine path and other environment variables needed for STM32
.EXAMPLE
   .\install-stm32-tools.ps1 
   Install the tools for STM32 to default path, define required envisonment variables and update VSCode files with paths and set COM19 in tasks.json
.NOTES
    Tested on Windows 10
    Author:  nanoFramework Contributors
    Created: September 9th, 2018
.LINK
    https://https://github.com/nanoframework/Home
#>