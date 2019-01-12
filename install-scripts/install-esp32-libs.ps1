# This PS installs the pre-compiled EPS32 libraries from our Bintray repository

#Set script path in case running in psISE
if(!$PSScriptRoot){ $PSScriptRoot = Split-Path $MyInvocation.MyCommand.Path -Parent }
#Set location of nf-interpreter top-level
$nfRoot = "$PSScriptRoot\.."
$zipRoot = "$nfRoot\zips\"

# check if path already exists
$ESP32LibPathExists = Test-Path $env:ESP32_LIBS_PATH -ErrorAction SilentlyContinue

If($ESP32LibPathExists -eq $False)
{
    Write-Host "Downloading ESP32 pre-compiled libs..."

    $url = "https://bintray.com/nfbot/internal-build-tools/download_file?file_path=IDF_libs-v3.1.zip"
    $output = "$zipRoot\IDF_libs-v3.1.zip"
    
    # Stop security tripping us up
    [Net.ServicePointManager]::SecurityProtocol = "tls12, tls11, tls"

    # Create directory for zip files if necessary
    md -Force $zipRoot | Out-Null

    # download zip IDF
    (New-Object Net.WebClient).DownloadFile($url, $output)

    Write-Host "Installing ESP32 pre-compiled libs..."
    
    # unzip libs
    #Expand-7Zip -ArchiveFileName $output -TargetPath $env:ESP32_LIBS_PATH
    #unzip using PowerShell 5+ inbuilt command
    Expand-Archive -Path $output -DestinationPath $env:ESP32_LIBS_PATH
}
