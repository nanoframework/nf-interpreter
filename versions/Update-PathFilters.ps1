# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

<#
.SYNOPSIS
    Generates the nbgv 'pathFilters' of every component/platform version.json from versions/pathfilters.json.

.DESCRIPTION
    nbgv stable releases (up to 3.10) only support plain path prefixes in 'pathFilters'.
    To keep the spec readable and to pick up new boards automatically, the spec accepts
    '*', '?' and '**' wildcards, which this script expands against the files tracked by git.

    Spec rules:
    - Paths are repository-root relative and start with '/'.
    - An entry starting with '@' references a named set from 'sets'.
    - Every non-wildcard path must exist and every wildcard must match at least one path.
    - An exclude that is identical to an include is dropped (lets a leaf exclude "all other platforms" sets).
    - An exclude that isn't under any include is dropped (it has no effect).

    TODO: when nbgv 3.11 (glob support in pathFilters) ships as stable, consider emitting the wildcards
    directly instead of expanding them, so new boards don't require regenerating the version files.

.PARAMETER Check
    Doesn't write anything. Exits with code 1 if any version.json is out of date with the spec.

.EXAMPLE
    pwsh versions/Update-PathFilters.ps1
    pwsh versions/Update-PathFilters.ps1 -Check
#>

[CmdletBinding()]
param (
    [switch]$Check
)

$ErrorActionPreference = 'Stop'

$isAzurePipelines = $env:TF_BUILD -eq 'True'

# reports an error with an Azure Pipelines logging command (no-op when running locally)
function Write-PipelineError([string]$repoPath, [string]$message)
{
    if ($isAzurePipelines)
    {
        Write-Host "##vso[task.logissue type=error;sourcepath=$repoPath]$message"
    }
}

# spec errors (unknown set, path or pattern not matching anything) are terminating errors
trap
{
    Write-PipelineError 'versions/pathfilters.json' $_.Exception.Message
    Write-Host "ERROR: $($_.Exception.Message)" -ForegroundColor Red
    exit 1
}

$versionsDir = $PSScriptRoot
$repoRoot = (git -C $versionsDir rev-parse --show-toplevel).Trim()
$specPath = Join-Path $versionsDir 'pathfilters.json'
$spec = Get-Content $specPath -Raw | ConvertFrom-Json -AsHashtable

# all tracked paths (files and the directories that contain them), lower case key for case insensitive lookups
$trackedFiles = git -C $repoRoot ls-files --cached | ForEach-Object { '/' + $_ }
$tracked = @{}

foreach ($file in $trackedFiles)
{
    $tracked[$file.ToLowerInvariant()] = $file

    $dir = $file
    while (($idx = $dir.LastIndexOf('/')) -gt 0)
    {
        $dir = $dir.Substring(0, $idx)

        if ($tracked.ContainsKey($dir.ToLowerInvariant()))
        {
            break
        }

        $tracked[$dir.ToLowerInvariant()] = $dir
    }
}

function ConvertTo-PatternRegex([string]$pattern)
{
    $regex = '^'

    foreach ($segment in $pattern.TrimStart('/').Split('/'))
    {
        if ($segment -eq '**')
        {
            # zero or more complete path segments
            $regex += '(?:/[^/]+)*'
        }
        else
        {
            $regex += '/' + [regex]::Escape($segment).Replace('\*', '[^/]*').Replace('\?', '[^/]')
        }
    }

    return [regex]::new($regex + '$', 'IgnoreCase')
}

# wildcard expansion results, the same sets are used by many leaves
$patternCache = @{}

function Resolve-Entries([object[]]$entries, [string]$context)
{
    $resolved = [System.Collections.Generic.List[string]]::new()

    foreach ($entry in $entries)
    {
        if ($entry.StartsWith('@'))
        {
            $setName = $entry.Substring(1)

            if (-not $spec.sets.ContainsKey($setName))
            {
                throw "$context references unknown set '$setName'"
            }

            Resolve-Entries $spec.sets[$setName] "$context -> $entry" | ForEach-Object { $resolved.Add($_) }
        }
        elseif ($entry -match '[*?]')
        {
            if (-not $patternCache.ContainsKey($entry))
            {
                $regex = ConvertTo-PatternRegex $entry
                $patternCache[$entry] = @($tracked.Values.Where({ $regex.IsMatch($_) }))
            }

            if ($patternCache[$entry].Count -eq 0)
            {
                throw "$context pattern '$entry' doesn't match any tracked path"
            }

            $patternCache[$entry] | ForEach-Object { $resolved.Add($_) }
        }
        else
        {
            if (-not $tracked.ContainsKey($entry.ToLowerInvariant()))
            {
                throw "$context path '$entry' isn't tracked by git"
            }

            # use the casing stored in git
            $resolved.Add($tracked[$entry.ToLowerInvariant()])
        }
    }

    return $resolved
}

# true if the path itself or any of its parent directories is in the set
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

function New-PathSet([string[]]$paths)
{
    $set = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::OrdinalIgnoreCase)
    $paths | ForEach-Object { $set.Add($_) | Out-Null }

    return , $set
}

# removes duplicates and paths that are already covered by a parent directory in the same list
function Compress-Paths([string[]]$paths)
{
    $all = New-PathSet $paths
    $result = [System.Collections.Generic.List[string]]::new()

    foreach ($path in $all)
    {
        $parent = $path.Substring(0, [Math]::Max(0, $path.LastIndexOf('/')))

        if (-not (Test-UnderAny $parent $all))
        {
            $result.Add($path)
        }
    }

    $result.Sort([System.StringComparer]::OrdinalIgnoreCase)

    return $result
}

$stale = @()

foreach ($leafName in ($spec.leaves.Keys | Sort-Object))
{
    $leaf = $spec.leaves[$leafName]

    $rawIncludes = @(Resolve-Entries $leaf.include "leaf '$leafName' include")
    $includes = Compress-Paths $rawIncludes
    $excludes = @()

    if ($leaf.ContainsKey('exclude'))
    {
        # an explicit include wins over an identical exclude (e.g. own platform files listed in an "all platforms" set)
        $rawIncludeSet = New-PathSet $rawIncludes
        $includeSet = New-PathSet $includes
        $candidates = @(Resolve-Entries $leaf.exclude "leaf '$leafName' exclude") | Where-Object { -not $rawIncludeSet.Contains($_) }

        # drop excludes that aren't under any include, they have no effect
        $excludes = @(Compress-Paths $candidates | Where-Object { Test-UnderAny $_ $includeSet })
    }

    $pathFilters = @($includes | ForEach-Object { ":$_" }) + @($excludes | ForEach-Object { ":!$_" })

    $versionJson = [ordered]@{
        '$schema' = 'https://raw.githubusercontent.com/dotnet/Nerdbank.GitVersioning/main/src/NerdBank.GitVersioning/version.schema.json'
        inherit   = $true
    }

    if ($leaf.ContainsKey('properties'))
    {
        foreach ($key in $leaf.properties.Keys)
        {
            $versionJson[$key] = $leaf.properties[$key]
        }
    }

    $versionJson['pathFilters'] = $pathFilters

    $content = ($versionJson | ConvertTo-Json -Depth 10).Replace("`r`n", "`n").Replace("`n", "`r`n") + "`r`n"

    $leafPath = Join-Path $versionsDir (Join-Path $leafName 'version.json')
    $current = if (Test-Path $leafPath) { Get-Content $leafPath -Raw } else { $null }

    if ($current -ne $content)
    {
        $stale += $leafName

        if (-not $Check)
        {
            New-Item -ItemType Directory -Force (Split-Path $leafPath) | Out-Null
            [System.IO.File]::WriteAllText($leafPath, $content, [System.Text.UTF8Encoding]::new($false))
            Write-Host "Updated versions/$leafName/version.json ($($includes.Count) includes, $($excludes.Count) excludes)"
        }
    }
}

# leaf files (versions/<image>/<platform>/version.json) that aren't generated from the spec, e.g. hand made or
# left behind after removing a leaf from the spec
$orphans = @(Get-ChildItem $versionsDir -Directory | ForEach-Object { Get-ChildItem $_.FullName -Directory } |
    Where-Object { Test-Path (Join-Path $_.FullName 'version.json') } |
    ForEach-Object { "$($_.Parent.Name)/$($_.Name)" } |
    Where-Object { -not $spec.leaves.ContainsKey($_) })

foreach ($orphan in $orphans)
{
    Write-Host "versions/$orphan/version.json isn't generated from versions/pathfilters.json. Add the leaf to the spec or delete the file." -ForegroundColor Yellow
}

if ($Check)
{
    if ($stale.Count -gt 0 -or $orphans.Count -gt 0)
    {
        foreach ($leafName in $stale)
        {
            Write-Host "  versions/$leafName/version.json is out of date with versions/pathfilters.json"
            Write-PipelineError "versions/$leafName/version.json" "Out of date with versions/pathfilters.json. Run 'pwsh versions/Update-PathFilters.ps1' and commit the changes."
        }

        foreach ($orphan in $orphans)
        {
            Write-PipelineError "versions/$orphan/version.json" "Not generated from versions/pathfilters.json. Add the leaf to the spec or delete the file."
        }

        Write-Host "Run 'pwsh versions/Update-PathFilters.ps1' and commit the changes."
        exit 1
    }

    Write-Host 'All version.json path filters are up to date.'
}
elseif ($stale.Count -eq 0)
{
    Write-Host 'Nothing to update.'
}
