# Copyright (c) 2020 The nanoFramework project contributors
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

#Set location of nf-interpreter top-level
$nfRoot = "$PSScriptRoot\.."

$filePathExists = Test-Path "$nfRoot\cmake-variants.json" -ErrorAction SilentlyContinue
If ($filePathExists -eq $False -or $force) {
	"Create .\cmake-variants.json with install paths from .\cmake-variants.TEMPLATE-ESP32.json" | Write-Host -ForegroundColor White
	
	Copy-Item "$nfRoot\cmake-variants.TEMPLATE-ESP32.json" -Destination "$nfRoot\cmake-variants.json" -Force 
	$variants = (Get-Content "$nfRoot\cmake-variants.json")
	$variants = $variants.Replace('<absolute-path-to-the-IDF-folder-mind-the-forward-slashes>', $env:ESP32_IDF_PATH.ToString().Replace("\", "/")) 
	$variants = $variants.Replace('<absolute-path-to-the-bootloader-folder-mind-the-forward-slashes>', $env:ESP32_LIBS_PATH.ToString().Replace("\", "/"))
	$variants = $variants.Replace('<absolute-path-to-the-toolchain-prefix-folder-mind-the-forward-slashes>', $env:ESP32_TOOLCHAIN_PREFIX.ToString().Replace("\", "/"))  
	Set-Content -Path "$nfRoot\cmake-variants.json" -Value $variants 
}

# create build folder if necessary
mkdir -Force "$nfRoot\build" | Out-Null
$buildFolderPath = Resolve-Path $nfRoot\build

$filePathExists = Test-Path "$nfRoot\.vscode\tasks.json" -ErrorAction SilentlyContinue
#$filePathExists=$False
If ($filePathExists -eq $False -or $force) {
	"Create .\.vscode\tasks.json with install paths from .\vscode\tasks.TEMPLATE-ESP32.json" | Write-Host -ForegroundColor White

	Copy-Item "$nfRoot\.vscode\tasks.TEMPLATE-ESP32.json" -Destination "$nfRoot\.vscode\tasks.json" -Force  
	$tasks = (Get-Content "$nfRoot\.vscode\tasks.json")
	$tasks = $tasks.Replace('<absolute-path-to-the-IDF-folder-mind-the-forward-slashes>', $env:ESP32_IDF_PATH.ToString().Replace("\", "/")) 
	$tasks = $tasks.Replace('<absolute-path-to-the-bootloader-folder-mind-the-forward-slashes>', $env:ESP32_LIBS_PATH.ToString().Replace("\", "/"))
	$tasks = $tasks.Replace('<absolute-path-to-the-nanoframework-folder-mind-the-forward-slashes>', $buildFolderPath.ToString().Replace("\", "/"))  
	$tasks = $tasks.Replace('<absolute-path-to-the-toolchain-prefix-folder-mind-the-forward-slashes>', $env:ESP32_TOOLCHAIN_PREFIX.ToString().Replace("\", "/"))  
	
	# Update the COMPORT placeholder if the paramter is supplied.
	If (![string]::IsNullOrEmpty($COMPORT)) {
		$tasks = $tasks.Replace('<COMPORT>', $env:NANOCLR_COMPORT.ToString()) 
	}
	Set-Content -Path "$nfRoot\.vscode\tasks.json" -Value $tasks 
}

$filePathExists = Test-Path "$nfRoot\.vscode\launch.json" -ErrorAction SilentlyContinue
#$filePathExists=$False
If ($filePathExists -eq $False -or $force) {
	"Create .\.vscode\launch.json with install paths from .\vscode\launch.TEMPLATE-ESP32.json" | Write-Host -ForegroundColor White

	Copy-Item "$nfRoot\.vscode\launch.TEMPLATE-ESP32.json" -Destination "$nfRoot\.vscode\launch.json" -Force
	
	$launch = (Get-Content "$nfRoot\.vscode\launch.json")
	$launch = $launch.Replace('<absolute-path-to-the-build-folder-mind-the-forward-slashes>', $buildFolderPath.ToString().Replace("\", "/")) 
	$launch = $launch.Replace('<absolute-path-to-esp32-openocd-mind-the-forward-slashes>', $env:ESP32_OPENOCD_PATH.Replace("\", "/")) 
	$launch = $launch.Replace('<absolute-path-to-the-esp32-toolchain-folder-mind-the-forward-slashes>', $env:ESP32_TOOLCHAIN_PATH.Replace("\", "/")) 
	Set-Content -Path "$nfRoot\.vscode\launch.json" -Value $launch 
}

<#
.SYNOPSIS
    Install the default ESP32 tools and libraries needed to build nanoFramework, and setup the build environemnt.
.DESCRIPTION
	Power Shell Script to install the default tools to build nanoFramework, including setting the machine path and other environment variables needed for ESP32 WROOM 32.
	Use the -Force paramter to overwrite existing Environment variables.
.PARAMETER COMPORT
	The COM port for NANOCLR [e.g. COM1].
.PARAMETER Path
	The path to the folder where the tools should be installed.
.EXAMPLE
   .\install-esp32-tools.ps1 -COMPORT COM19
   Installs the tools for Espressif ESP32 to default path, define required envisonment variables and update VSCode files with paths and set COM19 in tasks.json
.NOTES
    Tested on Windows 10
    Author:  nanoFramework Contributors
    Created: September 9th, 2018
.LINK
    https://https://github.com/nanoframework/Home
#>
