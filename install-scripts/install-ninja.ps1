# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

# This PS installs Ninja build tool

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
    $Path = $Path + "\Ninja"
}

# check if path already exists
$ninjaPathExists = Test-Path $Path -ErrorAction SilentlyContinue

If ($ninjaPathExists -eq $False -or $force) {
    $url = "https://github.com/ninja-build/ninja/releases/download/v1.10.0/ninja-win.zip"
    $output = "$zipRoot\ninja.zip"
   
    # Don't download again if already exists
    if (![System.IO.File]::Exists($output) -or $force) {
        "Downloading Ninja..." | Write-Host -ForegroundColor White -NoNewline

        # Stop security tripping us up
        [Net.ServicePointManager]::SecurityProtocol = "tls12, tls11, tls"

        # download zip with Ninja tool
        (New-Object Net.WebClient).DownloadFile($url, $output)

        "OK" | Write-Host -ForegroundColor Green
    }

    # unzip to install path, if not on Azure
    if ($IsAzurePipelines -eq $False) {
        "Installing Ninja..." | Write-Host -ForegroundColor White -NoNewline

        # unzip tool
        Expand-Archive $output -DestinationPath $Path > $null

        "OK" | Write-Host -ForegroundColor Green
    }
}
else {
    "Skipping install of Ninja" | Write-Host -ForegroundColor Yellow
}

# set env variable, if not on Azure
if ($IsAzurePipelines -eq $False) {
    $env:NINJA_PATH = $Path
    # this call can fail if the script is not run with appropriate permissions
    [System.Environment]::SetEnvironmentVariable("NINJA_PATH", $env:NINJA_PATH, "User")

    "Set User Environment NINJA_PATH='" + $env:NINJA_PATH + "'" | Write-Host -ForegroundColor Yellow
}

# if not on Azure, add ninja to the PATH
if ($IsAzurePipelines -eq $False) {

    # check if it's already there
    $ninja = (Get-Command "ninja.exe" -ErrorAction SilentlyContinue)
    if ($ninja) {
        # reachable, we're good
        exit 0
    }

    # add to path
    try {
        [Environment]::SetEnvironmentVariable(
            "Path",
            [Environment]::GetEnvironmentVariable("Path", "User") + ";$Path",
            "User")      

        # this call can fail if the script is not run with appropriate permissions
        [Environment]::SetEnvironmentVariable(
            "Path",
            [Environment]::GetEnvironmentVariable("Path", [EnvironmentVariableTarget]::Machine) + ";$Path",
            "Machine")      

        "Add ninja to PATH" | Write-Host -ForegroundColor White
    }
    catch {
        "Fail to update PATH environment variable. Make sure to manually add '$Path' to the PATH." | Write-Host -ForegroundColor Red
    }
}
