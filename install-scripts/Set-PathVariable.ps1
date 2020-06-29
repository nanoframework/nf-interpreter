## Based on Hahndorf's awesome collection of PowerShell scripts.
## https://github.com/hahndorf/hacops

[CmdletBinding(SupportsShouldProcess = $true)]
Param(
    [parameter(Mandatory = $true)]
    [string]$NewLocation)

Begin {

    # requires –runasadministrator

    $regPath = "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"
    $hklm = [Microsoft.Win32.Registry]::LocalMachine

    Function GetOldPath() {     
        $regKey = $hklm.OpenSubKey($regPath, $FALSE)
        $envpath = $regKey.GetValue("Path", "", [Microsoft.Win32.RegistryValueOptions]::DoNotExpandEnvironmentNames)
        return $envPath
    }
}

Process {
    # Win32API error codes
    $ERROR_SUCCESS = 0
    $ERROR_DUP_NAME = 34 
    $ERROR_INVALID_DATA = 13

    $NewLocation = $NewLocation.Trim();

    If ($NewLocation -eq "" -or $NewLocation -eq $null) {
        Exit $ERROR_INVALID_DATA
    }
   
    [string]$oldPath = GetOldPath
    Write-Verbose "Old Path: $oldPath"

    # check whether the new location is already in the path
    $parts = $oldPath.split(";")
    If ($parts -contains $NewLocation) {
        Write-Warning ("The new location is already in the path ('" + $NewLocation + "')")
        Exit $ERROR_DUP_NAME        
    }

    # build the new path, make sure we don't have double semicolons
    $newPath = $oldPath + ";" + $NewLocation
    $newPath = $newPath -replace ";;", ";"

    if ($pscmdlet.ShouldProcess("%Path%", "Set $newPath")) {
        # add to the current session
        $env:path += ";$NewLocation"
        # save into registry
        $regKey = $hklm.OpenSubKey($regPath, $True)
        $regKey.SetValue("Path", $newPath, [Microsoft.Win32.RegistryValueKind]::ExpandString)
        Write-Output "The operation completed successfully."
    }

    Exit $ERROR_SUCCESS        
}

<#
.SYNOPSIS
    Adds a new item to the machine path environment variable
.DESCRIPTION
    Most more popular methods to change the %path% actually break it.
.PARAMETER NewLocation
    The new directory location to add at the end of the path. Can be an variable itself.
.EXAMPLE
   Set-PathVariable.ps1 -NewLocation "%bin%"
   Adds the string "%bin%" to the path, when using it it is expanded.
.NOTES
    Tested on Windows 10 and Server 2016
    Author:  Peter Hahndorf
    Created: October 7th, 2016   
.LINK
    https://github.com/hahndorf/hacops
    https://peter.hahndorf.eu/blog/AddingToPathVariable
#>
