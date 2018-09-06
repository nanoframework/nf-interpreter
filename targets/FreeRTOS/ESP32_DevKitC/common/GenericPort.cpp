//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <esp32_os.h>

#include "nanoCLR_Types.h"

uint32_t GenericPort_Write( int portNum, const char* data, size_t size )
{
   (void)portNum;
   (void)data;
    (void)data;

// FIXME
// This is commented out as it upsets the debugger through the same serial link
// Can be enabled if required
// Maybeb later it can automatically switch on if debugger/vs extension not in use or
// redirect output to the ESP32 app trace which is redirected to JTAG/openocd interface
// Add ComDirector
// TODO fix this when working https://github.com/nanoframework/Home/issues/389

    //char* p = (char*)data;
    //int counter = 0;

    //// send characters directly to the trace port
    //while(*p != '\0' || counter < (int)size)
    //{
    //    ets_printf( "%c", *p++); 
    //    counter++;
    //}
    //return counter;
    
    return (uint32_t)size;
}
