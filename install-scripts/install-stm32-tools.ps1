[CmdletBinding(SupportsShouldProcess = $true)]
param (
[Parameter(HelpMessage="Enter the path to the folder where the tools should be installed.",Position=2)][string]$Path,
[switch]$force = $false
)

# set board name
$BOARD_NAME = "STM32"

[Net.ServicePointManager]::SecurityProtocol = "tls12, tls11, tls"

Write-Host "Install Tools to build nanoCLR into default folders and configure json files ..."
$env:BOARD_NAME = $BOARD_NAME
$env:NANOCLR_COMPORT = $COMPORT

if($psISE) { $PSScriptRoot = Split-Path -Path $psISE.CurrentFile.FullPath} #In case running in psISE
if(!$PSScriptRoot){ $PSScriptRoot = Split-Path $MyInvocation.MyCommand.Path -Parent } # or older PS

#Set location of nf-interpreter
$nfRoot = "$PSScriptRoot\.."

# create build folder if necessary
md -Force "$nfRoot\build" | Out-Null

Write-Host "BOARD_NAME=" $env:BOARD_NAME

If(-Not [string]::IsNullOrEmpty($Path))
{
	# user has requested install on a specific path

	# force update of base path for tools
	$env:STM32_TOOLS_PATH = $Path
	Write-Host ("Set User Environment STM32_TOOLS_PATH='"+$env:STM32_TOOLS_PATH+"'")
	[System.Environment]::SetEnvironmentVariable("STM32_TOOLS_PATH", $env:STM32_TOOLS_PATH, "User")

	$env:GNU_GCC_TOOLCHAIN_PATH=($env:STM32_TOOLS_PATH+'\9-2019-q4-update')
	Write-Host ("Set User Environment GNU_GCC_TOOLCHAIN_PATH='"+$env:GNU_GCC_TOOLCHAIN_PATH+"'")
	[System.Environment]::SetEnvironmentVariable("GNU_GCC_TOOLCHAIN_PATH", $env:GNU_GCC_TOOLCHAIN_PATH, "User")

	# set flag to force updating all tool paths
	$PathSet = $true
}
else
{
	# no path requested, set to default location
	
	# force update of base path for path
	$env:STM32_TOOLS_PATH = 'C:\mytools'
	Write-Host ("Set User Environment STM32_TOOLS_PATH='"+$env:STM32_TOOLS_PATH+"'")
	[System.Environment]::SetEnvironmentVariable("STM32_TOOLS_PATH", $env:STM32_TOOLS_PATH, "User")

	$env:GNU_GCC_TOOLCHAIN_PATH='C:\GNU_Tools_ARM_Embedded\9-2019-q4-update'
	Write-Host ("Set User Environment GNU_GCC_TOOLCHAIN_PATH='"+$env:GNU_GCC_TOOLCHAIN_PATH+"'")
	[System.Environment]::SetEnvironmentVariable("GNU_GCC_TOOLCHAIN_PATH", $env:GNU_GCC_TOOLCHAIN_PATH, "User")

	# clear flag to force updating all tool paths
	$PathSet = $false
}

If([string]::IsNullOrEmpty($env:HEX2DFU_PATH) -or $PathSet -or $force)
{
	$env:HEX2DFU_PATH= ($env:STM32_TOOLS_PATH+'\hex2dfu')
	Write-Host ("Set User Environment HEX2DFU_PATH='"+$env:HEX2DFU_PATH+"'")
	[System.Environment]::SetEnvironmentVariable("HEX2DFU_PATH", $env:HEX2DFU_PATH, "User")
}

If([string]::IsNullOrEmpty($env:NINJA_PATH) -or $PathSet -or $force)
{
	$env:NINJA_PATH= ($env:STM32_TOOLS_PATH+'\ninja')
	Write-Host ("Set User Environment NINJA_PATH='"+$env:NINJA_PATH+"'")
	[System.Environment]::SetEnvironmentVariable("NINJA_PATH", $env:NINJA_PATH, "User")
}

# get build matrix (Not used anymore)
#Invoke-Expression -Command $PSScriptRoot\get-stm32-targets-to-build.ps1

# install tools and utilities
Invoke-Expression -Command $PSScriptRoot\install-arm-gcc-toolchain.ps1
Invoke-Expression -Command $PSScriptRoot\install-ninja.ps1
Invoke-Expression -Command $PSScriptRoot\install-nf-hex2dfu.ps1

# add Ninja to the path
# this call can fail if the script is not run with appropriate permissions
[System.Environment]::SetEnvironmentVariable("NINJA_PATH", $env:NINJA_PATH, "User")

<#
.SYNOPSIS
    Install the default tools to build nanoFramework for STM32 and setup build environemnt.
.DESCRIPTION
    Power Shell Script to install the default tools to build nano Framework, including setting the machine path and other environment variables needed for STM32
.EXAMPLE
   .\install-stm32-tools.ps1 
   Install the tools for STM32 to default path, define required environment variables and update VSCode files with paths and set COM19 in tasks.json
.NOTES
    Tested on Windows 10
    Author:  nanoFramework Contributors
    Created: September 9th, 2018
.LINK
    https://https://github.com/nanoframework/Home
#>
