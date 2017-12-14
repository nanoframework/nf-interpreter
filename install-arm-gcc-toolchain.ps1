# This PS installs the ARM GNU GCC toolchain from out Bintray repository if it's not already available

# check if path already exists
$GnuGccPathExists = Test-Path $env:GNU_GCC_TOOLCHAIN_PATH -ErrorAction SilentlyContinue

If($GnuGccPathExists -eq $False)
{
    Install-Module 7Zip4PowerShell -Force -Verbose

    Write-Host "Downloading ARM GNU GCC toolchain..."

    $url = "https://bintray.com/nfbot/internal-build-tools/download_file?file_path=gcc-arm-none-eabi-6-2017-q2-update-win32.7z"
    $output = "$PSScriptRoot\gcc-arm.7z"
    
    # download 7zip with toolchain
    (New-Object Net.WebClient).DownloadFile($url, $output)

    Write-Host "Installing ARM GNU GCC toolchain..."
    
    # unzip toolchain
    Expand-7Zip -ArchiveFileName $output -TargetPath $env:GNU_GCC_TOOLCHAIN_PATH
}
