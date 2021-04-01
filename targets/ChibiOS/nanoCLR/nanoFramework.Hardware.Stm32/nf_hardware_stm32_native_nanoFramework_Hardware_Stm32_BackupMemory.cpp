//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_hardware_stm32_native.h"


HRESULT Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_BackupMemory::ReadBytes___STATIC__VOID__U4__SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    uint32_t position = stack.Arg0().NumericByRef().u4;

    // dereference the data buffer from the argument
    CLR_RT_HeapBlock_Array* buffer = stack.Arg1().DereferenceArray();
    // get a the pointer to the byffer by using the first element of the array
    uint8_t* data = buffer->GetFirstElement();
    // get the length of the data buffer
    uint32_t dataLength =  buffer->m_numOfElements;

    // address of bkp register 0
     __IO uint32_t* baseAddress = &(RTC->BKP0R);
    uint8_t* storeAddress = ((uint8_t*)baseAddress + position);
    uint32_t* registerAddress = NULL;
    uint32_t tempRegisterValue;
    uint32_t counter = 0;
    uint32_t remainder;
    int32_t index = 3;

    // sanity check for out of range position
    if(position > (BACKUP_SIZE - 1) ) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);

    // sanity check for over sized data buffer
    if( (position + dataLength) > BACKUP_SIZE) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);

    // check if the store address is starting at a register address boundary
    remainder = (uint32_t)((uint32_t*)storeAddress) % sizeof(RTC_BKP0R_Msk);

    if(remainder > 0)
    {
        // read register
        registerAddress = (uint32_t*)(storeAddress - remainder);
        tempRegisterValue = *registerAddress;

        // adjust remainder to the amount of bytes to move
        remainder = 4 - remainder;

        do
        {
            *data = (uint8_t)(tempRegisterValue >> ( (remainder - 1) * 8));
            data++;
            remainder--;
            counter++;
        }
        while(remainder && (counter < dataLength));
        registerAddress++;
    }

    // keep reading into the buffer if there enough bytes
    if( (dataLength - counter) >= 4)
    {
        // load register
        tempRegisterValue = *registerAddress;

        do
        {
            *data = (uint8_t)(tempRegisterValue >> (index-- * 8));
            data++;
            counter++;

            if(index < 0)
            {
                index = 3;
                registerAddress++;

                // is there enough data in buffer for another round
                if( (dataLength - counter) < 4)
                {
                    // no, exit loop
                    break;
                }

                // load register
                tempRegisterValue = *registerAddress;
            }
        }
        while(1);
    }

    // now read the remaining bytes, if any
    if((dataLength - counter) > 0)
    {
        remainder = dataLength - counter;
        
        // read register
        tempRegisterValue = *registerAddress;

        // adjust remainder to the amount of bytes to move
        remainder = 4 - remainder;

        do
        {
            *data = (uint8_t)(tempRegisterValue >> (remainder * 8));
            remainder--;
            data++;
            counter++;
        }
        while(remainder);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_BackupMemory::WriteBytes___STATIC__VOID__U4__SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    // get position to write to
    uint32_t position = stack.Arg0().NumericByRef().u4;

    // dereference the data buffer from the argument
    CLR_RT_HeapBlock_Array* buffer = stack.Arg1().DereferenceArray();
    // get a the pointer to the byffer by using the first element of the array
    uint8_t* data = buffer->GetFirstElement();
    // get the length of the data buffer
    uint32_t dataLength =  buffer->m_numOfElements;

    // address of bkp register 0
    __IO uint32_t* baseAddress = &(RTC->BKP0R);
    uint8_t* storeAddress = ((uint8_t*)baseAddress + position);
    uint32_t* registerAddress = NULL;
    uint32_t tempRegisterValue;
    uint32_t counter = 0;
    uint32_t remainder;
    int32_t index = 3;

    // sanity check for out of range position
    if(position > (BACKUP_SIZE - 1) ) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);

    // sanity check for over sized data buffer
    if( (position + dataLength) > BACKUP_SIZE) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);

    // write has to be done register wide, so if a write address is "in the middle" of a register, need to read it first and overwrite as required
    // this can happen at the begining and/or at the end of the buffer

    // check if the store address is starting at a register address boundary
    remainder = (uint32_t)((uint32_t*)storeAddress) % sizeof(RTC_BKP0R_Msk);

    if(remainder > 0)
    {
        // read register
        registerAddress = (uint32_t*)(storeAddress - remainder);
        tempRegisterValue = *registerAddress;

        // adjust remainder to the amount of bytes to move
        remainder = 4 - remainder;

        // clear the bytes we'll be filling
        tempRegisterValue &= (uint32_t)(0xFFFFFF00 << ( (remainder - 1) * 8));

        do
        {
            tempRegisterValue |= (uint32_t)((uint32_t)*data << ( (remainder - 1) * 8));
            remainder--;
            data++;
            counter++;
        }
        while(remainder && (counter < dataLength));

        // write back register
        *registerAddress = tempRegisterValue;
        registerAddress++;
    }

    // keep processing the buffer if there enough bytes
    if( (dataLength - counter) >= 4)
    {
        // clear register
        *registerAddress = 0;

        do
        {
            *registerAddress |= (uint32_t)((uint32_t)*data << (index-- * 8));
            data++;
            counter++;

            if(index < 0)
            {
                index = 3;
                registerAddress++;

                // is there enough data in buffer for another round
                if( (dataLength - counter) < 4)
                {
                    // no, exit loop
                    break;
                }

                // clear register
                *registerAddress = 0;
            }
        }
        while(1);
    }

    // take care of the remaining bytes, if any
    if((dataLength - counter) > 0)
    {
        remainder = dataLength - counter;
        
        // read register
        tempRegisterValue = *registerAddress;

        // clear the bytes we'll be filling
        tempRegisterValue &= (uint32_t)(0x00FFFFFF >> ( (remainder - 1) * 8));

        // adjust remainder to the amount of bytes to move
        remainder = 4 - remainder;

        do
        {
            tempRegisterValue |= (uint32_t)((uint32_t)*data << (remainder * 8));
            remainder--;
            data++;
            counter++;
        }
        while(remainder);

        // write back register
        *registerAddress = tempRegisterValue;
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_BackupMemory::GetSize___STATIC__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    stack.SetResult_I4(BACKUP_SIZE);

    NANOCLR_NOCLEANUP_NOLABEL();
}
 