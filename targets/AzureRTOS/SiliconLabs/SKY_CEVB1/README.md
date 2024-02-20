# Skyworks CEVB Scheme #1 featuring SiLabs EFM32 Giant Gecko GG11

This is used for Si5575 CEVB, Si5400 CEVB, and Si5500 8x8 CEVB as of Feb 2024.

## See
https://skyworksinc.atlassian.net/wiki/spaces/TimingSoftware/pages/8175353962/Si5575+CEVB+Software+Control

https://skyworksinc.atlassian.net/wiki/spaces/TimingSoftware/pages/8084916281/Modifying+nanoFramework+CLR+for+Skyworks+EVB

## Key Files

CMakePresets.json
   Enable packages, targets, naming, etc.

target_system_device_spi_config.h
target_system_device_spi_config.cpp
   SPI

target_system_device_i2c_config.h
target_system_device_i2c_config.cpp
   I2C

target_nano_gg_adc_config.h
target_nano_gg_adc_config.cpp
   ADC

nanoBooter\main.c
nanoCLR\main.c
   Bootloader Mode / Ready LED

