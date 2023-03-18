#ifndef USB_SERIAL_H
#define USB_SERIAL_H

#include <stddef.h>
#include <stdint.h>
#include <freertos/FreeRTOS.h>

#if defined(__cplusplus)
extern "C"
{
#endif

    void UsbSerialIntitialise(void);
    size_t UsbSerialWrite(const uint8_t *data, size_t dataSize, TickType_t xTicksToWait);
    size_t UsbSerialRead(uint8_t *data, size_t dataSize, TickType_t xTicksToWait);

#if defined(__cplusplus)
}
#endif

#endif // USB_SERIAL_H
