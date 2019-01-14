# This PS installs Ninja build tool

if($psISE) { $PSScriptRoot = Split-Path -Path $psISE.CurrentFile.FullPath} #In case running in psISE
if(!$PSScriptRoot){ $PSScriptRoot = Split-Path $MyInvocation.MyCommand.Path -Parent } # or older PS
#Set location of nf-interpreter top-level
$nfRoot = "$PSScriptRoot\.."
$zipRoot = "$nfRoot\zips\"

# check if path already exists
$NinjaPathExists = Test-Path $env:NINJA_PATH -ErrorAction SilentlyContinue

If($NinjaPathExists -eq $False)
{
    Write-Host "Downloading Ninja..."

    $url = "https://github.com/ninja-build/ninja/releases/download/v1.8.2/ninja-win.zip"
    $output = "$zipRoot\ninja.zip"
     # Stop security tripping us up
    [Net.ServicePointManager]::SecurityProtocol = "tls12, tls11, tls"

    # Create directory for zip files if necessary
    md -Force $zipRoot | Out-Null

    # download zip with Ninja tool
    (New-Object Net.WebClient).DownloadFile($url, $output)

    Write-Host "Installing Ninja..."

    # unzip
    Expand-Archive $output -DestinationPath $env:NINJA_PATH
}
