# This PS installs nanoFramework hex2dfu command line utility

# check if path already exists
$hex2dfuPathExists = Test-Path $env:HEX2DFU_PATH -ErrorAction SilentlyContinue

If($hex2dfuPathExists -eq $False)
{
    & md $($env:HEX2DFU_PATH) > $null

    Write-Host "Downloading hex2dfu..."

    $url = "https://github.com/nanoframework/nf-tools/releases/download/v1.0.0/hex2dfu.exe"

    $output = "$env:HEX2DFU_PATH\hex2dfu.exe"
    
    # Stop security tripping us up
    [Net.ServicePointManager]::SecurityProtocol = "tls12, tls11, tls"

    # download executable with utility
    (New-Object Net.WebClient).DownloadFile($url, $output)
}
