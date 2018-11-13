# Copyright (c) 2018 The nanoFramework project contributors
# Portions Copyright (c) Sankarsan Kampa (a.k.a. k3rn31p4nic).  All rights reserved.
# See LICENSE file in the project root for full license information.

$STATUS=$args[0]
$WEBHOOK_URL=$args[1]
$MESSAGE=$args[2]

Write-Output "[Webhook] Sending webhook to Discord..."

Switch ($STATUS) {
  "success" {
    $EMBED_COLOR=3066993
    $STATUS_MESSAGE="Passed"
    Break
  }
  "failure" {
    $EMBED_COLOR=15158332
    $STATUS_MESSAGE="Failed"
    Break
  }
  "warning" {
    $EMBED_COLOR=12370112
    $STATUS_MESSAGE="Warning"
    Break
  }  
  default {
    Write-Output "Default!"
    Break
  }
}

if (!$env:Build_SourceVersion) {
  $env:Build_SourceVersion="$(git log -1 --pretty="%H")"
}

$AUTHOR_NAME="$(git log -1 "$env:Build_SourceVersion" --pretty="%aN")"
$COMMITTER_NAME="$(git log -1 "$env:Build_SourceVersion" --pretty="%cN")"
$COMMIT_SUBJECT="$(git log -1 "$env:Build_SourceVersion" --pretty="%s")"
$COMMIT_MESSAGE="$(git log -1 "$env:Build_SourceVersion" --pretty="%b")"

if ($AUTHOR_NAME -eq $COMMITTER_NAME) {
  $CREDITS = "$AUTHOR_NAME authored & committed"
}
else {
  $CREDITS = "$AUTHOR_NAME authored & $COMMITTER_NAME committed"
}

if ($env:System_PullRequest_PullRequestNumber) {
  $URL="https://github.com/$env:Build_Repository_Name/pull/$env:System_PullRequest_PullRequestNumber"
}
else {
  $URL=""
}

$BUILD_VERSION = [uri]::EscapeDataString($env:Build_BuildNumber)

$WEBHOOK_DATA="{
  ""embeds"": [ {
    ""color"": $EMBED_COLOR,
    ""author"": {
      ""name"": ""Job #$env:Build_BuildId (Build #$env:Build_BuildNumber) $STATUS_MESSAGE - $env:Build_Repository_Name"",
      ""url"": ""https://dev.azure.com/nanoframework/nf-interpreter/_build/results?buildId=$env:Build_BuildId&view=results"",
      ""icon_url"": ""$AVATAR""
    },
    ""title"": ""$COMMIT_SUBJECT"",
    ""url"": ""$URL"",
    ""description"": ""$COMMIT_MESSAGE $CREDITS $MESSAGE"",
    ""fields"": [
      {
        ""name"": ""Commit"",
        ""value"": ""[``$($env:Build_SourceVersion.substring(0, 7))``](https://github.com/$env:Build_Repository_Name/commit/$env:Build_SourceVersion)"",
        ""inline"": true
      },
      {
        ""name"": ""Branch/Tag"",
        ""value"": ""[``$env:Build_SourceBranchName``](https://github.com/$env:Build_Repository_Name/tree/$env:Build_SourceBranchName)"",
        ""inline"": true
      }
    ]
  } ],
  ""content"" : """",
  ""file"": """"
}"

Invoke-RestMethod -Uri $WEBHOOK_URL -Method POST -UserAgent AzurePipelines-Webhook -ContentType 'application/json' -Body $WEBHOOK_DATA

Write-Output "[Webhook] Successfully sent the webhook."
