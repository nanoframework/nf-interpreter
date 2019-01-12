# This PS installs IDF esp32 sdk

#Set script path in case running in psISE
if(!$PSScriptRoot){ $PSScriptRoot = Split-Path $MyInvocation.MyCommand.Path -Parent }
#Set location of nf-interpreter top-level
$nfRoot = "$PSScriptRoot\.."
$zipRoot = "$nfRoot\zips\"

# check if path already exists
$Esp32IdfPathExists = Test-Path $env:ESP32_IDF_PATH -ErrorAction SilentlyContinue

If($Esp32IdfPathExists -eq $False)
{
    Write-Host "Downloading ESP32 IDF..."

    $url = "https://github.com/espressif/esp-idf/releases/download/v3.1/esp-idf-v3.1.zip"
    $output = "$zipRoot\esp-idf-v3.1.zip"
    
     # Stop security tripping us up
    [Net.ServicePointManager]::SecurityProtocol = "tls12, tls11, tls"

    # Create directory for zip files if necessary
    md -Force $zipRoot | Out-Null
        
    # download zip with Espressif IDF
    (New-Object Net.WebClient).DownloadFile($url, $output)

    Write-Host "Installing Espressif IDF..."

    # unzip
    Expand-Archive $output -DestinationPath $env:ESP32_TOOLS_PATH
}

Write-Host "Installing python pyserial..."
# Make sure serial package is installed in python otherwise
# the esptool.py tool won't run
python -m pip install pyserial
