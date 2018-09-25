# This PS installs the pre-compiled EPS32 libraries from our Bintray repository

# check if path already exists
$ESP32LibPathExists = Test-Path $env:ESP32_LIBS_PATH -ErrorAction SilentlyContinue

If($ESP32LibPathExists -eq $False)
{
    Install-Module 7Zip4PowerShell -Force

    Write-Host "Downloading ESP32 pre-compiled libs..."

    $url = "https://bintray.com/nfbot/internal-build-tools/download_file?file_path=IDF_libs-v3.1.zip"
    $output = "$PSScriptRoot\IDF_libs-v3.1.zip"
 
    # download zip IDF
    (New-Object Net.WebClient).DownloadFile($url, $output)

    Write-Host "Installing ESP32 pre-compiled libs..."
    
    # unzip libs
    Expand-7Zip -ArchiveFileName $output -TargetPath $env:ESP32_LIBS_PATH
}
