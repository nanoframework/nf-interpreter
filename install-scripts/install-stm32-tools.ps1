# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

[CmdletBinding(SupportsShouldProcess = $true)]
param (
    [Parameter(HelpMessage = "Enter the path to the folder where the tools should be installed.")][string]$Path,
    [switch]$force = $false
)

# need this check here as the path can be passed with an empty string
if ([string]::IsNullOrEmpty($Path) -or $force) {

    # check if there is already a path set
    if($env:NF_TOOLS_PATH)
    {
        $Path = $env:NF_TOOLS_PATH
    }
    else {
        $Path = "C:\nftools"        
    }
}

# set environment variable
$env:NF_TOOLS_PATH = $Path

"Set User Environment NF_TOOLS_PATH='" + $Path + "'" | Write-Host -ForegroundColor White
[System.Environment]::SetEnvironmentVariable("NF_TOOLS_PATH", $Path, "User")

# this call can fail if the script is not run with appropriate permissions
[System.Environment]::SetEnvironmentVariable("NF_TOOLS_PATH", $Path, "Machine")

# need to pass the 'force' switch?
if ($force) {
    $commandArgs = " -force"
}

# install tools and utilities
Invoke-Expression $PSScriptRoot\install-cmake.ps1
Invoke-Expression "$PSScriptRoot\install-arm-gcc-toolchain.ps1 $commandArgs"
Invoke-Expression "$PSScriptRoot\install-ninja.ps1 $commandArgs"
Invoke-Expression "$PSScriptRoot\install-nf-hex2dfu.ps1 $commandArgs"
Invoke-Expression "$PSScriptRoot\install-openocd.ps1 $commandArgs"

<#
.SYNOPSIS
    Install the default tools to build nanoFramework for STM32 and setup build environment.
.DESCRIPTION
	Power Shell Script to install the default tools to build nanoFramework, including setting the machine path and other environment variables needed for STM32
	Use the -Force parameter to overwrite existing Environment variables. 
.EXAMPLE
   .\install-stm32-tools.ps1 
   Install the tools for STM32 to default path, define required environment variables
.NOTES
    Tested on Windows 10
    Author:  nanoFramework Contributors
    Created: September 9th, 2018
.LINK
    https://https://github.com/nanoframework/Home
#>
