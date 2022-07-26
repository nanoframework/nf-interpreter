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
