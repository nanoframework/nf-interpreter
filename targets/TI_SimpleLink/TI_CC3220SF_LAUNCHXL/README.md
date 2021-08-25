# TI CC3220SF LAUNCHXL

The CC3220SF LAUNCHXL board contains a TI CC3220SF device.

## Jumper Settings

* Set __RX__ and __TX__ to provide UART communications via the onboard USB debugger.

## Board Resources

The following table lists the Board names of the resources and their corresponding pins.

  |Board Resource|Pins|
  |--------------|:---|
  |`Board_ADC0`|P59  (Connect to analog signal)|
  |`Board_ADC1`|P60  (Connect to analog signal)|
  |`Board_CAPTURE0`|P04 |
  |`Board_CAPTURE1`|P05 |
  |`Board_GPIO_LED0`|P64  (__LED EN__ must be jumpered)|
  |`Board_GPIO_LED1`|P01  (__LED EN__ must be jumpered)|
  |`Board_GPIO_LED2`|P02  (__LED EN__ must be jumpered)|
  |`Board_GPIO_BUTTON0`|P04  (`SW2`)|
  |`Board_GPIO_BUTTON1`|P15  (`SW3`)|
  |`Board_I2C0`|P01 - `SCL`, P02 - `SDA`|
  |`Board_I2C_TMP`|P01 - `SCL`, P02 - `SDA`  (__SCL__ and __SDA__ must be jumpered)|
  |`Board_I2S0`|P50 - `XR1`, P53 - `CLK`, P62 - `CLKX`, P63 - `FSX`, P64 - `XR0`|
  |`Board_PWM0`|P01  (__LED EN__ must be jumpered)|
  |`Board_PWM1`|P02  (__LED EN__ must be jumpered)|
  |`Board_SD0`|P06 - `DATA`, P07 - `CLK`, P08 - `CMD`|
  |`Board_SDFatFS0`|P06 - `DATA`, P07 - `CLK`, P08 - `CMD`|
  |`Board_SPI0`|P05 - `CLK`, P06 - `MISO`, P07 - `MOSI`, P08 - `CS`|
  |`Board_SPI_MASTER`|P05 - `CLK`, P06 - `MISO`, P07 - `MOSI`, P08 - `CS`|
  |`Board_SPI_SLAVE`|P05 - `CLK`, P06 - `MISO`, P07 - `MOSI`, P08 - `CS`|
  |`Board_SPI_MASTER_READY`|P18|
  |`Board_SPI_SLAVE_READY`|P03|
  |`Board_UART0`|P57 - `RX`, P55 - `TX`  (UART provided through emulation, __RX__ and __TX__ must be jumpered)|
  |`Board_UART1`|P08 - `RX`, P07 - `TX`|

## ADC Examples Pin Connections

>__Important:__ The inputs to the ADC are required to be well within 1.4&nbsp;VDC to avoid clipping, which will cause distortion.
The ADC inputs can be damaged if an input voltage higher than 1.8&nbsp;VDC is applied to these pin. The 1.8&nbsp;VDC hard limit must be considered from both the SW and HW points of view.
Always add a resistor divider/buffer to bring down the measurement signal voltage to within the ADC limits. See the [CC32xx ADC Appnote](http://processors.wiki.ti.com/index.php/CC32xx_ADC_Appnote)
for more details.

The __CC3220SF LAUNCHXL__ has a protective circuit to allow an input of 3.3&nbsp;VDC.
The `5V` should NOT be used for this may damage the ADC inputs. For testing, the following jumpers must be present:
`J5` = `OPAMP EN`, `J10` = `GND`, `J13` = `BRD`.

>__Note:__ There are duplicate pins P59 and P60 on the board. Choose the ones connected to the BoosterPack headers (the pins not connected to the BoosterPack headers have an asterisk next to them).

## Reset Behavior

By default, the application is not retained after a power reset. To enable
retention of the application for debug purposes, the application should
add the following pre-defined symbol: `__SF_DEBUG__`. This will place a header,
`Board_debugHeader`, into flash. This header is defined in the Board.c file.

>__Note:__ The user should ensure the `Board_debugHeader` is erased if
retention of the application is no longer desired. The Board.c file
will erase the `Board_debugHeader` if `__SF_NODEBUG__` is defined.

## Floating point

The current build is set to add support for single-precision floating point.
Meaning that `System.Math` API supports only the `float` overloads. The `double` ones will throw a `NotImplementedException`.

## nanoBooter

Because this CPU uses a proprietary boot mechanism that copies the application from the SPI flash memory to the Cortex-M application core, it does not use nanoBooter.

## Flashing nanoCLR

Follow the instructions [here](http://docs.nanoframework.net/articles/ti-cc32xx/flash-nanoclr.html) on how to flash nanoCLR image in the board.
