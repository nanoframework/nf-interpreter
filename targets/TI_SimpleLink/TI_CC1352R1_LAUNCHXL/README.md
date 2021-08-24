# TI CC1352R1 SimpleLink LaunchPad

The CC1352R1 LaunchPad board contains a TI CC1352R1 device.

## Documentation

- Detailed documentation for this target reference can be found [here](http://docs.nanoframework.net/content/reference-targets/ti-cc1352r1-launchxlhtml).
- Instructions on how to flash the CC1352R1 LaunchPad can be found [here](http://docs.nanoframework.net/content/ti-simplelink/flash-nanoclr-cc13x2_26x2.html)

## Jumper Settings

* For any examples that use LEDs, make sure the LED's jumper settings are closed.

## Board resources used in driver examples:

The following table lists the Board names of the resources used by
the driver examples, and their corresponding pins.  Note that there may be
other resources configured in the board files that are not used by any
driver example.  Only the example resources are listed in the table.

  |Board Resource|Pin|
  |--------------|:---|
  |`Board_ADC0`|DIO23|
  |`Board_ADC1`|DIO24|
  |`Board_ADCBUF0CHANNEL0`|DIO23|
  |`Board_GPIO_BUTTON0`|DIO15|
  |`Board_GPIO_BUTTON1`|DIO14|
  |`Board_GPIO_LED0`|DIO6  (Red - must be jumpered)|
  |`Board_GPIO_LED1`|DIO7  (Green - must be jumpered)|
  |`Board_I2C_TMP`|DIO4 - `SCL`, DIO5 - `SDA`|
  |`Board_PIN_LED0`|DIO6  (Red - must be jumpered)|
  |`Board_PIN_LED1`|DIO7  (Green - must be jumpered)|
  |`Board_PIN_LED2`|DIO6  (Red - must be jumpered)|
  |`Board_PIN_BUTTON0`|DIO13|
  |`Board_PIN_BUTTON1`|DIO14|
  |`Board_PWM0`|DIO6|
  |`Board_PWM1`|DIO7|
  |`Board_SD0`|DIO8 - `MISO`, DIO9 - `MOSI`, DIO10 - `CLK`, DIO21 - `CS`|
  |`Board_SPI0`|DIO8, DIO9, DIO10, DIO11|
  |`Board_SPI_MASTER`|DIO8 - `MISO`, DIO9 - `MOSI`, DIO10 - `CLK`, DIO11 - `CS`|
  |`Board_SPI_SLAVE`|DIO8 - `MISO`, DIO9 - `MOSI`, DIO10 - `CLK`, DIO11 - `CS`|
  |`Board_SPI_MASTER_READY`|DIO15|
  |`Board_SPI_SLAVE_READY`|DIO21|
  |`Board_UART0`|DIO12 - `RX`, DIO13 - `TX`|

## Floating point

The current build is set to add support for single-precision floating point.
Meaning that `System.Math` API supports only the `float` overloads. The `double` ones will throw a `NotImplementedException`.

## nanoBooter

Because this CPU has a proprietary boot mechanism it does use nanoBooter.

## Flashing nanoCLR

Follow the instructions [here](http://docs.nanoframework.net/content/ti-cc32xx/flash-nanoclr.html) on how to flash nanoCLR image in the board.
