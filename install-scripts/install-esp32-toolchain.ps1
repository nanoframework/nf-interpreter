# This PS installs the xtensa ESP32 toolchain from Espressif downloads repository 
# if it's not already available at ESP32_TOOLCHAIN_PATH

if($psISE) { $PSScriptRoot = Split-Path -Path $psISE.CurrentFile.FullPath} #In case running in psISE
if(!$PSScriptRoot){ $PSScriptRoot = Split-Path $MyInvocation.MyCommand.Path -Parent } # or older PS
#Set location of nf-interpreter top-level
$nfRoot = "$PSScriptRoot\.."
$zipRoot = "$nfRoot\zips\"

# check if path already exists
$XtensaPathExists = Test-Path $env:ESP32_TOOLCHAIN_PATH -ErrorAction SilentlyContinue

If($XtensaPathExists -eq $False)
{
    # Download xtensa ESP32 toolchain and install

    $url = "https://dl.espressif.com/dl/xtensa-esp32-elf-win32-1.22.0-80-g6c4433a-5.2.0.zip"
    $output = "$zipRoot\xtensa-esp32-elf-win32.zip"
    
    # Don't download again if already exists. User can remove from zips to force... 
    if(![System.IO.File]::Exists($output)){
         Write-Host "Downloading Xtensa ESP32 toolchain..."
       
         # Stop security tripping us up
        [Net.ServicePointManager]::SecurityProtocol = "tls12, tls11, tls"

        # Create directory for zip files if necessary
        md -Force $zipRoot | Out-Null

        # download zip with toolchain
        (New-Object Net.WebClient).DownloadFile($url, $output)
    }

    Write-Host "Installing Xtensa ESP32 toolchain..."
    
    # unzip toolchain (This used the older of the two Expand-7Zip addons)
    #Expand-7Zip -ArchiveFileName $output -TargetPath $env:ESP32_TOOLCHAIN_PATH

    #unzip using PowerShell 5+ inbuilt command
    Expand-Archive -Path $output -DestinationPath $env:ESP32_TOOLCHAIN_PATH
}
