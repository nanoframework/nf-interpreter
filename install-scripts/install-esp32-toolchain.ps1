# This PS installs the xtensa ESP32 toolchain from Espressif downloads repository 
# if it's not already available at ESP32_TOOLCHAIN_PATH

#Set script path in case running in psISE
if(!$PSScriptRoot){ $PSScriptRoot = Split-Path $MyInvocation.MyCommand.Path -Parent }
#Set location of nf-interpreter top-level
$nfRoot = "$PSScriptRoot\.."

# check if path already exists
$XtensaPathExists = Test-Path $env:ESP32_TOOLCHAIN_PATH -ErrorAction SilentlyContinue

If($XtensaPathExists -eq $False)
{
    Write-Host "Downloading Xtensa ESP32 toolchain..."

    # Download xtensa ESP32 toolchain and install
    $url = "https://dl.espressif.com/dl/xtensa-esp32-elf-win32-1.22.0-80-g6c4433a-5.2.0.zip"
    $output = "$nfRoot\xtensa-esp32-elf-win32.zip"
    
    # download 7zip with toolchain
    (New-Object Net.WebClient).DownloadFile($url, $output)

    Write-Host "Installing Xtensa ESP32 toolchain..."
    
    # unzip toolchain (This used the older of the two Expand-7Zip addons)
    #Expand-7Zip -ArchiveFileName $output -TargetPath $env:ESP32_TOOLCHAIN_PATH

    #unzip using PowerShell 5+ inbuilt command
    Expand-Archive -Path $output -DestinationPath $env:ESP32_TOOLCHAIN_PATH
}
