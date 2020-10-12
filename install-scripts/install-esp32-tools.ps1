# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

[CmdletBinding(SupportsShouldProcess = $true)]
param (
	[Parameter(HelpMessage = "Enter the path to the folder where the tools should be installed.")][string]$Path,
	[Parameter(HelpMessage = "Please enter the COM port for ESP32 flash utility [e.g. COM1].")][string]$COMPort,
	[switch]$force = $false
)

If ([string]::IsNullOrEmpty($COMPort)) {
	"Please use parameter -COMPort to set COM port for ESP32 flash utility [e.g. -COMPort COM1], or edit tasks.json" | Write-Host -ForegroundColor Yellow
}

$env:NANOCLR_COMPORT = $COMPort

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

"Set User Environment NF_TOOLS_PATH='" + $Path + "'" | Write-Host -ForegroundColor White

# set environment variable
$env:NF_TOOLS_PATH = $Path
[System.Environment]::SetEnvironmentVariable("NF_TOOLS_PATH", $Path, "User")

# this call can fail if the script is not run with appropriate permissions
[System.Environment]::SetEnvironmentVariable("NF_TOOLS_PATH", $Path, "Machine")

# need to pass the 'force' switch?
if ($force) {
	$commandArgs = " -force"
}

# call the script for each of the tools
Invoke-Expression $PSScriptRoot\install-cmake.ps1
Invoke-Expression "$PSScriptRoot\install-esp32-toolchain.ps1 $commandArgs"
Invoke-Expression "$PSScriptRoot\install-esp32-libs.ps1 $commandArgs"
Invoke-Expression "$PSScriptRoot\install-esp32-idf.ps1 $commandArgs"
Invoke-Expression "$PSScriptRoot\install-ninja.ps1 $commandArgs"
Invoke-Expression "$PSScriptRoot\install-esp32-openocd.ps1 $commandArgs"

<#
.SYNOPSIS
    Install the default ESP32 tools and libraries needed to build nanoFramework, and setup the build environment.
.DESCRIPTION
	Power Shell Script to install the default tools to build nanoFramework, including setting the machine path and other environment variables needed for ESP32 WROOM 32.
	Use the -Force parameter to overwrite existing Environment variables.
.PARAMETER COMPort
	The COM port for NANOCLR [e.g. COM1].
.PARAMETER Path
	The path to the folder where the tools should be installed.
.EXAMPLE
   .\install-esp32-tools.ps1 -COMPORT COM19
   Installs the tools for Espressif ESP32 to default path, define required environment variables and update VSCode files with paths and set COM19 in tasks.json
.NOTES
    Tested on Windows 10
    Author:  nanoFramework Contributors
    Created: September 9th, 2018
.LINK
    https://https://github.com/nanoframework/Home
#>
