Write-Host "launch.json auto generation script for debugging nanoFramework"
Write-Host "______________________________________________________________"


If (Test-Path .\.vscode\launch.json){
  # File exists
    Read-Host "WARNING, This file will overwrite the existing launch.json, Press any key to continue..."
  }

Write-Host "creating a copy of the template..."
Copy-Item ".\.vscode\launch.TEMPLATE.json" -Destination ".\.vscode\launch.json"


Write-Host "resolving build folder path..."
$buildFolderPath = Resolve-Path .\build
(Get-Content .\.vscode\launch.json).replace('<absolute-path-to-the-build-folder-mind-the-forward-slashes>', $buildFolderPath.ToString().Replace("\", "/")) | Set-Content .\.vscode\launch.json


Write-Host "Searching for the GCC toolchain..."
#likely locations of the GCC toolchain (newest first).
$arrayGccPaths = @(
"C:\Program Files (x86)\GNU Tools ARM Embedded\6 2017-q4-update\bin\arm-none-eabi-gdb.exe",
"C:\Program Files (x86)\GNU Tools ARM Embedded\6 2017-q3-update\bin\arm-none-eabi-gdb.exe",
"C:\Program Files (x86)\GNU Tools ARM Embedded\6 2017-q2-update\bin\arm-none-eabi-gdb.exe", 
"C:\Program Files (x86)\GNU Tools ARM Embedded\6 2017-q1-update\bin\arm-none-eabi-gdb.exe"
)

foreach ($element in $arrayGccPaths) {
If (Test-Path $element){
  # File exists
  (Get-Content .\.vscode\launch.json).replace('<path-to-gdb-inside-the-gcc-toolchain-mind-the-forward-slash>', $element.ToString().Replace("\", "/")) | Set-Content .\.vscode\launch.json
  Write-Host "found at: $element..."
  }
}

While ((Get-Content .\.vscode\launch.json).Contains('<path-to-gdb-inside-the-gcc-toolchain-mind-the-forward-slash>')) {
Write-Host "Unable to locate the GCC toolchain..."
$gccFilePathStr = Read-Host 'Please enter the full (absolute) path to the GCC toolchain'
  If (Test-Path $gccFilePathStr){
  # File exists
  (Get-Content .\.vscode\launch.json).replace('<path-to-gdb-inside-the-gcc-toolchain-mind-the-forward-slash>', $gccFilePathStr.ToString().Replace("\", "/")) | Set-Content .\.vscode\launch.json
  }
}




#find path to open OCD
Write-Host "Searching for Open OCD..."
#likely locations of open OCD.
$arrayOcdPaths = @(
"C:\Program Files (x86)\openocd-0.10.0", 
"C:\Program Files\GNU ARM Eclipse\OpenOCD\0.10.0-201704182147-dev"
)

$ocdFound = $false
foreach ($element in $arrayOcdPaths) {
If (Test-Path "$element\bin\openocd.exe"){
  # File exists
  (Get-Content .\.vscode\launch.json).replace('C:/Program Files (x86)/openocd-0.10.0', $element.ToString().Replace("\", "/")) | Set-Content .\.vscode\launch.json
  $ocdFound = $true
  Write-Host "found at: $element..."
  }
}

While (-not $ocdFound) {
Write-Host "Unable to locate Open OCD..."
$ocdFilePathStr = Read-Host 'Please enter the full (absolute) path to Open OCDs exe'
  If (Test-Path $ocdFilePathStr){
  # File exists
  (Get-Content .\.vscode\launch.json).replace('C:/Program Files (x86)/openocd-0.10.0', $ocdFilePathStrt.ToString().TrimEnd("\bin\openocd.exe").Replace("\", "/")) | Set-Content .\.vscode\launch.json
  $ocdFound = $true
  }
}



$boardConfigFile = Read-Host 'Enter the board series e.g. stm32f7discovery (valid types found in the scripts\boards folder of the openOCD path)'
$boardConfigStr = $boardConfigFile.TrimEnd('.cfg')
#we should perform validation on this
(Get-Content .\.vscode\launch.json).replace('<board-config-file>', $boardConfigStr) | Set-Content .\.vscode\launch.json

(Get-Content .\.vscode\launch.json).replace('name the launch configuration 1 (booter) here', "$boardConfigStr (Booter)") | Set-Content .\.vscode\launch.json
(Get-Content .\.vscode\launch.json).replace('name the launch configuration 2 (CLR) here', "$boardConfigStr (CLR)") | Set-Content .\.vscode\launch.json



$interfaceConfigFile =  Read-Host 'enter the interface used for connecting to the board e.g. stlink-v2 (valid types found in the scripts\interface folder of the openOCD path)'

#we should perform validation on this
(Get-Content .\.vscode\launch.json).replace('<interface-config-file>', $interfaceConfigFile.TrimEnd('.cfg')) | Set-Content .\.vscode\launch.json

Write-Host "launch.json generated."