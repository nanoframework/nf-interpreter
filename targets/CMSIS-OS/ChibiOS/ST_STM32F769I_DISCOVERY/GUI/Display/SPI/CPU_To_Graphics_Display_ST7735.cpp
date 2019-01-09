
#include "nanoCLR_Types.h"
#include "..\..\..\..\src\CLR\GUI\Devices\Includes\Cpu_To_Graphics_Display.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-function"

bool CPU_TO_GRAPHICS_DISPLAY::InitializeCpuDisplayDriver()
{
    //spi = new SPI(mosiPin, NC, sclkPin); // create new SPI instance and initialise
    //spi->format(8, 0);         // 8 bits, Mode 0
    //spi->frequency(20000000);  // SPI clock frequency
    //ce = new DigitalOut(cePin, 1);
    //dc = new DigitalOut(dcPin, 0);
    return TRUE;
}
void CPU_TO_GRAPHICS_DISPLAY::SendCommandToDisplay(CLR_UINT8 c)
{
    //dc->write(0);
    //ce->write(0);
    //spi->write(c);
    //ce->write(1);
}
void CPU_TO_GRAPHICS_DISPLAY::SendCommandToDisplay(CLR_UINT8 command, CLR_UINT8 value)
{
    return;
}
void CPU_TO_GRAPHICS_DISPLAY::SendDataToDisplay(CLR_UINT16 value)         // Word data
{
    //dc->write(1);
    //ce->write(0);
    //spi->write(value);
    //ce->write(1);
}
void CPU_TO_GRAPHICS_DISPLAY::SendDataToDisplay(const CLR_UINT16 data[], CLR_UINT16 count)
{
    int i;
    for (i = 0; i < count; i += 2)
    {
        (void)SendCommandToDisplay((CLR_UINT16)data[i]);
        (void)SendDataToDisplay((CLR_UINT16)data[i + 1]);
    }
}
void CPU_TO_GRAPHICS_DISPLAY::SendDataToDisplay(CLR_UINT8 * pParams, int numberOfParameters)
{
    return;
}

#pragma GCC diagnostic pop

