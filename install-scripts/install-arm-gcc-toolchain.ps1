# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

# This PS installs the ARM GNU GCC toolchain if it's not already available

[CmdletBinding(SupportsShouldProcess = $true)]
param (
    [Parameter(HelpMessage = "Path to the folder where the tool is to be installed.")][string]$Path,
    [Parameter(HelpMessage = "GNU GCC version requested.")][string]$Version,
    [switch]$force = $false
)

# set default GNU GCC version
if ([string]::IsNullOrEmpty($Version)) {
    $Version = "11.3.rel1"
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
        $Path = $env:NF_TOOLS_PATH
    }
    else {
        # use default
        $Path = 'C:\nftools'
    }

    # append the tool path
    $Path = $Path + "\GNU_Tools_ARM_Embedded\$Version"
}

# check if path already exists
$gnuGccPathExists = Test-Path $Path -ErrorAction SilentlyContinue    

# download, if needed
If ($gnuGccPathExists -eq $False -or $force) {
    $host-target = "arm-gnu-toolchain-$Version-mingw-w64-i686-arm-none-eabi.zip"
    $url = "https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu/$Version/binrel/$host-target
    $output = "$zipRoot\gcc-arm.zip"

    # Don't download again if already exists
    if (![System.IO.File]::Exists($output) -or $force) {
        "Download URL is: '$url'" | Write-Host -ForegroundColor White

        "Downloading ARM GNU GCC toolchain..." | Write-Host -ForegroundColor White -NoNewline

        # Stop security tripping us up
        [Net.ServicePointManager]::SecurityProtocol = "tls12, tls11, tls"
            
        # download toolchain zip file
        (New-Object Net.WebClient).DownloadFile($url, $output)

        "OK" | Write-Host -ForegroundColor Green
    }

    # unzip to install path, if not on Azure
    if ($IsAzurePipelines -eq $False) {
        # Install 7Zip4PowerShell module from PSGallery if not already installed
        #Install-Module -Name 7Zip4Powershell -RequiredVersion 1.10.0 -Scope CurrentUser

        "Installing ARM GNU GCC toolchain..." | Write-Host -ForegroundColor White -NoNewline

        # unzip toolchain
        #Expand-7Zip -ArchiveFileName $output -TargetPath $Path > $null
        Expand-Archive -Force -Path $output -DestinationPath $env:Agent_TempDirectory > $null
        # move subfolder to expected location and delete old dir
        Move-Item $env:Agent_TempDirectory\gcc-arm\$host-target $Path
        "OK" | Write-Host -ForegroundColor Green
    }
}
else {
    "Skipping install of ARM GNU GCC toolchain" | Write-Host -ForegroundColor Yellow
}

# set env variable, if not on Azure
if ($IsAzurePipelines -eq $False) {
    # need to replace forward slash for paths to work with GCC and CMake
    $Path = "$Path".Replace('\', '/')

    "Setting User Environment Variable ARM_GCC_PATH='" + $env:ARM_GCC_PATH + "'" | Write-Host -ForegroundColor Yellow

    $env:ARM_GCC_PATH = $Path

    try {
        # this call can fail if the script is not run with appropriate permissions
        [System.Environment]::SetEnvironmentVariable("ARM_GCC_PATH", $env:ARM_GCC_PATH, "User")
    }
    catch {
        "Failed to set User Environment Variable ARM_GCC_PATH. Make sure to manually add 'ARM_GCC_PATH' with '" + $env:ARM_GCC_PATH + "'." | Write-Host -ForegroundColor Red
    }
}

# SIG # Begin signature block
# MIIikAYJKoZIhvcNAQcCoIIigTCCIn0CAQExDzANBglghkgBZQMEAgEFADB5Bgor
# BgEEAYI3AgEEoGswaTA0BgorBgEEAYI3AgEeMCYCAwEAAAQQH8w7YFlLCE63JNLG
# KX7zUQIBAAIBAAIBAAIBAAIBADAxMA0GCWCGSAFlAwQCAQUABCDy/U4Pm3oozfo0
# ed59DCl0BO52I3XxpB1yVhGmbzxJFKCCDg8wggPFMIICraADAgECAhACrFwmagtA
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
# EIEZMcWHkoyvJNRFXACjvNV4MK6u282mMjGCE9cwghPTAgEBMG4wWjELMAkGA1UE
# BhMCVVMxGDAWBgNVBAoTDy5ORVQgRm91bmRhdGlvbjExMC8GA1UEAxMoLk5FVCBG
# b3VuZGF0aW9uIFByb2plY3RzIENvZGUgU2lnbmluZyBDQQIQDP8BdPDQJNgmxzG3
# FCJmOTANBglghkgBZQMEAgEFAKCBhDAYBgorBgEEAYI3AgEMMQowCKACgAChAoAA
# MBkGCSqGSIb3DQEJAzEMBgorBgEEAYI3AgEEMBwGCisGAQQBgjcCAQsxDjAMBgor
# BgEEAYI3AgEVMC8GCSqGSIb3DQEJBDEiBCARPKN12aKexDDqr/b8+7EHNrcdowNP
# zRwnapfel345DzANBgkqhkiG9w0BAQEFAASCAQAuVCsOUrVFoTKqomvB7nfymrTN
# aXa0X2I2VAmbyfWpCbb/bwbBTeiLKTZ0JAuxktt6ZJGNrxjuFe9kpoK22GMoaeTN
# 9wZKjYRjuC6U3dWP1PDFLdfFsVxhUlwYuP32hd4M0heEzSuLfSZrTG4HPsWBkQmK
# fkudq722mynqoIOThRgE0rAqToK15MglKnLPGOaHvi4JjZzANwl5m6b5jJBzN7dD
# 3XsEcUCsp0+zixvH0uqDW+mS009+dPD23zRd+JATsMDliYevAEwca1+cP3XcqBMn
# VbSTiym/HjVMiQhtit2nENe0qHZRxdSGstFDQ0i5AfLb2fuQbkRNKq7iMstzoYIR
# szCCEa8GCisGAQQBgjcDAwExghGfMIIRmwYJKoZIhvcNAQcCoIIRjDCCEYgCAQMx
# DzANBglghkgBZQMEAgEFADB4BgsqhkiG9w0BCRABBKBpBGcwZQIBAQYJYIZIAYb9
# bAcBMDEwDQYJYIZIAWUDBAIBBQAEIOHGYvtm0N62GotDRnifxVJvEEfq7PT9aZv/
# FZgD9bSXAhEA5Wjy1vvJiFhP+85CjJzqjBgPMjAyMjA0MjIxNjQ5MjRaoIINfDCC
# BsYwggSuoAMCAQICEAp6SoieyZlCkAZjOE2Gl50wDQYJKoZIhvcNAQELBQAwYzEL
# MAkGA1UEBhMCVVMxFzAVBgNVBAoTDkRpZ2lDZXJ0LCBJbmMuMTswOQYDVQQDEzJE
# aWdpQ2VydCBUcnVzdGVkIEc0IFJTQTQwOTYgU0hBMjU2IFRpbWVTdGFtcGluZyBD
# QTAeFw0yMjAzMjkwMDAwMDBaFw0zMzAzMTQyMzU5NTlaMEwxCzAJBgNVBAYTAlVT
# MRcwFQYDVQQKEw5EaWdpQ2VydCwgSW5jLjEkMCIGA1UEAxMbRGlnaUNlcnQgVGlt
# ZXN0YW1wIDIwMjIgLSAyMIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEA
# uSqWI6ZcvF/WSfAVghj0M+7MXGzj4CUu0jHkPECu+6vE43hdflw26vUljUOjges4
# Y/k8iGnePNIwUQ0xB7pGbumjS0joiUF/DbLW+YTxmD4LvwqEEnFsoWImAdPOw2z9
# rDt+3Cocqb0wxhbY2rzrsvGD0Z/NCcW5QWpFQiNBWvhg02UsPn5evZan8Pyx9PQo
# z0J5HzvHkwdoaOVENFJfD1De1FksRHTAMkcZW+KYLo/Qyj//xmfPPJOVToTpdhiY
# mREUxSsMoDPbTSSF6IKU4S8D7n+FAsmG4dUYFLcERfPgOL2ivXpxmOwV5/0u7NKb
# AIqsHY07gGj+0FmYJs7g7a5/KC7CnuALS8gI0TK7g/ojPNn/0oy790Mj3+fDWgVi
# fnAs5SuyPWPqyK6BIGtDich+X7Aa3Rm9n3RBCq+5jgnTdKEvsFR2wZBPlOyGYf/b
# ES+SAzDOMLeLD11Es0MdI1DNkdcvnfv8zbHBp8QOxO9APhk6AtQxqWmgSfl14Zvo
# aORqDI/r5LEhe4ZnWH5/H+gr5BSyFtaBocraMJBr7m91wLA2JrIIO/+9vn9sExjf
# xm2keUmti39hhwVo99Rw40KV6J67m0uy4rZBPeevpxooya1hsKBBGBlO7UebYZXt
# PgthWuo+epiSUc0/yUTngIspQnL3ebLdhOon7v59emsCAwEAAaOCAYswggGHMA4G
# A1UdDwEB/wQEAwIHgDAMBgNVHRMBAf8EAjAAMBYGA1UdJQEB/wQMMAoGCCsGAQUF
# BwMIMCAGA1UdIAQZMBcwCAYGZ4EMAQQCMAsGCWCGSAGG/WwHATAfBgNVHSMEGDAW
# gBS6FtltTYUvcyl2mi91jGogj57IbzAdBgNVHQ4EFgQUjWS3iSH+VlhEhGGn6m8c
# No/drw0wWgYDVR0fBFMwUTBPoE2gS4ZJaHR0cDovL2NybDMuZGlnaWNlcnQuY29t
# L0RpZ2lDZXJ0VHJ1c3RlZEc0UlNBNDA5NlNIQTI1NlRpbWVTdGFtcGluZ0NBLmNy
# bDCBkAYIKwYBBQUHAQEEgYMwgYAwJAYIKwYBBQUHMAGGGGh0dHA6Ly9vY3NwLmRp
# Z2ljZXJ0LmNvbTBYBggrBgEFBQcwAoZMaHR0cDovL2NhY2VydHMuZGlnaWNlcnQu
# Y29tL0RpZ2lDZXJ0VHJ1c3RlZEc0UlNBNDA5NlNIQTI1NlRpbWVTdGFtcGluZ0NB
# LmNydDANBgkqhkiG9w0BAQsFAAOCAgEADS0jdKbR9fjqS5k/AeT2DOSvFp3Zs4yX
# gimcQ28BLas4tXARv4QZiz9d5YZPvpM63io5WjlO2IRZpbwbmKrobO/RSGkZOFvP
# iTkdcHDZTt8jImzV3/ZZy6HC6kx2yqHcoSuWuJtVqRprfdH1AglPgtalc4jEmIDf
# 7kmVt7PMxafuDuHvHjiKn+8RyTFKWLbfOHzL+lz35FO/bgp8ftfemNUpZYkPopzA
# ZfQBImXH6l50pls1klB89Bemh2RPPkaJFmMga8vye9A140pwSKm25x1gvQQiFSVw
# BnKpRDtpRxHT7unHoD5PELkwNuTzqmkJqIt+ZKJllBH7bjLx9bs4rc3AkxHVMnhK
# SzcqTPNc3LaFwLtwMFV41pj+VG1/calIGnjdRncuG3rAM4r4SiiMEqhzzy350yPy
# nhngDZQooOvbGlGglYKOKGukzp123qlzqkhqWUOuX+r4DwZCnd8GaJb+KqB0W2Nm
# 3mssuHiqTXBt8CzxBxV+NbTmtQyimaXXFWs1DoXW4CzM4AwkuHxSCx6ZfO/IyMWM
# WGmvqz3hz8x9Fa4Uv4px38qXsdhH6hyF4EVOEhwUKVjMb9N/y77BDkpvIJyu2XMy
# WQjnLZKhGhH+MpimXSuX4IvTnMxttQ2uR2M4RxdbbxPaahBuH0m3RFu0CAqHWlkE
# dhGhp3cCExwwggauMIIElqADAgECAhAHNje3JFR82Ees/ShmKl5bMA0GCSqGSIb3
# DQEBCwUAMGIxCzAJBgNVBAYTAlVTMRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAX
# BgNVBAsTEHd3dy5kaWdpY2VydC5jb20xITAfBgNVBAMTGERpZ2lDZXJ0IFRydXN0
# ZWQgUm9vdCBHNDAeFw0yMjAzMjMwMDAwMDBaFw0zNzAzMjIyMzU5NTlaMGMxCzAJ
# BgNVBAYTAlVTMRcwFQYDVQQKEw5EaWdpQ2VydCwgSW5jLjE7MDkGA1UEAxMyRGln
# aUNlcnQgVHJ1c3RlZCBHNCBSU0E0MDk2IFNIQTI1NiBUaW1lU3RhbXBpbmcgQ0Ew
# ggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIKAoICAQDGhjUGSbPBPXJJUVXHJQPE
# 8pE3qZdRodbSg9GeTKJtoLDMg/la9hGhRBVCX6SI82j6ffOciQt/nR+eDzMfUBML
# JnOWbfhXqAJ9/UO0hNoR8XOxs+4rgISKIhjf69o9xBd/qxkrPkLcZ47qUT3w1lbU
# 5ygt69OxtXXnHwZljZQp09nsad/ZkIdGAHvbREGJ3HxqV3rwN3mfXazL6IRktFLy
# dkf3YYMZ3V+0VAshaG43IbtArF+y3kp9zvU5EmfvDqVjbOSmxR3NNg1c1eYbqMFk
# dECnwHLFuk4fsbVYTXn+149zk6wsOeKlSNbwsDETqVcplicu9Yemj052FVUmcJgm
# f6AaRyBD40NjgHt1biclkJg6OBGz9vae5jtb7IHeIhTZgirHkr+g3uM+onP65x9a
# bJTyUpURK1h0QCirc0PO30qhHGs4xSnzyqqWc0Jon7ZGs506o9UD4L/wojzKQtwY
# SH8UNM/STKvvmz3+DrhkKvp1KCRB7UK/BZxmSVJQ9FHzNklNiyDSLFc1eSuo80Vg
# vCONWPfcYd6T/jnA+bIwpUzX6ZhKWD7TA4j+s4/TXkt2ElGTyYwMO1uKIqjBJgj5
# FBASA31fI7tk42PgpuE+9sJ0sj8eCXbsq11GdeJgo1gJASgADoRU7s7pXcheMBK9
# Rp6103a50g5rmQzSM7TNsQIDAQABo4IBXTCCAVkwEgYDVR0TAQH/BAgwBgEB/wIB
# ADAdBgNVHQ4EFgQUuhbZbU2FL3MpdpovdYxqII+eyG8wHwYDVR0jBBgwFoAU7Nfj
# gtJxXWRM3y5nP+e6mK4cD08wDgYDVR0PAQH/BAQDAgGGMBMGA1UdJQQMMAoGCCsG
# AQUFBwMIMHcGCCsGAQUFBwEBBGswaTAkBggrBgEFBQcwAYYYaHR0cDovL29jc3Au
# ZGlnaWNlcnQuY29tMEEGCCsGAQUFBzAChjVodHRwOi8vY2FjZXJ0cy5kaWdpY2Vy
# dC5jb20vRGlnaUNlcnRUcnVzdGVkUm9vdEc0LmNydDBDBgNVHR8EPDA6MDigNqA0
# hjJodHRwOi8vY3JsMy5kaWdpY2VydC5jb20vRGlnaUNlcnRUcnVzdGVkUm9vdEc0
# LmNybDAgBgNVHSAEGTAXMAgGBmeBDAEEAjALBglghkgBhv1sBwEwDQYJKoZIhvcN
# AQELBQADggIBAH1ZjsCTtm+YqUQiAX5m1tghQuGwGC4QTRPPMFPOvxj7x1Bd4ksp
# +3CKDaopafxpwc8dB+k+YMjYC+VcW9dth/qEICU0MWfNthKWb8RQTGIdDAiCqBa9
# qVbPFXONASIlzpVpP0d3+3J0FNf/q0+KLHqrhc1DX+1gtqpPkWaeLJ7giqzl/Yy8
# ZCaHbJK9nXzQcAp876i8dU+6WvepELJd6f8oVInw1YpxdmXazPByoyP6wCeCRK6Z
# JxurJB4mwbfeKuv2nrF5mYGjVoarCkXJ38SNoOeY+/umnXKvxMfBwWpx2cYTgAnE
# tp/Nh4cku0+jSbl3ZpHxcpzpSwJSpzd+k1OsOx0ISQ+UzTl63f8lY5knLD0/a6fx
# ZsNBzU+2QJshIUDQtxMkzdwdeDrknq3lNHGS1yZr5Dhzq6YBT70/O3itTK37xJV7
# 7QpfMzmHQXh6OOmc4d0j/R0o08f56PGYX/sr2H7yRp11LB4nLCbbbxV7HhmLNriT
# 1ObyF5lZynDwN7+YAN8gFk8n+2BnFqFmut1VwDophrCYoCvtlUG3OtUVmDG0YgkP
# Cr2B2RP+v6TR81fZvAT6gt4y3wSJ8ADNXcL50CN/AAvkdgIm2fBldkKmKYcJRyvm
# fxqkhQ/8mJb2VVQrH4D6wPIOK+XW+6kvRBVK5xMOHds3OBqhK/bt1nz8MYIDdjCC
# A3ICAQEwdzBjMQswCQYDVQQGEwJVUzEXMBUGA1UEChMORGlnaUNlcnQsIEluYy4x
# OzA5BgNVBAMTMkRpZ2lDZXJ0IFRydXN0ZWQgRzQgUlNBNDA5NiBTSEEyNTYgVGlt
# ZVN0YW1waW5nIENBAhAKekqInsmZQpAGYzhNhpedMA0GCWCGSAFlAwQCAQUAoIHR
# MBoGCSqGSIb3DQEJAzENBgsqhkiG9w0BCRABBDAcBgkqhkiG9w0BCQUxDxcNMjIw
# NDIyMTY0OTI0WjArBgsqhkiG9w0BCRACDDEcMBowGDAWBBSFCPOGUVyz0wd9trS3
# wH8bSl5B3jAvBgkqhkiG9w0BCQQxIgQgmjonl1rAIIXQk/qU+ihvz6sP/txRkEk0
# wKa/dxK7jP4wNwYLKoZIhvcNAQkQAi8xKDAmMCQwIgQgnaaQFcNJxsGJeEW6NYKt
# cMiPpCk722q+nCvSU5J55jswDQYJKoZIhvcNAQEBBQAEggIAnWkbp2pLW5D3KUpu
# Z8n72liP3JgP7w50t+AwLzPpkxX1U157WEP2eld2WAtP5+VvSYTYiacWL1saYshp
# t5fSFgcCAU4x8p5JHdcrMEbmyvw5uXcJAuP/18ZhASVvC7u6K7bUB76YDsQleS9A
# 05AprAE8hvuPxVwk1bn7rYqkYfirzz4Bf9Qqc45svG1+covr55vUulDTnKMO+6+c
# pTk5SzVfW3fW9QkeLwHG/hNpIcNczxXipL0SvvCFfKB3hTeGlkKMuCi6dsq2JKwj
# uGr3s0JHrSNtn3HjzWJVk/KAuyMqTtsL2QgXQtmbgnMuYHceQOv8rU3BuQg9CzIB
# 3MzNKZZDftj75XmCWAC7ibdGyiyZF5VaD79MBV4UNW5tC0z+7AW4DbdslSBzekEd
# zA/Ufknob7kpk9kjC4ho4Bu/hplkjUIycNbzjVTKAZ5uNiP/5u0CxeDMpRoaUruu
# PS7YMH6MdUmKEVaTnzjkzUQM4nyhmCHvHHy7xBTEBfw9rwXuGyXYQ+3FVbzePOsB
# OkpQAh7C6Pt95wnjjuXUf5yFwtTKBzCVoGAuCTXew2Mnv6sBqD0HIt3J4ETziIkj
# q8myh/ZR3tel7bJsjP9NDA8XK+nN2nExk7b4YaC6RFb7TZ9i3/nj+WezAHbEoJf2
# 8xGUPQAlFdk/ibSAkCbF8KC3DQc=
# SIG # End signature block
