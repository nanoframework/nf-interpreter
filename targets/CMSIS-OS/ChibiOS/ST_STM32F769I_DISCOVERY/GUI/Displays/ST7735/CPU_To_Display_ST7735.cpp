
#include "CPU_To_Display_ST7735.h"

bool CPU_To_Display_ST7735::InitializeCommunicationsChannel()
{
    spi = new SPI(mosiPin, NC, sclkPin); // create new SPI instance and initialise
    spi->format(8, 0);         // 8 bits, Mode 0
    spi->frequency(20000000);  // SPI clock frequency
    ce = new DigitalOut(cePin, 1);
    dc = new DigitalOut(dcPin, 0);

}
void CPU_To_Display_ST7735::SendCommandToDisplay(CLR_UINT8 c)
{
    dc->write(0);
    ce->write(0);
    spi->write(c);
    ce->write(1);

}
void CPU_To_Display_ST7735::SendDataToDisplay(CLR_UINT16 value)         // Word data
{
    dc->write(1);
    ce->write(0);
    spi->write(value);
    ce->write(1);

}
void CPU_To_Display_ST7735::SendDataToDisplay(const CLR_UINT16 data[], CLR_UINT16 count)
{
    int i;
    for (i = 0; i < count; i += 2)
    {
        (void)SendCommandToDisplay((CLR_UINT16)data[i]);
        (void)SendDataToDisplay((CLR_UINT16)data[i + 1]);
    }
}
