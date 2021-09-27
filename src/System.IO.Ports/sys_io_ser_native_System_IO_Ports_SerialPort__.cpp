//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_io_ser_native.h"

///////////////////////////////////////////////////////////////////////////
// memory allocated here for the buffer has to be released by the caller //
///////////////////////////////////////////////////////////////////////////
HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::SetupWriteLine(CLR_RT_StackFrame &stack, char **buffer, uint32_t *length, bool* isNewAllocation)
{
        NANOCLR_HEADER();

    const char *text;
    const char *newLine;
    uint32_t textLength;
    uint32_t newLineLength;
    bool addNewLine;

    // get a pointer to the managed object instance (no need to check as this has already been done by the caller)
    CLR_RT_HeapBlock *pThis = stack.This();

    // get pointer to string
    text = stack.Arg1().RecoverString();

    // check for NULL string
    if (text != NULL)
    {
        // get length
        textLength = hal_strlen_s(text);

        // set buffer length
        *length = textLength;

        // new line parameter
        addNewLine = (bool)stack.Arg2().NumericByRefConst().u1;

        // should a new line be added?
        if (addNewLine)
        {
            // get string for new line
            newLine = pThis[Library_sys_io_ser_native_System_IO_Ports_SerialPort::FIELD___newLine].RecoverString();

            newLineLength = hal_strlen_s(newLine);

            // allocate memory for buffer
            *buffer = (char *)platform_malloc(textLength + newLineLength);

            // sanity check for successful allocation
            if (*buffer)
            {
                // clear buffer
                memset(*buffer, 0, *length);

                // flag allocation
                *isNewAllocation = true;

                // update buffer length
                *length += newLineLength;

                // concatenate both strings
                strcat(*buffer, text);
                strcat(*buffer, newLine);
            }
            else
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
            }
        }
        else
        {
            // buffer pointing to text
            *buffer = (char *)text;

            // flag NO allocation
            *isNewAllocation = false;
        }
    }

    NANOCLR_NOCLEANUP();
}
