#include "UsbSerial.h"
#include <hal/usb_serial_jtag_ll.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void UsbSerialIntitialise(void)
{
}

size_t UsbSerialWrite(const uint8_t *data, size_t dataSize, TickType_t xTicksToWait)
{
    assert(data);
    assert(dataSize >= 1);

    const TickType_t startTicks = xTaskGetTickCount();

    size_t writtenTotalSize = 0;
    do
    {
        if (usb_serial_jtag_ll_txfifo_writable())
        {
            const size_t writtenSize = usb_serial_jtag_ll_write_txfifo(data, dataSize);
            usb_serial_jtag_ll_txfifo_flush();
            data += writtenSize;
            dataSize -= writtenSize;
            writtenTotalSize += writtenSize;

            if (dataSize <= 0)
                break;
        }
        else
        {
            vTaskDelay(1);
        }
    } while (xTaskGetTickCount() - startTicks < xTicksToWait);

    return writtenTotalSize;
}

size_t UsbSerialRead(uint8_t *data, size_t dataSize, TickType_t xTicksToWait)
{
    assert(data);
    assert(dataSize >= 1);

    const TickType_t startTicks = xTaskGetTickCount();

    size_t readTotalSize = 0;
    do
    {
        if (usb_serial_jtag_ll_rxfifo_data_available())
        {
            const size_t readSize = usb_serial_jtag_ll_read_rxfifo(data, dataSize);
            data += readSize;
            dataSize -= readSize;
            readTotalSize += readSize;

            if (dataSize <= 0)
                break;
        }
        else
        {
            vTaskDelay(1);
        }
    } while (xTaskGetTickCount() - startTicks < xTicksToWait);

    return readTotalSize;
}
