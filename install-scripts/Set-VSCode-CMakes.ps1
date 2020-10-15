# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

# This PS sets/updates Visual Studio Code cmake-variants.json from the targets

function Set-VSCode-CMakes {
    param (
        [switch]$force = $false
    )

    # flag for file changes
    $fileChanges = $false

    $choicesToUpdate = @()

    # path to cmake-variants.json file
    $filePath = Join-Path -Path $(Get-VSCodeFolderPath) -ChildPath "cmake-variants.json"

    # file exists
    $fileExists = Test-Path $filePath -ErrorAction SilentlyContinue

    if ($fileExists -ne $true) {
        # create file by copying from the template
        Copy-Item "$(Get-VSCodeFolderPath)\cmake-variants.TEMPLATE.json" -Destination "$(Get-VSCodeFolderPath)\cmake-variants.json" -Force

        # read cmake-variants file as json
        $cmakeVariants = Get-Content -Raw -Encoding UTF8 $filePath | ConvertFrom-Json

        # replace choices with empty value
        $cmakeVariants.linkage | Add-Member -MemberType NoteProperty -Name "choices" -Value $(New-Object PSObject) -Force

        # save back
        $cmakeVariants | ConvertTo-Json -depth 32 -Compress | Out-File $filePath -Encoding UTF8 -Force
    }
    else {
        try {
            # read cmake-variants file as json
            $cmakeVariants = Get-Content -Raw -Encoding UTF8 $filePath | ConvertFrom-Json        
        }
        catch {
            "There seems to be a formatting error on the '$filePath' file. Can't update..." | Write-Error
            Exit 1
        }            
    }

    if ($null -ne $cmakeVariants) {

        # get all cmake-variants.json file from the targets and targets community 
        Get-ChildItem -Path "$(Get-RepoPath)\targets*" -Include "cmake-variants.json" -Recurse |
            Foreach-object {

                $cmakeTemplate = Get-Content -Raw -Encoding UTF8 $_.FullName | ConvertFrom-Json

                foreach ($newChoice in $cmakeTemplate.linkage.choices) {

                    $addChoice = $true
            
                    # loop through each existing linkage choice
                    foreach ($existingChoice in $cmakeVariants.linkage.choices) {
            
                        if(($existingChoice | Get-Member -MemberType NoteProperty).Name -eq ($newChoice | Get-Member -MemberType NoteProperty).Name)
                        {
                            # choice already exists
                            if($force)
                            {
                                $addChoice = $true
                            }
                            else {
                                $addChoice = $false                    
                            }
            
                            Continue
                        }
                    }
            
                    if($addChoice) {
                        # copy config
                        $choicesToUpdate += $newChoice
            
                        # flag changes
                        $fileChanges = $true
                    }
                }
            }

        # add any choices that are to be added
        foreach($choice in $choicesToUpdate)
        {
            # need to access PS object
            $object = ($choice | Get-Member -MemberType NoteProperty)
            
            # need to grab the name to be used as key
            $key = $object.Name.ToString()
            
            # now add it, use with force, in case this to update the value
            $cmakeVariants.linkage.choices | Add-Member -MemberType NoteProperty -Name $key -Value $choice."$key" -Force
        }

        # save changes, if needed
        if ($fileChanges) {

            # save file with updated contents
            $cmakeVariants | ConvertTo-Json -depth 32 -Compress | Out-File $filePath -Encoding UTF8 -Force

            Repair-Patterns $filePath

            Update-Paths $filePath
        }
    }
}
