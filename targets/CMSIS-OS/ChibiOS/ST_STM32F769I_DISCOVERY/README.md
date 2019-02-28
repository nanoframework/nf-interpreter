## Configuration of ChibiOS, HAL and MCU

For a successful build the following changes are required:

For memory maps, uuid etc. the reference document used was: http://www.st.com/content/ccc/resource/technical/document/reference_manual/group0/96/8b/0d/ec/16/22/43/71/DM00224583/files/DM00224583.pdf/jcr:content/translations/en.DM00224583.pdf

In _halconf.g_ (in both nanoBooter and nanoCLR folders), when compared with a default file available from (https://github.com/ChibiOS/ChibiOS/tree/master/demos/STM32/RT-STM32F769I-DISCOVERY):
- HAL_USE_SERIAL to TRUE
- SERIAL_DEFAULT_BITRATE to 115200

In _mcuconf.h_ (in both nanoBooter and nanoCLR folders), when compared with a default file available from (https://github.com/ChibiOS/ChibiOS/tree/master/demos/STM32/RT-STM32F769I-DISCOVERY):
- STM32_SERIAL_USE_USART1 to TRUE

NOTE: this configuration was successfully tested in an ST_STM32F769I_DISCOVERY board using the Serial port through the onboard 7ST Link USB connection.

## ADC configurations

The following ADC channels (and respective GPIO pins) are available to the managed API, in the respective index:
- PA6, ADC1 IN6
- PA4  ADC1 IN4
- PC2  ADC1 IN12
- PF10 ADC1 IN8
- PF8  ADC3 IN6
- PB8  ADC3 IN7
- Temp Sensor ADC1
- VrefInt ADC1
- Vbatt ADC1

## Floating point

The current build is set to add support for single-precision floating point.
Meaning that `System.Math` API supports only the `float` overloads. The `double` ones will throw a `NotImplementedException`.
