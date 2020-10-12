# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

# This PS installs the xtensa ESP32 toolchain from Espressif downloads repository 

[CmdletBinding(SupportsShouldProcess = $true)]
param (
    [Parameter(HelpMessage = "Path to the folder where the tool is to be installed.")][string]$Path,
    [switch]$force = $false
)

# check if running on Azure Pipelines by looking at this two environment variables
$IsAzurePipelines = $env:Agent_HomeDirectory -and $env:Build_BuildNumber

if ($IsAzurePipelines) {
    $zipRoot = "$env:Agent_TempDirectory"

    $Path = "$env:Agent_TempDirectory\ESP32_Tools"
}
else {
    #Set location of nf-interpreter top-level
    $nfRoot = "$PSScriptRoot\.."
    $zipRoot = "$nfRoot\zips"

    # Create directory for zip files if necessary
    mkdir -Force $zipRoot | Out-Null
}

if ([string]::IsNullOrEmpty($Path) -or $force) {
    # no path requested
    # check for NF_TOOLS_PATH
    if ($env:NF_TOOLS_PATH) {
        $Path = $env:NF_TOOLS_PATH
    }
    else {
        # use default
        $Path = 'C:\nftools'
    }

    $finalPath = $Path + "\xtensa-esp32-elf"

    # check if path already exists
    $xtensaPathExists = Test-Path $finalPath -ErrorAction SilentlyContinue
}
else {
    # check if path already exists
    $xtensaPathExists = Test-Path $Path -ErrorAction SilentlyContinue
    
    # need this here for the replacements ahead
    $finalPath = $Path
}

If ($xtensaPathExists -eq $False -or $force) {
    # Download xtensa ESP32 toolchain and install

    $url = "https://dl.espressif.com/dl/xtensa-esp32-elf-win32-1.22.0-80-g6c4433a-5.2.0.zip"
    $output = "$zipRoot\xtensa-esp32-elf-win32.zip"
    
    # Don't download again if already exists
    if (![System.IO.File]::Exists($output) -or $force) {
        "Downloading Xtensa ESP32 toolchain..." | Write-Host -ForegroundColor White -NoNewline

        # Stop security tripping us up
        [Net.ServicePointManager]::SecurityProtocol = "tls12, tls11, tls"

        # download zip with toolchain
        (New-Object Net.WebClient).DownloadFile($url, $output)
        
        "OK" | Write-Host -ForegroundColor Green
    }

    # unzip to install path, if not on Azure
    if ($IsAzurePipelines -eq $False) {
        "Installing Xtensa ESP32 toolchain @ '$Path'..." | Write-Host -ForegroundColor White -NoNewline

        #unzip using PowerShell 5+ inbuilt command
        Expand-Archive -Path $output -DestinationPath $Path

        "OK" | Write-Host -ForegroundColor Green

        # set path destination after expand operation
        $Path = $finalPath
    }
}
else {
    "Skipping instal of Xtensa ESP32 toolchain" | Write-Host -ForegroundColor Yellow

    # set path destination after expand operation
    $Path = $finalPath
}

# set env variable, if not on Azure
if ($IsAzurePipelines -eq $False) {
    # need to replace forward slash for paths to work with GCC and CMake
    $Path = "$Path".Replace('\', '/')

    $env:ESP32_TOOLCHAIN_PATH = $Path
    # this call can fail if the script is not run with appropriate permissions
    [System.Environment]::SetEnvironmentVariable("ESP32_TOOLCHAIN_PATH", $env:ESP32_TOOLCHAIN_PATH, "User")

    "Set User Environment ESP32_TOOLCHAIN_PATH='" + $env:ESP32_TOOLCHAIN_PATH + "'" | Write-Host -ForegroundColor Yellow
}
