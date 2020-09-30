# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

# This PS installs the ARM GNU GCC toolchain from our Bintray repository if it's not already available

[CmdletBinding(SupportsShouldProcess = $true)]
param (
    [Parameter(HelpMessage = "Path to the folder where the tool is to be installed.")][string]$Path,
    [Parameter(HelpMessage = "GNU GCC version requested.")][string]$Version,
    [switch]$force = $false
)

# set default GNU GCC version
if ([string]::IsNullOrEmpty($Version)) {
    $Version = "9-2020-q2-update"
}


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
    $Path = $Path + "\GNU_Tools_ARM_Embedded\$Version"
}

# check if path already exists
$gnuGccPathExists = Test-Path $Path -ErrorAction SilentlyContinue    

# download, if needed
If ($gnuGccPathExists -eq $False -or $force) {
    $url = "https://bintray.com/nfbot/internal-build-tools/download_file?file_path=gcc-arm-none-eabi-$Version-win32.7z"
    $output = "$zipRoot\gcc-arm.7z"

    # Don't download again if already exists
    if (![System.IO.File]::Exists($output) -or $force) {
        "Download URL is: '$url'" | Write-Host -ForegroundColor White

        "Downloading ARM GNU GCC toolchain..." | Write-Host -ForegroundColor White -NoNewline

        # Stop security tripping us up
        [Net.ServicePointManager]::SecurityProtocol = "tls12, tls11, tls"
            
        # download 7zip with toolchain
        (New-Object Net.WebClient).DownloadFile($url, $output)

        "OK" | Write-Host -ForegroundColor Green
    }

    # unzip to install path, if not on Azure
    if ($IsAzurePipelines -eq $False) {
        # Install 7Zip4PowerShell module from PSGallery if not already installed
        Install-Module -Name 7Zip4Powershell -RequiredVersion 1.10.0 -Scope CurrentUser

        "Installing ARM GNU GCC toolchain..." | Write-Host -ForegroundColor White -NoNewline

        # unzip toolchain
        Expand-7Zip -ArchiveFileName $output -TargetPath $Path > $null

        "OK" | Write-Host -ForegroundColor Green
    }
}
else {
    "Skipping install of ARM GNU GCC toolchain" | Write-Host -ForegroundColor Yellow
}

# set env variable, if not on Azure
if ($IsAzurePipelines -eq $False) {
    # need to replace forward slash for paths to work with GCC and CMake
    $Path = "$Path".Replace('\', '/')

    $env:GNU_GCC_TOOLCHAIN_PATH = $Path
    # this call can fail if the script is not run with appropriate permissions
    [System.Environment]::SetEnvironmentVariable("GNU_GCC_TOOLCHAIN_PATH", $env:GNU_GCC_TOOLCHAIN_PATH, "User")

    "Set User Environment GNU_GCC_TOOLCHAIN_PATH='" + $env:GNU_GCC_TOOLCHAIN_PATH + "'" | Write-Host -ForegroundColor Yellow
}
