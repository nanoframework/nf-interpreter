# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

[CmdletBinding(SupportsShouldProcess = $true)]
param (
    [Parameter(Mandatory=$true)]
    [ValidateSet("ESP32", "STM32", "NXP", "TI")]
    [string]$TargetSeries,
	[Parameter(HelpMessage = "Enter the path to the folder where the tools should be installed.")][string]$Path,
    [switch]$force = $false
)

$commandArgs = ""

# any path set?
if (-not [string]::IsNullOrEmpty($Path)) {
    $commandArgs = " -Path '$Path'"
}

# need to pass the 'force' switch?
if ($force) {
    $commandArgs += " -force"
}

if($TargetSeries -eq "ESP32")
{
    Invoke-Expression "$PSScriptRoot\install-esp32-tools.ps1 $commandArgs"
}
elseif ($TargetSeries -eq "STM32") {
    Invoke-Expression "$PSScriptRoot\install-stm32-tools.ps1 $commandArgs"
}
elseif ($TargetSeries -eq "NXP") {
    Invoke-Expression "$PSScriptRoot\install-stm32-tools.ps1 $commandArgs"
    Invoke-Expression "$PSScriptRoot\install-srecord.ps1 $commandArgs"
}
elseif ($TargetSeries -eq "TI") {
    Invoke-Expression "$PSScriptRoot\install-stm32-tools.ps1 $commandArgs"
}
else {
    # shouldn't reach here
}
