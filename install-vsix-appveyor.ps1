# This PS installs the nanoFramework Visual Studio Extension for VS2017

$vsixPath = "$($env:USERPROFILE)\nanoFramework.Tools.VS2017.Extension.vsix"
(New-Object Net.WebClient).DownloadFile('https://visualstudiogallery.msdn.microsoft.com/18fe3370-32c4-49d6-bf02-1b0ab0b1e537/file/261095/11/nanoFramework.Tools.VS2017.Extension.vsix', $vsixPath)
"`"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\IDE\VSIXInstaller.exe`" /q /a $vsixPath" | out-file ".\install-vsix.cmd" -Encoding ASCII
& .\install-vsix.cmd
