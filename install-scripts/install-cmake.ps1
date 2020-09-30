# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

# This PS installs CMake

# requires this module
Install-Module -Name WebKitDev -Scope CurrentUser

# default version is:
[version]$version="3.15.7"

# check if CMake is installed
$cmake = (Get-Command "cmake.exe" -ErrorAction SilentlyContinue)
if($cmake)
{
    if($cmake.Version -ge $version)
    {
        "Skipping instal of CMake. Found v$version" | Write-Host -ForegroundColor Yellow

        exit 0
    }
}

"Installing CMake v$version..." | Write-Host -ForegroundColor White

$url = ('https://cmake.org/files/v{0}.{1}/cmake-{0}.{1}.{2}-win64-x64.msi' -f $version.Major, $version.Minor, $version.Build);

$options = @(
    'ADD_CMAKE_TO_PATH="System"'
);

Install-FromMsi -Name 'cmake' -Url $url -Options $options
