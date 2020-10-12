# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

# This PS sets/updates Visual Studio Code tasks.json

function Set-VSCode-Tasks {
    param (
        [switch]$force = $false
    )

    # flag for file changes
    $fileChanges = $false

    # path to tasks.json file
    $filePath = Join-Path -Path $(Get-VSCodeFolderPath) -ChildPath "tasks.json"

    # file exists
    $fileExists = Test-Path $filePath -ErrorAction SilentlyContinue

    if ($fileExists -ne $true) {
        # create file
        Set-Content -Path $filePath -Value '{ "version": "2.0.0", "tasks": [ ], "inputs": [] }'  -Encoding UTF8
    }

    # read task file as json
    $tasks = Get-Content -Raw -Encoding UTF8 $filePath | ConvertFrom-Json

    if ($null -ne $tasks) {

        # read tasks template file as json
        $tasksTemplate = Get-Content -Raw -Encoding UTF8 "$(Get-VSCodeFolderPath)\tasks.TEMPLATE.json" | ConvertFrom-Json

        # placeholder for the tasks to add
        $tasksToAdd = @()
        $tasksToRemove = @()

        # placeholder for the inputs to add
        $inputsToAdd = @()
        $inputsToRemove = @()

        foreach ($templateEntry in $tasksTemplate.tasks) {

            $addTask = $true

            # loop through each existing task
            foreach ($existingTask in $tasks.tasks) {

                if($($existingTask.label) -eq $($templateEntry.label))
                {
                    # task already exists
                    if($force)
                    {
                        $tasksToRemove += $existingTask
                        $addTask = $true
                    }
                    else {
                        $addTask = $false                    
                    }

                    Continue
                }
            }

            if($addTask) {
                # copy task
                $tasksToAdd += $templateEntry

                # flag changes
                $fileChanges = $true
            }
        }

        foreach ($templateEntry in $tasksTemplate.inputs) {

            $addInput = $true

            # loop through each existing input
            foreach ($existingInput in $tasks.inputs) {

                if($($existingInput.label) -eq $($templateEntry.id))
                {
                    # input already exists
                    if($force)
                    {
                        $inputsToRemove += $existingInput
                        $addInput = $true
                    }
                    else {
                        $addInput = $false                    
                    }

                    Continue
                }
            }

            if($addInput) {
                # copy input
                $inputsToAdd += $templateEntry

                # flag changes
                $fileChanges = $true
            }
        }

        # remove any tasks that are to be removed
        if($tasksToRemove.Count -gt 0)
        {
            [System.Collections.ArrayList]$bkpTasks = $tasks.tasks
            foreach($task in $tasksToRemove)
            {
                $bkpTasks.Remove($task)
            }
            
            $tasks.tasks = $bkpTasks
        }

        # add any inputs that are to be added
        foreach($task in $tasksToAdd)
        {
            $tasks.tasks += $task
        }

        # remove any inputs that are to be removed
        if($inputsToRemove.Count -gt 0)
        {
            [System.Collections.ArrayList]$bkpInputs = $tasks.inputs
            foreach($input in $inputsToRemove)
            {
                $bkpInputs.Remove($input)
            }
            
            $tasks.inputs = $bkpInputs
        }

        # add any inputs that are to be added
        foreach($input in $inputsToAdd)
        {
            $tasks.inputs += $input
        }

        # save changes, if needed
        if ($fileChanges) {

            # save file with updated contents
            $tasks | ConvertTo-Json -depth 32 | Out-File $filePath -Encoding UTF8 -Force

            Repair-Patterns $filePath

            Update-Paths $filePath
        }
    }
}
