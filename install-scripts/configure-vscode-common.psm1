# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

# patterns to be replaced in the templates
$toolchainPattern = "<absolute-path-to-the-toolchain-folder-mind-the-forward-slashes>"
$esp32ToolchainPattern = "<absolute-path-to-the-esp32-toolchain-folder-mind-the-forward-slashes>"
$repoRootPattern = "<path-to-the-repo-root-mind-the-forward-slash>"
$openOcdPattern = "<absolute-path-to-openocd-mind-the-forward-slashes>"
$esp32OpenOcdPattern = "<absolute-path-to-esp32-openocd-mind-the-forward-slashes>"
$buildFolderPattern = "<absolute-path-to-the-build-folder-mind-the-forward-slashes>"
$esp32BootloaderFolderPattern = "<absolute-path-to-the-esp32-bootloader-folder-mind-the-forward-slashes>"
$esp32IdfFolderPattern = "<absolute-path-to-the-IDF-folder-mind-the-forward-slashes>"
$esp32LibsFolderPattern = "<absolute-path-to-the-esp32-libs-folder-mind-the-forward-slashes>"
$hex2dfuPattern = "<absolute-path-to-hex2dfu-mind-the-forward-slashes>"
$srecordPattern = "<absolute-path-to-srecord-folder-mind-the-forward-slashes>"

function Get-RepoPath {
    return Join-Path -Path $PSScriptRoot -ChildPath "\.." -Resolve
}

function Get-VSCodeFolderPath {
    return Join-Path -Path $(Get-RepoPath) -ChildPath ".vscode"
}

<#
.SYNOPSIS
Replaces various patterns present in the templates to with the correct paths for the current setup. 
#>
function Update-Paths {
    param (
        $filePath
    )

    # load content as string to allow replacing content
    $content = Get-Content -Path $filePath | Out-String

    # replace patterns in the file content
    # to be safe, make sure to replace the paths with the forward slashes

    if($env:GNU_GCC_TOOLCHAIN_PATH)
    {
        $content = $content -replace $toolchainPattern, $env:GNU_GCC_TOOLCHAIN_PATH.Replace('\', '/')
    }

    if($env:OPENOCD_PATH)
    {
        $content = $content -replace $openOcdPattern, $env:OPENOCD_PATH.Replace('\', '/')
    }

    if($env:HEX2DFU_PATH)
    {
        $content = $content -replace $hex2dfuPattern, $env:HEX2DFU_PATH.Replace('\', '/')
    }

    if($env:ESP32_TOOLCHAIN_PATH)
    {
        $content = $content -replace $esp32ToolchainPattern, $env:ESP32_TOOLCHAIN_PATH.Replace('\', '/')
    }

    if($env:ESP32_OPENOCD_PATH)
    {
        $content = $content -replace $esp32OpenOcdPattern, $env:ESP32_OPENOCD_PATH.Replace('\', '/')
    }
    
    if($env:ESP32_LIBS_PATH)
    {
        $content = $content -replace $esp32LibsFolderPattern, $env:ESP32_LIBS_PATH.Replace('\', '/')
        $content = $content -replace $esp32BootloaderFolderPattern, $env:ESP32_LIBS_PATH.Replace('\', '/')
    }

    if($env:ESP32_IDF_PATH)
    {
        $content = $content -replace $esp32IdfFolderPattern, $env:ESP32_IDF_PATH.Replace('\', '/')
    }

    if($env:SRECORD_PATH)
    {
        $content = $content -replace $srecordPattern, $env:SRECORD_PATH.Replace('\', '/')
    }

    $content = $content -replace $repoRootPattern, $(Get-RepoPath).Replace('\', '/')
    $content = $content -replace $buildFolderPattern, "$(Get-RepoPath)\build".Replace('\', '/')

    # save back to the file
    $content | Out-File -FilePath $filePath -Encoding utf8 -Force
}

<#
.SYNOPSIS
Repair unwanted patterns in json content result of a bad encoding.
#>
function Repair-Patterns {
    param (
        $filePath
    )

    # replace unwanted patterns
    $replacePatterns = @{
        "\\u003c" = "<"
        "\\u003e" = ">"
        "\\u0027" = "'"
    }

    $inputJson = Get-Content -Path $filePath | Out-String

    foreach ($pattern in $replacePatterns.GetEnumerator())
    {
        $inputJson = $inputJson -replace $pattern.Key, $pattern.Value
    }

    $inputJson | Out-File -FilePath $filePath -Encoding utf8 -Force
}
