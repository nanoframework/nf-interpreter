# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

# This PS installs Python

# requires this module
Install-Module -Name WebKitDev -Scope CurrentUser

# default version is:
[version]$version="3.9.2"

# check if Python is installed
$cmake = (Get-Command "python.exe" -ErrorAction SilentlyContinue)
if($cmake)
{
    if($cmake.Version -ge $version)
    {
        "Skipping instal of Python. Found v$version" | Write-Host -ForegroundColor Yellow

        exit 0
    }
}

"Installing Python v$version..." | Write-Host -ForegroundColor White

$url = ('https://www.python.org/ftp/python/{0}.{1}.{2}/python-{0}.{1}.{2}-amd64.exe' -f $version.Major, $version.Minor, $version.Build);

$options = @(
    '/passive InstallAllUsers=1 PrependPath=1 Include_test=0'
);

Install-FromExe -Name 'python' -Url $url -Options $options
