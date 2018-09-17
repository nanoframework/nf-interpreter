# This PS installs the OpenOCD ESP32 toolchain from Espressif downloads repository 
# if it's not already available at ESP32_TOOLS_PATH\openocd-esp32

$env:ESP32_OPENOCD_PATH=($env:ESP32_TOOLS_PATH+'\openocd-esp32') 
# check if path already exists
$OpenOCDPathExists = Test-Path $env:ESP32_OPENOCD_PATH -ErrorAction SilentlyContinue

If($OpenOCDPathExists -eq $False)
{
    Write-Host "Downloading OpenOCD for ESP32 ..."

    $url = "https://github.com/espressif/openocd-esp32/releases/download/v0.10.0-esp32-20180724/openocd-esp32-win32-0.10.0-esp32-20180724.zip"
    $output = "$PSScriptRoot\openocd-esp32.zip"

    # download zip with OpenOcd tool
    (New-Object Net.WebClient).DownloadFile($url, $output)

    Write-Host "Installing OpenOCD..."

    # unzip
    #Expand-Archive $output -DestinationPath $env:ESP32_OPENOCD_PATH
	
	Expand-Archive $output -DestinationPath "$PSScriptRoot\openocd-esp32"
	Move-Item -Path ("$PSScriptRoot\openocd-esp32\openocd-esp32\") -Destination $env:ESP32_OPENOCD_PATH
	Remove-Item "$PSScriptRoot\openocd-esp32" -Force
}
