# Silabs EFM32 Giant Gecko GG11 Starter Kit (revC hardware)

This target is for revC hardware. This can be easily identified by the presence of a USB-C connector for JLink connection.
Opposed to revB which carries a micro USB connector instead. For revB user the firmware for target `SL_STK3701A_REVB`.

## Setting the VCP baud rate to 921600

https://community.silabs.com/s/article/kba-bt-1208-using-virtual-com-port-vcom-x

:warning:
Only possible for revC hardware.

## ADC channels

ADC has enabled the following channels:
0 - reading from PE11 (pin 6 on the expansion header)
1 - internal temperature
2 - VCC

:warning:
Vref is 2.5V for all channels, except for VCC which is 5V.

## PWM

PWM has enabled the following GPIOs:
0 - PC1 (pin 15 on the expansion header)
1 - PI1 (through hole connector at bottom side)

## I2C

I2C0 (this is referenced as I2C1 in C#)
Available in EXP header
I2C0_SDA: PC0, EXP Header Pin 16
I2C0_SCL: PC1, EXP Header Pin 15

I2C2 (this is referenced as I2C3 in C#)
Connected to the following devices on the GG11 board:
Si7021: Relative Humidity and Temperature Sensor
Si7210: Hall-Effect Sensor

I2C2_SCL is: PI5
I2C2_SDA is: PI4

## SPI

SPI0 (this is referenced as SPI1 in C#)
Available in EXP header

SPI0_SCK: PE12, EXP Header Pin 8
SPI0_MOSI: PE10, EXP Header Pin 4
SPI0_MISO: PE11, EXP Header Pin 6
SPI0_CS: PE13, EXP Header Pin 10

SPI1 (this is referenced as SPI2 in C#)
Connected to board LCD module

SPI1_SCK: PC15
SPI1_MOSI: PA14
SPI1_CS: PC14
