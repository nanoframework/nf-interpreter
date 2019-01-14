# This PS installs the ARM GNU GCC toolchain from our Bintray repository if it's not already available

if($psISE) { $PSScriptRoot = Split-Path -Path $psISE.CurrentFile.FullPath} #In case running in psISE
if(!$PSScriptRoot){ $PSScriptRoot = Split-Path $MyInvocation.MyCommand.Path -Parent } # or older PS
#Set location of nf-interpreter top-level
$nfRoot = "$PSScriptRoot\.."
$zipRoot = "$nfRoot\zips\"

# check if path already exists
$GnuGccPathExists = Test-Path $env:GNU_GCC_TOOLCHAIN_PATH -ErrorAction SilentlyContinue

If($GnuGccPathExists -eq $False)
{
    Write-Host "Downloading ARM GNU GCC toolchain..."
    
    $url = "https://bintray.com/nfbot/internal-build-tools/download_file?file_path=gcc-arm-none-eabi-7-2018-q2-update-win32.7z"
    $output = "$zipRoot\gcc-arm.7z"
    
     # Stop security tripping us up
    [Net.ServicePointManager]::SecurityProtocol = "tls12, tls11, tls"

    # Create directory for zip files if necessary
    md -Force $zipRoot | Out-Null
        
    # download 7zip with toolchain
    (New-Object Net.WebClient).DownloadFile($url, $output)

    # Install 7Zip4PowerShell module from PSGallery if not already installed
    Install-Module -Name 7Zip4Powershell -RequiredVersion 1.8.0 -Scope CurrentUser

    Write-Host "Installing ARM GNU GCC toolchain..."
    
    # unzip toolchain
    Expand-7Zip -ArchiveFileName $output -TargetPath $env:GNU_GCC_TOOLCHAIN_PATH
}
