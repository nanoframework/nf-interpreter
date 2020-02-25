# This PS installs CMake


# current default version is:
$version = "3.15.7"
$major, $minor, $patch = $version.split('.');


Write-Host "Installing CMake v$version..."

$url = ('https://cmake.org/files/v{0}.{1}/cmake-{2}-win64-x64.msi' -f $major, $minor, $version);

$options = @(
    'ADD_CMAKE_TO_PATH="System"'
);

Install-FromMsi -Name 'cmake' -Url $url -Options $options;
