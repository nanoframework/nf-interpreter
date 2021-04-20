# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

# This PS installs IDF esp32 sdk

[CmdletBinding(SupportsShouldProcess = $true)]
param (
    [Parameter(HelpMessage = "Path to the folder where the tool is to be installed.")][string]$Path,
    [switch]$force = $false
)

$idfVersion = "esp-idf-v3.3.1"

# check if running on Azure Pipelines by looking at this two environment variables
$IsAzurePipelines = $env:Agent_HomeDirectory -and $env:Build_BuildNumber

if ($IsAzurePipelines) {
    $zipRoot = "$env:Agent_TempDirectory"

    $Path = "$env:Agent_TempDirectory\ESP32_Tools\$idfVersion"
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

    $finalPath = $Path + "\$idfVersion"

    # check if path already exists
    $esp32IdfPathExists = Test-Path $finalPath -ErrorAction SilentlyContinue
}
else {
    # check if path already exists
    $esp32IdfPathExists = Test-Path $Path -ErrorAction SilentlyContinue
    
    # need this here for the replacements ahead
    $finalPath = $Path
}

If ($esp32IdfPathExists -eq $False -or $force) {
    $url = "https://dl.espressif.com/dl/esp-idf/releases/$idfVersion.zip"
    $output = "$zipRoot\esp-idf.zip"

    # Don't download again if already exists
    if (![System.IO.File]::Exists($output) -or $force) {
        "Downloading ESP32 IDF..." | Write-Host -ForegroundColor White -NoNewline
        
        # Stop security tripping us up
        [Net.ServicePointManager]::SecurityProtocol = "tls12, tls11, tls"

        # download zip with Espressif IDF
        (New-Object Net.WebClient).DownloadFile($url, $output)

        "OK" | Write-Host -ForegroundColor Green
    }

    # unzip to install path, if not on Azure
    if ($IsAzurePipelines -eq $False) {
        "Installing Espressif IDF @ '$Path'..." | Write-Host -ForegroundColor White -NoNewline

        # unzip
        Expand-Archive -Path $output -DestinationPath $Path

        "OK" | Write-Host -ForegroundColor Green
    }
    
    # set path destination after expand operation
    $Path = $finalPath
}
else {
    "Skipping install of Espressif IDF" | Write-Host -ForegroundColor Yellow
    
    # set path destination after expand operation
    $Path = $finalPath
}

# set env variable, if not on Azure
if ($IsAzurePipelines -eq $False) {
    # need to replace forward slash for paths to work with GCC and CMake
    $Path = "$Path".Replace('\', '/')

    $env:ESP32_IDF_PATH = $Path
    # this call can fail if the script is not run with appropriate permissions
    [System.Environment]::SetEnvironmentVariable("ESP32_IDF_PATH", $env:ESP32_IDF_PATH, "User")

    "Set User Environment ESP32_IDF_PATH='" + $env:ESP32_IDF_PATH + "'" | Write-Host -ForegroundColor Yellow
}

# install python pyserial, if not on Azure
if ($IsAzurePipelines -eq $False) {
    "Installing python pyserial..." | Write-Host -ForegroundColor White
    
    # Make sure serial package is installed in python otherwise
    # the esptool.py tool won't run
    python -m pip install pyserial --upgrade pip
}

# on Azure, adjust IDF path
if ($IsAzurePipelines -eq $True) {
    # need to replace forward slash for paths to work with GCC and CMake
    $tempDirPath = "$Path".Replace('\', '/')

    Write-Host "$("##vso[task.setvariable variable=ESP32_IDF_PATH]")$tempDirPath"
}

# SIG # Begin signature block
# MIIeIQYJKoZIhvcNAQcCoIIeEjCCHg4CAQExDzANBglghkgBZQMEAgEFADB5Bgor
# BgEEAYI3AgEEoGswaTA0BgorBgEEAYI3AgEeMCYCAwEAAAQQH8w7YFlLCE63JNLG
# KX7zUQIBAAIBAAIBAAIBAAIBADAxMA0GCWCGSAFlAwQCAQUABCCS1YBpCvgawrCu
# 0f5POC01KaQsR2mQqa5FyUg3NtFfkqCCDg8wggPFMIICraADAgECAhACrFwmagtA
# m48LefKuRiV3MA0GCSqGSIb3DQEBBQUAMGwxCzAJBgNVBAYTAlVTMRUwEwYDVQQK
# EwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5jb20xKzApBgNV
# BAMTIkRpZ2lDZXJ0IEhpZ2ggQXNzdXJhbmNlIEVWIFJvb3QgQ0EwHhcNMDYxMTEw
# MDAwMDAwWhcNMzExMTEwMDAwMDAwWjBsMQswCQYDVQQGEwJVUzEVMBMGA1UEChMM
# RGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3d3cuZGlnaWNlcnQuY29tMSswKQYDVQQD
# EyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5jZSBFViBSb290IENBMIIBIjANBgkqhkiG
# 9w0BAQEFAAOCAQ8AMIIBCgKCAQEAxszlc+b71LvlLS0ypt/lgT/JzSVJtnEqw9WU
# NGeiChywX2mmQLHEt7KP0JikqUFZOtPclNY823Q4pErMTSWC90qlUxI47vNJbXGR
# fmO2q6Zfw6SE+E9iUb74xezbOJLjBuUIkQzEKEFV+8taiRV+ceg1v01yCT2+OjhQ
# W3cxG42zxyRFmqesbQAUWgS3uhPrUQqYQUEiTmVhh4FBUKZ5XIneGUpX1S7mXRxT
# LH6YzRoGFqRoc9A0BBNcoXHTWnxV215k4TeHMFYE5RG0KYAS8Xk5iKICEXwnZreI
# t3jyygqoOKsKZMK/Zl2VhMGhJR6HXRpQCyASzEG7bgtROLhLywIDAQABo2MwYTAO
# BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUsT7DaQP4
# v0cB1JgmGggC72NkK8MwHwYDVR0jBBgwFoAUsT7DaQP4v0cB1JgmGggC72NkK8Mw
# DQYJKoZIhvcNAQEFBQADggEBABwaBpfc15yfPIhmBghXIdshR/gqZ6q/GDJ2QBBX
# wYrzetkRZY41+p78RbWe2UwxS7iR6EMsjrN4ztvjU3lx1uUhlAHaVYeaJGT2imbM
# 3pw3zag0sWmbI8ieeCIrcEPjVUcxYRnvWMWFL04w9qAxFiPI5+JlFjPLvxoboD34
# yl6LMYtgCIktDAZcUrfE+QqY0RVfnxK+fDZjOL1EpH/kJisKxJdpDemM4sAQV7jI
# dhKRVfJIadi8KgJbD0TUIDHb9LpwJl2QYJ68SxcJL7TLHkNoyQcnwdJc9+ohuWgS
# nDycv578gFybY83sR6olJ2egN/MAgn1U16n46S4To3foH0owggSRMIIDeaADAgEC
# AhAHsEGNpR4UjDMbvN63E4MjMA0GCSqGSIb3DQEBCwUAMGwxCzAJBgNVBAYTAlVT
# MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
# b20xKzApBgNVBAMTIkRpZ2lDZXJ0IEhpZ2ggQXNzdXJhbmNlIEVWIFJvb3QgQ0Ew
# HhcNMTgwNDI3MTI0MTU5WhcNMjgwNDI3MTI0MTU5WjBaMQswCQYDVQQGEwJVUzEY
# MBYGA1UEChMPLk5FVCBGb3VuZGF0aW9uMTEwLwYDVQQDEyguTkVUIEZvdW5kYXRp
# b24gUHJvamVjdHMgQ29kZSBTaWduaW5nIENBMIIBIjANBgkqhkiG9w0BAQEFAAOC
# AQ8AMIIBCgKCAQEAwQqv4aI0CI20XeYqTTZmyoxsSQgcCBGQnXnufbuDLhAB6GoT
# NB7HuEhNSS8ftV+6yq8GztBzYAJ0lALdBjWypMfL451/84AO5ZiZB3V7MB2uxgWo
# cV1ekDduU9bm1Q48jmR4SVkLItC+oQO/FIA2SBudVZUvYKeCJS5Ri9ibV7La4oo7
# BJChFiP8uR+v3OU33dgm5BBhWmth4oTyq22zCfP3NO6gBWEIPFR5S+KcefUTYmn2
# o7IvhvxzJsMCrNH1bxhwOyMl+DQcdWiVPuJBKDOO/hAKIxBG4i6ryQYBaKdhDgaA
# NSCik0UgZasz8Qgl8n0A73+dISPumD8L/4mdywIDAQABo4IBPzCCATswHQYDVR0O
# BBYEFMtck66Im/5Db1ZQUgJtePys4bFaMB8GA1UdIwQYMBaAFLE+w2kD+L9HAdSY
# JhoIAu9jZCvDMA4GA1UdDwEB/wQEAwIBhjATBgNVHSUEDDAKBggrBgEFBQcDAzAS
# BgNVHRMBAf8ECDAGAQH/AgEAMDQGCCsGAQUFBwEBBCgwJjAkBggrBgEFBQcwAYYY
# aHR0cDovL29jc3AuZGlnaWNlcnQuY29tMEsGA1UdHwREMEIwQKA+oDyGOmh0dHA6
# Ly9jcmwzLmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEhpZ2hBc3N1cmFuY2VFVlJvb3RD
# QS5jcmwwPQYDVR0gBDYwNDAyBgRVHSAAMCowKAYIKwYBBQUHAgEWHGh0dHBzOi8v
# d3d3LmRpZ2ljZXJ0LmNvbS9DUFMwDQYJKoZIhvcNAQELBQADggEBALNGxKTz6gq6
# clMF01GjC3RmJ/ZAoK1V7rwkqOkY3JDl++v1F4KrFWEzS8MbZsI/p4W31Eketazo
# Nxy23RT0zDsvJrwEC3R+/MRdkB7aTecsYmMeMHgtUrl3xEO3FubnQ0kKEU/HBCTd
# hR14GsQEccQQE6grFVlglrew+FzehWUu3SUQEp9t+iWpX/KfviDWx0H1azilMX15
# lzJUxK7kCzmflrk5jCOCjKqhOdGJoQqstmwP+07qXO18bcCzEC908P+TYkh0z9gV
# rlj7tyW9K9zPVPJZsLRaBp/QjMcH65o9Y1hD1uWtFQYmbEYkT1K9tuXHtQYx1Rpf
# /dC8Nbl4iukwggWtMIIElaADAgECAhAM/wF08NAk2CbHMbcUImY5MA0GCSqGSIb3
# DQEBCwUAMFoxCzAJBgNVBAYTAlVTMRgwFgYDVQQKEw8uTkVUIEZvdW5kYXRpb24x
# MTAvBgNVBAMTKC5ORVQgRm91bmRhdGlvbiBQcm9qZWN0cyBDb2RlIFNpZ25pbmcg
# Q0EwHhcNMjAxMTI2MDAwMDAwWhcNMjMxMTMwMjM1OTU5WjCBqDEUMBIGA1UEBRML
# NjAzIDM4OSAwNjgxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpXYXNoaW5ndG9uMRAw
# DgYDVQQHEwdSZWRtb25kMS0wKwYDVQQKEyQuTkVUIG5hbm9GcmFtZXdvcmsgKC5O
# RVQgRm91bmRhdGlvbikxLTArBgNVBAMTJC5ORVQgbmFub0ZyYW1ld29yayAoLk5F
# VCBGb3VuZGF0aW9uKTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALxP
# 0PKK+4q46QcvKvyo9IcuRJSSYhctdYtq6O4OXHQlcauUOOGteoSheB/Aynzt5OQ+
# v+IOg2eb6JS090E9DyXVJKzs34fCoHqIZNOpdyxcSAVYgM1sYnFurTFI175THg/e
# son1ARyiqxYDaPlMcjJ5hiIid5xDlGMVFzU4WvlpMXCtjZp4bQOGVl78smTXwMvz
# iqmfFOsuv1MtGvhpcxl7k5M4FA0BksnliRMmCMvXQliLniXwcZNy3xbTtFnqXXIa
# gYQHEduPyoLhOd8Cit/TAyzHTPS/XEQOd7QUnz2v3XOx8P3dEpCT6ioPiyq9y9v+
# NV22SWeMSAxWocL5xkMCAwEAAaOCAh4wggIaMB8GA1UdIwQYMBaAFMtck66Im/5D
# b1ZQUgJtePys4bFaMB0GA1UdDgQWBBR/5rPX9+2cdJnzQUdaNJwIoH7JUjA0BgNV
# HREELTAroCkGCCsGAQUFBwgDoB0wGwwZVVMtV0FTSElOR1RPTi02MDMgMzg5IDA2
# ODAOBgNVHQ8BAf8EBAMCB4AwEwYDVR0lBAwwCgYIKwYBBQUHAwMwgZkGA1UdHwSB
# kTCBjjBFoEOgQYY/aHR0cDovL2NybDMuZGlnaWNlcnQuY29tL05FVEZvdW5kYXRp
# b25Qcm9qZWN0c0NvZGVTaWduaW5nQ0EuY3JsMEWgQ6BBhj9odHRwOi8vY3JsNC5k
# aWdpY2VydC5jb20vTkVURm91bmRhdGlvblByb2plY3RzQ29kZVNpZ25pbmdDQS5j
# cmwwTAYDVR0gBEUwQzA3BglghkgBhv1sAwEwKjAoBggrBgEFBQcCARYcaHR0cHM6
# Ly93d3cuZGlnaWNlcnQuY29tL0NQUzAIBgZngQwBBAEwgYQGCCsGAQUFBwEBBHgw
# djAkBggrBgEFBQcwAYYYaHR0cDovL29jc3AuZGlnaWNlcnQuY29tME4GCCsGAQUF
# BzAChkJodHRwOi8vY2FjZXJ0cy5kaWdpY2VydC5jb20vTkVURm91bmRhdGlvblBy
# b2plY3RzQ29kZVNpZ25pbmdDQS5jcnQwDAYDVR0TAQH/BAIwADANBgkqhkiG9w0B
# AQsFAAOCAQEAV1W9PfOR6rsxNB3gU0w5dPm+kp/DqCFBV24BB9CJx1dMWh8AijYm
# qyUpQ5n3S6x4lIU8KeFgC253LQA5wF6OjDoj86t/fohrsBcUbU9XsJ+AqscyEZYR
# fbrFicrII5VQXMus77/h7JfCAxMy4IKym0IOPEA+4wo1+mGNyGzsdTd4fqLibuUB
# SFhQry8tS8JFAnil8J6F9WK3GvJn6gZhbavPZr442KUsb0EomhYmni25kaotNrmQ
# D7Q+k2GMyx7DtgKF86uIbyfSoMavS4Yf9N7hVXmLeTeGrC5GqqcyDfe+reWOPDU6
# EIEZMcWHkoyvJNRFXACjvNV4MK6u282mMjGCD2gwgg9kAgEBMG4wWjELMAkGA1UE
# BhMCVVMxGDAWBgNVBAoTDy5ORVQgRm91bmRhdGlvbjExMC8GA1UEAxMoLk5FVCBG
# b3VuZGF0aW9uIFByb2plY3RzIENvZGUgU2lnbmluZyBDQQIQDP8BdPDQJNgmxzG3
# FCJmOTANBglghkgBZQMEAgEFAKCBhDAYBgorBgEEAYI3AgEMMQowCKACgAChAoAA
# MBkGCSqGSIb3DQEJAzEMBgorBgEEAYI3AgEEMBwGCisGAQQBgjcCAQsxDjAMBgor
# BgEEAYI3AgEVMC8GCSqGSIb3DQEJBDEiBCAemON/meVz/krG6SGsiyZe71I8Q1Tl
# QcX+geLF3ZK0DzANBgkqhkiG9w0BAQEFAASCAQB9I8MOj+cLg9xyfy4E3JmBN5BY
# wTxowpYl3Inh83tO8ftuky9bEtFXlKqSBOpJWvTjWMJ7+qrOsz9jeYOuXYeHPMwf
# YyXTj9W4n1825qi2C7zh1fn53H1uMstnFjSho4EFqsUNx8vMBB2A9Vbz4tDefHT4
# sT+yX9q08fzGotYpm/iZovwz8C/QCXceI+qKsI4vK4yOwfei7biwjEr6avthgwCW
# +ZgutXoR1wbGiyjXs6EKkwUimMZ49YZluCv9RJZgaxb1Zr7FUTEFJhAhM9I1gEg4
# +CFdyyDOiO/Wyjv30/7jPk1k0UYZMPXR+0QNt4ZW/GzOKJ5btGuvo/lQq8o1oYIN
# RDCCDUAGCisGAQQBgjcDAwExgg0wMIINLAYJKoZIhvcNAQcCoIINHTCCDRkCAQMx
# DzANBglghkgBZQMEAgEFADB3BgsqhkiG9w0BCRABBKBoBGYwZAIBAQYJYIZIAYb9
# bAcBMDEwDQYJYIZIAWUDBAIBBQAEIMVJXzGJYW3YDgtZol10fCRHMnwGYAJ01bs5
# arRIY476AhALnhFKsHtl5OALYBcU0XdVGA8yMDIxMDMxNzE1MDAzM1qgggo3MIIE
# /jCCA+agAwIBAgIQDUJK4L46iP9gQCHOFADw3TANBgkqhkiG9w0BAQsFADByMQsw
# CQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3d3cu
# ZGlnaWNlcnQuY29tMTEwLwYDVQQDEyhEaWdpQ2VydCBTSEEyIEFzc3VyZWQgSUQg
# VGltZXN0YW1waW5nIENBMB4XDTIxMDEwMTAwMDAwMFoXDTMxMDEwNjAwMDAwMFow
# SDELMAkGA1UEBhMCVVMxFzAVBgNVBAoTDkRpZ2lDZXJ0LCBJbmMuMSAwHgYDVQQD
# ExdEaWdpQ2VydCBUaW1lc3RhbXAgMjAyMTCCASIwDQYJKoZIhvcNAQEBBQADggEP
# ADCCAQoCggEBAMLmYYRnxYr1DQikRcpja1HXOhFCvQp1dU2UtAxQtSYQ/h3Ib5Fr
# DJbnGlxI70Tlv5thzRWRYlq4/2cLnGP9NmqB+in43Stwhd4CGPN4bbx9+cdtCT2+
# anaH6Yq9+IRdHnbJ5MZ2djpT0dHTWjaPxqPhLxs6t2HWc+xObTOKfF1FLUuxUOZB
# OjdWhtyTI433UCXoZObd048vV7WHIOsOjizVI9r0TXhG4wODMSlKXAwxikqMiMX3
# MFr5FK8VX2xDSQn9JiNT9o1j6BqrW7EdMMKbaYK02/xWVLwfoYervnpbCiAvSwnJ
# laeNsvrWY4tOpXIc7p96AXP4Gdb+DUmEvQECAwEAAaOCAbgwggG0MA4GA1UdDwEB
# /wQEAwIHgDAMBgNVHRMBAf8EAjAAMBYGA1UdJQEB/wQMMAoGCCsGAQUFBwMIMEEG
# A1UdIAQ6MDgwNgYJYIZIAYb9bAcBMCkwJwYIKwYBBQUHAgEWG2h0dHA6Ly93d3cu
# ZGlnaWNlcnQuY29tL0NQUzAfBgNVHSMEGDAWgBT0tuEgHf4prtLkYaWyoiWyyBc1
# bjAdBgNVHQ4EFgQUNkSGjqS6sGa+vCgtHUQ23eNqerwwcQYDVR0fBGowaDAyoDCg
# LoYsaHR0cDovL2NybDMuZGlnaWNlcnQuY29tL3NoYTItYXNzdXJlZC10cy5jcmww
# MqAwoC6GLGh0dHA6Ly9jcmw0LmRpZ2ljZXJ0LmNvbS9zaGEyLWFzc3VyZWQtdHMu
# Y3JsMIGFBggrBgEFBQcBAQR5MHcwJAYIKwYBBQUHMAGGGGh0dHA6Ly9vY3NwLmRp
# Z2ljZXJ0LmNvbTBPBggrBgEFBQcwAoZDaHR0cDovL2NhY2VydHMuZGlnaWNlcnQu
# Y29tL0RpZ2lDZXJ0U0hBMkFzc3VyZWRJRFRpbWVzdGFtcGluZ0NBLmNydDANBgkq
# hkiG9w0BAQsFAAOCAQEASBzctemaI7znGucgDo5nRv1CclF0CiNHo6uS0iXEcFm+
# FKDlJ4GlTRQVGQd58NEEw4bZO73+RAJmTe1ppA/2uHDPYuj1UUp4eTZ6J7fz51Kf
# k6ftQ55757TdQSKJ+4eiRgNO/PT+t2R3Y18jUmmDgvoaU+2QzI2hF3MN9PNlOXBL
# 85zWenvaDLw9MtAby/Vh/HUIAHa8gQ74wOFcz8QRcucbZEnYIpp1FUL1LTI4gdr0
# YKK6tFL7XOBhJCVPst/JKahzQ1HavWPWH1ub9y4bTxMd90oNcX6Xt/Q/hOvB46NJ
# ofrOp79Wz7pZdmGJX36ntI5nePk2mOHLKNpbh6aKLzCCBTEwggQZoAMCAQICEAqh
# JdbWMht+QeQF2jaXwhUwDQYJKoZIhvcNAQELBQAwZTELMAkGA1UEBhMCVVMxFTAT
# BgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3LmRpZ2ljZXJ0LmNvbTEk
# MCIGA1UEAxMbRGlnaUNlcnQgQXNzdXJlZCBJRCBSb290IENBMB4XDTE2MDEwNzEy
# MDAwMFoXDTMxMDEwNzEyMDAwMFowcjELMAkGA1UEBhMCVVMxFTATBgNVBAoTDERp
# Z2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3LmRpZ2ljZXJ0LmNvbTExMC8GA1UEAxMo
# RGlnaUNlcnQgU0hBMiBBc3N1cmVkIElEIFRpbWVzdGFtcGluZyBDQTCCASIwDQYJ
# KoZIhvcNAQEBBQADggEPADCCAQoCggEBAL3QMu5LzY9/3am6gpnFOVQoV7YjSsQO
# B0UzURB90Pl9TWh+57ag9I2ziOSXv2MhkJi/E7xX08PhfgjWahQAOPcuHjvuzKb2
# Mln+X2U/4Jvr40ZHBhpVfgsnfsCi9aDg3iI/Dv9+lfvzo7oiPhisEeTwmQNtO4V8
# CdPuXciaC1TjqAlxa+DPIhAPdc9xck4Krd9AOly3UeGheRTGTSQjMF287Dxgaqwv
# B8z98OpH2YhQXv1mblZhJymJhFHmgudGUP2UKiyn5HU+upgPhH+fMRTWrdXyZMt7
# HgXQhBlyF/EXBu89zdZN7wZC/aJTKk+FHcQdPK/P2qwQ9d2srOlW/5MCAwEAAaOC
# Ac4wggHKMB0GA1UdDgQWBBT0tuEgHf4prtLkYaWyoiWyyBc1bjAfBgNVHSMEGDAW
# gBRF66Kv9JLLgjEtUYunpyGd823IDzASBgNVHRMBAf8ECDAGAQH/AgEAMA4GA1Ud
# DwEB/wQEAwIBhjATBgNVHSUEDDAKBggrBgEFBQcDCDB5BggrBgEFBQcBAQRtMGsw
# JAYIKwYBBQUHMAGGGGh0dHA6Ly9vY3NwLmRpZ2ljZXJ0LmNvbTBDBggrBgEFBQcw
# AoY3aHR0cDovL2NhY2VydHMuZGlnaWNlcnQuY29tL0RpZ2lDZXJ0QXNzdXJlZElE
# Um9vdENBLmNydDCBgQYDVR0fBHoweDA6oDigNoY0aHR0cDovL2NybDQuZGlnaWNl
# cnQuY29tL0RpZ2lDZXJ0QXNzdXJlZElEUm9vdENBLmNybDA6oDigNoY0aHR0cDov
# L2NybDMuZGlnaWNlcnQuY29tL0RpZ2lDZXJ0QXNzdXJlZElEUm9vdENBLmNybDBQ
# BgNVHSAESTBHMDgGCmCGSAGG/WwAAgQwKjAoBggrBgEFBQcCARYcaHR0cHM6Ly93
# d3cuZGlnaWNlcnQuY29tL0NQUzALBglghkgBhv1sBwEwDQYJKoZIhvcNAQELBQAD
# ggEBAHGVEulRh1Zpze/d2nyqY3qzeM8GN0CE70uEv8rPAwL9xafDDiBCLK938ysf
# DCFaKrcFNB1qrpn4J6JmvwmqYN92pDqTD/iy0dh8GWLoXoIlHsS6HHssIeLWWywU
# NUMEaLLbdQLgcseY1jxk5R9IEBhfiThhTWJGJIdjjJFSLK8pieV4H9YLFKWA1xJH
# cLN11ZOFk362kmf7U2GJqPVrlsD0WGkNfMgBsbkodbeZY4UijGHKeZR+WfyMD+Nv
# tQEmtmyl7odRIeRYYJu6DC0rbaLEfrvEJStHAgh8Sa4TtuF8QkIoxhhWz0E0tmZd
# tnR79VYzIi8iNrJLokqV2PWmjlIxggJNMIICSQIBATCBhjByMQswCQYDVQQGEwJV
# UzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3d3cuZGlnaWNlcnQu
# Y29tMTEwLwYDVQQDEyhEaWdpQ2VydCBTSEEyIEFzc3VyZWQgSUQgVGltZXN0YW1w
# aW5nIENBAhANQkrgvjqI/2BAIc4UAPDdMA0GCWCGSAFlAwQCAQUAoIGYMBoGCSqG
# SIb3DQEJAzENBgsqhkiG9w0BCRABBDAcBgkqhkiG9w0BCQUxDxcNMjEwMzE3MTUw
# MDMzWjArBgsqhkiG9w0BCRACDDEcMBowGDAWBBTh14Ko4ZG+72vKFpG1qrSUpiSb
# 8zAvBgkqhkiG9w0BCQQxIgQgGHODaixDWDltZdaxN+OqCFB1davwpUrkXhOAVGud
# f4QwDQYJKoZIhvcNAQEBBQAEggEALlQJgLjBPUAc+cElYxC+HVFCKfGFizT7qy3S
# Dogf36a4FeR4yLqa3K1lwW48lbcr0CcvLHvA/DUR8RPwaMOZR8WPVGmRZtSOlduJ
# vbLTAY6QuCmsMbtPrTeVigxrrZm2ZyqozfI1ibb3JJw5ILI00q3cl7vgOvigKZsd
# IX3pZ1JA4pj00a4GLFGv4MvUrNa1rxk5/02NX/DiL30BQgyFNQJ5n39tbM4Pnf6G
# VVpsEd2nsijgR6jgsVC1OVFNdiOaOCP4pDTmMjK/aShDMcESgAF7OCWs8YWixLeV
# DTUUHxmjQs4IhDlTdRY0uJxoJ+1PxbInWgKTrtUsXVQ/anWUTw==
# SIG # End signature block
