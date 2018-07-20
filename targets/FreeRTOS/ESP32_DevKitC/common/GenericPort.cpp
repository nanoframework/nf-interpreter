//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <esp32_os.h>

#include "nanoCLR_Types.h"

uint32_t GenericPort_Write( int portNum, const char* data, size_t size )
{
    (void)portNum;

    char* p = (char*)data;
    int counter = 0;

    // send characters directly to the trace port
    while(*p != '\0' || counter < (int)size)
    {
        ets_printf( "%c", *p++); 
        counter++;
    }
    return counter;
}
