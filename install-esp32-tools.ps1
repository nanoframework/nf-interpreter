[CmdletBinding(SupportsShouldProcess = $true)]
param (
[Parameter(HelpMessage="Please enter the COM port for ESP32 flash utility [e.g. COM1 or /dev/ttyUSB0].",Position=2)][string]$COMPORT,
[switch]$force = $false
)


[Net.ServicePointManager]::SecurityProtocol = "tls12, tls11, tls"
Write-Host "Install Tools to build nanoCLR into default folders and configure json files ..."
$env:BOARD_NAME = "ESP32_DEVKITC"

If([string]::IsNullOrEmpty($COMPORT))
{
	Write-Warning ("Please use paramter -C <comport> to set COM port for ESP32 flash utility [e.g. -C COM1], or edit tasks.json")
}

$env:NANOCLR_COMPORT = $COMPORT

# check if build folder` already exists
$buildPathExists = Test-Path "$PSScriptRoot\build\" -ErrorAction SilentlyContinue
If($buildPathExists -eq $False)
{
	Write-Host "Create build folder ..."
	mkdir $("$PSScriptRoot\build\" ) > $null
}

Write-Host "BOARD_NAME=" $env:BOARD_NAME

Write-Host "Set User Environment Variables ... (can be slow if many Applications running.)"
If([string]::IsNullOrEmpty($env:ESP32_TOOLS_PATH) -or $force)
{
	$env:ESP32_TOOLS_PATH= 'C:\ESP32_TOOLS'
	Write-Host ("Set User Environment ESP32_TOOLS_PATH='"+$env:ESP32_TOOLS_PATH+"'")
	[System.Environment]::SetEnvironmentVariable("ESP32_TOOLS_PATH", $env:ESP32_TOOLS_PATH, "User")
}
If([string]::IsNullOrEmpty($env:ESP32_TOOLCHAIN_PREFIX) -or $force)
{
	$env:ESP32_TOOLCHAIN_PREFIX= ($env:ESP32_TOOLS_PATH+'\1.22.0-80')
	Write-Host ("Set User Environment ESP32_TOOLCHAIN_PREFIX='"+$env:ESP32_TOOLCHAIN_PREFIX+"'")
	[System.Environment]::SetEnvironmentVariable("ESP32_TOOLCHAIN_PREFIX", $env:ESP32_TOOLCHAIN_PREFIX, "User")
}
If([string]::IsNullOrEmpty($env:ESP32_TOOLCHAIN_PATH) -or $force)
{
	$env:ESP32_TOOLCHAIN_PATH= ($env:ESP32_TOOLS_PATH+'\1.22.0-80\xtensa-esp32-elf')
	Write-Host ("Set User Environment ESP32_TOOLCHAIN_PATH='"+$env:ESP32_TOOLCHAIN_PATH+"'")
	[System.Environment]::SetEnvironmentVariable("ESP32_TOOLCHAIN_PATH", $env:ESP32_TOOLCHAIN_PATH, "User")
}
If([string]::IsNullOrEmpty($env:ESP32_LIBS_PATH) -or $force)
{
	$env:ESP32_LIBS_PATH=($env:ESP32_TOOLS_PATH+'\libs-v3.1')
	Write-Host ("Set User Environment ESP32_LIBS_PATH='"+$env:ESP32_LIBS_PATH+"'")
	[System.Environment]::SetEnvironmentVariable("ESP32_LIBS_PATH", $env:ESP32_LIBS_PATH, "User")
}
If([string]::IsNullOrEmpty($env:ESP32_IDF_PATH) -or [string]::IsNullOrEmpty($env:IDF_PATH) -or $force)
{
	$env:IDF_PATH=$env:ESP32_IDF_PATH=($env:ESP32_TOOLS_PATH+'\esp-idf-v3.1')
	Write-Host ("Set User Environment ESP32_IDF_PATH='"+$env:ESP32_IDF_PATH+"'")
	Write-Host ("Set User Environment IDF_PATH='"+$env:IDF_PATH+"'")
	[System.Environment]::SetEnvironmentVariable("ESP32_IDF_PATH", $env:ESP32_IDF_PATH, "User")
	[System.Environment]::SetEnvironmentVariable("IDF_PATH", $env:IDF_PATH, "User")
}
If([string]::IsNullOrEmpty($env:NINJA_PATH) -or $force)
{
	$env:NINJA_PATH= ($env:ESP32_TOOLS_PATH+'\ninja')
	Write-Host ("Set User Environment NINJA_PATH='"+$env:NINJA_PATH+"'")
	[System.Environment]::SetEnvironmentVariable("NINJA_PATH", $env:NINJA_PATH, "User")
}


Invoke-Expression -Command .\install-esp32-toolchain.ps1
Invoke-Expression -Command .\install-esp32-libs.ps1
Invoke-Expression -Command .\install-esp32-idf.ps1
Invoke-Expression -Command .\install-ninja.ps1
Invoke-Expression -Command .\install-esp32-openocd.ps1
	
Write-Host ("Adding Ninja to the path "+$env:NINJA_PATH)
.\Set-PathVariable.ps1 -NewLocation $env:NINJA_PATH

$filePathExists = Test-Path "$PSScriptRoot\cmake-variants.json" -ErrorAction SilentlyContinue
If($filePathExists -eq $False -or $force)
{
	Write-Host ("Create .\cmake-variants.json with install paths from .\cmake-variants.TEMPLATE-ESP32.json")
	Copy-Item "$PSScriptRoot\cmake-variants.TEMPLATE-ESP32.json" -Destination "$PSScriptRoot\cmake-variants.json" -Force 
	$variants = (Get-Content "$PSScriptRoot\cmake-variants.json")
	$variants = $variants.Replace('<absolute-path-to-the-IDF-folder-mind-the-forward-slashes>', $env:ESP32_IDF_PATH.ToString().Replace("\", "/")) 
	$variants = $variants.Replace('<absolute-path-to-the-bootloader-folder-mind-the-forward-slashes>', $env:ESP32_LIBS_PATH.ToString().Replace("\", "/"))
	$variants = $variants.Replace('<absolute-path-to-the-toolchain-prefix-folder-mind-the-forward-slashes>', $env:ESP32_TOOLCHAIN_PREFIX.ToString().Replace("\", "/"))  
	Set-Content -Path "$PSScriptRoot\cmake-variants.json" -Value $variants 
}

$filePathExists = Test-Path "$PSScriptRoot\.vscode\cmake-kits.json" -ErrorAction SilentlyContinue
If($filePathExists -eq $False -or $force)
{
	Copy-Item "$PSScriptRoot\.vscode\cmake-kits.TEMPLATE-ESP32.json" -Destination "$PSScriptRoot\.vscode\cmake-kits.json" -Force 
}

$buildFolderPath = Resolve-Path .\build

$filePathExists = Test-Path "$PSScriptRoot\.vscode\tasks.json" -ErrorAction SilentlyContinue
$filePathExists=$False
If($filePathExists -eq $False -or $force)
{
	Write-Host ("Create .\.vscode\tasks.json with install paths from .\vscode\tasks.TEMPLATE-ESP32.json")
	Copy-Item "$PSScriptRoot\.vscode\tasks.TEMPLATE-ESP32.json" -Destination "$PSScriptRoot\.vscode\tasks.json" -Force  
	$tasks = (Get-Content "$PSScriptRoot\.vscode\tasks.json")
	$tasks = $tasks.Replace('<absolute-path-to-the-IDF-folder-mind-the-forward-slashes>', $env:ESP32_IDF_PATH.ToString().Replace("\", "/")) 
	$tasks = $tasks.Replace('<absolute-path-to-the-bootloader-folder-mind-the-forward-slashes>', $env:ESP32_LIBS_PATH.ToString().Replace("\", "/"))
	$tasks = $tasks.Replace('<absolute-path-to-the-nanoframework-folder-mind-the-forward-slashes>', $buildFolderPath.ToString().Replace("\", "/"))  
	$tasks = $tasks.Replace('<absolute-path-to-the-toolchain-prefix-folder-mind-the-forward-slashes>', $env:ESP32_TOOLCHAIN_PREFIX.ToString().Replace("\", "/"))  
	
	# Update the COPORT placeholder if the paramter is supplied.
	If(![string]::IsNullOrEmpty($COMPORT))
	{
		$tasks = $tasks.Replace('<COMPORT>', $env:NANOCLR_COMPORT.ToString()) 
	}
	Set-Content -Path "$PSScriptRoot\.vscode\tasks.json" -Value $tasks 
}

$filePathExists = Test-Path ".\.vscode\launch.json" -ErrorAction SilentlyContinue
$filePathExists=$False
If($filePathExists -eq $False -or $force)
{
	Write-Host "Create .\.vscode\launch.json with install paths from .\vscode\launch.TEMPLATE-ESP32.json"
	Copy-Item ".\.vscode\launch.TEMPLATE-ESP32.json" -Destination ".\.vscode\launch.json" -Force
	
	$launch = (Get-Content '.\.vscode\launch.json')
	$launch = $launch.Replace('<absolute-path-to-the-build-folder-mind-the-forward-slashes>', $buildFolderPath.ToString().Replace("\", "/")) 
	$launch = $launch.Replace('<absolute-path-to-openocd-mind-the-forward-slashes>', $env:ESP32_OPENOCD_PATH.Replace("\", "/")) 
	$launch = $launch.Replace('<absolute-path-to-the-toolchain-folder-mind-the-forward-slashes>', $env:ESP32_TOOLCHAIN_PATH.Replace("\", "/")) 
	Set-Content -Path '.\.vscode\launch.json' -Value $launch 
}

<#
.SYNOPSIS
    Install the default ESP32 tools and libraries neede to build nanoFramework, and setup the build environemnt.
.DESCRIPTION
    Power Shell Script to install the default tools to build nanoFramework, including setting the machine path and other environment variables needed for ESP32 DevKitC. Use the -Force paramter to over-write existing Environment variables. 
.PARAMETER COMPORT
	The COM port for NANOCLR [e.g. COM1].
.EXAMPLE
   .\install-esp32-tools.ps1 -COMPORT COM19
   Installs the tools for Espressive ESP32 to default path, define required envisonment variables and update VSCode files with paths and set COM19 in tasks.json
.NOTES
    Tested on Windows 10
    Author:  nanoFramework Contributors
    Created: September 9th, 2018
.LINK
    https://https://github.com/nanoframework/Home
#>
