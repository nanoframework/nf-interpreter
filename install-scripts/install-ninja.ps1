# This PS installs Ninja build tool

# check if path already exists
$NinjaPathExists = Test-Path $env:NINJA_PATH -ErrorAction SilentlyContinue

If($NinjaPathExists -eq $False)
{
    Write-Host "Downloading Ninja..."

    $url = "https://github.com/ninja-build/ninja/releases/download/v1.8.2/ninja-win.zip"
    $output = "$PSScriptRoot\ninja.zip"

    # download zip with Ninja tool
    (New-Object Net.WebClient).DownloadFile($url, $output)

    Write-Host "Installing Ninja..."

    # unzip
    Expand-Archive $output -DestinationPath $env:NINJA_PATH
}
