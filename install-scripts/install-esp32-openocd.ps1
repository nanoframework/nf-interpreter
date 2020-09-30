# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

# This PS installs the OpenOCD ESP32 toolchain from Espressif downloads repository 

[CmdletBinding(SupportsShouldProcess = $true)]
param (
    [Parameter(HelpMessage = "Path to the folder where the tool is to be installed.")][string]$Path,
    [switch]$force = $false
)

#Set location of nf-interpreter top-level
$nfRoot = "$PSScriptRoot\.."
$zipRoot = "$nfRoot\zips"

# Create directory for zip files if necessary
mkdir -Force $zipRoot | Out-Null

if ([string]::IsNullOrEmpty($Path) -or $force) {
    # no path requested
    # check for NF_TOOLS_PATH
    if ($env:NF_TOOLS_PATH) {
        $Path = $env:NF_TOOLS_PATH
    }
    else {
        # use default
        $Path = "C:\nftools"
    }

    # append the tool path
    $Path = $Path + "\openocd-esp32"
}

# check if path already exists
$openOCDPathExists = Test-Path $Path -ErrorAction SilentlyContinue

If ($openOCDPathExists -eq $False -or $force) {
    $url = "https://github.com/espressif/openocd-esp32/releases/download/v0.10.0-esp32-20190212/openocd-esp32-win32-0.10.0-esp32-20190212.zip"
    $output = "$zipRoot\openocd-esp32.zip"
    
    # Don't download again if already exists
    if (![System.IO.File]::Exists($output) -or $force) {
        "Downloading OpenOCD for ESP32 ..." | Write-Host -ForegroundColor White -NoNewline

        # Stop security tripping us up
        [Net.ServicePointManager]::SecurityProtocol = "tls12, tls11, tls"

        # Create directory for zip files if necessary
        mkdir -Force $zipRoot | Out-Null
        
        # download zip with OpenOcd tool
        (New-Object Net.WebClient).DownloadFile($url, $output)

        "OK" | Write-Host -ForegroundColor Green
    }

    "Installing OpenOCD for ESP32 @ '$Path'..." | Write-Host -ForegroundColor White -NoNewline

    # unzip
    # set the destination path one level up to place it at the intended directory
    Expand-Archive -Path $output -DestinationPath "$Path\.."

    "OK" | Write-Host -ForegroundColor Green
}
else {
    "Skipping instal of OpenOCD for ESP32" | Write-Host -ForegroundColor Yellow
}

$env:ESP32_OPENOCD_PATH = $Path
# this call can fail if the script is not run with appropriate permissions
[System.Environment]::SetEnvironmentVariable("ESP32_OPENOCD_PATH", $env:ESP32_OPENOCD_PATH, "User")

"Set User Environment ESP32_OPENOCD_PATH='" + $env:ESP32_OPENOCD_PATH + "'" | Write-Host -ForegroundColor Yellow
