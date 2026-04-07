//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <Target_GenericPort_stdio.h>

// -- //

void StdioPort_Init()
{
    StdioConfigPins_UART();
    sdStart(&STDIO_SERIAL_DRIVER, NULL);
}

uint32_t StdioPort_Write(const char *data, size_t size)
{
    size_t writeResult = chnWriteTimeout(
        &STDIO_SERIAL_DRIVER,
        (const uint8_t *)data,
        size,
        1000); //(systime_t)TIMEOUT_INFINITE); // 1000); //

    return (uint32_t)writeResult;
}
