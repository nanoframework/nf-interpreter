# Skyworks Field Programmer EVB featuring SiLabs EFM32 Giant Gecko GG11

## See
https://skyworksinc.atlassian.net/wiki/spaces/TimingSoftware/pages/8263237766/Gecko+Field+Programmer

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
   ADC <- NOT USED, ADC is disabled

nanoBooter\main.c
nanoCLR\main.c
   Bootloader Mode / Ready LED

