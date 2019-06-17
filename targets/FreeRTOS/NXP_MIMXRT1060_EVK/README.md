

Pre-configured cmake-variants.json for this board

```
{
  "buildType": {
    "default": "MinSizeRel",
    "choices": {
      "debug": {
        "short": "Debug",
        "long": "Emit debug information without performing optimizations",
        "buildType": "Debug"
      },
      "release": {
        "short": "Release",
        "long": "Enable optimizations, omit debug info",
        "buildType": "Release"
      },
      "minsize": {
        "short": "MinSizeRel",
        "long": "Optimize for smallest binary size",
        "buildType": "MinSizeRel"
      },
      "reldeb": {
        "short": "RelWithDebInfo",
        "long": "Perform optimizations AND include debugging information",
        "buildType": "RelWithDebInfo"
      }
    }
  },
  "linkage": {
    "default": "",
    "choices": {
      "MIMXRT1060_EVK": {
        "short": "MIMXRT1060_EVK",
        "settings": {
          "BUILD_VERSION": "0.9.99.999",
          "TOOLCHAIN_PREFIX": "C:/nanoFramework_Tools/GNU_ARM_Toolchain/7 2018-q2-update",
          "TARGET_SERIES": "IMXRT10xx",
          "RTOS": "FREERTOS",
          "FREERTOS_BOARD": "NXP_MIMXRT1060_EVK",
          "FREERTOS_VERSION": "v1.4.8",
          "CMSIS_VERSION": "5.4.0",
          "FATFS_VERSION": "R0.13c",
          "NF_BUILD_RTM": "OFF",
          "NF_FEATURE_DEBUGGER": "ON",
          "NF_FEATURE_HAS_SDCARD": "ON",
          "NF_FEATURE_RTC": "OFF",
          "NF_SECURITY_OPENSSL": "OFF",
          "API_Windows.Devices.Gpio": "ON",
          "API_Windows.Devices.Spi": "OFF",
          "API_Windows.Devices.I2c": "OFF",
          "API_Windows.Devices.Adc": "OFF",
          "API_Windows.Devices.Pwm": "OFF",
          "API_Windows.Devices.Wifi": "OFF",
          "API_System.Net": "OFF",
          "API_System.Math": "ON",
          "API_Windows.Networking.Sockets": "OFF",
          "API_Windows.Storage": "ON"
        }
      }
    }
  }
}
```

