# Skyworks EVB featuring Silabs EFM32 Giant Gecko GG11

## ADC channels

ADC has enabled the following channels:
0 - reading from PA15 (MCU_ADC1)
1 - reading from PE15 (MCU_ADC1)
2 - reading from PE14 (MCU_ADC2)
3 - reading from PE13 (MCU_ADC3)
4 - reading from PE12 (MCU_ADC4)
5 - reading from PE11 (MCU_ADC5)
1 - internal temperature
2 - VCC

## I2C

I2C0 (this is referenced as I2C1 in C#)
SCL is: PA1
SDA is: PA0

I2C1 (this is referenced as I2C2 in C#)
SCL is: PC5
SDA is: PC4

## SPI

US1 (this is referenced as SPI2 in C#)
SCK: PD2
MOSI: PD0
MISO: PD1
CS: PD3
