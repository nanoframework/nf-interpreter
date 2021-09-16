//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
// select which STM32 uart/serial port used.

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(TRACE_TO_STDIO)

// Generalised method to init stdio. This method is expected to initialise and required GPIO and driver.
#define STDIOPORT_INIT()                     StdioPort_Init()
#define STDIOPORT_WRITE(portNum, data, size) StdioPort_Write(portNum, data, size)
    void StdioPort_Init();
    uint32_t StdioPort_Write(int portNum, const char *data, size_t size);
#else
#define STDIOPORT_INIT()
#define STDIOPORT_WRITE(...)
#endif

#ifdef __cplusplus
}
#endif
