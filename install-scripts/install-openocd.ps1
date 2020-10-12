# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

# This PS installs the OpenOCD 

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
        $Path = E:\stm32_tools#"C:\nftools"
    }

    # append the tool path
    $Path = $Path + "\openocd"
}

# check if path already exists
$openOCDPathExists = Test-Path $Path -ErrorAction SilentlyContinue

If ($openOCDPathExists -eq $False -or $force) {
    $url = "https://github.com/xpack-dev-tools/openocd-xpack/releases/download/v0.10.0-13/xpack-openocd-0.10.0-13-win32-x64.zip"
    $output = "$zipRoot\openocd.zip"
    
    # Don't download again if already exists
    if (![System.IO.File]::Exists($output) -or $force) {
        "Downloading OpenOCD..." | Write-Host -ForegroundColor White -NoNewline

        # Stop security tripping us up
        [Net.ServicePointManager]::SecurityProtocol = "tls12, tls11, tls"

        # Create directory for zip files if necessary
        mkdir -Force $zipRoot | Out-Null
        
        # download zip with OpenOcd tool
        (New-Object Net.WebClient).DownloadFile($url, $output)

        "OK" | Write-Host -ForegroundColor Green
    }

    "Installing OpenOCD @ '$Path'..." | Write-Host -ForegroundColor White -NoNewline

    # unzip to temp directory
    Expand-Archive -Path $output -DestinationPath "$Path\temp"

    # move to final location
    Get-ChildItem -Path "$Path\temp\xPack\OpenOCD\0.10.0-13" -Recurse | Move-Item -Destination "$Path"

    # remove temp directory
    Remove-Item $Path\temp -Recurse -Force

    "OK" | Write-Host -ForegroundColor Green
}
else {
    "Skipping instal of OpenOCD" | Write-Host -ForegroundColor Yellow
}

$env:OPENOCD_PATH = $Path
# this call can fail if the script is not run with appropriate permissions
[System.Environment]::SetEnvironmentVariable("OPENOCD_PATH", $env:OPENOCD_PATH, "User")

"Set User Environment OPENOCD_PATH='" + $env:OPENOCD_PATH + "'" | Write-Host -ForegroundColor Yellow
