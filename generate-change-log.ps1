# Copyright (c) 2018 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.

# generate change log when build is NOT a pull-request or not a tag (master OR release)
if ($env:appveyor_pull_request_number -or 
    ($env:APPVEYOR_REPO_BRANCH -eq "master" -and $env:APPVEYOR_REPO_TAG -eq 'true') -or
    ($env:APPVEYOR_REPO_BRANCH -match "^release*" -and $env:APPVEYOR_REPO_TAG -eq 'true') -or
    $env:APPVEYOR_REPO_TAG -eq "true")
{
    'Skip change log processing...' | Write-Host -ForegroundColor White
}
else
{
    # need this to keep ruby happy
    md c:\tmp

    if ($env:APPVEYOR_REPO_BRANCH -eq "master" -or $env:APPVEYOR_REPO_BRANCH -match "^release*")
    {
        # generate change log including future version
        bundle exec github_changelog_generator --token $env:GitHubToken --future-release "v$env:GitVersion_MajorMinorPatch"
    }
    else 
    {
        # generate change log
        # version includes commits
        bundle exec github_changelog_generator --token $env:GitHubToken        
    }

    # updated changelog and the updated assembly info files
    git add CHANGELOG.md
    git commit -m "Update CHANGELOG for v$env:GitVersion_NuGetVersionV2"
    git push origin --porcelain -q > $null
}
