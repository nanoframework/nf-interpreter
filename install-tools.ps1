 [CmdletBinding(SupportsShouldProcess = $true)]
 param (
    [Parameter(Mandatory=$true,HelpMessage="Please enter one of the following NANOCLR_WINDOWS, ESP32_DEVKITC, STM32.",Position=1)][string]$BOARD_NAME = "ESP32_DEVKITC",
	[Parameter(Mandatory=$true,HelpMessage="Please enter the COM port for NANOCLR [e.g. COM1 or /dev/ttyUSB0].",Position=2)][string]$COMPORT = "COM1",
	[switch]$force = $false
 )


$BOARD_NAMES = "NANOCLR_WINDOWS", "ESP32_DEVKITC", "STM32"

[Net.ServicePointManager]::SecurityProtocol = "tls12, tls11, tls"
If ($BOARD_NAMES -contains $BOARD_NAME)
{
	Write-Host "Install Tools to build nanoCLR into default folders and configure json files ..."
	$env:BOARD_NAME = $BOARD_NAME
	$env:NANOCLR_COMPORT = $COMPORT
	
	Write-Host "BOARD_NAME=" $env:BOARD_NAME

	If($env:BOARD_NAME -eq "NANOCLR_WINDOWS")
	{

	}
	ElseIf($env:BOARD_NAME -eq "ESP32_DEVKITC")
	{
		Write-Host "Set User Environment Variables ... (can be slow if many Applications running.)"
		If([string]::IsNullOrEmpty($env:ESP32_TOOLS_PATH) -or $force)
		{
			$env:ESP32_TOOLS_PATH= 'C:\ESP32_TOOLS'
			Write-Host ("Set User Environment ESP32_TOOLS_PATH='"+$env:ESP32_TOOLS_PATH+"'")
			[System.Environment]::SetEnvironmentVariable("ESP32_TOOLS_PATH", $env:ESP32_TOOLS_PATH, "User")
		}
		If([string]::IsNullOrEmpty($env:ESP32_TOOLCHAIN_PATH) -or $force)
		{
			$env:ESP32_TOOLCHAIN_PATH= ($env:ESP32_TOOLS_PATH+'\1.22.0-80\xtensa-esp32-elf')
			Write-Host ("Set User Environment ESP32_TOOLCHAIN_PATH='"+$env:ESP32_TOOLCHAIN_PATH+"'")
			[System.Environment]::SetEnvironmentVariable("ESP32_TOOLCHAIN_PATH", $env:ESP32_TOOLCHAIN_PATH, "User")
		}
		If([string]::IsNullOrEmpty($env:ESP32_LIBS_PATH) -or $force)
		{
			$env:ESP32_LIBS_PATH=($env:ESP32_TOOLS_PATH+'\libs-v3.0')
			Write-Host ("Set User Environment ESP32_LIBS_PATH='"+$env:ESP32_LIBS_PATH+"'")
			[System.Environment]::SetEnvironmentVariable("ESP32_LIBS_PATH", $env:ESP32_LIBS_PATH, "User")
		}
		If([string]::IsNullOrEmpty($env:ESP32_IDF_PATH) -or [string]::IsNullOrEmpty($env:IDF_PATH) -or $force)
		{
			$env:IDF_PATH=$env:ESP32_IDF_PATH=($env:ESP32_TOOLS_PATH+'\esp-idf-v3.0')
			Write-Host ("Set User Environment ESP32_IDF_PATH='"+$env:ESP32_IDF_PATH+"'")
			Write-Host ("Set User Environment IDF_PATH='"+$env:IDF_PATH+"'")
			[System.Environment]::SetEnvironmentVariable("ESP32_IDF_PATH", $env:ESP32_IDF_PATH, "User")
			[System.Environment]::SetEnvironmentVariable("IDF_PATH", $env:IDF_PATH, "User")
		}
		If([string]::IsNullOrEmpty($env:NINJA_PATH) -or $force)
		{
			$env:NINJA_PATH= ($env:ESP32_TOOLS_PATH+'\ninja')
			Write-Host ("Set User Environment NINJA_PATH='"+$env:NINJA_PATH+"'")
			[System.Environment]::SetEnvironmentVariable("NINJA_PATH", $env:NINJA_PATH, "User")
		}
		
		
		Invoke-Expression -Command .\install-esp32-toolchain.ps1
		Invoke-Expression -Command .\install-esp32-libs.ps1
		Invoke-Expression -Command .\install-esp32-idf.ps1
		Invoke-Expression -Command .\install-ninja.ps1
		Invoke-Expression -Command .\install-esp32-openocd.ps1
			
		Write-Host ("Adding Ninja to the path "+$env:NINJA_PATH)
		.\Set-PathVariable.ps1 -NewLocation $env:NINJA_PATH
		
		$filePathExists = Test-Path "$PSScriptRoot\cmake-variants.json" -ErrorAction SilentlyContinue
		If($filePathExists -eq $False -or $force)
		{
			Copy-Item "$PSScriptRoot\cmake-variants.TEMPLATE-ESP32.json" -Destination "$PSScriptRoot\cmake-variants.json" -Force 
		}
		
		$filePathExists = Test-Path "$PSScriptRoot\.vscode\cmake-kits.json" -ErrorAction SilentlyContinue
		If($filePathExists -eq $False -or $force)
		{
			Copy-Item "$PSScriptRoot\.vscode\cmake-kits.TEMPLATE-ESP32.json" -Destination "$PSScriptRoot\.vscode\cmake-kits.json" -Force 
		}
		
		$filePathExists = Test-Path "$PSScriptRoot\.vscode\tasks.json" -ErrorAction SilentlyContinue
		If($filePathExists -eq $False -or $force)
		{
			Write-Host ("Create .\.vscode\tasks.json with install paths from .\vscode\tasks.TEMPLATE-ESP32.json")
			Copy-Item "$PSScriptRoot\.vscode\tasks.TEMPLATE-ESP32.json" -Destination "$PSScriptRoot\.vscode\tasks.json" -Force  
			$tasks = (Get-Content "$PSScriptRoot\.vscode\tasks.json")
			$tasks = $tasks.Replace('<absolute-path-to-the-IDF-folder-mind-the-forward-slashes>', $env:ESP32_IDF_PATH.ToString().Replace("\", "/")) 
			$tasks = $tasks.Replace('<COMPORT>', $env:NANOCLR_COMPORT.ToString().Replace("\", "/")) 
			Set-Content -Path "$PSScriptRoot\.vscode\tasks.json" -Value $tasks 
		}
		
		$filePathExists = Test-Path ".\.vscode\launch.json" -ErrorAction SilentlyContinue
		$filePathExists=$False
		If($filePathExists -eq $False -or $force)
		{
			Write-Host "Create .\.vscode\launch.json with install paths from .\vscode\launch.TEMPLATE-ESP32.json"
			Copy-Item ".\.vscode\launch.TEMPLATE-ESP32.json" -Destination ".\.vscode\launch.json" -Force
			$buildFolderPath = Resolve-Path .\build
			$launch = (Get-Content '.\.vscode\launch.json')
			$launch = $launch.Replace('<absolute-path-to-the-build-folder-mind-the-forward-slashes>', $buildFolderPath.ToString().Replace("\", "/")) 
			$launch = $launch.Replace('<absolute-path-to-openocd-mind-the-forward-slashes>', $env:ESP32_OPENOCD_PATH.Replace("\", "/")) 
			$launch = $launch.Replace('<absolute-path-to-the-toolchain-folder-mind-the-forward-slashes>', $env:ESP32_TOOLCHAIN_PATH.Replace("\", "/")) 
			Set-Content -Path '.\.vscode\launch.json' -Value $launch 
		}
	}
	ElseIf($env:BOARD_NAME -eq "STM32")
	{
		$env:GNU_GCC_TOOLCHAIN_PATH='C:\GNU_Tools_ARM_Embedded\7-2017-q4-major'
		$env:HEX2DFU_PATH= 'C:\mytools\hex2dfu'
		$env:NINJA_PATH= 'C:\mytools\ninja'
		
		# get build matrix
		Invoke-Expression -Command .\get-stm32-targets-to-build.ps1

		# install tools and utilities
		Invoke-Expression -Command .\install-arm-gcc-toolchain.ps1
		Invoke-Expression -Command .\install-ninja.ps1
		Invoke-Expression -Command .\install-nf-hex2dfu.ps1
		
		$test = [System.Environment]::GetEnvironmentVariable("NINJA_PATH", "User")
		If($test -eq "")
		{
			[System.Environment]::SetEnvironmentVariable("NINJA_PATH", $env:NINJA_PATH, "User")
		}
	}
}
Else
{
    Write-Host ("Specify the type/name of board you wish to install tools for.")
	Write-Host ("Set parameter -BOARD_NAME to one of [" +($BOARD_NAMES -Join ',')+ "]")
}