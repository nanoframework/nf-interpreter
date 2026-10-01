# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

<#
.SYNOPSIS
    Checks that the nbgv path filters of a component/platform cover every repository file compiled into that image.

.DESCRIPTION
    A missing path filter means a change to that file won't bump the version, so two different binaries could
    ship with the same version. This script collects the real inputs of an image from a ninja build directory
    (sources and headers from the deps log) and verifies that each one, if it lives in this repository, is matched
    by the path filters in versions/<Component>/<Platform>/version.json.

    Files outside the repository (SDKs fetched into build/_deps, toolchains) and generated files in the build
    directory are ignored: their versions are pinned by CMake/preset files, which the filters must cover anyway.

    CMake configure inputs (CMakeLists.txt, *.cmake, configure_file() templates) are shared by all the images
    of a build, so uncovered ones are reported as warnings only, unless -StrictConfigureInputs is set.

.PARAMETER BuildDir
    Path to a ninja build directory that has been fully built.

.PARAMETER Component
    Image to check: nanoBooter, nanoCLR or nanoMCUboot. The ninja target is '<Component>.elf', except for nanoMCUboot
    which is built as 'nanoMcubooter.elf'.

.PARAMETER Platform
    Leaf under versions/<Component>, e.g. ChibiOS or ThreadX-ST.

.PARAMETER ShowUnused
    Also lists include filters that don't match any input of this build. These are candidates for
    tightening the spec, as long as no other target of the same platform uses them.

.PARAMETER StrictConfigureInputs
    Treat uncovered CMake configure inputs as errors.

.PARAMETER WarnOnly
    Report uncovered files as warnings and exit with 0. In Azure Pipelines the task is then marked as
    'succeeded with issues'. Used for branch builds, where a coverage gap shouldn't block publishing.

.NOTES
    When running in Azure Pipelines (TF_BUILD is set) the issues are also reported with logging commands,
    so they show in the build summary and in the PR checks.

.EXAMPLE
    pwsh versions/Test-BuildCoverage.ps1 -BuildDir build -Component nanoBooter -Platform ChibiOS
#>

[CmdletBinding()]
param (
    [Parameter(Mandatory)]
    [string]$BuildDir,

    [Parameter(Mandatory)]
    [ValidateSet('nanoBooter', 'nanoCLR', 'nanoMCUboot')]
    [string]$Component,

    [Parameter(Mandatory)]
    [string]$Platform,

    [switch]$ShowUnused,

    [switch]$StrictConfigureInputs,

    [switch]$WarnOnly
)

$ErrorActionPreference = 'Stop'

$isAzurePipelines = $env:TF_BUILD -eq 'True'

# reports an issue with an Azure Pipelines logging command (no-op when running locally)
function Write-PipelineIssue([string]$type, [string]$repoPath, [string]$message)
{
    if ($isAzurePipelines)
    {
        Write-Host "##vso[task.logissue type=$type;sourcepath=$($repoPath.TrimStart('/'))]$message"
    }
}

$repoRoot = [System.IO.Path]::GetFullPath((git -C $PSScriptRoot rev-parse --show-toplevel).Trim())
$buildRoot = [System.IO.Path]::GetFullPath((Resolve-Path $BuildDir).Path)
$versionFile = Join-Path $PSScriptRoot "$Component/$Platform/version.json"

if (-not (Test-Path $versionFile))
{
    throw "Can't find $versionFile"
}

$pathFilters = (Get-Content $versionFile -Raw | ConvertFrom-Json).pathFilters
$includes = @($pathFilters | Where-Object { $_.StartsWith(':/') } | ForEach-Object { $_.Substring(1) })
$excludes = @($pathFilters | Where-Object { $_.StartsWith(':!/') } | ForEach-Object { $_.Substring(2) })

if ($pathFilters.Count -ne ($includes.Count + $excludes.Count))
{
    throw "$versionFile has path filters that aren't root relative (':/' or ':!/'). Regenerate it with Update-PathFilters.ps1."
}

$includeSet = [System.Collections.Generic.HashSet[string]]::new([string[]]$includes, [System.StringComparer]::OrdinalIgnoreCase)
$excludeSet = [System.Collections.Generic.HashSet[string]]::new([string[]]$excludes, [System.StringComparer]::OrdinalIgnoreCase)

# true if the path itself or any of its parent directories is in the set (nbgv path filter semantics)
function Test-UnderAny([string]$path, [System.Collections.Generic.HashSet[string]]$set)
{
    while ($path.Length -gt 0)
    {
        if ($set.Contains($path))
        {
            return $true
        }

        $path = $path.Substring(0, [Math]::Max(0, $path.LastIndexOf('/')))
    }

    return $false
}

function Test-Under([string]$path, [string]$parent)
{
    return $path.Equals($parent, 'OrdinalIgnoreCase') -or $path.StartsWith("$parent/", 'OrdinalIgnoreCase')
}

function Test-Included([string]$path)
{
    return Test-UnderAny $path $includeSet
}

function Test-Covered([string]$path)
{
    return (Test-UnderAny $path $includeSet) -and -not (Test-UnderAny $path $excludeSet)
}

# converts a path reported by ninja into a '/'-rooted repository path, or $null if it isn't a tracked repo file
function ConvertTo-RepoPath([string]$path)
{
    $fullPath = if ([System.IO.Path]::IsPathRooted($path)) { $path } else { Join-Path $buildRoot $path }
    $fullPath = [System.IO.Path]::GetFullPath($fullPath)

    # compare with a trailing separator, so siblings like 'build-tools' aren't taken as descendants of 'build'
    if ($fullPath.StartsWith($buildRootPrefix, 'OrdinalIgnoreCase') -or -not $fullPath.StartsWith($repoRootPrefix, 'OrdinalIgnoreCase'))
    {
        return $null
    }

    $repoPath = $fullPath.Substring($repoRootPrefix.Length - 1).Replace('\', '/')

    # generated or ignored files (e.g. ESP32 sdkconfig) can't be covered by path filters
    if (-not $trackedFiles.Contains($repoPath))
    {
        return $null
    }

    return $repoPath
}

$repoRootPrefix = $repoRoot.TrimEnd('\', '/') + [System.IO.Path]::DirectorySeparatorChar
$buildRootPrefix = $buildRoot.TrimEnd('\', '/') + [System.IO.Path]::DirectorySeparatorChar

$trackedFiles = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::OrdinalIgnoreCase)
$gitFiles = git -C $repoRoot ls-files --cached

if ($LASTEXITCODE -ne 0)
{
    throw 'git failed to list the tracked files'
}

$gitFiles | ForEach-Object { $trackedFiles.Add('/' + $_) | Out-Null }

# executable name of each image (versions/<Component> is named after the image)
$executableName = if ($Component -eq 'nanoMCUboot') { 'nanoMcubooter' } else { $Component }
$elfTarget = "$executableName.elf"

# all transitive inputs of the image (sources, objects, libraries)
$targetInputs = ninja -C $buildRoot -t inputs $elfTarget

if ($LASTEXITCODE -ne 0)
{
    throw "ninja failed to list the inputs of '$elfTarget'"
}

$objects = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::OrdinalIgnoreCase)
$compileInputs = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::OrdinalIgnoreCase)

foreach ($item in $targetInputs)
{
    # objects of other executables show up through order-only dependencies (C++ module scanning), skip them
    if ($item -match '\.(obj|o)$' -and
        ($item -notmatch 'CMakeFiles/([^/]+)\.elf\.dir/' -or $Matches[1] -eq $executableName))
    {
        $objects.Add($item) | Out-Null
    }
}

# sources and headers come from the deps log: '<object>: #deps N, ...' followed by indented dependency lines
$depsLog = ninja -C $buildRoot -t deps

if ($LASTEXITCODE -ne 0)
{
    throw 'ninja failed to read the deps log'
}

$inObject = $false

foreach ($line in $depsLog)
{
    if ($line.Length -eq 0)
    {
        continue
    }

    if ($line[0] -ne ' ')
    {
        $separator = $line.IndexOf(': #deps')
        $inObject = $separator -gt 0 -and $objects.Contains($line.Substring(0, $separator))
    }
    elseif ($inObject)
    {
        $compileInputs.Add($line.Trim()) | Out-Null
    }
}

if ($objects.Count -eq 0)
{
    throw "No objects found for '$elfTarget'. Is '$BuildDir' a complete ninja build of this component?"
}

# inputs of the CMake regeneration rule, i.e. every file read while configuring
$configureInputs = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::OrdinalIgnoreCase)
$regenerateQuery = ninja -C $buildRoot -t query build.ninja

if ($LASTEXITCODE -ne 0)
{
    throw 'ninja failed to query the CMake configure inputs'
}

$inInputs = $false

foreach ($line in $regenerateQuery)
{
    if ($line -match '^\s+input:')
    {
        $inInputs = $true
    }
    elseif ($line -match '^\s+outputs:')
    {
        $inInputs = $false
    }
    elseif ($inInputs -and $line -match '^\s+(?:\|\|?\s+)?(\S.*)$')
    {
        $configureInputs.Add($Matches[1]) | Out-Null
    }
}

$usedPaths = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::OrdinalIgnoreCase)
$uncovered = [System.Collections.Generic.SortedSet[string]]::new([System.StringComparer]::OrdinalIgnoreCase)

foreach ($item in $compileInputs)
{
    $repoPath = ConvertTo-RepoPath $item

    if ($repoPath)
    {
        $usedPaths.Add($repoPath) | Out-Null

        if (-not (Test-Covered $repoPath))
        {
            $uncovered.Add($repoPath) | Out-Null
        }
    }
}

$uncoveredConfigure = [System.Collections.Generic.SortedSet[string]]::new([System.StringComparer]::OrdinalIgnoreCase)
$configurePaths = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::OrdinalIgnoreCase)

foreach ($item in $configureInputs)
{
    $repoPath = ConvertTo-RepoPath $item

    if ($repoPath)
    {
        $configurePaths.Add($repoPath) | Out-Null

        # configure inputs are shared by all the images: only report the ones that no include matches,
        # the excluded ones belong to other images or platforms on purpose
        if (-not $usedPaths.Contains($repoPath) -and -not (Test-Included $repoPath))
        {
            $uncoveredConfigure.Add($repoPath) | Out-Null
        }
    }
}

Write-Host "$Component ($Platform): $($objects.Count) objects, $($usedPaths.Count) repository files compiled in."

$compiledIssueType = if ($WarnOnly) { 'warning' } else { 'error' }
$configureIssueType = if ($StrictConfigureInputs -and -not $WarnOnly) { 'error' } else { 'warning' }

if ($uncovered.Count -gt 0)
{
    Write-Host "`nCompiled files NOT covered by versions/$Component/$Platform path filters:" -ForegroundColor Red

    # keep the build summary readable, the full list is in the log
    $maxAnnotations = 20
    $annotated = 0

    foreach ($path in $uncovered)
    {
        Write-Host "  $path"

        if ($annotated -lt $maxAnnotations)
        {
            Write-PipelineIssue $compiledIssueType $path "Compiled into $Component but not covered by versions/$Component/$Platform path filters: a change to this file won't bump the version. Fix versions/pathfilters.json and run versions/Update-PathFilters.ps1."
            $annotated++
        }
    }

    if ($uncovered.Count -gt $maxAnnotations)
    {
        Write-PipelineIssue $compiledIssueType "versions/$Component/$Platform/version.json" "...and $($uncovered.Count - $maxAnnotations) more files not covered by the path filters, see the log."
    }
}

if ($uncoveredConfigure.Count -gt 0)
{
    $color = if ($configureIssueType -eq 'error') { 'Red' } else { 'Yellow' }
    Write-Host "`nCMake configure inputs NOT covered by the path filters:" -ForegroundColor $color

    foreach ($path in $uncoveredConfigure)
    {
        Write-Host "  $path"

        # informational only, unless strict: configure inputs are shared by all images (e.g. CLR only templates)
        if ($StrictConfigureInputs)
        {
            Write-PipelineIssue $configureIssueType $path "CMake configure input not covered by versions/$Component/$Platform path filters."
        }
    }
}

if ($ShowUnused)
{
    $unused = $includes | Where-Object {
        $include = $_
        -not ($usedPaths | Where-Object { Test-Under $_ $include }) -and
        -not ($configurePaths | Where-Object { Test-Under $_ $include })
    }

    if ($unused)
    {
        Write-Host "`nInclude filters not matching any compiled file or configure input of this build:" -ForegroundColor Cyan
        $unused | ForEach-Object { Write-Host "  :$_" }
    }
}

$hasIssues = $uncovered.Count -gt 0 -or ($StrictConfigureInputs -and $uncoveredConfigure.Count -gt 0)

if (-not $hasIssues)
{
    Write-Host "`nAll compiled repository files are covered." -ForegroundColor Green
    exit 0
}

if (-not $WarnOnly)
{
    exit 1
}

Write-Host "`nCoverage gaps reported as warnings (-WarnOnly)." -ForegroundColor Yellow

if ($isAzurePipelines)
{
    Write-Host '##vso[task.complete result=SucceededWithIssues;]'
}
