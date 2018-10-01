# Copyright (c) 2018 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.

# this evaluates if it's OK to exit the build even before starting it

if($env:APPVEYOR_REPO_COMMIT_AUTHOR -eq "nfbot" -and $env:APPVEYOR_REPO_COMMIT_MESSAGE_EXTENDED -like "[version update]")
{
    # commit was from nfbot AND it's taged as a version update]
    # OK to skip AppVeyor build 
    "Version update only. Skipping build." | Write-Host -BackgroundColor Blue -ForegroundColor White 
    Exit-AppveyorBuild
}