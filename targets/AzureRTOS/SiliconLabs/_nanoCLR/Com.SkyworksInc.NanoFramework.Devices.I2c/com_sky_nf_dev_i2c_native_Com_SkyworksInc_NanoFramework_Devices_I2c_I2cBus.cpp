// Copyright Skyworks Solutions, Inc. All Rights Reserved.

#include "com_sky_nf_dev_i2c_native_target.h"

typedef Library_corlib_native_System_SpanByte SpanByte;
typedef Library_com_sky_nf_dev_i2c_native_Com_SkyworksInc_NanoFramework_Devices_I2c_I2cException I2cException;

////////////////////////////////////////////
// declaration of the the I2C PAL structs //
////////////////////////////////////////////
#if defined(I2C0) && (GECKO_USE_I2C0 == TRUE)
NF_PAL_I2C I2C0_PAL;
#endif
#if defined(I2C1) && (GECKO_USE_I2C1 == TRUE)
NF_PAL_I2C I2C1_PAL;
#endif
#if defined(I2C2) && (GECKO_USE_I2C2 == TRUE)
NF_PAL_I2C I2C2_PAL;
#endif

#if defined(I2C0) && (GECKO_USE_I2C0 == TRUE)
uint8_t I2C0_DeviceCounter = 0;
#endif
#if defined(I2C1) && (GECKO_USE_I2C1 == TRUE)
uint8_t I2C1_DeviceCounter = 0;
#endif
#if defined(I2C2) && (GECKO_USE_I2C2 == TRUE)
uint8_t I2C2_DeviceCounter = 0;
#endif

// estimate the time required to perform the I2C transaction
bool IsLongRunningOperation(
    uint16_t writeSize,
    uint16_t readSize,
    float byteTime,
    uint32_t &estimatedDurationMiliseconds)
{
    // add an extra byte to account for the address
    estimatedDurationMiliseconds = byteTime * (writeSize + readSize + 1);

    if (estimatedDurationMiliseconds > CLR_RT_Thread::c_TimeQuantum_Milliseconds)
    {
        // total operation time will exceed thread quantum, so this is a long running operation
        return true;
    }
    else
    {
        return false;
    }
}

// ThreadX I2C Working thread
static void I2CWorkingThread_entry(uint32_t arg)
{
    NF_PAL_I2C *palI2c = (NF_PAL_I2C *)arg;
    I2C_TransferSeq_TypeDef i2cTransfer;

    if (palI2c->ReadSize != 0 && palI2c->WriteSize != 0)
    {
        // this is a Write/Read transaction
        i2cTransfer.flags = I2C_FLAG_WRITE_READ;

        i2cTransfer.buf[0].data = palI2c->WriteBuffer;
        i2cTransfer.buf[0].len = palI2c->WriteSize;
        i2cTransfer.buf[1].data = palI2c->ReadBuffer;
        i2cTransfer.buf[1].len = palI2c->ReadSize;

        // Perform the transfer and return status from the transfer
        palI2c->TransactionResult = I2CSPM_Transfer(palI2c->Configuration->port, &i2cTransfer);
    }
    else
    {
        if (palI2c->ReadSize == 0)
        {
            // this is Write only transaction
            i2cTransfer.flags = I2C_FLAG_WRITE;

            i2cTransfer.buf[0].data = palI2c->WriteBuffer;
            i2cTransfer.buf[0].len = palI2c->WriteSize;

            // Perform the transfer and return status from the transfer
            palI2c->TransactionResult = I2CSPM_Transfer(palI2c->Configuration->port, &i2cTransfer);
        }
        else
        {
            // this is a Read only transaction
            i2cTransfer.flags = I2C_FLAG_READ;
            i2cTransfer.buf[0].data = palI2c->ReadBuffer;
            i2cTransfer.buf[0].len = palI2c->ReadSize;

            // Perform the transfer and return status from the transfer
            palI2c->TransactionResult = I2CSPM_Transfer(palI2c->Configuration->port, &i2cTransfer);
        }
    }

    // fire event for I2C transaction complete
    Events_Set(SYSTEM_EVENT_FLAG_I2C_MASTER);

    // terminate this thread
    tx_thread_terminate(palI2c->WorkingThread);
}

HRESULT InitI2c(uint8_t busIndex, I2cBusSpeed busSpeed, struct NF_PAL_I2C *palI2c)
{
    NANOCLR_HEADER();

    I2CSPM_Init_TypeDef i2cInit = {
        NULL,
        0,
        0,
        0,
        0,
#if defined(_SILICON_LABS_32B_SERIES_0)
        0,
#elif defined(_SILICON_LABS_32B_SERIES_1)
        0,
        0,
#endif
        0,
        I2C_FREQ_STANDARD_MAX,
        i2cClockHLRStandard,
    };

    // config GPIO pins used by the I2C peripheral
    // init the PAL struct for this I2C bus and assign the respective driver
    // all this occurs if not already done
    // why do we need to check if this is already done? because several I2cDevice objects can be created associated to
    // the same bus just using different addresses
    switch (busIndex)
    {

        ////////////////////////////////////
        // Gecko I2C bus index is 0 based //
        ////////////////////////////////////

#if defined(I2C0) && (GECKO_USE_I2C0 == TRUE)
        case 1:
            if (I2C0_PAL.Configuration == NULL)
            {
                I2C0_PAL.Configuration = (I2CSPM_Init_TypeDef *)platform_malloc(sizeof(I2CSPM_Init_TypeDef));

                if (I2C1_PAL.Configuration == NULL)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
                }

                // copy init struct
                memcpy(I2C0_PAL.Configuration, &i2cInit, sizeof(I2CSPM_Init_TypeDef));

                ConfigPins_I2C0();

                I2C0_PAL.Configuration->port = I2C0;
                palI2c = I2C1_PAL;

                // increase device counter
                I2C0_DeviceCounter++;
            }
            break;
#endif

#if defined(I2C1) && (GECKO_USE_I2C1 == TRUE)
        case 2:
            if (I2C1_PAL.Configuration == NULL)
            {
                I2C1_PAL.Configuration = (I2CSPM_Init_TypeDef *)platform_malloc(sizeof(I2CSPM_Init_TypeDef));

                if (I2C1_PAL.Configuration == NULL)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
                }

                // copy init struct
                memcpy(I2C1_PAL.Configuration, &i2cInit, sizeof(I2CSPM_Init_TypeDef));

                ConfigPins_I2C1();

                I2C1_PAL.Configuration->port = I2C1;
                palI2c = I2C1_PAL;

                // increase device counter
                I2C1_DeviceCounter++;
            }
            break;
#endif

#if defined(I2C2) && (GECKO_USE_I2C2 == TRUE)
        case 3:
            if (I2C2_PAL.Configuration == NULL)
            {
                I2C2_PAL.Configuration = (I2CSPM_Init_TypeDef *)platform_malloc(sizeof(I2CSPM_Init_TypeDef));

                if (I2C2_PAL.Configuration == NULL)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
                }

                // copy init struct
                memcpy(I2C2_PAL.Configuration, &i2cInit, sizeof(I2CSPM_Init_TypeDef));

                ConfigPins_I2C2();

                I2C2_PAL.Configuration->port = I2C2;
                palI2c = &I2C2_PAL;

                // increase device counter
                I2C2_DeviceCounter++;
            }

            break;
#endif

        default:
            // this I2C bus is not valid
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            break;
    }

    // Get a general low-level I2C configuration, depending on user's managed parameters
    if (busSpeed == I2cBusSpeed_StandardMode)
    {
        palI2c->Configuration->i2cMaxFreq = I2C_FREQ_STANDARD_MAX;
        palI2c->Configuration->i2cClhr = i2cClockHLRStandard;
    }
    else
    {
        palI2c->Configuration->i2cMaxFreq = I2C_FREQ_FAST_MAX;
        palI2c->Configuration->i2cClhr = i2cClockHLRAsymetric;
    }

    I2CSPM_Init(palI2c->Configuration);

    // compute rough estimate on the time to tx/rx a byte (in milliseconds)
    if (busSpeed == I2cBusSpeed_StandardMode)
    {
        // 100kbit/s: this is roughly 0.10ms per byte, give or take
        palI2c->ByteTime = 0.1;
    }
    else
    {
        // 400kbit/s: this is roughly 0.02ms per byte, give or take
        palI2c->ByteTime = 0.02;
    }

    // clear pointer to working thread
    palI2c->WorkingThread = NULL;
    palI2c->WorkingThreadStack = NULL;

    NANOCLR_NOCLEANUP();
}

HRESULT DisposeI2c(uint8_t busIndex, NF_PAL_I2C *&palI2c)
{
    NANOCLR_HEADER();

    // get the driver for the I2C bus
    // Gecko I2C bus index is 0 based
    switch (busIndex)
    {
        ////////////////////////////////////
        // Gecko I2C bus index is 0 based //
        ////////////////////////////////////

#if defined(I2C0) && (GECKO_USE_I2C0 == TRUE)
        case 1:
            palI2c = &I2C0_PAL;

            // free memory
            platform_free(I2C0_PAL.Configuration);

            // clears configuration
            I2C0_PAL.Configuration = NULL;

            break;
#endif

#if defined(I2C1) && (GECKO_USE_I2C1 == TRUE)
        case 2:
            // free memory
            platform_free(I2C1_PAL.Configuration);

            // clears configuration
            I2C1_PAL.Configuration = NULL;

            break;
#endif

#if defined(I2C2) && (GECKO_USE_I2C2 == TRUE)
        case 3:
            // free memory
            platform_free(I2C2_PAL.Configuration);

            // clears configuration
            I2C2_PAL.Configuration = NULL;

            break;
#endif

        default:
            // the requested I2C bus is not valid
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            break;
    }

    // stop working thread, if it's running
    if (palI2c->WorkingThread != NULL)
    {
        // delete thread
        tx_thread_delete(palI2c->WorkingThread);

        // free stack memory
        platform_free(palI2c->WorkingThreadStack);

        // clear pointers
        palI2c->WorkingThread = NULL;
        palI2c->WorkingThreadStack = NULL;
    }

    NANOCLR_NOCLEANUP();
}

static HRESULT ThrowOnError(CLR_RT_StackFrame &stack, I2C_TransferReturn_TypeDef transactionResult, int bytesTransfered)
{
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Index i2cExceptionTypeDef;
    CLR_RT_HeapBlock *hbObj;

    CLR_RT_HeapBlock &res = stack.m_owningThread->m_currentException;

    // find <I2cException> type definition, don't bother checking the result as it exists for sure
    g_CLR_RT_TypeSystem.FindTypeDef("I2cException", "Com.SkyworksInc.NanoFramework.Devices.I2c", i2cExceptionTypeDef);

    if ((Library_corlib_native_System_Exception::CreateInstance(res, i2cExceptionTypeDef, CLR_E_FAIL, &stack)) == S_OK)
    {
        // dereference the exception object in order to reach its fields
        hbObj = res.Dereference();

        // figure out what was the error and set the status field
        switch (transactionResult)
        {
            case i2cTransferNack:
                hbObj[I2cException::FIELD___status].SetInteger(
                    (CLR_UINT32)I2cTransferStatus_SlaveAddressNotAcknowledged);
                break;

            case i2cTransferBusErr:
            case i2cTransferArbLost:
            case i2cTransferUsageFault:
            case i2cTransferSwFault:
                hbObj[I2cException::FIELD___status].SetInteger((CLR_UINT32)I2cTransferStatus_ClockStretchTimeout);
                break;

            default:
                hbObj[I2cException::FIELD___status].SetInteger((CLR_UINT32)I2cTransferStatus_UnknownError);

                // set the bytes transferred field
                hbObj[I2cException::FIELD___bytesTransferred].SetInteger(bytesTransfered);
        }
    }

    NANOCLR_NOCLEANUP_NOLABEL();
}

static HRESULT NativeTransmit(uint8_t busIndex, NF_PAL_I2C *&palI2c, CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        bool isLongRunningOperation = false;

        CLR_RT_HeapBlock hbTimeout;
        CLR_INT64 *timeout;
        bool eventResult = true;
        uint32_t estimatedDurationMiliseconds;

        CLR_RT_HeapBlock *writeSpanByte;
        CLR_RT_HeapBlock *readSpanByte;
        CLR_RT_HeapBlock_Array *writeBuffer = NULL;
        CLR_RT_HeapBlock_Array *readBuffer = NULL;
        int readOffset = 0;
        int writeOffset = 0;
        I2C_TransferSeq_TypeDef i2cTransfer;
        I2C_TransferReturn_TypeDef transactionResult = i2cTransferInProgress;

        // get the driver for the I2C bus
        switch (busIndex)
        {
            ////////////////////////////////////
            // Gecko I2C bus index is 0 based //
            ////////////////////////////////////

#if defined(I2C0) && (GECKO_USE_I2C0 == TRUE)
            case 1:
                palI2c = &I2C0_PAL;
                break;
#endif

#if defined(I2C1) && (GECKO_USE_I2C1 == TRUE)
            case 2:
                palI2c = &I2C1_PAL;
                break;
#endif

#if defined(I2C2) && (GECKO_USE_I2C2 == TRUE)
            case 3:
                palI2c = &I2C2_PAL;
                break;
#endif

            default:
                // the requested I2C bus is not valid
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                break;
        }

        // dereference the write and read SpanByte from the arguments
        writeSpanByte = stack.Arg3().Dereference();

        if (writeSpanByte != NULL)
        {
            // get buffer
            writeBuffer = writeSpanByte[SpanByte::FIELD___array].DereferenceArray();

            if (writeBuffer != NULL)
            {
                // Get the write offset, only the elements defined by the span must be written, not the whole array
                writeOffset = writeSpanByte[SpanByte::FIELD___start].NumericByRef().s4;

                // use the span length as write size, only the elements defined by the span must be written
                palI2c->WriteSize = writeSpanByte[SpanByte::FIELD___length].NumericByRef().s4;
            }
        }

        if (writeBuffer == NULL)
        {
            // nothing to write, have to zero this
            palI2c->WriteSize = 0;
        }

        readSpanByte = stack.Arg4().Dereference();

        if (readSpanByte != NULL)
        {
            // get buffer
            readBuffer = readSpanByte[SpanByte::FIELD___array].DereferenceArray();

            if (readBuffer != NULL)
            {
                // Get the read offset, only the elements defined by the span must be read, not the whole array
                readOffset = readSpanByte[SpanByte::FIELD___start].NumericByRef().s4;

                // use the span length as read size, only the elements defined by the span must be read
                palI2c->ReadSize = readSpanByte[SpanByte::FIELD___length].NumericByRef().s4;
            }
        }

        if (readBuffer == NULL)
        {
            // nothing to read, have to zero this
            palI2c->ReadSize = 0;
        }

        // check if this is a long running operation
        isLongRunningOperation = IsLongRunningOperation(
            palI2c->WriteSize,
            palI2c->ReadSize,
            palI2c->ByteTime,
            (uint32_t &)estimatedDurationMiliseconds);

        if (isLongRunningOperation)
        {
            // if this is a long running operation, set a timeout equal to the estimated transaction duration in
            // milliseconds this value has to be in ticks to be properly loaded by SetupTimeoutFromTicks() below
            hbTimeout.SetInteger((CLR_INT64)estimatedDurationMiliseconds * TIME_CONVERSION__TO_MILLISECONDS);

            NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));

            // protect the buffers from GC so DMA can find them where they are supposed to be
            CLR_RT_ProtectFromGC gcWriteBuffer(*writeBuffer);
            CLR_RT_ProtectFromGC gcReadBuffer(*readBuffer);
        }

        // this is going to be used to check for the right event in case of simultaneous I2C transaction
        if (!isLongRunningOperation || stack.m_customState == 1)
        {

            // get slave address from connection settings field
            i2cTransfer.addr = (uint16_t)stack.Arg2().NumericByRef().s4 << 1;

            if (writeBuffer != NULL)
            {
                // grab the pointer to the array by starting and the offset specified in the span
                palI2c->WriteBuffer = (uint8_t *)writeBuffer->GetElement(writeOffset);
            }

            if (readBuffer != NULL)
            {
                // grab the pointer to the array by starting and the offset specified in the span
                palI2c->ReadBuffer = (uint8_t *)readBuffer->GetElement(readOffset);
            }
        }

        if (isLongRunningOperation)
        {
            // this is a long running operation and hasn't started yet
            // perform I2C transaction using driver's ASYNC API which is launching a thread to perform it
            if (stack.m_customState == 1)
            {
                // spawn working thread to perform the I2C transaction

                // 1. allocate memory for I2C thread
                palI2c->WorkingThreadStack = (uint32_t *)platform_malloc(I2C_THREAD_STACK_SIZE);

                if (palI2c->WorkingThreadStack == NULL)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
                }

                // 2. create thread
                uint16_t status = tx_thread_create(
                    palI2c->WorkingThread,
#if !defined(BUILD_RTM)
                    (CHAR *)"I2C Thread",
#else
                    NULL,
#endif
                    I2CWorkingThread_entry,
                    (uint32_t)palI2c,
                    palI2c->WorkingThreadStack,
                    I2C_THREAD_STACK_SIZE,
                    I2C_THREAD_PRIORITY,
                    I2C_THREAD_PRIORITY,
                    TX_NO_TIME_SLICE,
                    TX_AUTO_START);

                if (status != TX_SUCCESS)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
                }

                // bump custom state
                stack.m_customState = 2;
            }
        }
        else
        {
            // this is NOT a long running operation
            // perform I2C transaction

            if (palI2c->ReadSize != 0 && palI2c->WriteSize != 0)
            {
                // this is a Write/Read transaction
                i2cTransfer.flags = I2C_FLAG_WRITE_READ;

                i2cTransfer.buf[0].data = palI2c->WriteBuffer;
                i2cTransfer.buf[0].len = palI2c->WriteSize;
                i2cTransfer.buf[1].data = palI2c->ReadBuffer;
                i2cTransfer.buf[1].len = palI2c->ReadSize;

                // Perform the transfer and return status from the transfer
                transactionResult = I2CSPM_Transfer(palI2c->Configuration->port, &i2cTransfer);
            }
            else
            {
                if (palI2c->ReadSize == 0)
                {
                    // this is Write only transaction
                    i2cTransfer.flags = I2C_FLAG_WRITE;

                    i2cTransfer.buf[0].data = palI2c->WriteBuffer;
                    i2cTransfer.buf[0].len = palI2c->WriteSize;

                    // Perform the transfer and return status from the transfer
                    transactionResult = I2CSPM_Transfer(palI2c->Configuration->port, &i2cTransfer);
                }
                else
                {
                    // this is a Read only transaction
                    i2cTransfer.flags = I2C_FLAG_READ;
                    i2cTransfer.buf[0].data = palI2c->ReadBuffer;
                    i2cTransfer.buf[0].len = palI2c->ReadSize;

                    // Perform the transfer and return status from the transfer
                    transactionResult = I2CSPM_Transfer(palI2c->Configuration->port, &i2cTransfer);
                }
            }
        }

        while (eventResult)
        {
            if (!isLongRunningOperation)
            {
                // this is not a long running operation so nothing to do here
                break;
            }

            if (palI2c->WorkingThread->tx_thread_state == TX_TERMINATED)
            {
                // I2C working thread is now complete
                break;
            }

            // non-blocking wait allowing other threads to run while we wait for the I2C transaction to complete
            NANOCLR_CHECK_HRESULT(
                g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_I2cMaster, eventResult));
        }

        if (isLongRunningOperation)
        {
            // pop timeout heap block from stack
            stack.PopValue();
        }

        if (eventResult || !isLongRunningOperation)
        {
            // event occurred
            // OR this is NOT a long running operation

            if (isLongRunningOperation)
            {
                // get transaction result from I2C struct
                transactionResult = palI2c->TransactionResult;

                // delete thread
                tx_thread_delete(palI2c->WorkingThread);

                // free stack memory
                platform_free(palI2c->WorkingThreadStack);

                // clear pointers
                palI2c->WorkingThread = NULL;
                palI2c->WorkingThreadStack = NULL;
            }

            // get the result from the working thread execution
            if (transactionResult != i2cTransferDone)
            {

                NANOCLR_CHECK_HRESULT(ThrowOnError(stack, transactionResult, palI2c->WriteSize + palI2c->ReadSize));
            }
            else
            {
                // successful transaction
                // set the result field
            }
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_com_sky_nf_dev_i2c_native_Com_SkyworksInc_NanoFramework_Devices_I2c_I2cBus::
    NativeTransmit___VOID__I4__U1__SystemSpanByte__SystemSpanByte(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint8_t busIndex;
    I2cBusSpeed busSpeed;
    NF_PAL_I2C *palI2c = NULL;
    CLR_RT_HeapBlock_Array *busSpeedCollection = NULL;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get bus index
    busIndex = (uint8_t)stack.Arg2().NumericByRef().s4;

    // get a pointer to the managed I2C connectionSettings object instance
    busSpeedCollection = (CLR_RT_HeapBlock_Array *)pThis[FIELD_STATIC___busSpeed].DereferenceArray();

    busSpeed = (I2cBusSpeed)*busSpeedCollection->GetElement(busIndex);

    // init I2C bus
    NANOCLR_CHECK_HRESULT(InitI2c(busIndex, busSpeed, palI2c));

    // perform transfer
    // dereference the write and read SpanByte from the arguments
    NANOCLR_CHECK_HRESULT(NativeTransmit(busIndex, palI2c, stack));

    // dispose I2C bus
    NANOCLR_CHECK_HRESULT(DisposeI2c(busIndex, palI2c));

    NANOCLR_NOCLEANUP();
}
