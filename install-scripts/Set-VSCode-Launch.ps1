# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

# This PS sets/updates Visual Studio Code launch.json from the targets

function Set-VSCode-Launch {
    param (
        [switch]$force = $false
    )

    # flag for file changes
    $fileChanges = $false

    # path to launch.json file
    $filePath = Join-Path -Path $(Get-VSCodeFolderPath) -ChildPath "launch.json"

    # file exists
    $fileExists = Test-Path $filePath -ErrorAction SilentlyContinue

    if ($fileExists -ne $true) {
        # create file
        Set-Content -Path "$filePath" -Value '{ "version": "0.2.0", "configurations": [ ] }' -Encoding UTF8
    }

    $configsToAdd = @()
    $configsToRemove = @()

    try {
        # read launch file as json
        $launch = Get-Content -Raw -Encoding UTF8 $filePath | ConvertFrom-Json        
    }
    catch {
        "There seems to be a formatting error on the '$filePath' file. Can't update..." | Write-Error
        Exit 1
    }

    if ($null -ne $launch) {

        # get all launch.json file from the targets and targets community 
        Get-ChildItem -Path "$(Get-RepoPath)\targets*" -Include "launch.json" -Recurse |
            Foreach-object {

                $launchTemplate = Get-Content -Raw -Encoding UTF8 $_.FullName | ConvertFrom-Json

                foreach ($newConfig in $launchTemplate.configurations) {

                    $addConfig = $true
            
                    # loop through each existing configuration
                    foreach ($existingConfig in $launch.configurations) {
            
                        if($($existingConfig.name) -eq $($newConfig.name))
                        {
                            # config already exists
                            if($force)
                            {
                                $configsToRemove += $existingConfig
                                $addConfig = $true
                            }
                            else {
                                $addConfig = $false                    
                            }
            
                            Continue
                        }
                    }
            
                    if($addConfig) {
                        # copy config
                        $configsToAdd += $newConfig
            
                        # flag changes
                        $fileChanges = $true
                    }
                }
            }

        # remove any configs that are to be removed
        [System.Collections.ArrayList]$bkpConfigs= $launch.configurations
        foreach($config in $configsToRemove)
        {
            $bkpConfigs.Remove($config)
        }

        if($configsToRemove.Count -gt 0)
        {
            $launch.configurations = $bkpConfigs
        }

        # add any configs that are to be added
        foreach($config in $configsToAdd)
        {
            $launch.configurations += $config
        }

        # save changes, if needed
        if ($fileChanges) {

            # save file with updated contents
            $launch | ConvertTo-Json -depth 32 | Out-File $filePath -Encoding UTF8 -Force

            Repair-Patterns $filePath

            Update-Paths $filePath
        }
    }
}
