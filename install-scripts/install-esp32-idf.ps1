# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

# This PS installs IDF esp32 sdk

[CmdletBinding(SupportsShouldProcess = $true)]
param (
    [Parameter(HelpMessage = "Path to the folder where the tool is to be installed.")][string]$Path,
    [switch]$force = $false
)

$idfVersion = "esp-idf-v3.3.1"

# check if running on Azure Pipelines by looking at this two environment variables
$IsAzurePipelines = $env:Agent_HomeDirectory -and $env:Build_BuildNumber

if ($IsAzurePipelines) {
    $zipRoot = "$env:Agent_TempDirectory"

    $Path = "$env:Agent_TempDirectory\ESP32_Tools\$idfVersion"
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

    $finalPath = $Path + "\$idfVersion"

    # check if path already exists
    $esp32IdfPathExists = Test-Path $finalPath -ErrorAction SilentlyContinue
}
else {
    # check if path already exists
    $esp32IdfPathExists = Test-Path $Path -ErrorAction SilentlyContinue
    
    # need this here for the replacements ahead
    $finalPath = $Path
}

If ($esp32IdfPathExists -eq $False -or $force) {
    $url = "https://dl.espressif.com/dl/esp-idf/releases/$idfVersion.zip"
    $output = "$zipRoot\esp-idf.zip"

    # Don't download again if already exists
    if (![System.IO.File]::Exists($output) -or $force) {
        "Downloading ESP32 IDF..." | Write-Host -ForegroundColor White -NoNewline
        
        # Stop security tripping us up
        [Net.ServicePointManager]::SecurityProtocol = "tls12, tls11, tls"

        # download zip with Espressif IDF
        (New-Object Net.WebClient).DownloadFile($url, $output)

        "OK" | Write-Host -ForegroundColor Green
    }

    # unzip to install path, if not on Azure
    if ($IsAzurePipelines -eq $False) {
        "Installing Espressif IDF @ '$Path'..." | Write-Host -ForegroundColor White -NoNewline

        # unzip
        Expand-Archive -Path $output -DestinationPath $Path

        "OK" | Write-Host -ForegroundColor Green
    }
    
    # set path destination after expand operation
    $Path = $finalPath
}
else {
    "Skipping install of Espressif IDF" | Write-Host -ForegroundColor Yellow
    
    # set path destination after expand operation
    $Path = $finalPath
}

# set env variable, if not on Azure
if ($IsAzurePipelines -eq $False) {
    # need to replace forward slash for paths to work with GCC and CMake
    $Path = "$Path".Replace('\', '/')

    $env:ESP32_IDF_PATH = $Path
    # this call can fail if the script is not run with appropriate permissions
    [System.Environment]::SetEnvironmentVariable("ESP32_IDF_PATH", $env:ESP32_IDF_PATH, "User")

    "Set User Environment ESP32_IDF_PATH='" + $env:ESP32_IDF_PATH + "'" | Write-Host -ForegroundColor Yellow
}

# install python pyserial, if not on Azure
if ($IsAzurePipelines -eq $False) {
    "Installing python pyserial..." | Write-Host -ForegroundColor White
    
    # Make sure serial package is installed in python otherwise
    # the esptool.py tool won't run
    python -m pip install pyserial --upgrade pip
}

# on Azure, adjust IDF path
if ($IsAzurePipelines -eq $True) {
    # need to replace forward slash for paths to work with GCC and CMake
    $tempDirPath = "$Path".Replace('\', '/')

    Write-Host "$("##vso[task.setvariable variable=ESP32_IDF_PATH]")$tempDirPath"
}
