//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_hardware_esp32_rmt_native.h"

typedef Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannel RmtChannel;

static rmt_sync_manager_handle_t s_syncManagerHandle = NULL;

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitSyncManager::
    NativeCreateSyncManager___VOID__SZARRAY_I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array *transmitChannelArray = NULL;
    rmt_sync_manager_config_t config = {};
    esp_err_t err;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Only 1 SyncManager allowed
    if (s_syncManagerHandle != NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    // Get array of TransmitterChannel handle objects
    transmitChannelArray = stack.Arg1().DereferenceArray();
    FAULT_ON_NULL(transmitChannelArray);

    config.array_size = transmitChannelArray->m_numOfElements;

    if (config.array_size == 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    config.tx_channel_array = (rmt_channel_handle_t *)transmitChannelArray->GetFirstElement();

    for (size_t index = 0; index < config.array_size; index++)
    {
        if (!RmtChannel::CheckChannel(config.tx_channel_array[index]))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }
    }

    err = rmt_new_sync_manager(&config, &s_syncManagerHandle);
    if (err != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(RmtChannel::RmtMapEspErrToClrErr(err));
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitSyncManager::
    NativeResetSyncManager___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        if (s_syncManagerHandle == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        esp_err_t err = rmt_sync_reset(s_syncManagerHandle);
        if (err != ESP_OK)
        {
            NANOCLR_SET_AND_LEAVE(RmtChannel::RmtMapEspErrToClrErr(err));
        }
    }
    NANOCLR_NOCLEANUP();
}

void RmtDeleteSyncManager()
{
    if (s_syncManagerHandle)
    {
        // ignore errors, as we are cleaning up
        rmt_del_sync_manager(s_syncManagerHandle);
    }

    s_syncManagerHandle = NULL;
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitSyncManager::
    NativeDisposeSyncManager___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    RmtDeleteSyncManager();

    NANOCLR_NOCLEANUP_NOLABEL();
}
