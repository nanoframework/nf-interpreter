//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#include <esp32_os.h>

#include "nanoCLR_Types.h"

extern "C" void SwoInit()
{
}

extern "C" void SwoPrintChar(char c)
{
}

extern "C" void SwoPrintString(const char *s)
{
    // print char until terminator is found
    while(*s)
    {
        SwoPrintChar(*s++);
    }
}

int GenericPort_Write( int portNum, const char* data, size_t size )
{
    char* p = (char*)data;
    int counter = 0;

    /// send characters directly to the trace port
    // while(*p != '\0' || counter < size)
    // {
    //     ets_printf( "%c", *p++); 
    //     counter++;
    // }
    // return counter;
    
    return size;
}
