//
// Copyright (c) 2020 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "net_thread_native.h"

esp_err_t initOpenThread(ThreadDeviceType deviceType, esp_openthread_radio_mode_t radioMode, int port, int baud_rate);
void OpenThreadCliInput(const char *commandLine, bool waitResponse);
HRESULT OpenThreadCliOutput(CLR_RT_HeapBlock &arrayBlock);

void startOpenThreadTask();
bool startStopOpenThread(bool start);
void GetThreadLocalMeshAddress(unsigned char *addr);
void JoinerStart(const char *pskc, const char *url);
bool NF_ESP32_OpenThread_Close();

#define Networking_Thread_OpenThread Library_net_thread_native_nanoFramework_Networking_Thread_OpenThread
#define OpenThreadDataSet            Library_net_thread_native_nanoFramework_Networking_Thread_OpenThreadDataset

//  Load thread dataset from managed object
void LoadDataset(otOperationalDataset &dataset, CLR_RT_HeapBlock *datasetObject)
{
    OpenThreadDataset_componentInUseFlags inUse;
    CLR_RT_HeapBlock_Array *arrayObject;

    inUse =
        (OpenThreadDataset_componentInUseFlags)datasetObject[OpenThreadDataSet::FIELD___inUseFlags].NumericByRef().u4;
    memset(&dataset, 0, sizeof(otOperationalDataset));

    // Active timestamp
    dataset.mActiveTimestamp.mSeconds = 1;
    dataset.mActiveTimestamp.mTicks = 0;
    dataset.mActiveTimestamp.mAuthoritative = false;
    dataset.mComponents.mIsActiveTimestampPresent = true;

    // Channel
    dataset.mChannel = (uint16_t)datasetObject[OpenThreadDataSet::FIELD___channel].NumericByRef().u2;
    dataset.mComponents.mIsChannelPresent = inUse & OpenThreadDataset_componentInUseFlags_channel;

    // Pan ID
    dataset.mPanId = (uint16_t)datasetObject[OpenThreadDataSet::FIELD___panId].NumericByRef().u2;
    dataset.mComponents.mIsPanIdPresent = inUse & OpenThreadDataset_componentInUseFlags_panId;

    // Network Name
    dataset.mComponents.mIsNetworkNamePresent = inUse & OpenThreadDataset_componentInUseFlags_networkName;
    if (dataset.mComponents.mIsNetworkNamePresent)
    {
        arrayObject = datasetObject[OpenThreadDataSet::FIELD___networkName].DereferenceArray();
        assert(arrayObject->m_numOfElements <= OT_NETWORK_NAME_MAX_SIZE);
        memcpy(dataset.mNetworkName.m8, arrayObject->GetElement(0), arrayObject->m_numOfElements);
    }

    // Extended Pan ID
    dataset.mComponents.mIsExtendedPanIdPresent = inUse & OpenThreadDataset_componentInUseFlags_extendedPanId;
    if (dataset.mComponents.mIsExtendedPanIdPresent)
    {
        arrayObject = datasetObject[OpenThreadDataSet::FIELD___extendedPanId].DereferenceArray();
        assert(arrayObject->m_numOfElements == sizeof(dataset.mExtendedPanId.m8));
        memcpy(dataset.mExtendedPanId.m8, arrayObject->GetElement(0), arrayObject->m_numOfElements);
    }

    // Network Key
    dataset.mComponents.mIsNetworkKeyPresent = inUse & OpenThreadDataset_componentInUseFlags_networkKey;
    if (dataset.mComponents.mIsNetworkKeyPresent)
    {
        arrayObject = datasetObject[OpenThreadDataSet::FIELD___networkKey].DereferenceArray();
        assert(arrayObject->m_numOfElements == sizeof(dataset.mNetworkKey.m8));
        memcpy(dataset.mNetworkKey.m8, arrayObject->GetElement(0), arrayObject->m_numOfElements);
    }

    // PSKc - Pre
    dataset.mComponents.mIsPskcPresent = inUse & OpenThreadDataset_componentInUseFlags_pskc;
    if (dataset.mComponents.mIsPskcPresent)
    {
        arrayObject = datasetObject[OpenThreadDataSet::FIELD___pskc].DereferenceArray();
        assert(arrayObject->m_numOfElements == sizeof(dataset.mPskc.m8));
        memcpy(dataset.mPskc.m8, arrayObject->GetElement(0), arrayObject->m_numOfElements);
    }
}

// Store thread dataset to managed object
HRESULT StoreDataset(CLR_RT_StackFrame &stack, otOperationalDataset &dataset, CLR_RT_HeapBlock *datasetObject)
{
    NANOCLR_HEADER();
    {
        int inUse = 0;

        // Channel
        if (dataset.mComponents.mIsChannelPresent)
        {
            datasetObject[OpenThreadDataSet::FIELD___channel].NumericByRef().u2 = dataset.mChannel;
            inUse |= OpenThreadDataset_componentInUseFlags_channel;
        }

        // Pan ID
        if (dataset.mComponents.mIsPanIdPresent)
        {
            datasetObject[OpenThreadDataSet::FIELD___panId].NumericByRef().u2 = dataset.mPanId;
            inUse |= OpenThreadDataset_componentInUseFlags_panId;
        }

        // Network Name
        if (dataset.mComponents.mIsNetworkNamePresent)
        {
            int len = hal_strlen_s(dataset.mNetworkName.m8);

            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(
                datasetObject[OpenThreadDataSet::FIELD___networkName],
                len,
                g_CLR_RT_WellKnownTypes.m_UInt8));
            memcpy(
                datasetObject[OpenThreadDataSet::FIELD___networkName].DereferenceArray()->GetElement(0),
                dataset.mNetworkName.m8,
                len);

            inUse |= OpenThreadDataset_componentInUseFlags_networkName;
        }

        // Extended Pan ID
        if (dataset.mComponents.mIsExtendedPanIdPresent)
        {
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(
                datasetObject[OpenThreadDataSet::FIELD___extendedPanId],
                OT_EXT_PAN_ID_SIZE,
                g_CLR_RT_WellKnownTypes.m_UInt8));
            memcpy(
                datasetObject[OpenThreadDataSet::FIELD___extendedPanId].DereferenceArray()->GetElement(0),
                dataset.mExtendedPanId.m8,
                OT_EXT_PAN_ID_SIZE);

            inUse |= OpenThreadDataset_componentInUseFlags_extendedPanId;
        }

        // Network Key
        if (dataset.mComponents.mIsNetworkKeyPresent)
        {
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(
                datasetObject[OpenThreadDataSet::FIELD___networkKey],
                OT_NETWORK_KEY_SIZE,
                g_CLR_RT_WellKnownTypes.m_UInt8));
            memcpy(
                datasetObject[OpenThreadDataSet::FIELD___networkKey].DereferenceArray()->GetElement(0),
                dataset.mNetworkKey.m8,
                OT_NETWORK_KEY_SIZE);

            inUse |= OpenThreadDataset_componentInUseFlags_networkKey;
        }

        // Save in use flags
        datasetObject[OpenThreadDataSet::FIELD___inUseFlags].NumericByRef().u4 = inUse;
    }
    NANOCLR_NOCLEANUP();
}

//
//  Called form the OpenThread.Start()
//
HRESULT Library_net_thread_native_nanoFramework_Networking_Thread_OpenThread::NativeCreateStack___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        esp_openthread_radio_mode_t radioMode;
        int port;
        int baud_rate;
        ThreadDeviceType deviceType;
        esp_err_t err;
        otError oterr;

        // get a pointer to the managed object instance
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        radioMode =
            (esp_openthread_radio_mode_t)pThis[Networking_Thread_OpenThread::FIELD___radioType].NumericByRef().s4;

        // Get port / bus and validate
        port = pThis[Networking_Thread_OpenThread::FIELD___port].NumericByRef().s4;
        if (radioMode == RADIO_MODE_UART_RCP)
        {
            if (port > UART_NUM_MAX || port < 0)
                NANOCLR_SET_AND_LEAVE(CLR_E_INDEX_OUT_OF_RANGE);
        }
        else if (radioMode == RADIO_MODE_SPI_RCP)
        {
            if (port > MAX_SPI_DEVICES || port < 0)
                NANOCLR_SET_AND_LEAVE(CLR_E_INDEX_OUT_OF_RANGE);
        }

        deviceType = (ThreadDeviceType)pThis[Networking_Thread_OpenThread::FIELD___threadDeviceType].NumericByRef().s4;
        baud_rate = pThis[Networking_Thread_OpenThread::FIELD___speed].NumericByRef().s4;

        err = initOpenThread(deviceType, radioMode, port, baud_rate);
        if (err != ERR_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }

        otInstance *instance = esp_openthread_get_instance();

        // Bring up IPV6 interface
        oterr = otIp6SetEnabled(instance, true);
        if (oterr != OT_ERROR_NONE)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }

        startOpenThreadTask();
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_net_thread_native_nanoFramework_Networking_Thread_OpenThread::NativeSetActiveDataset___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        otError oterr;
        otOperationalDataset dataset;
        otInstance *instance = esp_openthread_get_instance();

        // get a pointer to the managed object instance
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // Fill in active dataset from managed object
        CLR_RT_HeapBlock *datasetRef = pThis[Networking_Thread_OpenThread::FIELD___dataset].Dereference();
        if (datasetRef != NULL)
        {
            LoadDataset(dataset, datasetRef);

            // Set active dataset
            oterr = otDatasetSetActive(instance, &dataset);
            if (oterr != OT_ERROR_NONE)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
            }
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_net_thread_native_nanoFramework_Networking_Thread_OpenThread::NativeGetActiveDataset___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        otOperationalDataset dataset;
        otError oterr;
        otInstance *instance = esp_openthread_get_instance();

        // get a pointer to the managed object instance
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // Save active dataset to managed object
        CLR_RT_HeapBlock *datasetRef = pThis[Networking_Thread_OpenThread::FIELD___dataset].Dereference();
        if (datasetRef != NULL)
        {
            // Save active dataset to managed object
            oterr = otDatasetGetActive(instance, &dataset);
            if (oterr != OT_ERROR_NONE)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_ENTRY_NOT_FOUND);
            }
            StoreDataset(stack, dataset, datasetRef);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_net_thread_native_nanoFramework_Networking_Thread_OpenThread::NativeJoinerStart___VOID__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        const char *pskc = stack.Arg1().DereferenceString()->StringText();
        JoinerStart(pskc, NULL);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_net_thread_native_nanoFramework_Networking_Thread_OpenThread::NativeStartThread___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        if (!startStopOpenThread(true))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_net_thread_native_nanoFramework_Networking_Thread_OpenThread::NativeStopThread___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        if (!startStopOpenThread(false))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_net_thread_native_nanoFramework_Networking_Thread_OpenThread::NativeDispose___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        NF_ESP32_OpenThread_Close();
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_net_thread_native_nanoFramework_Networking_Thread_OpenThread::
    NativeSendConsoleInput___VOID__STRING__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        const char *commandLine = NULL;
        bool waitResponse = false;

        commandLine = stack.Arg1().DereferenceString()->StringText();
        waitResponse = (stack.Arg2().NumericByRef().s4 != 0);

        OpenThreadCliInput(commandLine, waitResponse);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_net_thread_native_nanoFramework_Networking_Thread_OpenThread::NativeGetConsoleOutput___SZARRAY_STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock &retvalue = stack.PushValueAndClear();

        NANOCLR_CHECK_HRESULT(OpenThreadCliOutput(retvalue));
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_net_thread_native_nanoFramework_Networking_Thread_OpenThread::NativeGetMeshLocalAddress___SZARRAY_U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        unsigned char *iparray;

        // create the return array (16 bytes length) for ipaddress
        stack.PushValueAndClear();

        NANOCLR_CHECK_HRESULT(
            CLR_RT_HeapBlock_Array::CreateInstance(stack.TopValue(), 16, g_CLR_RT_WellKnownTypes.m_UInt8));

        // Get address of frist element
        iparray = stack.TopValue().DereferenceArray()->GetFirstElement();

        // Fill in with mesh local address & return
        GetThreadLocalMeshAddress(iparray);
    }
    NANOCLR_NOCLEANUP();
}
