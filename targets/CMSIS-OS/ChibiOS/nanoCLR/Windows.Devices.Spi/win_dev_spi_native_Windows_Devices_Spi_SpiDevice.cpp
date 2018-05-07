//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>
#include <LaunchCLR.h>
#include <string.h>
#include <targetPAL.h>
#include "win_dev_spi_native.h"

#define SPI_TRANSACTION_TIMEOUT_PER_BYTE_MS      50

///////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.Spi.SpiMode (in managed code) !!! //
///////////////////////////////////////////////////////////////////////////////////////

enum DataBitOrder
{
    DataBitOrder_MSB = 0,
    DataBitOrder_LSB
};

///////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.Spi.SpiMode (in managed code) !!! //
///////////////////////////////////////////////////////////////////////////////////////

enum SpiModes
{
    SpiModes_Mode0 = 0,
    SpiModes_Mode1,
    SpiModes_Mode2,
    SpiModes_Mode3
};

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

// define this type here to make it shorter and improve code readability
typedef Library_win_dev_spi_native_Windows_Devices_Spi_SpiConnectionSettings SpiConnectionSettings;


/////////////////////////////////////////////////////
// SPI PAL strucs declared in win_dev_spi_native.h //
/////////////////////////////////////////////////////
#if STM32_SPI_USE_SPI1
NF_PAL_SPI SPI1_PAL;
#endif
#if STM32_SPI_USE_SPI2
NF_PAL_SPI SPI2_PAL;
#endif
#if STM32_SPI_USE_SPI3
NF_PAL_SPI SPI3_PAL;
#endif
#if STM32_SPI_USE_SPI4
NF_PAL_SPI SPI4_PAL;
#endif
#if STM32_SPI_USE_SPI5
NF_PAL_SPI SPI5_PAL;
#endif
#if STM32_SPI_USE_SPI6
NF_PAL_SPI SPI6_PAL;
#endif

static void SpiCallback(SPIDriver *spip)
{
    (void)spip;

    NATIVE_INTERRUPT_START

    NF_PAL_SPI* palSpi;

  #if STM32_SPI_USE_SPI1
    if (spip == &SPID1)
    {
        palSpi = &SPI1_PAL;
    }
  #endif
  #if STM32_SPI_USE_SPI2
    if (spip == &SPID2)
    {
        palSpi = &SPI2_PAL;
    }
  #endif
  #if STM32_SPI_USE_SPI3
    if (spip == &SPID3)
    {
        palSpi = &SPI3_PAL;
    }
  #endif
  #if STM32_SPI_USE_SPI4
    if (spip == &SPID4)
    {
        palSpi = &SPI4_PAL;
    }
  #endif
  #if STM32_SPI_USE_SPI5
    if (spip == &SPID5)
    {
        palSpi = &SPI5_PAL;
    }  
  #endif
  #if STM32_SPI_USE_SPI6
    if (spip == &SPID6)
    {
        palSpi = &SPI6_PAL;
    }
  #endif

    // check if there is any Rx operation due
    if(palSpi->SequentialTxRx)
    {
        // yes there is!
        // clear flag and...
        palSpi->SequentialTxRx = false;

        // ... start it
        spiStartReceive(palSpi->Driver, palSpi->ReadSize, palSpi->ReadBuffer);
    }
    else
    {
        // all done here!

        spiUnselect(palSpi->Driver);
        // need to deassert the chip select line because the current ChibiOS driver doesn't implement the above spiUnselect()
        palSetPad(palSpi->Configuration.ssport, palSpi->Configuration.sspad);

        // Release the bus
        spiReleaseBus(palSpi->Driver);

        // fire event for SPI transaction complete
        Events_Set( SYSTEM_EVENT_FLAG_SPI_MASTER );
    }

    NATIVE_INTERRUPT_END
};

// Computes the prescaler registers given a frequency
uint16_t Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::ComputePrescaler(uint8_t busIndex, int32_t requestedFrequency)
{
    uint16_t pre = 0;
  #if defined(STM32F4xx_MCUCONF) || defined(STM32F7xx_MCUCONF)
    // SP1, SPI4, SPI5 and SPI6 on APB2
	int32_t clock = STM32_SPII2S_MAX >> 1;
    
    // SPI2 and SPI3 on APB1
    if (busIndex == 2 || busIndex == 3)
    {
        clock >>= 1;
    }
  #elif defined(STM32F0xx_MCUCONF)
    int32_t clock = 12000000 >> 1;
  #else
    #error "Please check max SPI frequency"
  #endif

	if (clock > requestedFrequency << 3)
    {
		clock >>= 4;
		pre |= SPI_CR1_BR_2;
	}
	if (clock > requestedFrequency << 1)
    {
		clock >>= 2;
		pre |= SPI_CR1_BR_1;
	}

	if (clock > requestedFrequency)
    {
        pre |= SPI_CR1_BR_0;
    }

    return pre;
}

// Give a complete low-level SPI configuration from user's managed connectionSettings
void Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::GetConfig(int busIndex, CLR_RT_HeapBlock* config, SPIConfig* llConfig, bool bufferIs16bits)
{
    int csPin = config[ SpiConnectionSettings::FIELD___csLine ].NumericByRef().s4;
    uint16_t CR1 = SPI_CR1_SSI | SPI_CR1_MSTR;

  #ifdef STM32F7xx_MCUCONF
    // 8bit transfer mode as default
    uint16_t CR2 = SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;
  #endif

    switch (config[ SpiConnectionSettings::FIELD___spiMode ].NumericByRef().s4)
    {
        case SpiModes_Mode1:
            CR1 |= SPI_CR1_CPHA;
            break;

        case SpiModes_Mode2:
            CR1 |= SPI_CR1_CPOL;
            break;

        case SpiModes_Mode3:
            CR1 |= SPI_CR1_CPHA | SPI_CR1_CPOL;
            break;

        default:   // Default to Mode0 if invalid mode specified
            break;
    }

    // set data transfer length according to SPI connection settings and...
    // ... buffer data size. Have to use the shortest one.
    if (config[ SpiConnectionSettings::FIELD___databitLength ].NumericByRef().s4 == 16)
    {
        // databitLength is 16bits

        if(bufferIs16bits)
        {
            // Set data transfer length to 16 bits
  #ifdef STM32F4xx_MCUCONF
            CR1 |= SPI_CR1_DFF;
  #endif
  #ifdef STM32F7xx_MCUCONF
            CR2 = SPI_CR2_DS_3 | SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;
  #endif
            // Sets the order of bytes transmission : MSB first or LSB first
            int bitOrder = config[ SpiConnectionSettings::FIELD___bitOrder ].NumericByRef().s4;
            if (bitOrder == DataBitOrder_LSB) 
            {
                CR1 |= SPI_CR1_LSBFIRST;
            }
        }
        else
        {
            // have to force transfer length to 8bit
  #ifdef STM32F4xx_MCUCONF
            CR1 &= ~SPI_CR1_DFF;
  #endif
  #ifdef STM32F7xx_MCUCONF
            CR2 = SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;
  #endif            
        }
    }
    else
    {
        // databitLength is 8bits

        if(bufferIs16bits)
        {
            // have to force transfer length to 16bit 
  #ifdef STM32F4xx_MCUCONF
            CR1 |= SPI_CR1_DFF;
  #endif
  #ifdef STM32F7xx_MCUCONF
            CR2 = SPI_CR2_DS_3 | SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;
  #endif

        }
        else
        {
            // set transfer length to 8bits
            // this is the default so it's already set this way
        }
    }

    // SPI on STM32 needs a prescaler value to set the frequency used
    CR1 |= ComputePrescaler(busIndex, config[ SpiConnectionSettings::FIELD___clockFrequency ].NumericByRef().s4);

    // Create the low level configuration
  #if (SPI_SUPPORTS_CIRCULAR == TRUE) 
    llConfig->circular = SPI_USE_CIRCULAR;
  #endif
    llConfig->end_cb = SpiCallback;
    llConfig->ssport = GPIO_PORT(csPin);
    llConfig->sspad = csPin % 16;
    llConfig->cr1 = CR1;
  #ifdef STM32F4xx_MCUCONF
    llConfig->cr2 = 0;
  #endif
  #ifdef STM32F7xx_MCUCONF
    llConfig->cr2 = CR2;
  #endif

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
        NF_PAL_SPI* palSpi;
        uint8_t busIndex;
        bool fullDuplex;
        bool databitLengthIs16;

        CLR_RT_HeapBlock    hbTimeout;
        CLR_INT64*          timeout;
        bool                eventResult = true;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        
        // get a pointer to the managed spi connectionSettings object instance
        CLR_RT_HeapBlock* pConfig = pThis[ FIELD___connectionSettings ].Dereference();

        // get bus index
        // this is coded with a multiplication, need to perform and int division to get the number
        // see the comments in the SpiDevice() constructor in managed code for details
        busIndex = (uint8_t)(pThis[ FIELD___deviceId ].NumericByRef().s4 / 1000);

        // get the PAL struct for the SPI bus
        switch (busIndex)
        {
    #if STM32_SPI_USE_SPI1
            case 1:
                palSpi = &SPI1_PAL;
                break;
    #endif
    #if STM32_SPI_USE_SPI2
            case 2:
                palSpi = &SPI2_PAL;
                break;
    #endif
    #if STM32_SPI_USE_SPI3
            case 3:
                palSpi = &SPI3_PAL;
                break;
    #endif
    #if STM32_SPI_USE_SPI4
            case 4:
                palSpi = &SPI4_PAL;
                break;
    #endif
    #if STM32_SPI_USE_SPI5
            case 5:
                palSpi = &SPI5_PAL;
                break;
    #endif
    #if STM32_SPI_USE_SPI6
            case 6:
                palSpi = &SPI6_PAL;
                break;
    #endif
            default:
                // the requested SPI bus is not valid
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                break;
        }

        // Are we using SPI full-duplex for transfer?
        fullDuplex = (bool)stack.Arg3().NumericByRef().u1;

        // dereference the write and read buffers from the arguments
        CLR_RT_HeapBlock_Array* writeBuffer = stack.Arg1().DereferenceArray();
        if (writeBuffer != NULL)
        {
            // get the size of the buffer by reading the number of elements in the HeapBlock array
            palSpi->WriteSize = writeBuffer->m_numOfElements;

            // sanity check for buffer size
            if(bufferIs16bits)
            {
                if((palSpi->WriteSize * 2) > palSpi->WriteBufferSize)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
                }
            }
            else
            {
                if(palSpi->WriteSize > palSpi->WriteBufferSize)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
                }
            }
        }

        CLR_RT_HeapBlock_Array* readBuffer = stack.Arg2().DereferenceArray();
        if (readBuffer != NULL)
        {
            // get the size of the buffer by reading the number of elements in the HeapBlock array
            palSpi->ReadSize = readBuffer->m_numOfElements;

            // sanity check for buffer size
            if(bufferIs16bits)
            {
                if((palSpi->ReadSize * 2) > palSpi->ReadBufferSize)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
                }
            }
            else
            {
                if(palSpi->ReadSize > palSpi->ReadBufferSize)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
                }
            }          
        }

        // set a timeout equal to N milliseconds per read and write byte for the Spi transaction timeout
        // it should be enough to ensure SPI bus read and write, including any potential timeout there
        // this value has to be in ticks to be properly loaded by SetupTimeoutFromTicks() bellow
        hbTimeout.SetInteger((CLR_INT64)SPI_TRANSACTION_TIMEOUT_PER_BYTE_MS * (palSpi->WriteSize + palSpi->ReadSize) * TIME_CONVERSION__TO_MILLISECONDS);

        NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks( hbTimeout, timeout ));

        // this is going to be used to check for the right event in case of simultaneous Spi transaction
        if(stack.m_customState == 1)
        {
            // Get a complete low-level SPI configuration, depending on user's managed parameters and buffer element size
            GetConfig(busIndex, pConfig, &palSpi->Configuration, bufferIs16bits);

            if (writeBuffer != NULL)
            {
                // copy to DMA write buffer
                if(bufferIs16bits)
                {
                    // buffer data width is 16bits
                    // get the pointer to the write buffer as UINT16
                    // memcpy copies N bytes, so we need to double the array size because it's really an UINT16 (2 bytes)
                    memcpy(palSpi->WriteBuffer, (uint8_t*)writeBuffer->GetFirstElementUInt16(), (palSpi->WriteSize * 2));

                    // flush DMA buffer to ensure cache coherency
                    // (only required for Cortex-M7)
                    cacheBufferFlush(palSpi->WriteBuffer, (palSpi->WriteSize * 2));
                }
                else
                {
                    // buffer data width is 8bits
                    // get the pointer to the write buffer as BYTE
                    memcpy(palSpi->WriteBuffer, (uint8_t*)writeBuffer->GetFirstElement(), palSpi->WriteSize);

                    // flush DMA buffer to ensure cache coherency
                    // (only required for Cortex-M7)
                    cacheBufferFlush(palSpi->WriteBuffer, palSpi->WriteSize);
                }               
            }

            // because the bus access is shared, acquire and select the appropriate bus
            spiAcquireBus(palSpi->Driver);
            spiStart(palSpi->Driver, &palSpi->Configuration);

            spiSelect(palSpi->Driver);
            // need to assert the chip select line because the current ChibiOS driver doesn't implement the above spiSelect()
            palClearPad(palSpi->Configuration.ssport, palSpi->Configuration.sspad);

            if (palSpi->WriteSize != 0 && palSpi->ReadSize != 0)
            {
                // Transmit+Receive
                if (fullDuplex)
                {
                    // Full duplex
                    // single operation, clear flag
                    palSpi->SequentialTxRx = false;

                    // Uses the largest buffer size as transfer size
                    spiStartExchange(palSpi->Driver, palSpi->WriteSize > palSpi->ReadSize ? palSpi->WriteSize : palSpi->ReadSize, palSpi->WriteBuffer, palSpi->ReadBuffer);
                }
                else
                {
                    // flag that an Rx is required after the Tx operation completes
                    palSpi->SequentialTxRx = true;

                    // start send operation
                    spiStartSend(palSpi->Driver, palSpi->WriteSize, palSpi->WriteBuffer);
                    // receive operation will be started in the callback after the above completes
                }
            }
            else
            {
                // Transmit only or Receive only
                if (palSpi->ReadSize != 0) 
                {
                    // single operation, clear flag
                    palSpi->SequentialTxRx = false;

                    // start receive
                    spiStartReceive(palSpi->Driver, palSpi->ReadSize, palSpi->ReadBuffer);
                }
                else
                {
                    // single operation, clear flag
                    palSpi->SequentialTxRx = false;

                    // start send
                    spiStartSend(palSpi->Driver, palSpi->WriteSize, palSpi->WriteBuffer);
                }
            }            

            // bump custom state
            stack.m_customState = 2;
        }

        while(eventResult)
        {
            if(palSpi->Driver->state == SPI_READY)
            {
                // SPI driver is ready meaning that the SPI transaction(s) is(are) completed
                break;
            }

            // non-blocking wait allowing other threads to run while we wait for the Spi transaction to complete
            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.WaitEvents( stack.m_owningThread, *timeout, CLR_RT_ExecutionEngine::c_Event_SpiMaster, eventResult ));
        }

        // pop timeout heap block from stack
        stack.PopValue();

        if(eventResult)
        {
            // event occurred
            if(palSpi->ReadSize > 0)
            {
                // because this was a Read transaction, need to copy from DMA buffer to managed buffer
                if(bufferIs16bits)
                {
                    // buffer data width is 16bits
                    
                    // invalidate cache over read buffer to ensure that content from DMA is read
                    // (only required for Cortex-M7)
                    cacheBufferInvalidate(palSpi->ReadBuffer, (palSpi->ReadSize * 2));

                    // copy Spi read buffer into managed buffer
                    // get the pointer to the read buffer as UINT16
                    // memcpy copies N bytes, so we need to double the array size because it's really an UINT16 (2 bytes)
                    memcpy(readBuffer->GetFirstElementUInt16(), palSpi->ReadBuffer, (palSpi->ReadSize * 2));
                }
                else
                {
                    // buffer data width is 8bits

                    // invalidate cache over read buffer to ensure that content from DMA is read
                    // (only required for Cortex-M7)
                    cacheBufferInvalidate(palSpi->ReadBuffer, palSpi->ReadSize);

                    // copy Spi read buffer into managed buffer
                    // get the pointer to the read buffer as BYTE
                    memcpy(readBuffer->GetFirstElement(), palSpi->ReadBuffer, palSpi->ReadSize);
                }
            }
        }

        // null pointers and vars
        pThis = NULL;
        pConfig = NULL;
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::NativeInit___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        
        // get bus index
        // this is coded with a multiplication, need to perform and int division to get the number
        // see the comments in the SpiDevice() constructor in managed code for details
        uint8_t busIndex = (uint8_t)(pThis[ FIELD___deviceId ].NumericByRef().s4 / 1000);

        // init the PAL struct for this SPI bus and assign the respective driver
        // all this occurs if not already done
        // why do we need this? because several SPIDevice objects can be created associated to the same bus
        switch (busIndex)
        {
    #if STM32_SPI_USE_SPI1
            case 1:
                if(SPI1_PAL.Driver == NULL)
                {
                    Init_SPI1();
                    SPI1_PAL.Driver = &SPID1;
                }
                break;
    #endif
    #if STM32_SPI_USE_SPI2
            case 2:
                if(SPI2_PAL.Driver == NULL)
                {
                    Init_SPI2();
                    SPI2_PAL.Driver = &SPID2;
                }
                break;
    #endif
    #if STM32_SPI_USE_SPI3
            case 3:
                if(SPI3_PAL.Driver == NULL)
                {
                    Init_SPI3();
                    SPI3_PAL.Driver = &SPID3;
                }
                break;
    #endif
    #if STM32_SPI_USE_SPI4
            case 4:
                if(SPI4_PAL.Driver == NULL)
                {
                    Init_SPI4();
                    SPI4_PAL.Driver = &SPID4;
                }
                break;
    #endif
    #if STM32_SPI_USE_SPI5
            case 5:
                if(SPI5_PAL.Driver == NULL)
                {
                    Init_SPI5();
                    SPI5_PAL.Driver = &SPID5;
                }
                break;
    #endif
    #if STM32_SPI_USE_SPI6
            case 6:
                if(SPI6_PAL.Driver == NULL)
                {
                    Init_SPI6();
                    SPI6_PAL.Driver = &SPID6;
                }
                break;
    #endif
            default:
                // this SPI bus is not valid
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                break;
        }

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::DisposeNative___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // set pin to input to save power
        // clear interrupts
        // release the pin
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::GetDeviceSelector___STATIC__STRING( CLR_RT_StackFrame& stack )
{
   NANOCLR_HEADER();
   {
       // declare the device selector string whose max size is "SPI1,SPI2,SPI3,SPI4,SPI5,SPI6," + terminator and init with the terminator
       char deviceSelectorString[ 30 + 1] = { 0 };

   #if STM32_SPI_USE_SPI1
       strcat(deviceSelectorString, "SPI1,");
   #endif
   #if STM32_SPI_USE_SPI2
       strcat(deviceSelectorString, "SPI2,");
   #endif
   #if STM32_SPI_USE_SPI3
       strcat(deviceSelectorString, "SPI3,");
   #endif
   #if STM32_SPI_USE_SPI4
       strcat(deviceSelectorString, "SPI4,");
   #endif
   #if STM32_SPI_USE_SPI5
       strcat(deviceSelectorString, "SPI5,");
   #endif
   #if STM32_SPI_USE_SPI6
       strcat(deviceSelectorString, "SPI6,");
   #endif

       // replace the last comma with a terminator
       deviceSelectorString[hal_strlen_s(deviceSelectorString) - 1] = '\0';

       // because the caller is expecting a result to be returned
       // we need set a return result in the stack argument using the appropriate SetResult according to the variable type (a string here)
       stack.SetResult_String(deviceSelectorString);
   }
   NANOCLR_NOCLEANUP();
}

