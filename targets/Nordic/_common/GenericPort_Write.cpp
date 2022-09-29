//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <zephyr.h>

#include <nanoCLR_Types.h>
#include <nanoCLR_Runtime.h>

//
// Overrides the debug out functionality. Use zephyr's printk
//
uint32_t GenericPort_Write(int portNum, const char *data, size_t size)
{
    // (db) Note below is from ESP32 build. keeping b/c maybe true.

    // It's only enabled if the C# programm is in running mode and no source level debugger (C# debugger)
    // is attached. In all other conditions it's better to be quiet.
    // If the debugger is attached we don't output the data because the debugger use the same serial link.
    // Maybe later it can also redirect output to the ESP32 app trace
    // which is redirected to JTAG/openocd interface
    (void)portNum;

    printk("%s", data);
    return (uint32_t)size;
}
