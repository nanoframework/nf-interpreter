# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

# This PS installs the pre-compiled EPS32 libraries from our Bintray repository

[CmdletBinding(SupportsShouldProcess = $true)]
param (
    [Parameter(HelpMessage = "Path to the folder where the tool is to be installed.")][string]$Path,
    [switch]$force = $false
)

$libsVersion = "libs-v3.3.1"

# check if running on Azure Pipelines by looking at this two environment variables
$IsAzurePipelines = $env:Agent_HomeDirectory -and $env:Build_BuildNumber

if ($IsAzurePipelines) {
    $zipRoot = "$env:Agent_TempDirectory"

    $Path = "$env:Agent_TempDirectory\ESP32_Tools\libs"
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

    $Path = $Path + "\$libsVersion"
}

# check if path already exists
$esp32LibPathExists = Test-Path $Path -ErrorAction SilentlyContinue

If ($esp32LibPathExists -eq $False -or $force) {
    $url = "https://bintray.com/nfbot/internal-build-tools/download_file?file_path=IDF_$libsVersion.zip"
    $output = "$zipRoot\esp-idf-libs.zip"

    # Don't download again if already exists. User can remove from zips to force... 
    if (![System.IO.File]::Exists($output) -or $force) {    
        "Downloading ESP32 pre-compiled libs..." | Write-Host -ForegroundColor White -NoNewline

        # Stop security tripping us up
        [Net.ServicePointManager]::SecurityProtocol = "tls12, tls11, tls"

        # download zip IDF
        (New-Object Net.WebClient).DownloadFile($url, $output)
    }

    # unzip to install path, if not on Azure
    if ($IsAzurePipelines -eq $False) {
        "Installing ESP32 pre-compiled libs @ '$Path'..." | Write-Host -ForegroundColor White -NoNewline

        # unzip
        Expand-Archive -Path $output -DestinationPath $Path

        "OK" | Write-Host -ForegroundColor Green
    }
}
else {
    "Skipping install of ESP32 pre-compiled libs" | Write-Host -ForegroundColor Yellow
}

# set env variable, if not on Azure
if ($IsAzurePipelines -eq $False) {
    # need to replace forward slash for paths to work with GCC and CMake
    $Path = "$Path".Replace('\', '/')

    $env:ESP32_LIBS_PATH = $Path
    # this call can fail if the script is not run with appropriate permissions
    [System.Environment]::SetEnvironmentVariable("ESP32_LIBS_PATH", $env:ESP32_LIBS_PATH, "User")

    "Set User Environment ESP32_LIBS_PATH='" + $env:ESP32_LIBS_PATH + "'" | Write-Host -ForegroundColor Yellow
}

# on Azure, adjust libs path
if ($IsAzurePipelines -eq $True) {
    # need to replace forward slash for paths to work with GCC and CMake
    $tempDirPath = "$Path".Replace('\', '/')

    Write-Host "$("##vso[task.setvariable variable=ESP32_LIBS_PATH]")$tempDirPath"
}
