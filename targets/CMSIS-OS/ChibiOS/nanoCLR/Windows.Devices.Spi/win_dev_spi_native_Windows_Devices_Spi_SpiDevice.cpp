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
#include "win_dev_spi_native_target.h"


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

    NF_PAL_SPI* palSpi = NULL;

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

        // fire event for SPI transaction complete
        Events_Set( SYSTEM_EVENT_FLAG_SPI_MASTER );
    }

    NATIVE_INTERRUPT_END
};

// Computes the SPI peripheral baud rate according to the requested frequency
uint16_t ComputeBaudRate(uint8_t busIndex, int32_t requestedFrequency, int32_t& actualFrequency)
{
    uint16_t divider = 0;
    int32_t maxSpiFrequency;

  #if defined(STM32L0XX)

    // SP1 is feed by APB2 (STM32_PCLK2)
    actualFrequency = STM32_PCLK2;

    // SPI2 is feed by APB1 (STM32_PCLK1)
    if (busIndex == 1)
    {
        actualFrequency = STM32_PCLK1;
    }

    // from datasheet
    maxSpiFrequency = 12000000;

  #elif defined(STM32F0XX)

    (void)busIndex;

    // STM32F0 SPI is always feed by APB1
    actualFrequency = STM32_PCLK1;

    // from datasheet
    maxSpiFrequency = 18000000;

  #elif defined(STM32F4XX) || defined(STM32F7XX)

    // SP1, SPI4, SPI5 and SPI6 are feed by APB2 (STM32_PCLK2)
    actualFrequency = STM32_PCLK2;

    // SPI2 and SPI3 are feed by APB1 (STM32_PCLK1)
    if (busIndex == 2 || busIndex == 3)
    {
        actualFrequency = STM32_PCLK1;
    }

    // this is not really accurate because there are different max SPI clocks depending on which APB clock source if feeding the SPI
    // because ChibiOS doesn't offer that we have to go with minimum common denominator
    maxSpiFrequency = STM32_SPII2S_MAX;

  #elif defined(STM32H7XX)

    // SP1, SPI4, SPI5 and SPI6 are feed by APB2 (STM32_PCLK2)
    actualFrequency = STM32_PCLK2;
    maxSpiFrequency = STM32_SPI456_MAX;

    // SPI1, SPI2 and SPI3 are feed by APB1 (STM32_PCLK1)
    if (busIndex == 2 || busIndex == 3)
    {
        actualFrequency = STM32_PCLK1;
        maxSpiFrequency = STM32_SPI123_MAX;
    }

  #else

    #error "Error setting max SPI frequency. Check if the target series is defined."

  #endif
    

    // when requested frequency is 0, means that the developer hasn't set ClockFrequency in SpiConnectionSettings
    // default to the max possible SPI frequency
    if(requestedFrequency == 0)
    {
        requestedFrequency = maxSpiFrequency;
    }

    for( ; divider < 8; divider++)
    {
        actualFrequency = actualFrequency / 2;
        
        if(actualFrequency <= requestedFrequency)
        {
            // best match for the requested frequency
            // just check if it's below the max SPI frequency
            if(actualFrequency <= maxSpiFrequency)
            {
                // we are good with this value
                break;
            }
        }
    }

    // the baud rate bits are in the position B5:3 so need to left shit the divider value 
    return divider << 3;
}

// Give a complete low-level SPI configuration from user's managed connectionSettings
void GetSPIConfig(int busIndex, CLR_RT_HeapBlock* config, SPIConfig* llConfig, bool bufferIs16bits)
{
    int32_t actualFrequency;

    // clear values
    llConfig->cr1 = 0;
    llConfig->cr2 = 0;

    // get chip select pin
    int csPin = config[ SpiConnectionSettings::FIELD___csLine ].NumericByRef().s4;

    // SPI mode
    switch (config[ SpiConnectionSettings::FIELD___spiMode ].NumericByRef().s4)
    {
        case SpiModes_Mode1:
            llConfig->cr1 |= SPI_CR1_CPHA;
            break;

        case SpiModes_Mode2:
            llConfig->cr1 |= SPI_CR1_CPOL;
            break;

        case SpiModes_Mode3:
            llConfig->cr1 |= SPI_CR1_CPHA | SPI_CR1_CPOL;
            break;

        default:   // Default to Mode0 if invalid mode specified
            break;
    }

    // compute baud rate of SPI peripheral according to the requested frequency
    llConfig->cr1 |= ComputeBaudRate(busIndex, config[ SpiConnectionSettings::FIELD___clockFrequency ].NumericByRef().s4, (int32_t&)actualFrequency);

    // set data transfer length according to SPI connection settings and...
    // ... buffer data size. Have to use the shortest one.
    if (config[ SpiConnectionSettings::FIELD___databitLength ].NumericByRef().s4 == 16)
    {
        // databitLength is 16bits

        if(bufferIs16bits)
        {
            // Set data transfer length to 16 bits
  #ifdef STM32F4XX
            llConfig->cr1 |= SPI_CR1_DFF;
  #endif
  #ifdef STM32F7XX
            llConfig->cr2 = SPI_CR2_DS_3 | SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;
  #endif
            // Sets the order of bytes transmission : MSB first or LSB first
            int bitOrder = config[ SpiConnectionSettings::FIELD___bitOrder ].NumericByRef().s4;
            if (bitOrder == DataBitOrder_LSB) 
            {
                llConfig->cr1 |= SPI_CR1_LSBFIRST;
            }
        }
        else
        {
            // have to force transfer length to 8bit
  #ifdef STM32F4XX
            llConfig->cr1 &= ~SPI_CR1_DFF;
  #endif
  #ifdef STM32F7XX
            llConfig->cr2 = SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;
  #endif            
        }
    }
    else
    {
        // databitLength is 8bits

        if(bufferIs16bits)
        {
            // have to force transfer length to 16bit 
  #ifdef STM32F4XX
            llConfig->cr1 |= SPI_CR1_DFF;
  #endif
  #ifdef STM32F7XX
            llConfig->cr2 = SPI_CR2_DS_3 | SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;
  #endif

        }
        else
        {
            // set transfer length to 8bits
  #ifdef STM32F4XX
            llConfig->cr1 &= ~SPI_CR1_DFF;
  #endif
  #ifdef STM32F7XX
            llConfig->cr2 |= SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;
  #endif            
        }
    }

    // Create the low level configuration
  #if (SPI_SUPPORTS_CIRCULAR == TRUE) 
    llConfig->circular = SPI_USE_CIRCULAR;
  #endif
    llConfig->end_cb = SpiCallback;
    llConfig->ssport = GPIO_PORT(csPin);
    llConfig->sspad = csPin % 16;
}

// estimate the time required to perform the SPI transaction
bool IsLongRunningOperation(uint32_t writeSize, uint32_t readSize, bool bufferIs16bits, float byteTime, uint32_t& estimatedDurationMiliseconds)
{
    if(bufferIs16bits)
    {
        // double the buffers size
        writeSize = 2 * writeSize;
        readSize = 2 * readSize;
    }

    estimatedDurationMiliseconds = byteTime * (writeSize + readSize);

    if(estimatedDurationMiliseconds > CLR_RT_Thread::c_TimeQuantum_Milliseconds)
    {
        // total operation time will exceed thread quantum, so this is a long running operation
        return true;
    }
    else
    {
        return false;        
    }
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
        NF_PAL_SPI* palSpi = NULL;
        uint8_t busIndex;
        bool fullDuplex;
        bool isLongRunningOperation = false;

        CLR_RT_HeapBlock    hbTimeout;
        CLR_INT64*          timeout;
        bool                eventResult = true;
        uint32_t            estimatedDurationMiliseconds;

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
            // get the size of the buffer by reading the number of elements in the CLR_RT_HeapBlock_Array
            palSpi->WriteSize = writeBuffer->m_numOfElements;
        }

        CLR_RT_HeapBlock_Array* readBuffer = stack.Arg2().DereferenceArray();
        if (readBuffer != NULL)
        {
            // get the size of the buffer by reading the number of elements in the CLR_RT_HeapBlock_Array
            palSpi->ReadSize = readBuffer->m_numOfElements;
        }

        // check if this is a long running operation
        isLongRunningOperation = IsLongRunningOperation(palSpi->WriteSize, palSpi->ReadSize, bufferIs16bits, palSpi->ByteTime, (uint32_t&)estimatedDurationMiliseconds);

        if(isLongRunningOperation)
        {
            // if this is a long running operation, set a timeout equal to the estimated transaction duration in milliseconds
            // this value has to be in ticks to be properly loaded by SetupTimeoutFromTicks() below
            hbTimeout.SetInteger((CLR_INT64)estimatedDurationMiliseconds * TIME_CONVERSION__TO_MILLISECONDS);

            NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks( hbTimeout, timeout ));
            
            // protect the buffers from GC so DMA can find them where they are supposed to be
            CLR_RT_ProtectFromGC gcWriteBuffer( *writeBuffer );
            CLR_RT_ProtectFromGC gcReadBuffer( *readBuffer );
        }

        // setup the operation and init buffers
        if(!isLongRunningOperation || stack.m_customState == 1)
        {
            // get the LL SPI configuration, depending on user's managed parameters and buffer element size
            GetSPIConfig(busIndex, pConfig, &palSpi->Configuration, bufferIs16bits);

            // if this is not a long run operation we'll be using the async API thus operations are sequencial
            // need to remove the callback from the SPI LL config
            if(!isLongRunningOperation)
            {
                palSpi->Configuration.end_cb = NULL;
            }

            if (writeBuffer != NULL)
            {
                // set DMA write buffer
                if(bufferIs16bits)
                {
                    // buffer data width is 16bits
                    // get the pointer to the write buffer as UINT16 because it's really an UINT16 (2 bytes)
                    palSpi->WriteBuffer = (uint8_t*)writeBuffer->GetFirstElementUInt16();

                    // flush DMA buffer to ensure cache coherency
                    // (only required for Cortex-M7)
                    cacheBufferFlush(palSpi->WriteBuffer, (palSpi->WriteSize * 2));
                }
                else
                {
                    // buffer data width is 8bits
                    // get the pointer to the write buffer as BYTE
                    palSpi->WriteBuffer = (uint8_t*)writeBuffer->GetFirstElement();

                    // flush DMA buffer to ensure cache coherency
                    // (only required for Cortex-M7)
                    cacheBufferFlush(palSpi->WriteBuffer, palSpi->WriteSize);
                }               
            }

            if (readBuffer != NULL)
            {
                // set DMA read buffer
                if(palSpi->ReadSize > 0)
                {
                    if(bufferIs16bits)
                    {
                        // buffer data width is 16bits
                        palSpi->ReadBuffer = (uint8_t*)readBuffer->GetFirstElementUInt16();
                    }
                    else
                    {
                        // buffer data width is 8bits
                        // get the pointer to the read buffer as BYTE
                        palSpi->ReadBuffer = (uint8_t*)readBuffer->GetFirstElement();
                    }
                }
            }

            // because the bus access is shared, acquire and select the appropriate bus
            spiAcquireBus(palSpi->Driver);
            spiStart(palSpi->Driver, &palSpi->Configuration);

            // just to satisfy the driver ceremony, no actual implementation for STM32
            spiSelect(palSpi->Driver);
        }

        if(isLongRunningOperation)
        {
            // this is a long running operation and hasn't started yet
            // perform SPI operation using driver's ASYNC API
            if(stack.m_customState == 1)
            {
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
        }
        else
        {
            // this is NOT a long running operation
            // perform SPI operation using driver's SYNC API
            if (palSpi->WriteSize != 0 && palSpi->ReadSize != 0)
            {
                // Transmit+Receive
                if (fullDuplex)
                {
                    // Full duplex
                    // Uses the largest buffer size as transfer size
                    spiExchange(palSpi->Driver, palSpi->WriteSize > palSpi->ReadSize ? palSpi->WriteSize : palSpi->ReadSize, palSpi->WriteBuffer, palSpi->ReadBuffer);
                }
                else
                {
                    // send operation
                    spiSend(palSpi->Driver, palSpi->WriteSize, palSpi->WriteBuffer);
                    // receive operation
                    spiReceive(palSpi->Driver, palSpi->ReadSize, palSpi->ReadBuffer);
                }
            }
            else
            {
                // Transmit only or Receive only
                if (palSpi->ReadSize != 0) 
                {
                    // receive
                    spiReceive(palSpi->Driver, palSpi->ReadSize, palSpi->ReadBuffer);
                }
                else
                {
                    // send
                    spiSend(palSpi->Driver, palSpi->WriteSize, palSpi->WriteBuffer);
                }
            }
            
            // just to satisfy the driver ceremony, no actual implementation for STM32
            spiUnselect(palSpi->Driver);

            // Release the bus
            spiReleaseBus(palSpi->Driver);
        }

        while(eventResult)
        {
            if(!isLongRunningOperation)
            {
                // this is not a long running operation so nothing to do here
                break;
            }

            if(palSpi->Driver->state == SPI_READY)
            {
                // SPI driver is ready meaning that the SPI transaction(s) is(are) completed
                break;
            }

            // non-blocking wait allowing other threads to run while we wait for the Spi transaction to complete
            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.WaitEvents( stack.m_owningThread, *timeout, CLR_RT_ExecutionEngine::c_Event_SpiMaster, eventResult ));
        }

        if(isLongRunningOperation)
        {
            // pop timeout heap block from stack
            stack.PopValue();
        }
        
        if(eventResult)
        {
            spiReleaseBus(palSpi->Driver);

            // event occurred
            if(palSpi->ReadSize > 0)
            {
                // because this was a Read transaction, need to copy from DMA buffer to managed buffer
                if(bufferIs16bits)
                {
                    // buffer data width is 16bits
                    
                    // invalidate cache over read buffer to ensure that content from DMA is read
                    // (only required for Cortex-M7)
                    // get the pointer to the read buffer as UINT16 because it's really an UINT16 (2 bytes)
                    cacheBufferInvalidate(palSpi->ReadBuffer, (palSpi->ReadSize * 2));
                }
                else
                {
                    // buffer data width is 8bits

                    // invalidate cache over read buffer to ensure that content from DMA is read
                    // (only required for Cortex-M7)
                    // get the pointer to the read buffer as BYTE
                    cacheBufferInvalidate(palSpi->ReadBuffer, palSpi->ReadSize);
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
        NF_PAL_SPI* palSpi = NULL;
        int32_t actualFrequency;
        
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        
        // get a pointer to the managed spi connectionSettings object instance
        CLR_RT_HeapBlock* pConfig = pThis[ FIELD___connectionSettings ].Dereference();
        
        // get bus index
        // this is coded with a multiplication, need to perform and int division to get the number
        // see the comments in the SpiDevice() constructor in managed code for details
        uint8_t busIndex = (uint8_t)(pThis[ FIELD___deviceId ].NumericByRef().s4 / 1000);

        // config GPIO pins used by the SPI peripheral
        // init the PAL struct for this SPI bus and assign the respective driver
        // all this occurs if not already done
        // why do we need this? because several SPIDevice objects can be created associated to the same bus
        switch (busIndex)
        {
    #if STM32_SPI_USE_SPI1
            case 1:
                if(SPI1_PAL.Driver == NULL)
                {
                    ConfigPins_SPI1();

                    SPI1_PAL.Driver = &SPID1;
                    palSpi = &SPI1_PAL;
                }
                break;
    #endif
    #if STM32_SPI_USE_SPI2
            case 2:
                if(SPI2_PAL.Driver == NULL)
                {
                    ConfigPins_SPI2();
                    
                    SPI2_PAL.Driver = &SPID2;
                    palSpi = &SPI2_PAL;
                }
                break;
    #endif
    #if STM32_SPI_USE_SPI3
            case 3:
                if(SPI3_PAL.Driver == NULL)
                {
                    ConfigPins_SPI3();
                    
                    SPI3_PAL.Driver = &SPID3;
                    palSpi = &SPI3_PAL;
                }
                break;
    #endif
    #if STM32_SPI_USE_SPI4
            case 4:
                if(SPI4_PAL.Driver == NULL)
                {
                    ConfigPins_SPI4();
                    
                    SPI4_PAL.Driver = &SPID4;
                    palSpi = &SPI4_PAL;
                }
                break;
    #endif
    #if STM32_SPI_USE_SPI5
            case 5:
                if(SPI5_PAL.Driver == NULL)
                {
                    ConfigPins_SPI5();
                    
                    SPI5_PAL.Driver = &SPID5;
                    palSpi = &SPI5_PAL;
                }
                break;
    #endif
    #if STM32_SPI_USE_SPI6
            case 6:
                if(SPI6_PAL.Driver == NULL)
                {
                    ConfigPins_SPI6();
                    
                    SPI6_PAL.Driver = &SPID6;
                    palSpi = &SPI6_PAL;
                }
                break;
    #endif
            default:
                // this SPI bus is not valid
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                break;
        }

        // compute rough estimate on the time to tx/rx a byte (in milliseconds)
        ComputeBaudRate(busIndex, pConfig[ SpiConnectionSettings::FIELD___clockFrequency ].NumericByRef().s4, (int32_t&)actualFrequency);
        palSpi->ByteTime = (1.0 / actualFrequency) * 1000;
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::DisposeNative___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    uint8_t busIndex;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    // get bus index
    // this is coded with a multiplication, need to perform and int division to get the number
    // see the comments in the SpiDevice() constructor in managed code for details
    busIndex = (uint8_t)(pThis[ FIELD___deviceId ].NumericByRef().s4 / 1000);

    // get the PAL struct for the SPI bus
    switch (busIndex)
    {
      #if STM32_SPI_USE_SPI1
        case 1:
            spiStop(&SPID1);
            SPI1_PAL.Driver = NULL;
            break;
      #endif

      #if STM32_SPI_USE_SPI2
        case 2:
            spiStop(&SPID2);
            SPI2_PAL.Driver = NULL;
            break;
      #endif

      #if STM32_SPI_USE_SPI3
        case 3:
            spiStop(&SPID3);
            SPI3_PAL.Driver = NULL;
            break;
      #endif

      #if STM32_SPI_USE_SPI4
        case 4:
            spiStop(&SPID4);
            SPI4_PAL.Driver = NULL;
            break;
      #endif

      #if STM32_SPI_USE_SPI5
        case 5:
            spiStop(&SPID5);
            SPI5_PAL.Driver = NULL;
            break;
      #endif

      #if STM32_SPI_USE_SPI6
        case 6:
            spiStop(&SPID6);
            SPI6_PAL.Driver = NULL;
            break;
      #endif

        default:
            // the requested SPI bus is not valid
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            break;
    }

    NANOCLR_NOCLEANUP();
}
