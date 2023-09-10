## Configuration of ChibiOS, HAL and MCU

For a successful build the following changes are required:

For memory maps, uuid etc. the reference document used was: http://www.st.com/content/ccc/resource/technical/document/reference_manual/group0/96/8b/0d/ec/16/22/43/71/DM00224583/files/DM00224583.pdf/jcr:content/translations/en.DM00224583.pdf

In _halconf.g_ (in both nanoBooter and nanoCLR folders), when compared with a default file available from (https://github.com/ChibiOS/ChibiOS/tree/master/demos/STM32/RT-STM32F769I-DISCOVERY):
- HAL_USE_SERIAL to TRUE
- SERIAL_DEFAULT_BITRATE to 921600

In _mcuconf.h_ (in both nanoBooter and nanoCLR folders), when compared with a default file available from (https://github.com/ChibiOS/ChibiOS/tree/master/demos/STM32/RT-STM32F769I-DISCOVERY):
- STM32_SERIAL_USE_USART3 to TRUE

NOTE: this configuration was successfully tested in an ST_NUCLEO144_F767ZI board using the Serial port through the onboard ST Link USB connection.

