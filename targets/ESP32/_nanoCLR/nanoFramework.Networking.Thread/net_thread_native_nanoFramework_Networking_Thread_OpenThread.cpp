//
// Copyright (c) 2020 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "net_thread_native.h"

esp_err_t initOpenThread(esp_openthread_radio_mode_t radioMode, int port, ThreadDeviceType deviceType );
void startOpenThreadTask();
bool NF_ESP32_OpenThread_Close();
 

#define Networking_Thread_OpenThread Library_net_thread_native_nanoFramework_Networking_Thread_OpenThread
#define OpenThreadDataSet Library_net_thread_native_nanoFramework_Networking_Thread_OpenThreadDataset


//  Load thread dataset from managed object
void LoadDataset(otOperationalDataset & dataset, CLR_RT_HeapBlock *datasetObject)
{
    OpenThreadDataset_componentInUseFlags inUse;
    CLR_RT_HeapBlock_Array *arrayObject;

    inUse = (OpenThreadDataset_componentInUseFlags)datasetObject[OpenThreadDataSet::FIELD___inUseFlags].NumericByRef().u4;
debug_printf("inUse %X\n", inUse);
    memset(&dataset, 0, sizeof(otOperationalDataset));

    // Active timestamp
    dataset.mActiveTimestamp.mSeconds = 1;
    dataset.mActiveTimestamp.mTicks = 0;
    dataset.mActiveTimestamp.mAuthoritative = false;
    dataset.mComponents.mIsActiveTimestampPresent = true;

    // Channel
    dataset.mChannel = (uint16_t)datasetObject[OpenThreadDataSet::FIELD___channel].NumericByRef().u2;
    dataset.mComponents.mIsChannelPresent = inUse & OpenThreadDataset_componentInUseFlags_channel;

debug_printf("Channel present %d chan %d\n", dataset.mComponents.mIsChannelPresent, dataset.mChannel);

    // Pan ID
    dataset.mPanId = (uint16_t)datasetObject[OpenThreadDataSet::FIELD___panId].NumericByRef().u2;
    dataset.mComponents.mIsPanIdPresent = inUse & OpenThreadDataset_componentInUseFlags_panId;

debug_printf("Pan ID present %d chan %d\n", dataset.mComponents.mIsPanIdPresent, dataset.mPanId);

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
debug_printf("NetworkKey array %X sizes %d %d\n", arrayObject, arrayObject->m_numOfElements, sizeof(dataset.mNetworkKey.m8));


        assert(arrayObject->m_numOfElements == sizeof(dataset.mNetworkKey.m8));
        memcpy(dataset.mNetworkKey.m8, arrayObject->GetElement(0), arrayObject->m_numOfElements);

debug_printf("NetworkKey present %d %X:%X:%X:%X:%X:%X:%X:%X\n", dataset.mComponents.mIsNetworkKeyPresent, 
    dataset.mNetworkKey.m8[0],
    dataset.mNetworkKey.m8[1],
    dataset.mNetworkKey.m8[2],
    dataset.mNetworkKey.m8[3],
    dataset.mNetworkKey.m8[4],
    dataset.mNetworkKey.m8[5],
    dataset.mNetworkKey.m8[6],
    dataset.mNetworkKey.m8[7] );

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

//
//  Called form the OpenThread.Start()
//
HRESULT Library_net_thread_native_nanoFramework_Networking_Thread_OpenThread::NativeCreateStack___VOID( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();
    {
        esp_openthread_radio_mode_t radioMode;
        int port;
        ThreadDeviceType deviceType;
        esp_err_t err;
        otError oterr;
        otOperationalDataset dataset;

        // get a pointer to the managed object instance
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        radioMode = (esp_openthread_radio_mode_t)pThis[Networking_Thread_OpenThread::FIELD___radioType].NumericByRef().s4;
        
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

        err = initOpenThread(radioMode, port, deviceType);
        if (err != ERR_OK)
        {
            // FIX ME - add other errors based on err value
        debug_printf("initOpenThread fails %d", err);
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }

        // Fill in active dataset from managed object
        LoadDataset(dataset, pThis[Networking_Thread_OpenThread::FIELD___dataset].Dereference());
        
        otInstance * instance = esp_openthread_get_instance();

        // Set active dataset
        oterr = otDatasetSetActive(instance, &dataset);
        if (oterr !=  OT_ERROR_NONE)
        {
        debug_printf("otDatasetSetActive fails %d", oterr);
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        } 

        // Bring up IPV6 interface
        oterr = otIp6SetEnabled(instance, true);
        if (oterr !=  OT_ERROR_NONE)
        {
        debug_printf("otIp6SetEnabled fails %d", oterr);
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        } 

        // Start Thread operation
        oterr = otThreadSetEnabled(instance, true);
        if (oterr !=  OT_ERROR_NONE)
        {
        debug_printf("otThreadSetEnabled fails %d", oterr);
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        } 

        startOpenThreadTask();
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_net_thread_native_nanoFramework_Networking_Thread_OpenThread::NativeDispose___VOID( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();
    {
        NF_ESP32_OpenThread_Close();
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
