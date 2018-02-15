# This PS installs the ARM GNU GCC toolchain from our Bintray repository if it's not already available

# check if path already exists
$GnuGccPathExists = Test-Path $env:GNU_GCC_TOOLCHAIN_PATH -ErrorAction SilentlyContinue

If($GnuGccPathExists -eq $False)
{
    Install-Module 7Zip4PowerShell -Force -Verbose

    Write-Host "Downloading ARM GNU GCC toolchain..."
    #todo: add file and re-enable
    #$url = "https://bintray.com/nfbot/internal-build-tools/download_file?file_path=gcc-arm-none-eabi-6-2017-q2-update-win32.7z"
    $url = "https://developer.arm.com/-/media/Files/downloads/gnu-rm/7-2017q4/gcc-arm-none-eabi-7-2017-q4-major-win32.zip?revision=df1b65d3-7c8d-4e82-b114-e0b6ad7c2e6d?product=GNU%20Arm%20Embedded%20Toolchain,ZIP,,Windows,7-2017-q4-major"
    #$output = "$PSScriptRoot\gcc-arm.7z"
    $output = "$PSScriptRoot\gcc-arm.zip"
    
    # download 7zip with toolchain
    (New-Object Net.WebClient).DownloadFile($url, $output)

    Write-Host "Installing ARM GNU GCC toolchain..."
    
    # unzip toolchain
    Expand-7Zip -ArchiveFileName $output -TargetPath $env:GNU_GCC_TOOLCHAIN_PATH
}
