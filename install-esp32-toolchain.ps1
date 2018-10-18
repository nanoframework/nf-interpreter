# This PS installs the xtensa ESP32 toolchain from Espressif downloads repository 
# if it's not already available at ESP32_TOOLCHAIN_PATH

# check if path already exists
$XtensaPathExists = Test-Path $env:ESP32_TOOLCHAIN_PATH -ErrorAction SilentlyContinue

If($XtensaPathExists -eq $False)
{
    Install-Module 7Zip4PowerShell -Force

    Write-Host "Downloading Xtensa ESP32 toolchain..."

    # Download xtensa ESP32 toolchain and install
    $url = "https://dl.espressif.com/dl/xtensa-esp32-elf-win32-1.22.0-80-g6c4433a-5.2.0.zip"
    $output = "$PSScriptRoot\xtensa-esp32-elf-win32.zip"
    
    # download 7zip with toolchain
    (New-Object Net.WebClient).DownloadFile($url, $output)

    Write-Host "Installing Xtensa ESP32 toolchain..."
    
    # unzip toolchain
    Expand-7Zip -ArchiveFileName $output -TargetPath "$env:ESP32_TOOLCHAIN_PATH\.."
}
