## Configuration of Chibios, HAL and MCU ##

In this folder you'll find a 'port' of the nanoframework to work on a WildernessLabs Netduino 3 Wifi board. You can find [here] some more information on the board specifics, used MCU, RAM and FLASH memory available.



**NOTE: the configuration was successfully tested on a WildernessLabs NETDUINO3_WIFI board using the Serial over USB connection on USB port 1 that creates a virtual COM port.**

## ADC configurations

The following ADC channels (and respective GPIO pins) are available to the managed API, in the respective index:
- PC0, ADC1 IN10
- PC1  ADC1 IN11
- PC2  ADC2 IN14
- PC3  ADC2 IN15
- PC4  ADC3 IN12
- PC5  ADC3 IN13
- Temp Sensor ADC1
- VrefInt ADC1
- Vbatt ADC1

These first 6 channels are exposed in the board J4 connector as A0 to A5.


## Floating point

The current build is set to add support for single-precision floating point.
Meaning that `System.Math` API supports only the `float` overloads. The `double` ones will throw a `NotImplementedException`.
