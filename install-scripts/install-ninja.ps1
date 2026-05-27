# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

# This PS installs Ninja build tool

[CmdletBinding(SupportsShouldProcess = $true)]
param (
    [Parameter(HelpMessage = "Path to the folder where the tool is to be installed.")][string]$Path,
    [switch]$force = $false
)

# check if running on Azure Pipelines by looking at this two environment variables
$IsAzurePipelines = $env:Agent_HomeDirectory -and $env:Build_BuildNumber

if ($IsAzurePipelines) {
    $zipRoot = "$env:Agent_TempDirectory"
}
else {
    #Set location of nf-interpreter top-level
    $nfRoot = "$PSScriptRoot\.."
    $zipRoot = "$nfRoot\zips"

    # Create directory for zip files if necessary
    mkdir -Force $zipRoot | Out-Null
}

if ([string]::IsNullOrEmpty($Path) -or $force) {
    # no path requested
    # check for NF_TOOLS_PATH
    if ($env:NF_TOOLS_PATH) {
        $Path = $env:NF_TOOLS_PATH
    }
    else {
        # use default
        $Path = 'C:\nftools'
    }

    # append the tool path
    $Path = $Path + "\Ninja"
}

# check if path already exists
$ninjaPathExists = Test-Path $Path -ErrorAction SilentlyContinue

If ($ninjaPathExists -eq $False -or $force) {
    $url = "https://github.com/ninja-build/ninja/releases/download/v1.13.1/ninja-win.zip"
    $output = "$zipRoot\ninja.zip"
   
    [Net.ServicePointManager]::SecurityProtocol = [Net.ServicePointManager]::SecurityProtocol -bor [Net.SecurityProtocolType]::Tls12

    # Don't download again if already exists
    if (![System.IO.File]::Exists($output) -or $force) {
        "Downloading Ninja..." | Write-Host -ForegroundColor White -NoNewline

        try {
            $wc = New-Object Net.WebClient
            $wc.DownloadFile($url, $output)
            if (-not (Test-Path $output)) {
                throw "Download failed: $output not found."
            }

            Write-Host "OK" -ForegroundColor Green
        }
        catch {
            Write-Host "Download failed: $_" -ForegroundColor Red
            throw
        }
        finally {
            if ($wc) { $wc.Dispose() }
        }
    }

    # unzip to install path, if not on Azure
    if ($IsAzurePipelines -eq $False) {
        "Installing Ninja..." | Write-Host -ForegroundColor White -NoNewline

        # unzip tool
        Expand-Archive $output -DestinationPath $Path > $null

        "OK" | Write-Host -ForegroundColor Green
    }
}
else {
    "Skipping install of Ninja" | Write-Host -ForegroundColor Yellow
}

# set env variable, if not on Azure
if ($IsAzurePipelines -eq $False) {
    $env:NINJA_PATH = $Path

    "Setting User Environment Variable NINJA_PATH='" + $env:NINJA_PATH + "'" | Write-Host -ForegroundColor Yellow

    try {
        # this call can fail if the script is not run with appropriate permissions
        [System.Environment]::SetEnvironmentVariable("NINJA_PATH", $env:NINJA_PATH, "User")
    }
    catch {
        "Failed to set User Environment Variable NINJA_PATH. Make sure to manually add 'NINJA_PATH' with '" + $env:NINJA_PATH + "'." | Write-Host -ForegroundColor Red
    }
}

# if not on Azure, add ninja to the PATH
if ($IsAzurePipelines -eq $False) {

    # check if it's already there
    $ninja = (Get-Command "ninja.exe" -ErrorAction SilentlyContinue)
    if ($ninja) {
        # reachable, we're good
        exit 0
    }

    # add to path
    try {
        "Adding ninja to User PATH" | Write-Host -ForegroundColor Yellow

        [Environment]::SetEnvironmentVariable(
            "Path",
            [Environment]::GetEnvironmentVariable("Path", "User") + ";$Path",
            "User")      
    }
    catch {
        "Failed to update User PATH environment variable. Make sure to manually add '$Path' to the User PATH." | Write-Host -ForegroundColor Red
    }
}

# SIG # Begin signature block
# MIIshwYJKoZIhvcNAQcCoIIseDCCLHQCAQExDzANBglghkgBZQMEAgEFADB5Bgor
# BgEEAYI3AgEEoGswaTA0BgorBgEEAYI3AgEeMCYCAwEAAAQQH8w7YFlLCE63JNLG
# KX7zUQIBAAIBAAIBAAIBAAIBADAxMA0GCWCGSAFlAwQCAQUABCAnvM9h92prXFZO
# 0z7pvB97MJCIkhgPMp4DRlm2enMRqKCCE7wwggVkMIIDTKADAgECAhAGzuExvm1V
# yAf3wMf7ROYgMA0GCSqGSIb3DQEBDAUAMEwxCzAJBgNVBAYTAlVTMRcwFQYDVQQK
# Ew5EaWdpQ2VydCwgSW5jLjEkMCIGA1UEAxMbRGlnaUNlcnQgQ1MgUlNBNDA5NiBS
# b290IEc1MB4XDTIxMDExNTAwMDAwMFoXDTQ2MDExNDIzNTk1OVowTDELMAkGA1UE
# BhMCVVMxFzAVBgNVBAoTDkRpZ2lDZXJ0LCBJbmMuMSQwIgYDVQQDExtEaWdpQ2Vy
# dCBDUyBSU0E0MDk2IFJvb3QgRzUwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIK
# AoICAQC2M3OA2GIDcBQsERw5XnyufIOGHf4mL0wkrYvqg1+pvD1b/AuYTAJHMOzi
# /uzoNFtmXr871yymJf+MWbPf6tp8KdlGUHIIHW7RGwrdH82ZifoPD3PE4ZwddTLN
# b5faKmqVsmzJCdDqC3t9FwZJme/W3uDIU9SuxnfxhrsjHLjA31n3jn3R74LmJota
# OLX/ddWy2U8J8zeIUNoRpIoUFNFTBAB982pEGP5QcDIHHKiaDjodxQofbgsmabc8
# oldwLIb6TG6VqVhDuawS1v8/7ddDF2tMzp7EkKv/+hBQmqOQV9bnjBCunxYazzUd
# f9d27YqcNacouKddIfwwN93eCBlPFcbnptqQR473lFNMjlMCvv2Z5eqG0K8DAtOb
# qpPxqyiOIAH/TPvMtylA9YekEhMFH0Nu11FQnzi0IO0XCRKPzLkZr5/NvmkR069V
# EG0XhnmWUsayAJ3lrziwNfSIa48OBD187q/N02oQSsbNhsoiPaFKXPsO/4jfXGKn
# wLke2axsfjg3/neTJcKFik+1NwZaBoEU8c6UnZmR6jJazmc9bgRmrQxPLaMu9571
# eJ33Cv1+j+NCilWWvPGfNy38nl+V/owYG/yO/UuQr9cDaBJjrOKTp6LLBOVPZM4D
# +sYUn9mL6MzUYoxr5AAsGZ8aBsYxgVT7UySar1WZup11rrjC3QIDAQABo0IwQDAd
# BgNVHQ4EFgQUaAGTsdJKQEJplEYsHFqIqSW0R08wDgYDVR0PAQH/BAQDAgGGMA8G
# A1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEMBQADggIBAJL87rgCeRcCUX0hxUln
# p6TxqCQ46wxo6lpCa5z0c8FpSi2zNwVQQpiSngZ5LC4Gmfbv3yugzbOSAYO1oMsn
# tTwjGphJouwtmaVZQ6zSsZPWV9ccvJPWxkDhs28ZVbcT1+VDM6S1q8vawTFkDXTW
# LO3DjW7ru68ZR2FhLcD0BblveNw690JAZVORvZkNk5JUpqk3WSuby5nGvD33BITw
# lDMdD4JaOcsuRcMoGaOym5jI/DFrYI/26YYovOA8fXRdFolbaSTHEIvES7s2T9RZ
# P8OwpJGZ+C7RSgGd9YgS779aEWpZT1lrWmfzj7QTD8DYLz0ocqoZfxF9alufledf
# t5RP8T6hWv8tzJ3fJ3ePMnMcZwp28/pcsb+8Hb0MKJuyxxdnCzMPw7023Pu6Qgur
# 7YTDYtaEFqmxB2upbu7Gz+awRCnC8LNhgCqLb9IUXCWHVGTzpEzBofina+r+6jr8
# edsOj9zG88nUbN7pg6GOHSLsyTqyAHvcO6dCGn/ci6kRPY6nwCBvXQldQ0Tmj2bM
# qVsH8e+beg6zVOGU/Q4sxpPXVf1xmDW4CUr/xikoLPZSLdsUGJIn4hZ+jMrUYb6C
# h5HrmDc/v19ddz80rBs4Q6tocpkyHjoaGaWjOEwj16PnzNUqkheQC1pLvRa9+4Zq
# 4omZ7OSgVRjJowgfE+AyCHLQMIIGkDCCBHigAwIBAgIQCt4y6VCbRKo0sdrxvA7I
# czANBgkqhkiG9w0BAQsFADBMMQswCQYDVQQGEwJVUzEXMBUGA1UEChMORGlnaUNl
# cnQsIEluYy4xJDAiBgNVBAMTG0RpZ2lDZXJ0IENTIFJTQTQwOTYgUm9vdCBHNTAe
# Fw0yMTA3MTUwMDAwMDBaFw0zMTA3MTQyMzU5NTlaMFsxCzAJBgNVBAYTAlVTMRgw
# FgYDVQQKEw8uTkVUIEZvdW5kYXRpb24xMjAwBgNVBAMTKS5ORVQgRm91bmRhdGlv
# biBQcm9qZWN0cyBDb2RlIFNpZ25pbmcgQ0EyMIICIjANBgkqhkiG9w0BAQEFAAOC
# Ag8AMIICCgKCAgEAznkpxwyD+QKdO6sSwPBJD5wtJL9ncyIlhHFM8rWvU5lCCoMp
# 9TcAiXvCIIaJCeOxjXFv0maraGhSr8SANVefC74HBfDTAl/JyoWmOfBxRY/30/0q
# ivfUtoxrw91SR3Gu3eucWxxb4b+hoIpTgbKU+//cnSvi8EmBTk7ntfFkAWw/6Lov
# +nMXU+qEzm/TuCT8qWX2IffLkdXIt4UqQS8Jqjxn7cGLhjqDA9w+5zXpSxSu/JhK
# OecY05XcdGlGnQBPc8RBzUD3ZzXMPoPBSFiH7UZs23iVmVXCJoU9IFaN3WSLD/jZ
# 3TXE8RxJxoY1DODwr4t6kTSQdDPrx3aPrtAcJFblh3JMP0SpZZpV8DHALVZkKKfF
# u2SOL9Wv57MJ6M/mhfyUot2vLVxVlWlplgwOhcHP7a40cVBczF/cAb+IBz+tuB1q
# wGGi4B3qnE2kpYju6xYz75hVcfFqXGmy3+NMZIF6oMJUSLUZmU7HUDCUyMgHt6SP
# 42r7vzRyPJEMXARiNwe5jI6oAWxyeX6dN4ZXiBDa1lVaVuK8yUd7ShbETPbTPaZ5
# BaV/yxcl1rqExPqKzIH+y/a6F33KXSYVGTSFcg/tSEd4vuXbBUuIf2UpPVkK+J2/
# 0J/o8sBSkF3nFZ/USwrvcMKEiINKokHvmivypLkhSfMIEismXSO6rke8ElECAwEA
# AaOCAV0wggFZMBIGA1UdEwEB/wQIMAYBAf8CAQAwHQYDVR0OBBYEFCgOTIkcmZfx
# gfCPCN5XEku8uHjPMB8GA1UdIwQYMBaAFGgBk7HSSkBCaZRGLBxaiKkltEdPMA4G
# A1UdDwEB/wQEAwIBhjATBgNVHSUEDDAKBggrBgEFBQcDAzB5BggrBgEFBQcBAQRt
# MGswJAYIKwYBBQUHMAGGGGh0dHA6Ly9vY3NwLmRpZ2ljZXJ0LmNvbTBDBggrBgEF
# BQcwAoY3aHR0cDovL2NhY2VydHMuZGlnaWNlcnQuY29tL0RpZ2lDZXJ0Q1NSU0E0
# MDk2Um9vdEc1LmNydDBFBgNVHR8EPjA8MDqgOKA2hjRodHRwOi8vY3JsMy5kaWdp
# Y2VydC5jb20vRGlnaUNlcnRDU1JTQTQwOTZSb290RzUuY3JsMBwGA1UdIAQVMBMw
# BwYFZ4EMAQMwCAYGZ4EMAQQBMA0GCSqGSIb3DQEBCwUAA4ICAQA66iJQhuBqWcn+
# rukrkzv8/2CGpyWTqoospvt+Nr2zsgl1xX97v588lngs94uqlTB90YLR4227PZQz
# HQAFEs0K0/UNUfoKPC9DZSn4xJxjPsLaK8N+l4d6oZBAb7AXglpxfk4ocrzM0KWY
# Tnaz3+lt0uGi8VgP8Wggh24FLxzxxiC5SqwZ7wfU7w1g7YugDn6xbcH4+yd6ZpCn
# MDcxYkGKqOtjM7V3bd3Rbl/PDySZ+chy/n6Q6ZNj9Oz/WFYlYOId7CMmI5SzbMWp
# qbdwvPNkrSYwFRtnRV9rwJo/q9jctfwrG9FQBkHMXiLHRQPw4oEoROk0AYCm26zv
# dEqc1pPCIEQHtXyOW+GqX2MORRdiePFfmG8b1xlIw8iBJOorlbEJA6tvOpDTb1I5
# +ph6tM4DwrMrS0LbGPJv0ah9fh26ZPta1xF0KC4/gdyqY7Bmij+X+atdrRZ0jdqc
# SHspWYc9U6SaXWKVXFwvkc0b19dkzECS7ebrPQuC0+euLpvDMzHIafzL21XHB1+g
# ncuwbt7/RknJoDbFKsx5x0qDQ6vfJmrajyNAMd5fGQdgcUHs75G+KWvg7M4RtGRq
# 6NHrXnBg1LHQlRbLDSCXbIoXkywzzksKuxxm9sn2gdz0/v4o4vPQHrxk8Mj6i23U
# 6h97uJQWVPhLWhQtcjc8MJmU2i5xlDCCB7wwggWkoAMCAQICEAIxmw9vuH3F75Lk
# NzIvo/AwDQYJKoZIhvcNAQELBQAwWzELMAkGA1UEBhMCVVMxGDAWBgNVBAoTDy5O
# RVQgRm91bmRhdGlvbjEyMDAGA1UEAxMpLk5FVCBGb3VuZGF0aW9uIFByb2plY3Rz
# IENvZGUgU2lnbmluZyBDQTIwHhcNMjMxMjA2MDAwMDAwWhcNMjYxMjA4MjM1OTU5
# WjCB+zETMBEGCysGAQQBgjc8AgEDEwJVUzEbMBkGCysGAQQBgjc8AgECEwpXYXNo
# aW5ndG9uMR0wGwYDVQQPDBRQcml2YXRlIE9yZ2FuaXphdGlvbjEUMBIGA1UEBRML
# NjAzIDM4OSAwNjgxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpXYXNoaW5ndG9uMRAw
# DgYDVQQHEwdSZWRtb25kMS4wLAYDVQQKEyUuTkVUIE5hbm8gRnJhbWV3b3JrICgu
# TkVUIEZvdW5kYXRpb24pMS4wLAYDVQQDEyUuTkVUIE5hbm8gRnJhbWV3b3JrICgu
# TkVUIEZvdW5kYXRpb24pMIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEA
# lsjv+rp18PKloMxGocKTMBfb1z1yvu/X4tCSDv7XQUjjIKdpFThtqy95p6HJFOi4
# bxHSz1UKk4Mf6VurKMkVhn63NqNAxavePPUcmnvOwxr3zuLYpeNMjHEJLcc8N5U3
# VzxZK9d/0MSQD/rfIXjdn47BImAG7ZrVJ1HAKMAOOQauoj3ZAxwd2Dnd+IbzDIP/
# glXG0+K9FGhf3oXrAUwvCCO+iTintYE8wiuxLav3UlyLoa8wOJCo2FiCsy+E9IGJ
# Zu9J2kUUbrwVihfD3fnqd6p097hCgy8n8SGl+t219LaAKwEUzQzC3XexbZU2dZrP
# 2XhFxCWDEfFgGA9f1N3Du6POy96eplqa9jOatfREIh98GzFMnJlzhPU+RMENpYfn
# khKbhYlb53rlcadT7RUcQf75qHsZNM6FMg9OLuEbPc1Ajjx1bzyTshK3cTRNzWPA
# 7PnxUMYh+TVbc8LX80+Txz1C3I4sn60mbGHJId2q8yTvi6kAoHk0a9KmjG1luR7e
# ri6VByKByxL06dPH6LMkAJoXdJ8zM5uGexNp4hJaoiGlWKQTA30IqvBRx+UYU4eW
# /+e78Karc8MbfV+cqu8CWaHtTtTU9xi6yAXS+773U1QsCzFnKbk2dZndaW0/tOo+
# VWbxbrNIZfOJSxWjTD6Xq484yLfLmw20A2ZCzfOB5hUCAwEAAaOCAdkwggHVMB8G
# A1UdIwQYMBaAFCgOTIkcmZfxgfCPCN5XEku8uHjPMB0GA1UdDgQWBBTCoK04Wcn9
# ZlPugnBrFvcTgHjANzA9BgNVHSAENjA0MDIGBWeBDAEDMCkwJwYIKwYBBQUHAgEW
# G2h0dHA6Ly93d3cuZGlnaWNlcnQuY29tL0NQUzAOBgNVHQ8BAf8EBAMCB4AwEwYD
# VR0lBAwwCgYIKwYBBQUHAwMwgZsGA1UdHwSBkzCBkDBGoESgQoZAaHR0cDovL2Ny
# bDMuZGlnaWNlcnQuY29tL05FVEZvdW5kYXRpb25Qcm9qZWN0c0NvZGVTaWduaW5n
# Q0EyLmNybDBGoESgQoZAaHR0cDovL2NybDQuZGlnaWNlcnQuY29tL05FVEZvdW5k
# YXRpb25Qcm9qZWN0c0NvZGVTaWduaW5nQ0EyLmNybDCBhQYIKwYBBQUHAQEEeTB3
# MCQGCCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdpY2VydC5jb20wTwYIKwYBBQUH
# MAKGQ2h0dHA6Ly9jYWNlcnRzLmRpZ2ljZXJ0LmNvbS9ORVRGb3VuZGF0aW9uUHJv
# amVjdHNDb2RlU2lnbmluZ0NBMi5jcnQwCQYDVR0TBAIwADANBgkqhkiG9w0BAQsF
# AAOCAgEAH9LTVkatT8HyjgCH2d2/YIjTmchnqi+eTiTJdzH2XRmoTeAu6RoaWdUL
# cImpmkvmvAY1C+7oejdVPbF3Zloq8SLvuYiHs/Typr8+uA+DBukwBWqVuAY6B1N6
# +cgz+xPQXza7fE9YzPZ5b/x39uCEljNa7EH0BP4HBETgwKmPjxkfy+vNdIY7wV7I
# FrtqlLt73XshXXs32Uozay01PoclicRHYX58sjXZw6SC0cQpBVx0/2usfBaVYV5u
# ezwL9rABe4/6Xu4E98fOub8NVrbDOYcB3AN8oXZHQLFjpDZ0+ucYepeidpvHeQDn
# xlSvoDzJR9uSA9t77S17qzn1E7+adK8DwYsLbI9/CmGSPRMlUrTvbpMWCUULr3t9
# s47AHWAP0TwUcETfxvRgleyheKjNh1pNVDSku2ZXniJ/GGsJ8dBr5KkuG6sjd2Ga
# jd8tIqV6xcj/xLUqfI/xzow2xauZQuJKoQEVxRDc9y4yfwyRLfglLPccfv29p79x
# In1tAV2yKofy7gQHoHQrtQyQHFP2hdihEpu78/GrSmaRhevFFzwPzov22tQPlPQZ
# jyV4FQw6PYDm37gPhsVwEacUA4+noUx7Ki8QyVDDMwLCgKSCId9TwBWSwAVFIh4I
# QVJ0Yz+oqfELvQae3H6R25/9oYj0YVE0XetN74GHqVELv2NzOOMxghghMIIYHQIB
# ATBvMFsxCzAJBgNVBAYTAlVTMRgwFgYDVQQKEw8uTkVUIEZvdW5kYXRpb24xMjAw
# BgNVBAMTKS5ORVQgRm91bmRhdGlvbiBQcm9qZWN0cyBDb2RlIFNpZ25pbmcgQ0Ey
# AhACMZsPb7h9xe+S5DcyL6PwMA0GCWCGSAFlAwQCAQUAoIHOMBkGCSqGSIb3DQEJ
# AzEMBgorBgEEAYI3AgEEMBwGCisGAQQBgjcCAQsxDjAMBgorBgEEAYI3AgEVMC8G
# CSqGSIb3DQEJBDEiBCCthuzoA++vu7Ebw4A5oe04uC6hRE1B/P/+obmTfPPd0zBi
# BgorBgEEAYI3AgEMMVQwUqAsgCoAbgBhAG4AbwBGAHIAYQBtAGUAdwBvAHIAawAg
# AHMAYwByAGkAcAB0AHOhIoAgaHR0cHM6Ly9naXRodWIuY29tL25hbm9GcmFtZXdv
# cmswDQYJKoZIhvcNAQEBBQAEggIAN8yQjtfgmHIjB3TZd3FTFrZWVGtC5MGo0tpD
# +Rkmbmz7YU/m0PxmGRZCNXDktHCE/LMPQQeo0G3tOnK8UJkCacG5HgbXfoNHpkYg
# 7ybMYmunl8mK/5K+kKCcXs5VGGrNCRApvFC7RYOKDdQlmf4OkltiLwrVW1LOvWtp
# TLl0v4vnJzcxSL0iG5D2DV0uWxSxBdpOM8IO3XtGbPlQGQecv9kPbH0sNt+4z2Tu
# bPKaO3CeRUmHTblib2EKU3vKTYhskk/eYmsqBU0M3CTgcrGnG4uBmi/aU1WCfdSM
# 1przIF00np1AlWzkLicHqCwtXRUtUcGCoo/i7O+lRGV7ZxRa2hZpMBN1G+X6CypI
# 3TGJUQxvmtkvnty5Avu4+/EchDCN8RUpe1KrkmOmkENMDjv8cwzdAhd9H5QfyIxG
# T05X4UHlFwXu/sKLaDL/0qhiywQp0lLjfYnZlz2jbsNWVshCnFJjSwxj+vGkkVbB
# vjBLDoWmJBDNb6mkqJiqYRLPbY441iSSKEuV10ko7Ygr2F906RIYW0CJUMdgbQOs
# NKMHe4UQDsMqW4Pd46vABCplkQ6xWDCHS/ZR1VqvW0lnprwiidwZqaysNMiX/3Zr
# LFPzYuzBp62k3Xjd5EAIecpJnwAl20tJLe9EgpgdI1Yuy4IbiLwNHBWPuAmYW2Xc
# gRjGvEihghSyMIIUrgYKKwYBBAGCNwMDATGCFJ4wghSaBgkqhkiG9w0BBwKgghSL
# MIIUhwIBAzEPMA0GCWCGSAFlAwQCAQUAMIIBagYLKoZIhvcNAQkQAQSgggFZBIIB
# VTCCAVECAQEGCisGAQQBhFkKAwEwMTANBglghkgBZQMEAgEFAAQg7bhN92n4rEs0
# IJafq2ZDWfrCKRyI2yGkYZzD9tt9yxECBmjCIvcj2xgTMjAyNTEwMjIwMDQzMTMu
# OTE2WjAEgAIB9KCB6aSB5jCB4zELMAkGA1UEBhMCVVMxEzARBgNVBAgTCldhc2hp
# bmd0b24xEDAOBgNVBAcTB1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jw
# b3JhdGlvbjEtMCsGA1UECxMkTWljcm9zb2Z0IElyZWxhbmQgT3BlcmF0aW9ucyBM
# aW1pdGVkMScwJQYDVQQLEx5uU2hpZWxkIFRTUyBFU046N0ExQS0wNUUwLUQ5NDcx
# NTAzBgNVBAMTLE1pY3Jvc29mdCBQdWJsaWMgUlNBIFRpbWUgU3RhbXBpbmcgQXV0
# aG9yaXR5oIIPKTCCB4IwggVqoAMCAQICEzMAAAAF5c8P/2YuyYcAAAAAAAUwDQYJ
# KoZIhvcNAQEMBQAwdzELMAkGA1UEBhMCVVMxHjAcBgNVBAoTFU1pY3Jvc29mdCBD
# b3Jwb3JhdGlvbjFIMEYGA1UEAxM/TWljcm9zb2Z0IElkZW50aXR5IFZlcmlmaWNh
# dGlvbiBSb290IENlcnRpZmljYXRlIEF1dGhvcml0eSAyMDIwMB4XDTIwMTExOTIw
# MzIzMVoXDTM1MTExOTIwNDIzMVowYTELMAkGA1UEBhMCVVMxHjAcBgNVBAoTFU1p
# Y3Jvc29mdCBDb3Jwb3JhdGlvbjEyMDAGA1UEAxMpTWljcm9zb2Z0IFB1YmxpYyBS
# U0EgVGltZXN0YW1waW5nIENBIDIwMjAwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAw
# ggIKAoICAQCefOdSY/3gxZ8FfWO1BiKjHB7X55cz0RMFvWVGR3eRwV1wb3+yq0OX
# DEqhUhxqoNv6iYWKjkMcLhEFxvJAeNcLAyT+XdM5i2CgGPGcb95WJLiw7HzLiBKr
# xmDj1EQB/mG5eEiRBEp7dDGzxKCnTYocDOcRr9KxqHydajmEkzXHOeRGwU+7qt8M
# d5l4bVZrXAhK+WSk5CihNQsWbzT1nRliVDwunuLkX1hyIWXIArCfrKM3+RHh+Sq5
# RZ8aYyik2r8HxT+l2hmRllBvE2Wok6IEaAJanHr24qoqFM9WLeBUSudz+qL51HwD
# YyIDPSQ3SeHtKog0ZubDk4hELQSxnfVYXdTGncaBnB60QrEuazvcob9n4yR65pUN
# BCF5qeA4QwYnilBkfnmeAjRN3LVuLr0g0FXkqfYdUmj1fFFhH8k8YBozrEaXnsSL
# 3kdTD01X+4LfIWOuFzTzuoslBrBILfHNj8RfOxPgjuwNvE6YzauXi4orp4Sm6tF2
# 45DaFOSYbWFK5ZgG6cUY2/bUq3g3bQAqZt65KcaewEJ3ZyNEobv35Nf6xN6FrA6j
# F9447+NHvCjeWLCQZ3M8lgeCcnnhTFtyQX3XgCoc6IRXvFOcPVrr3D9RPHCMS6Ck
# g8wggTrtIVnY8yjbvGOUsAdZbeXUIQAWMs0d3cRDv09SvwVRd61evQIDAQABo4IC
# GzCCAhcwDgYDVR0PAQH/BAQDAgGGMBAGCSsGAQQBgjcVAQQDAgEAMB0GA1UdDgQW
# BBRraSg6NS9IY0DPe9ivSek+2T3bITBUBgNVHSAETTBLMEkGBFUdIAAwQTA/Bggr
# BgEFBQcCARYzaHR0cDovL3d3dy5taWNyb3NvZnQuY29tL3BraW9wcy9Eb2NzL1Jl
# cG9zaXRvcnkuaHRtMBMGA1UdJQQMMAoGCCsGAQUFBwMIMBkGCSsGAQQBgjcUAgQM
# HgoAUwB1AGIAQwBBMA8GA1UdEwEB/wQFMAMBAf8wHwYDVR0jBBgwFoAUyH7SaoUq
# G8oZmAQHJ89QEE9oqKIwgYQGA1UdHwR9MHsweaB3oHWGc2h0dHA6Ly93d3cubWlj
# cm9zb2Z0LmNvbS9wa2lvcHMvY3JsL01pY3Jvc29mdCUyMElkZW50aXR5JTIwVmVy
# aWZpY2F0aW9uJTIwUm9vdCUyMENlcnRpZmljYXRlJTIwQXV0aG9yaXR5JTIwMjAy
# MC5jcmwwgZQGCCsGAQUFBwEBBIGHMIGEMIGBBggrBgEFBQcwAoZ1aHR0cDovL3d3
# dy5taWNyb3NvZnQuY29tL3BraW9wcy9jZXJ0cy9NaWNyb3NvZnQlMjBJZGVudGl0
# eSUyMFZlcmlmaWNhdGlvbiUyMFJvb3QlMjBDZXJ0aWZpY2F0ZSUyMEF1dGhvcml0
# eSUyMDIwMjAuY3J0MA0GCSqGSIb3DQEBDAUAA4ICAQBfiHbHfm21WhV150x4aPpO
# 4dhEmSUVpbixNDmv6TvuIHv1xIs174bNGO/ilWMm+Jx5boAXrJxagRhHQtiFprSj
# MktTliL4sKZyt2i+SXncM23gRezzsoOiBhv14YSd1Klnlkzvgs29XNjT+c8hIfPR
# e9rvVCMPiH7zPZcw5nNjthDQ+zD563I1nUJ6y59TbXWsuyUsqw7wXZoGzZwijWT5
# oc6GvD3HDokJY401uhnj3ubBhbkR83RbfMvmzdp3he2bvIUztSOuFzRqrLfEvsPk
# VHYnvH1wtYyrt5vShiKheGpXa2AWpsod4OJyT4/y0dggWi8g/tgbhmQlZqDUf3Uq
# UQsZaLdIu/XSjgoZqDjamzCPJtOLi2hBwL+KsCh0Nbwc21f5xvPSwym0Ukr4o5sC
# cMUcSy6TEP7uMV8RX0eH/4JLEpGyae6Ki8JYg5v4fsNGif1OXHJ2IWG+7zyjTDfk
# mQ1snFOTgyEX8qBpefQbF0fx6URrYiarjmBprwP6ZObwtZXJ23jK3Fg/9uqM3j0P
# 01nzVygTppBabzxPAh/hHhhls6kwo3QLJ6No803jUsZcd4JQxiYHHc+Q/wAMcPUn
# YKv/q2O444LO1+n6j01z5mggCSlRwD9faBIySAcA9S8h22hIAcRQqIGEjolCK9F6
# nK9ZyX4lhthsGHumaABdWzCCB58wggWHoAMCAQICEzMAAABTUsIDi+Wa+E4AAAAA
# AFMwDQYJKoZIhvcNAQEMBQAwYTELMAkGA1UEBhMCVVMxHjAcBgNVBAoTFU1pY3Jv
# c29mdCBDb3Jwb3JhdGlvbjEyMDAGA1UEAxMpTWljcm9zb2Z0IFB1YmxpYyBSU0Eg
# VGltZXN0YW1waW5nIENBIDIwMjAwHhcNMjUwMjI3MTk0MDI2WhcNMjYwMjI2MTk0
# MDI2WjCB4zELMAkGA1UEBhMCVVMxEzARBgNVBAgTCldhc2hpbmd0b24xEDAOBgNV
# BAcTB1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3JhdGlvbjEtMCsG
# A1UECxMkTWljcm9zb2Z0IElyZWxhbmQgT3BlcmF0aW9ucyBMaW1pdGVkMScwJQYD
# VQQLEx5uU2hpZWxkIFRTUyBFU046N0ExQS0wNUUwLUQ5NDcxNTAzBgNVBAMTLE1p
# Y3Jvc29mdCBQdWJsaWMgUlNBIFRpbWUgU3RhbXBpbmcgQXV0aG9yaXR5MIICIjAN
# BgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAlzkp+i/zCu1Q53pMxjbNF6qhwdHM
# 8SCqZ2QCNT/t8WMyh3qRnEXYPTrzMC83O9xf6cic/uM3cUrYr1Hp2nbd409hcM79
# LGJ1+jd0rH1g4m7gGYuYHRxKInF7U6GcW4Jh/lzCzBlm0DY2seuvT/FJwVF4P1W7
# OM1V+18Qa8FvB2lxbh+s5iJHBTRozEY0p9A0RPk27gowiMA1ONeoBHPFedk+H/B1
# RdvBEIjxEZR0Xt5NQ3Mv/2s+bN89Imt7etD5DMH1PbJksfq1/1xI6Qluir4pSBLy
# 1eiOEJOV3Gn3SBiDwU82GCoOQEDaPb2l4ampCFmFsw+ge+NWjYvDIvCPnXmvE79b
# E9rZsosLGMggdkYSiaQ5RF3HBRT1alztZYKEWGLKWdz4tKLZ4vDmTe21dDm1oNdq
# o5qhENgBn7jGr+byoq/x5ixNEvIoyQbBCL5WrDS7oOoP471C5WpAjSqG1wQLIjE/
# 11gxwoV52aLsocYkii7Cbr+5q3CdsTuU3bgRpDOpXU6t4PI/W0QMqEl6uD+xCVEo
# 0NVyZNEhRlVNWiYyVlAD00b4smR7iGAWy314d/R7H95aDTwyK1yUy+vgiLZB7Vna
# 6yucTgbuir2tSNkfpLDP1vuoVf3C9UU8QE3xKnBf2wpx/jRsMTOK8A+B7K2pOZIy
# I2HVA+IylCHezkECAwEAAaOCAcswggHHMB0GA1UdDgQWBBRuZESrI2MaHwWzQj0h
# 1vRk//aPajAfBgNVHSMEGDAWgBRraSg6NS9IY0DPe9ivSek+2T3bITBsBgNVHR8E
# ZTBjMGGgX6BdhltodHRwOi8vd3d3Lm1pY3Jvc29mdC5jb20vcGtpb3BzL2NybC9N
# aWNyb3NvZnQlMjBQdWJsaWMlMjBSU0ElMjBUaW1lc3RhbXBpbmclMjBDQSUyMDIw
# MjAuY3JsMHkGCCsGAQUFBwEBBG0wazBpBggrBgEFBQcwAoZdaHR0cDovL3d3dy5t
# aWNyb3NvZnQuY29tL3BraW9wcy9jZXJ0cy9NaWNyb3NvZnQlMjBQdWJsaWMlMjBS
# U0ElMjBUaW1lc3RhbXBpbmclMjBDQSUyMDIwMjAuY3J0MAwGA1UdEwEB/wQCMAAw
# FgYDVR0lAQH/BAwwCgYIKwYBBQUHAwgwDgYDVR0PAQH/BAQDAgeAMGYGA1UdIARf
# MF0wUQYMKwYBBAGCN0yDfQEBMEEwPwYIKwYBBQUHAgEWM2h0dHA6Ly93d3cubWlj
# cm9zb2Z0LmNvbS9wa2lvcHMvRG9jcy9SZXBvc2l0b3J5Lmh0bTAIBgZngQwBBAIw
# DQYJKoZIhvcNAQEMBQADggIBAIhz/2VPMlLSGnTEmp99qQ3GGUnxhbrAMbPf1Yr2
# bPoy6x3lxp3i3YSON7XtMAombBDA4SAsxPx8Y2G8Ii95c5uaziwO4bQLAblGJsh1
# +0lZhzoAQxzOI5MPpsq3F3t+8CHh6JUak8u3MJscVPYNK/kPQAJ0/Hddk+N/AVEo
# BWIjIy0v/ZttmOte7lYsyFdPmuL74FuLkQ7bKB6AWQAH4BGgODSsbk924NXp1iZU
# 2gdV0AyZcLYofKsisfDUnZs8bALtwUgHPWFuV+li5vnbUUCmZ0ea5Tzn7Qn5XHjK
# VeDzLT+nBBx6GMPRUaETfIWC88OXAx5jiMV4jns2KHFjMw491Ukcij3vU75M6xtq
# iNeZ7e8R0nnpql2+LNyrvmYCVjP0ym+QlQZsyvLKKOiP39QbKxK7Y8l5lxGJwxF2
# N2CmEgtYZVapBKj0OKeXGDEv4kA/dJGBOlSOKCucIHnXlDBFIOwMVMrRFqhra4JB
# Kw+YJTIsc4FauSgJc4+tckaO64qkjHVWK1nLr9xXmVoYay+5jqOu63I64aaF1R4u
# TClW5uCmgsYQn5z4y2uAgNKmBm4UgGKY1D3xsAKrOIsfIRodRNImOMFuoNfRjVpy
# Izq/T1qwWxarvaBFXIVnDYN7XbAoXGLwydf7ik06aCQLBcgtzC/zQ53jjqa+LUE5
# qOiNMYID1DCCA9ACAQEweDBhMQswCQYDVQQGEwJVUzEeMBwGA1UEChMVTWljcm9z
# b2Z0IENvcnBvcmF0aW9uMTIwMAYDVQQDEylNaWNyb3NvZnQgUHVibGljIFJTQSBU
# aW1lc3RhbXBpbmcgQ0EgMjAyMAITMwAAAFNSwgOL5Zr4TgAAAAAAUzANBglghkgB
# ZQMEAgEFAKCCAS0wGgYJKoZIhvcNAQkDMQ0GCyqGSIb3DQEJEAEEMC8GCSqGSIb3
# DQEJBDEiBCAP7sFLwSK+hjB0dvjcLVvkeKIk/uiiQzv1ZODnWUwMGTCB3QYLKoZI
# hvcNAQkQAi8xgc0wgcowgccwgaAEIEB/HwRijKuU/vjAU7BdHBp/tHRfrRMPkNej
# ldGf2h0jMHwwZaRjMGExCzAJBgNVBAYTAlVTMR4wHAYDVQQKExVNaWNyb3NvZnQg
# Q29ycG9yYXRpb24xMjAwBgNVBAMTKU1pY3Jvc29mdCBQdWJsaWMgUlNBIFRpbWVz
# dGFtcGluZyBDQSAyMDIwAhMzAAAAU1LCA4vlmvhOAAAAAABTMCIEIL4tPtn6NGKq
# CCCg5wx9r+FvG3H4qC28m9m7nwJGwj0XMA0GCSqGSIb3DQEBCwUABIICAFVw+0Gt
# zKyhxSVPTo2szx+7HOF5vqwIEDYUOyE+SaM9k+MRrIKZON9emiXVLQstKUfAKMPV
# cXWcsatOwmiobZOeeChWwOAX+L/EoFyAcnPdCLFK8U9nBfd/jsdOhUdhiNg6NU8m
# Ml0VxSuuBG+DmA/2Qt8eEd664cna/K0iDqf2p/XDUIsIW9YNO4CMg2XE0ArtAUDM
# Y17yZs+uCmHsyoefSITqb4fC3Ho/glL3X5un9bERv+W9yRUojh8vwsAwgczvGLzC
# cajbt2/ig1sO7mhm2keuB3M0HDOdXWhgQuqtEsODpLGiYxLTCpm4z04+rogogUbe
# el2AmhFuBD42u+e8hTC1WpygZMRpVADIz4klUMMsEw7U7j09ohQI92mnvTRRvIDo
# UB8LCp95f4EF6XrYitTKPjQvkOUOBtZTwXvwVtYVu1tmi2zaWegtIpl3IcETN8k3
# gKuhNOPUkfXHqaYLwSsht27GIqy0SefgwQTSu7VkOx6wMD8GedLHmhKY/yf6s1k1
# fPuUSE3LkK/9IHEUtOlrkCLK2imSasQXAlekz64c/KD6ovTy2VLjDhNV7mZmJ+e9
# eUjj7sIiYFCrHlHLbprCIlhsZuX5OQLsDB90QcOxoZP56MhJa1M8vs1ZUFVNRMri
# Jtld6OPYJWBR2SkahhbtahSPPOAhPOkobDQh
# SIG # End signature block
