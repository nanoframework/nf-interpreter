//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#include <string.h>
#include "win_dev_spi_native_target.h"

// define this type here to make it shorter and improve code readability
typedef Library_win_dev_spi_native_Windows_Devices_Spi_SpiConnectionSettings SpiConnectionSettings;


/////////////////////////////////////////////////////
// SPI PAL strucs declared in win_dev_spi_native.h //
/////////////////////////////////////////////////////
NF_PAL_SPI SPI1_PAL;


void SpiCallback(SPI_Handle handle, SPI_Transaction *transaction)
{
    NATIVE_INTERRUPT_START

    // are there any other transactions pending?
    if(SPI1_PAL.transactionCount)
    {
        // start transfer of 1st transaction
        SPI_transfer(SPI1_PAL.masterSpi, &SPI1_PAL.transactions[SPI1_PAL.transactionCount--]);
    }
    else
    {
        // no more transactions
        // fire event for SPI transaction complete
        Events_Set( SYSTEM_EVENT_FLAG_SPI_MASTER );
    }
    
    NATIVE_INTERRUPT_END
}

// Give a complete low-level SPI configuration from user's managed connectionSettings
void GetSPIConfig(int busIndex, CLR_RT_HeapBlock* config, bool bufferIs16bits)
{
    SPI_Params spiParams;

    // Open SPI as slave (default)
    SPI_Params_init(&spiParams);
    spiParams.frameFormat = (SPI_FrameFormat)config[ SpiConnectionSettings::FIELD___spiMode ].NumericByRef().s4;
    spiParams.transferMode = SPI_MODE_CALLBACK;
    spiParams.mode = SPI_SLAVE;
    spiParams.transferCallbackFxn = SpiCallback;
    spiParams.dataSize = bufferIs16bits ? 16 : 8;
    spiParams.bitRate = (uint32_t)config[ SpiConnectionSettings::FIELD___clockFrequency ].NumericByRef().s4;
    SPI1_PAL.masterSpi = SPI_open(Board_SPI_SLAVE, &spiParams);
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::NativeTransfer___VOID__SZARRAY_U1__SZARRAY_U1__BOOLEAN( CLR_RT_StackFrame& stack )
{
    return NativeTransfer( stack, false );
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::NativeTransfer___VOID__SZARRAY_U2__SZARRAY_U2__BOOLEAN( CLR_RT_StackFrame& stack )
{
    return NativeTransfer( stack, true );
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::NativeTransfer( CLR_RT_StackFrame& stack, bool bufferIs16bits )
{
    NANOCLR_HEADER();
    {
        bool fullDuplex;

        CLR_RT_HeapBlock    hbTimeout;
        CLR_INT64*          timeout;
        bool                eventResult = true;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        
        // get a pointer to the managed spi connectionSettings object instance
        CLR_RT_HeapBlock* pConfig = pThis[ FIELD___connectionSettings ].Dereference();

        // Are we using SPI full-duplex for transfer?
        fullDuplex = (bool)stack.Arg3().NumericByRef().u1;

        // dereference the write and read buffers from the arguments
        CLR_RT_HeapBlock_Array* writeBuffer = stack.Arg1().DereferenceArray();
        CLR_RT_HeapBlock_Array* readBuffer = stack.Arg2().DereferenceArray();

        // set a timeout to an infinite timeout
        // the catch is that the working thread MUST ALWAYS return at some point
        // !! need to cast to CLR_INT64 otherwise it wont setup a proper timeout infinite
        hbTimeout.SetInteger((CLR_INT64)-1);

        NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks( hbTimeout, timeout ));
        
        // protect the buffers from GC so DMA can find them where they are supposed to be
        CLR_RT_ProtectFromGC gcWriteBuffer( *writeBuffer );
        CLR_RT_ProtectFromGC gcReadBuffer( *readBuffer );

        // setup the operation and init buffers
        if(stack.m_customState == 1)
        {
            // get the LL SPI configuration, depending on user's managed parameters and buffer element size
            GetSPIConfig(0, pConfig, bufferIs16bits); // index '0' in this case is a dummy value... there is only one SPI available

            // alloc mem for transactions
            if(fullDuplex)
            {
                // we'll be doing this on a single transaction
                SPI1_PAL.transactions = (SPI_Transaction*)platform_malloc(sizeof(SPI_Transaction));
                SPI1_PAL.transactionCount = 1;
            }
            else
            {
                // we need two transactions
                SPI1_PAL.transactions = (SPI_Transaction*)platform_malloc(sizeof(SPI_Transaction) * 2);
                SPI1_PAL.transactionCount = 2;
            }

            // check allocation
            if(SPI1_PAL.transactions == NULL)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
            }
            else
            {
                if(fullDuplex)
                {
                    if (writeBuffer != NULL)
                    {
                        // get the size of the buffer by reading the number of elements in the CLR_RT_HeapBlock_Array
                        SPI1_PAL.transactions[0].count = writeBuffer->m_numOfElements;

                        // set DMA write buffer
                        if(bufferIs16bits)
                        {
                            // buffer data width is 16bits
                            // get the pointer to the write buffer as UINT16 because it's really an UINT16 (2 bytes)
                            SPI1_PAL.transactions[0].txBuf = (uint16_t*)writeBuffer->GetFirstElementUInt16();
                        }
                        else
                        {
                            // buffer data width is 8bits
                            // get the pointer to the write buffer as BYTE
                            SPI1_PAL.transactions[0].txBuf = (uint8_t*)writeBuffer->GetFirstElement();
                        }               
                    }

                    if (readBuffer != NULL)
                    {
                        // we are in full duplex, the count for the SPI transaction was already set on the write

                        if(bufferIs16bits)
                        {
                            // buffer data width is 16bits
                            SPI1_PAL.transactions[0].rxBuf = (uint16_t*)readBuffer->GetFirstElementUInt16();
                        }
                        else
                        {
                            // buffer data width is 8bits
                            // get the pointer to the read buffer as BYTE
                            SPI1_PAL.transactions[0].rxBuf = (uint8_t*)readBuffer->GetFirstElement();
                        }
                    }   
                }
                else
                {
                    // on a sequential transmition we TX first and then RX

                    // TX on 1st transaction
                    if (writeBuffer != NULL)
                    {
                        // get the size of the buffer by reading the number of elements in the CLR_RT_HeapBlock_Array
                        SPI1_PAL.transactions[0].count = writeBuffer->m_numOfElements;

                        // set DMA write buffer
                        if(bufferIs16bits)
                        {
                            // buffer data width is 16bits
                            // get the pointer to the write buffer as UINT16 because it's really an UINT16 (2 bytes)
                            SPI1_PAL.transactions[0].txBuf = (uint16_t*)writeBuffer->GetFirstElementUInt16();
                        }
                        else
                        {
                            // buffer data width is 8bits
                            // get the pointer to the write buffer as BYTE
                            SPI1_PAL.transactions[0].txBuf = (uint8_t*)writeBuffer->GetFirstElement();
                        }               
                    }

                    // RX on 2nd transaction
                    if (readBuffer != NULL)
                    {
                        // get the size of the buffer by reading the number of elements in the CLR_RT_HeapBlock_Array
                        SPI1_PAL.transactions[1].count = readBuffer->m_numOfElements;

                        if(bufferIs16bits)
                        {
                            // buffer data width is 16bits
                            SPI1_PAL.transactions[1].rxBuf = (uint16_t*)readBuffer->GetFirstElementUInt16();
                        }
                        else
                        {
                            // buffer data width is 8bits
                            // get the pointer to the read buffer as BYTE
                            SPI1_PAL.transactions[1].rxBuf = (uint8_t*)readBuffer->GetFirstElement();
                        }
                    }
                }
                
                // start transfer of 1st transaction
                SPI_transfer(SPI1_PAL.masterSpi, &SPI1_PAL.transactions[0]);

                // take one from pending transcations counter
                SPI1_PAL.transactionCount--;

                // bump custom state
                stack.m_customState = 2;
            }
        }

        while(eventResult)
        {
            // non-blocking wait allowing other threads to run while we wait for the Spi transaction to complete
            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.WaitEvents( stack.m_owningThread, *timeout, CLR_RT_ExecutionEngine::c_Event_SpiMaster, eventResult ));
     
            if(!eventResult)
            {
                // timeout occurred
                NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
            }
        }

        // pop timeout heap block from stack
        stack.PopValue();
        
        // null pointers and vars
        pThis = NULL;
        pConfig = NULL;
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::NativeInit___VOID( CLR_RT_StackFrame& stack )
{
    (void)stack;

    NANOCLR_HEADER();

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::DisposeNative___VOID( CLR_RT_StackFrame& stack )
{
    (void)stack;

    NANOCLR_HEADER();

    SPI_close(SPI1_PAL.masterSpi);

    NANOCLR_NOCLEANUP_NOLABEL();
}
