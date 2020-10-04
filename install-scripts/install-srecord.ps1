# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

# This PS installs srecord tool

[CmdletBinding(SupportsShouldProcess = $true)]
param (
    [Parameter(HelpMessage = "Path to the folder where the tool is to be installed.")][string]$Path,
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
    $Path = $Path + "\srecord"
}

# check if path already exists
$ninjaPathExists = Test-Path $Path -ErrorAction SilentlyContinue

If ($ninjaPathExists -eq $False -or $force) {
    $url = "https://bintray.com/nfbot/internal-build-tools/download_file?file_path=srecord-1.64-win32.zip"
    $output = "$zipRoot\srecord.zip"
   
    # Don't download again if already exists
    if (![System.IO.File]::Exists($output) -or $force) {
        "Downloading SRecord..." | Write-Host -ForegroundColor White -NoNewline

        # Stop security tripping us up
        [Net.ServicePointManager]::SecurityProtocol = "tls12, tls11, tls"

        # download zip with Ninja tool
        (New-Object Net.WebClient).DownloadFile($url, $output)

        "OK" | Write-Host -ForegroundColor Green
    }

    # unzip to install path, if not on Azure
    if ($IsAzurePipelines -eq $False) {
        "Installing SRecord..." | Write-Host -ForegroundColor White -NoNewline

        # unzip tool
        Expand-Archive $output -DestinationPath $Path > $null

        "OK" | Write-Host -ForegroundColor Green
    }
}
else {
    "Skipping install of SRecord" | Write-Host -ForegroundColor Yellow
}

# set env variable, if not on Azure
if ($IsAzurePipelines -eq $False) {
    # need to replace forward slash for paths to work with GCC and CMake
    $Path = "$Path".Replace('\', '/')

    $env:SRECORD_PATH = $Path
    # this call can fail if the script is not run with appropriate permissions
    [System.Environment]::SetEnvironmentVariable("SRECORD_PATH", $env:SRECORD_PATH, "User")

    "Set User Environment SRECORD_PATH='" + $env:SRECORD_PATH + "'" | Write-Host -ForegroundColor Yellow
}

# on Azure, adjust SRecord path
if ($IsAzurePipelines -eq $True) {
    # need to replace forward slash for paths to work with GCC and CMake
    $newPath = "$env:Agent_TempDirectory".Replace('\', '/')
    Write-Host "$("##vso[task.setvariable variable=SRECORD_PATH]")$newPath"
}
