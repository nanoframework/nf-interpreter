For a successful build the following changes are required:

For memory maps, uuid etc. the reference document used was: http://www.st.com/content/ccc/resource/technical/document/reference_manual/group0/96/8b/0d/ec/16/22/43/71/DM00224583/files/DM00224583.pdf/jcr:content/translations/en.DM00224583.pdf

In _halconf.g_ (in both nanoBooter and nanoCLR folders), when compared with a default file available from (https://github.com/ChibiOS/ChibiOS/tree/master/demos/STM32/RT-STM32F769I-DISCOVERY):
- HAL_USE_SERIAL_USB to TRUE
- HAL_USE_USB to TRUE
- SERIAL_DEFAULT_BITRATE to 115200

In _mcuconf.h_ (in both nanoBooter and nanoCLR folders), when compared with a default file available from (https://github.com/ChibiOS/ChibiOS/tree/master/demos/STM32/RT-STM32F769I-DISCOVERY):
- STM32_SERIAL_USE_USART2 to TRUE
- STM32_USB_USE_OTG1 to TRUE


Currently the stable branch of CHIBIOS (16.1.8) does not contain board.c and board.h. As such it is necessary to download the ST_STM32F769I_DISCOVERY folder from https://github.com/ChibiOS/ChibiOS/tree/master/os/hal/boards/ST_STM32F769I_DISCOVERY and copy it to the CHIBIOS folder ../os/hal/boards

Also currently the stable branch of CHIBIOS (16.1.8) does not contain board.c and board.h. As such it is necessary to download the ST_STM32F769I_DISCOVERY folder from https://github.com/ChibiOS/ChibiOS/tree/master/os/hal/ports/STM32/STM32F7xx and replace the existing folder in ../os/hal/ports

Lastly the stable branch of CHIBIOS (16.1.8) does not contain information on the processor so it is necessary download the folder from https://github.com/ChibiOS/ChibiOS/tree/masteros/common/ext/CMSIS/ST/STM32F7xx and replace os/ext/CMSIS/ST/STM32F7xx

NOTE: this configuration was successfully tested in an ST_STM32F769I_DISCOVERY board using the Serial over USB connection on USB port 1 that creates a virtual COM port.
