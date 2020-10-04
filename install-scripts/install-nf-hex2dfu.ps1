# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

# This PS installs nanoFramework hex2dfu command line utility

[CmdletBinding(SupportsShouldProcess = $true)]
param (
    [Parameter(HelpMessage = "Path to the folder where the tool is to be installed.", Position = 0)][string]$Path,
    [switch]$force = $false
)

# check if running on Azure Pipelines by looking at this two environment variables
$IsAzurePipelines = $env:Agent_HomeDirectory -and $env:Build_BuildNumber

if ($IsAzurePipelines) {
    $zipRoot = "$env:Agent_TempDirectory"
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

    # append the tool path
    $Path = $Path + "\hex2dfu"
}

# check if path already exists
$hex2dfuPathExists = Test-Path $Path -ErrorAction SilentlyContinue

If ($hex2dfuPathExists -eq $False -or $force) {
    # need to have the destination directory to download into 
    if ($IsAzurePipelines -eq $False) {
        & mkdir $($Path) | Out-Null
        $downloadPath = $($Path)
    }
    else {
        $downloadPath = "$env:Agent_TempDirectory\hex2dfu"
        mkdir -Force $downloadPath | Out-Null
    }

    "Downloading hex2dfu..." | Write-Host -ForegroundColor White -NoNewline

    $url = "https://github.com/nanoframework/hex2dfu/releases/latest/download/hex2dfu.exe"
    
    $output = "$downloadPath\hex2dfu.exe"

    # Stop security tripping us up
    [Net.ServicePointManager]::SecurityProtocol = "tls12, tls11, tls"

    # download zip with Ninja tool
    (New-Object Net.WebClient).DownloadFile($url, $output)

    "OK" | Write-Host -ForegroundColor Green
}
else {
    "Skipping install of hex2dfu" | Write-Host -ForegroundColor Yellow
}

# set env variable, if not on Azure
if ($IsAzurePipelines -eq $False) {
    # need to replace forward slash for paths to work with GCC and CMake
    $Path = "$Path".Replace('\', '/')

    $env:HEX2DFU_PATH = $Path
    # this call can fail if the script is not run with appropriate permissions
    [System.Environment]::SetEnvironmentVariable("HEX2DFU_PATH", $env:HEX2DFU_PATH, "User")

    "Set User Environment HEX2DFU_PATH='" + $env:HEX2DFU_PATH + "'" | Write-Host -ForegroundColor Yellow
}
