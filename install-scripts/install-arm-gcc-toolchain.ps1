# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

# This PS installs the ARM GNU GCC toolchain from our Cloudsmith repository if it's not already available

[CmdletBinding(SupportsShouldProcess = $true)]
param (
    [Parameter(HelpMessage = "Path to the folder where the tool is to be installed.")][string]$Path,
    [Parameter(HelpMessage = "GNU GCC version requested.")][string]$Version,
    [switch]$force = $false
)

# set default GNU GCC version
if ([string]::IsNullOrEmpty($Version)) {
    $Version = "14.2.rel1"
}

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
        $toolPath = $env:NF_TOOLS_PATH
    }
    else {
        # use default
        $toolPath = 'C:\nftools'
    }

    # append the tool path
    $toolPath = $toolPath + "\GNU_Tools_ARM_Embedded\$Version"
}

# check if path already exists
$gnuGccPathExists = Test-Path $toolPath -ErrorAction SilentlyContinue    

# download, if needed
If ($gnuGccPathExists -eq $False -or $force) {
    $url = "https://developer.arm.com/-/media/Files/downloads/gnu/" + $Version + "/binrel/arm-gnu-toolchain-" + $Version + "-mingw-w64-i686-arm-none-eabi.zip"
    $output = "$zipRoot\arm-gnu-toolchain-" + $Version + "-mingw-w64-i686-arm-none-eabi.zip"

    # Don't download again if already exists
    if (![System.IO.File]::Exists($output) -or $force) {
        "Download URL is: '$url'" | Write-Host -ForegroundColor White

        "Downloading ARM GNU GCC toolchain..." | Write-Host -ForegroundColor White -NoNewline
            
        # download Zip with toolchain
        (New-Object Net.WebClient).DownloadFile($url, $output)

        "OK" | Write-Host -ForegroundColor Green
    }

    # unzip to tool path, if not on Azure
    if ($IsAzurePipelines -eq $False) {

        "Installing ARM GNU GCC toolchain..." | Write-Host -ForegroundColor White -NoNewline

        # unzip toolchain
        Expand-Archive $output -DestinationPath $toolPath > $null

        # update tool path to include versioned toolchain folder
        $toolPath = $toolPath + "\arm-gnu-toolchain-" + $Version + "-mingw-w64-i686-arm-none-eabi"

        "OK" | Write-Host -ForegroundColor Green
    }
}
else {
    "Skipping install of ARM GNU GCC toolchain" | Write-Host -ForegroundColor Yellow
}

# set env variable, if not on Azure
if ($IsAzurePipelines -eq $False) {
    # need to replace forward slash for paths to work with GCC and CMake
    $toolPath = "$toolPath".Replace('\', '/')

    "Setting User Environment Variable ARM_GCC_PATH='" + $env:ARM_GCC_PATH + "'" | Write-Host -ForegroundColor Yellow

    $env:ARM_GCC_PATH = $toolPath

    try {
        # this call can fail if the script is not run with appropriate permissions
        [System.Environment]::SetEnvironmentVariable("ARM_GCC_PATH", $env:ARM_GCC_PATH, "User")
    }
    catch {
        "Failed to set User Environment Variable ARM_GCC_PATH. Make sure to manually add 'ARM_GCC_PATH' with '" + $env:ARM_GCC_PATH + "'." | Write-Host -ForegroundColor Red
    }
}

# SIG # Begin signature block
# MIIshwYJKoZIhvcNAQcCoIIseDCCLHQCAQExDzANBglghkgBZQMEAgEFADB5Bgor
# BgEEAYI3AgEEoGswaTA0BgorBgEEAYI3AgEeMCYCAwEAAAQQH8w7YFlLCE63JNLG
# KX7zUQIBAAIBAAIBAAIBAAIBADAxMA0GCWCGSAFlAwQCAQUABCDU9MAvvlKIHjpv
# CxntV2GMKCBnOeh5lAkslt0J6fHxEaCCE7wwggVkMIIDTKADAgECAhAGzuExvm1V
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
# CSqGSIb3DQEJBDEiBCAvspAtMSvnBN6SDTQEY+aTfIqWpkYXP6JI0FLMloc7OTBi
# BgorBgEEAYI3AgEMMVQwUqAsgCoAbgBhAG4AbwBGAHIAYQBtAGUAdwBvAHIAawAg
# AHMAYwByAGkAcAB0AHOhIoAgaHR0cHM6Ly9naXRodWIuY29tL25hbm9GcmFtZXdv
# cmswDQYJKoZIhvcNAQEBBQAEggIAQk09RfomfF+tSDN7SyR/HPWVn32knD49t1VH
# pGJUM07UOt0chcLSy1/60TelcnAPHHFlraQxUZjJ5sMF7/DhcaI4gvzbcxDggoJn
# wR49R5zXKwsxCE7HtvYpxFZBw7tQD4KGnEM0tt68PtIiQ3lKvNTKgekto5t5FzVe
# /Vrxo2qfSZz6gAJkGHZT/x6rHCPr2hrpDQ3JpH+Ch0n+uU82bCZUwcU1m9d9ng2f
# +02gdfV34jrHUFbjf64Pk0pt18k9IAQM45Qti7dcbmyPf+1EXL0G2qFlg9jGMbNY
# QvRdfO9x5gsk3cts+S6Zls0b94dt4Ec5I6kUR3E7lt3ivqsr/2sKHbAkFomfjBnM
# 6pOnhEjHHjzt2nQUznkJgjbks7k18zGdJhyPdnzo4ZExPL6UeEe+iU6k5Cs7Agx7
# Z26fcsHdXABoIhWJ0UJQoZqnAzBQMOHO9E6wKLd8elV1ii49KjFo9ncXOC1FLXtn
# NBiRkw0JAlmYn5iCHakAb4Fqa0sH2OjEhqGjThevyFiQys+7o1bd4r0jZ7+0PVrz
# 02wVULQGjuOwbZLF9E6RIOiKbFDbICJUp5aesUvfRNtjPrOcXQunNhDd7Dh0wXPL
# PBZK1MOpAJI0bJu3kM8GpARBk5jcEuBaZxDL6nyuUhYmKty3QCbzRFCVHh2Kqu7b
# zZe1+7ShghSyMIIUrgYKKwYBBAGCNwMDATGCFJ4wghSaBgkqhkiG9w0BBwKgghSL
# MIIUhwIBAzEPMA0GCWCGSAFlAwQCAQUAMIIBagYLKoZIhvcNAQkQAQSgggFZBIIB
# VTCCAVECAQEGCisGAQQBhFkKAwEwMTANBglghkgBZQMEAgEFAAQgwbYCTZ3J0bL8
# pmX0Yf83fPbocC7ABKFmLJ0gkNOL3mgCBmdkGflexBgTMjAyNTAxMDMxODE4NDIu
# NDY2WjAEgAIB9KCB6aSB5jCB4zELMAkGA1UEBhMCVVMxEzARBgNVBAgTCldhc2hp
# bmd0b24xEDAOBgNVBAcTB1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jw
# b3JhdGlvbjEtMCsGA1UECxMkTWljcm9zb2Z0IElyZWxhbmQgT3BlcmF0aW9ucyBM
# aW1pdGVkMScwJQYDVQQLEx5uU2hpZWxkIFRTUyBFU046NDUxQS0wNUUwLUQ5NDcx
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
# nK9ZyX4lhthsGHumaABdWzCCB58wggWHoAMCAQICEzMAAABCmshvpRumfQYAAAAA
# AEIwDQYJKoZIhvcNAQEMBQAwYTELMAkGA1UEBhMCVVMxHjAcBgNVBAoTFU1pY3Jv
# c29mdCBDb3Jwb3JhdGlvbjEyMDAGA1UEAxMpTWljcm9zb2Z0IFB1YmxpYyBSU0Eg
# VGltZXN0YW1waW5nIENBIDIwMjAwHhcNMjQwNDE4MTc1OTE3WhcNMjUwNDE3MTc1
# OTE3WjCB4zELMAkGA1UEBhMCVVMxEzARBgNVBAgTCldhc2hpbmd0b24xEDAOBgNV
# BAcTB1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3JhdGlvbjEtMCsG
# A1UECxMkTWljcm9zb2Z0IElyZWxhbmQgT3BlcmF0aW9ucyBMaW1pdGVkMScwJQYD
# VQQLEx5uU2hpZWxkIFRTUyBFU046NDUxQS0wNUUwLUQ5NDcxNTAzBgNVBAMTLE1p
# Y3Jvc29mdCBQdWJsaWMgUlNBIFRpbWUgU3RhbXBpbmcgQXV0aG9yaXR5MIICIjAN
# BgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEA2twOzEyo1PCxyO4bhS8Z2L4leL3I
# OcD3HdEi52b+w/wmSrr+gksTBMfW8o6+ptz3lA25CHfTEqfjTp2m6XsklrQ8n65H
# BDKO5tyWr+58Qw36jnxSZUIuA3+ldPfhrOgNTEIGF/Qu+ysg6AYBXXTG07HZ+ja2
# MEABgVrrPfVOJfWz4hpnzWDWn6uMK/VxaaMYU1U1Hszn/TYxjMEKYnn0lNDICQqW
# nigmW2syE9yANxcvqAc6cijRxEy4QBeS/x3d1yqj6Q8PVk+jViUB34eYSt6DEkKc
# Mlpf3BU+i2NPD2uSuQUiZ9wNwjx4ewlvbNABTwbpLp4kngqE9mBeExgWi75HpmXa
# NKwwuHWZf/C9EpaQuUhGgSMjiIKbEaNcoIVKzv3cf0cW8bcK94vYA32QIgdJwvYV
# QkK5aHcn3zjIfn8wRZmVUExlxdHrydtxxQiAfnYvuWdMUarXfwwE79hcFrPnMiBR
# HP/iq4yaxIXzRO/nDoWOEZDLJXql5QBtu6ifriXwPhE0sRMu6Ry5tNMKXiQvjcT+
# M+zJdGIbCQT46hY1tmvilDKSSANcSIDx51FxI55HBArNvSDMiu2wj5X/akt0A/oH
# HE9Q8yfghV7fKZzpHQylrnFNjzjcXj+XJEcJAhs8vuqdGNOvsIrNs/lbKPJn7RUj
# nSKDU1AHtbkA3TsCAwEAAaOCAcswggHHMB0GA1UdDgQWBBQWZTL01RLFsDojzhz0
# iS+hUWbxWTAfBgNVHSMEGDAWgBRraSg6NS9IY0DPe9ivSek+2T3bITBsBgNVHR8E
# ZTBjMGGgX6BdhltodHRwOi8vd3d3Lm1pY3Jvc29mdC5jb20vcGtpb3BzL2NybC9N
# aWNyb3NvZnQlMjBQdWJsaWMlMjBSU0ElMjBUaW1lc3RhbXBpbmclMjBDQSUyMDIw
# MjAuY3JsMHkGCCsGAQUFBwEBBG0wazBpBggrBgEFBQcwAoZdaHR0cDovL3d3dy5t
# aWNyb3NvZnQuY29tL3BraW9wcy9jZXJ0cy9NaWNyb3NvZnQlMjBQdWJsaWMlMjBS
# U0ElMjBUaW1lc3RhbXBpbmclMjBDQSUyMDIwMjAuY3J0MAwGA1UdEwEB/wQCMAAw
# FgYDVR0lAQH/BAwwCgYIKwYBBQUHAwgwDgYDVR0PAQH/BAQDAgeAMGYGA1UdIARf
# MF0wUQYMKwYBBAGCN0yDfQEBMEEwPwYIKwYBBQUHAgEWM2h0dHA6Ly93d3cubWlj
# cm9zb2Z0LmNvbS9wa2lvcHMvRG9jcy9SZXBvc2l0b3J5Lmh0bTAIBgZngQwBBAIw
# DQYJKoZIhvcNAQEMBQADggIBAGzy+mEn+SNj81hNpgWnhhNr9e4ZwB1llRy/ljj0
# wN1JIrXa0tUBzlflRNZq4WQVM5BKDzoIXSsoUgbCN2QTCzM4Q0PcoKJiJ0tCuZp6
# foJQpNpdAc/zuK90XGosALsCJlUuFfJkL3WTjWFt8Mz6t2JZZtdis2yCHUuGIAfB
# l/gUl8EtbGCIfDGEsHyvgVSRJGOVApV3OVMPAaUsQQ3jqp28tIR9OOd+jOEtROc2
# 1mJfwhcpqqLsmUV2WvJXEaRXylXGDRqibW/hhErfv8wXpAloo8fxG7ONVxk1HTm9
# M9JbFpw2ICCQkRbnzQxKW6KOwtRnxrunx7Cze11eIv0JMYubjnKSSMzOWuqnLnA0
# a86b1wVU/1lKjB0SaVz6IJKKyC+UH6T0pmEVNDE3cem/mbB6g6Qp5VDmOLqA3vFW
# S3lFiu/KpEPc/RrJ0Kxyr449MHVjO4UtUOqlsYSJhU48nJ9hZVHGSamrgb1FKtBV
# VCrCRYxyaeg9whS7WfmwXME53y8MeZTqfbQK6VFTYWdKt/He3kVu16K3mkVHaohb
# d4FMMz9fT0tykS1HN51Zg7SBLcNH58dvOoDg6S79yNa3e53gzTjQ6tNNZypQ5Hrs
# HrJaEgUszFqvC+1S4DYZGx5TzyHqbX3yv0H3tNPRhR8xXPlGXKTaAX1kGlslVzQA
# xG7qMYID1DCCA9ACAQEweDBhMQswCQYDVQQGEwJVUzEeMBwGA1UEChMVTWljcm9z
# b2Z0IENvcnBvcmF0aW9uMTIwMAYDVQQDEylNaWNyb3NvZnQgUHVibGljIFJTQSBU
# aW1lc3RhbXBpbmcgQ0EgMjAyMAITMwAAAEKayG+lG6Z9BgAAAAAAQjANBglghkgB
# ZQMEAgEFAKCCAS0wGgYJKoZIhvcNAQkDMQ0GCyqGSIb3DQEJEAEEMC8GCSqGSIb3
# DQEJBDEiBCD4Iz64UADCD0IPO0LzK/cTI0vEu7i9Qoft6CwqDdmzRzCB3QYLKoZI
# hvcNAQkQAi8xgc0wgcowgccwgaAEIK9+c1C+wP2H74bAFJeodjGjQTspUszYeQw+
# AkLteuKqMHwwZaRjMGExCzAJBgNVBAYTAlVTMR4wHAYDVQQKExVNaWNyb3NvZnQg
# Q29ycG9yYXRpb24xMjAwBgNVBAMTKU1pY3Jvc29mdCBQdWJsaWMgUlNBIFRpbWVz
# dGFtcGluZyBDQSAyMDIwAhMzAAAAQprIb6Ubpn0GAAAAAABCMCIEIEdx/RmxSxBb
# q461auTbCma6cuSM0M0IcqjuxqzuIKizMA0GCSqGSIb3DQEBCwUABIICAIYQzeRW
# DkWsmtzLH3o35n92H484pdXp5AwBFsMwu1u4XyvlxbPlL9qb3rjw9lc6lNtA9Gw5
# k9dICklPWSnDZ0eyTdlmAaDo82a1I5rbjFzA50zXNwV75CRKWHhPjiwn1Z0dzf+y
# Ug6FdCZsJOFqptHJ4HTdaVDf7JI755j4IQXtWx85yRz81HAmYEfw7iHowNFyTkjX
# 7/nedV/4M7xtYXRwmKar1BG7sq7nztfE1pCSqCa/+gUuee604ZKi2/HGaMDRQLvY
# RCe1xf+0sVKUC7u2Gb4Ud+aY2fU+80xLoSDV1CR6ZX7KTXdKLWB/u6JQPX9bEDIn
# zk0LUom4Ym1Q+ffrn2SGF+UnpdhIpTp5yCZSfBKpj/JsVk589GgLdbRWVX0yEHwV
# 7LxfHRN42Ub0bismdgeZBbEdo8y0Gdr3U3sDB8upLCh+99I0KOIodXXJFNf0IPh0
# 6sp/hawCjlM+n7D0xNpAmhTRFIksPv4+JxG6d/2rfj4yLOLh32lKRM7agHZ/xEU+
# v986ijQEoUxa/GTbxNSR/ZxsJfjPuggKfHX4mStIzZRZK3+MviekrwpRGBfpQzoy
# d/yCaux9M8Ism7eaAIJ29pwu+OWv72sWXiYsGuJtr6F64tvbfW3EGx284Qz64gaz
# kkzFW7ILL3jARRsSRD6Bv96K2U5pnKj7F1jc
# SIG # End signature block
