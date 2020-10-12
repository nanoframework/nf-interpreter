//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL.h>

int DebuggerPort_Write( COM_HANDLE ComPortNum, const char* Data, size_t size, int maxRetries )
{
    NATIVE_PROFILE_PAL_COM();
    
    UINT32       transport = ExtractTransport(ComPortNum);
    const char*  dataTmp   = Data;
    INT32        totWrite  = 0;
    int          retries   = maxRetries + 1;

    while(size > 0 && retries > 0 )
    {
        int ret = 0;
        
        switch(transport)
        {
        case USART_TRANSPORT:
            ret = USART_Write( ConvertCOM_ComPort( ComPortNum ), dataTmp, size );
            break;
        case USB_TRANSPORT:
            ret = USB_Write( ConvertCOM_UsbStream( ComPortNum ), dataTmp, size );
            break;
        case SOCKET_TRANSPORT:
            ret = SOCKETS_Write( ConvertCOM_SockPort(ComPortNum), dataTmp, size );
            break;

        case GENERIC_TRANSPORT:
            return GenericPort_Write( ConvertCOM_GenericPort( ComPortNum ), dataTmp, size );
        }

        if(ret < 0)
        { // error condition, bug out
            break;
        }
        else if(ret == 0)
        { // didn't send any data ( assume buffer full )
            --retries;

            // if interrupts are off and buffer is full or out of retries
            // then there is nothing more to do.
            if(!INTERRUPTS_ENABLED_STATE() || retries <= 0 )
                break;

            Events_WaitForEvents(0, 1);
        }
        else
        { // succesfully transmitted at least some of the data
          // update counters and loop back to try sending more
            retries   = maxRetries + 1;
            size     -= ret;
            dataTmp  += ret;
            totWrite += ret;
        }
    }

    return totWrite;
}
