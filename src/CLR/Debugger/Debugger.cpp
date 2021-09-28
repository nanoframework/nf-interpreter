//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Debugging.h>
#include <nanoHAL.h>
#include <WireProtocol.h>
#include <WireProtocol_Message.h>
#include <WireProtocol_MonitorCommands.h>
#include "Debugger.h"
#include <corlib_native.h>
#include <target_common.h>

#define __min(a, b) (((a) < (b)) ? (a) : (b))

//--//

extern const CLR_RT_NativeAssemblyData *g_CLR_InteropAssembliesNativeData[];
extern uint16_t g_CLR_InteropAssembliesCount;

CLR_DBG_Debugger *g_CLR_DBG_Debugger;

BlockStorageDevice *CLR_DBG_Debugger::m_deploymentStorageDevice = NULL;

//--//

void CLR_DBG_Debugger::Debugger_WaitForCommands()
{
    NATIVE_PROFILE_CLR_DEBUGGER();

#if !defined(BUILD_RTM)
    CLR_Debug::Printf("Waiting for debug commands...\r\n");
#endif

    while (!CLR_EE_DBG_IS(RebootPending) && !CLR_EE_DBG_IS(ExitPending))
    {
        g_CLR_RT_ExecutionEngine.DebuggerLoop();
    }
}

void CLR_DBG_Debugger::Debugger_Discovery()
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_INT32 wait_sec = 5;

    uint8_t loopCounter = 1;

    CLR_INT64 expire = HAL_Time_CurrentTime() + (wait_sec * TIME_CONVERSION__TO_SECONDS);

    // Send "presence" ping.
    Monitor_Ping_Command cmd;
    cmd.Source = Monitor_Ping_c_Ping_Source_NanoCLR;

    while (true)
    {
        CLR_EE_DBG_EVENT_BROADCAST(
            CLR_DBG_Commands::c_Monitor_Ping,
            0,
            &cmd,
            WP_Flags_c_NoCaching | WP_Flags_c_NonCritical);

        // if we support soft reboot and the debugger is not stopped then we don't need to connect the debugger
        if (!CLR_EE_DBG_IS(Stopped) && ::CPU_IsSoftRebootSupported())
        {
            break;
        }

        g_CLR_RT_ExecutionEngine.DebuggerLoop();

        if (CLR_EE_DBG_IS(Enabled))
        {
            // Debugger on the other side, let's exit the discovery loop.
            CLR_Debug::Printf("Debugger found. Resuming boot sequence.\r\n");
            break;
        }

        CLR_INT64 now = HAL_Time_CurrentTime();

        if (expire < now)
        {
            // no response after timeout...
            CLR_Debug::Printf("No debugger found...\r\n");
            break;
        }

        // pause for multiples of 250ms so this is not flooding the channel with PING packets
        PLATFORM_DELAY(loopCounter * 250);

        loopCounter++;
    }

    g_CLR_RT_ExecutionEngine.WaitForDebugger();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CLR_DBG_Debugger::CreateInstance()
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    NANOCLR_HEADER();

    // alloc memory for debugger
    g_CLR_DBG_Debugger = (CLR_DBG_Debugger *)platform_malloc(sizeof(CLR_DBG_Debugger));

    // sanity check...
    FAULT_ON_NULL(g_CLR_DBG_Debugger);

    //... and clear memory
    memset(g_CLR_DBG_Debugger, 0, sizeof(CLR_DBG_Debugger));

    // alloc memory for debugger messaging
    g_CLR_DBG_Debugger->m_messaging = (CLR_Messaging *)platform_malloc(sizeof(CLR_Messaging));

    // sanity check...
    FAULT_ON_NULL(g_CLR_DBG_Debugger->m_messaging);

    //... and clear memory
    memset(g_CLR_DBG_Debugger->m_messaging, 0, sizeof(CLR_Messaging));

    NANOCLR_CHECK_HRESULT(g_CLR_DBG_Debugger->Debugger_Initialize(HalSystemConfig.DebuggerPort));

    BlockStorageStream stream;
    memset(&stream, 0, sizeof(BlockStorageStream));

    if (BlockStorageStream_Initialize(&stream, BlockUsage_DEPLOYMENT))
    {
        m_deploymentStorageDevice = stream.Device;
    }
    else
    {
        m_deploymentStorageDevice = NULL;
    }

    // UNDONE: FIXME: MFUpdate_Initialize();

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_DBG_Debugger::Debugger_Initialize(COM_HANDLE port)
{
    (void)port;

    NATIVE_PROFILE_CLR_DEBUGGER();
    NANOCLR_HEADER();

    m_messaging->Initialize(
        c_Debugger_Lookup_Request,
        c_Debugger_Lookup_Request_count,
        c_Debugger_Lookup_Reply,
        c_Debugger_Lookup_Reply_count);

    NANOCLR_NOCLEANUP_NOLABEL();
}

//--//

HRESULT CLR_DBG_Debugger::DeleteInstance()
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    NANOCLR_HEADER();

    g_CLR_DBG_Debugger->Debugger_Cleanup();

    // free messaging
    platform_free(g_CLR_DBG_Debugger->m_messaging);

    // free debugger
    platform_free(g_CLR_DBG_Debugger);

    g_CLR_DBG_Debugger = NULL;

    NANOCLR_NOCLEANUP_NOLABEL();
}

void CLR_DBG_Debugger::Debugger_Cleanup()
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    m_messaging->Cleanup();
}

//--//

// void CLR_DBG_Debugger::ProcessCommands()
// {
//     NATIVE_PROFILE_CLR_DEBUGGER();
//     m_messaging->m_controller.AdvanceState();
// }

void CLR_DBG_Debugger::BroadcastEvent(
    unsigned int cmd,
    unsigned int payloadSize,
    unsigned char *payload,
    unsigned int flags)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    WP_PrepareAndSendProtocolMessage(cmd, payloadSize, payload, flags);
}

//--//

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

#if defined(NANOCLR_APPDOMAINS)

CLR_RT_AppDomain *CLR_DBG_Debugger::GetAppDomainFromID(CLR_UINT32 id)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    NANOCLR_FOREACH_NODE(CLR_RT_AppDomain, appDomain, g_CLR_RT_ExecutionEngine.m_appDomains)
    {
        if (appDomain->m_id == id)
            return appDomain;
    }
    NANOCLR_FOREACH_NODE_END();

    return NULL;
}
#endif // NANOCLR_APPDOMAINS

CLR_RT_Thread *CLR_DBG_Debugger::GetThreadFromPid(CLR_INT32 pid)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    NANOCLR_FOREACH_NODE(CLR_RT_Thread, th, g_CLR_RT_ExecutionEngine.m_threadsReady)
    {
        if (th->m_pid == pid)
            return th;
    }
    NANOCLR_FOREACH_NODE_END();

    NANOCLR_FOREACH_NODE(CLR_RT_Thread, th, g_CLR_RT_ExecutionEngine.m_threadsWaiting)
    {
        if (th->m_pid == pid)
            return th;
    }
    NANOCLR_FOREACH_NODE_END();

    return NULL;
}

HRESULT CLR_DBG_Debugger::CreateListOfThreads(CLR_DBG_Commands::Debugging_Thread_List::Reply *&cmdReply, int &totLen)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    NANOCLR_HEADER();

    CLR_UINT32 *pidDst;
    int num;

    num = g_CLR_RT_ExecutionEngine.m_threadsReady.NumOfNodes() + g_CLR_RT_ExecutionEngine.m_threadsWaiting.NumOfNodes();

    totLen = sizeof(*cmdReply) + (num - 1) * sizeof(CLR_UINT32);

    cmdReply = (CLR_DBG_Commands::Debugging_Thread_List::Reply *)CLR_RT_Memory::Allocate_And_Erase(totLen, true);
    CHECK_ALLOCATION(cmdReply);

    cmdReply->m_num = num;

    pidDst = cmdReply->m_pids;

    NANOCLR_FOREACH_NODE(CLR_RT_Thread, thSrc, g_CLR_RT_ExecutionEngine.m_threadsReady)
    {
        *pidDst++ = thSrc->m_pid;
    }
    NANOCLR_FOREACH_NODE_END();

    NANOCLR_FOREACH_NODE(CLR_RT_Thread, thSrc, g_CLR_RT_ExecutionEngine.m_threadsWaiting)
    {
        *pidDst++ = thSrc->m_pid;
    }
    NANOCLR_FOREACH_NODE_END();

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_DBG_Debugger::CreateListOfCalls(
    CLR_INT32 pid,
    CLR_DBG_Commands::Debugging_Thread_Stack::Reply *&cmdReply,
    int &totLen)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    NANOCLR_HEADER();

    CLR_RT_Thread *th = GetThreadFromPid(pid);
    FAULT_ON_NULL(th);

    for (int pass = 0; pass < 2; pass++)
    {
        int num = 0;

        NANOCLR_FOREACH_NODE(CLR_RT_StackFrame, call, th->m_stackFrames)
        {
            if (pass == 1)
            {
                int tmp = num;

#ifndef CLR_NO_IL_INLINE
                if (call->m_inlineFrame)
                {
                    CLR_DBG_Commands::Debugging_Thread_Stack::Reply::Call &dst = cmdReply->m_data[tmp++];

                    dst.m_md = call->m_inlineFrame->m_frame.m_call;
                    dst.m_IP = (CLR_UINT32)(call->m_inlineFrame->m_frame.m_IP - call->m_inlineFrame->m_frame.m_IPStart);
#if defined(NANOCLR_APPDOMAINS)
                    dst.m_appDomainID = call->m_appDomain->m_id;
                    dst.m_flags = call->m_flags;
#endif
                }
#endif
                CLR_DBG_Commands::Debugging_Thread_Stack::Reply::Call &dst = cmdReply->m_data[tmp];

                dst.m_md = call->m_call;
                dst.m_IP = (CLR_UINT32)(call->m_IP - call->m_IPstart);

                if (dst.m_IP && call != th->CurrentFrame())
                {
                    // With the exception of when the IP is 0, for a breakpoint on Push,
                    // The call->m_IP is the next instruction to execute, not the currently executing one.
                    // For non-leaf frames, this will return the IP within the call.
                    dst.m_IP--;
                }

#if defined(NANOCLR_APPDOMAINS)
                dst.m_appDomainID = call->m_appDomain->m_id;
                dst.m_flags = call->m_flags;
#endif
            }

#ifndef CLR_NO_IL_INLINE
            if (call->m_inlineFrame)
            {
                num++;
            }
#endif

            num++;
        }
        NANOCLR_FOREACH_NODE_END();

        if (pass == 0)
        {
            totLen = sizeof(*cmdReply) + (num - 1) * sizeof(CLR_DBG_Commands::Debugging_Thread_Stack::Reply::Call);

            cmdReply = (CLR_DBG_Commands::Debugging_Thread_Stack::Reply *)CLR_RT_Memory::Allocate_And_Erase(
                totLen,
                CLR_RT_HeapBlock::HB_CompactOnFailure);
            CHECK_ALLOCATION(cmdReply);

            cmdReply->m_num = num;
            cmdReply->m_status = th->m_status;
            cmdReply->m_flags = th->m_flags;
        }
    }

    NANOCLR_NOCLEANUP();
}

#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

////////////////////////////////////////////////////////////////////////////////////////////////////

// developer note:
// all Monitor_nnnnnnn, Debugger_nnnnn and Profiller_nnnn handlers have to execute the respective reply
// if the handler fails to execute, it will return false and then the caller will reply with a failed reply

bool CLR_DBG_Debugger::Monitor_Ping(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    bool fStopOnBoot = true;

    //
    // There's someone on the other side!!
    //
    CLR_EE_DBG_SET(Enabled);

    if ((msg->m_header.m_flags & WP_Flags_c_Reply) == 0)
    {
        Monitor_Ping_Reply cmdReply;
        Monitor_Ping_Command *cmd = (Monitor_Ping_Command *)msg->m_payload;

        // default is to stop the debugger (backwards compatibility)
        fStopOnBoot = (cmd != NULL) && (cmd->Flags & Monitor_Ping_c_Ping_DbgFlag_Stop);

        cmdReply.Source = Monitor_Ping_c_Ping_Source_NanoCLR;

        // now fill in the flags
        cmdReply.Flags = CLR_EE_DBG_IS(StateProgramExited) != 0 ? Monitor_Ping_c_Ping_DbgFlag_AppExit : 0;

#if defined(WP_IMPLEMENTS_CRC32)
        cmdReply.Flags |= Monitor_Ping_c_Ping_WPFlag_SupportsCRC32;
#endif

        // Wire Protocol packet size
#if (WP_PACKET_SIZE == 512)
        cmdReply.Flags |= Monitor_Ping_c_PacketSize_0512;
#elif (WP_PACKET_SIZE == 256)
        cmdReply.Flags |= Monitor_Ping_c_PacketSize_0256;
#elif (WP_PACKET_SIZE == 128)
        cmdReply.Flags |= Monitor_Ping_c_PacketSize_0128;
#elif (WP_PACKET_SIZE == 1024)
        cmdReply.Flags |= Monitor_Ping_c_PacketSize_1024;
#endif

        // capability flags
        if (::Target_HasNanoBooter())
        {
            cmdReply.Flags |= Monitor_Ping_c_HasNanoBooter;
        }

        if (::Target_HasProprietaryBooter())
        {
            cmdReply.Flags |= Monitor_Ping_c_HasProprietaryBooter;
        }

        if (::Target_IFUCapable())
        {
            cmdReply.Flags |= Monitor_Ping_c_IFUCapable;
        }

        if (::Target_ConfigUpdateRequiresErase())
        {
            cmdReply.Flags |= Monitor_Ping_c_ConfigBlockRequiresErase;
        }

        // done, send reply
        WP_ReplyToCommand(msg, true, false, &cmdReply, sizeof(cmdReply));
    }
    else
    {
        Monitor_Ping_Reply *pingReply = (Monitor_Ping_Reply *)msg->m_payload;

        // default is to stop the debugger (backwards compatibility)
        fStopOnBoot = (pingReply != NULL) && (pingReply->Flags & Monitor_Ping_c_Ping_DbgFlag_Stop);

        // done, send reply
        WP_ReplyToCommand(msg, true, false, NULL, 0);
    }

    if (CLR_EE_DBG_IS_MASK(StateInitialize, StateMask))
    {
        if (fStopOnBoot)
        {
            CLR_EE_DBG_SET(Stopped);
        }
        else
        {
            CLR_EE_DBG_CLR(Stopped);
        }
    }

    return true;
}

bool CLR_DBG_Debugger::Monitor_FlashSectorMap(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    BlockStorageDevice *storageDevices = NULL;
    DeviceBlockInfo *devicesBlockInfos = NULL;
    Flash_BlockRegionInfo *pData = NULL;
    bool success = false;
    unsigned int rangeCount = 0;
    unsigned int rangeIndex = 0;
    uint32_t size = 0;

    if ((msg->m_header.m_flags & WP_Flags_c_Reply) == 0)
    {
        // get the number of available block storage devices
        unsigned int numDevices = BlockStorageList_GetNumDevices();

        // get an array of pointer to all the storage devices in the list and then request the device info
        storageDevices = (BlockStorageDevice *)platform_malloc(numDevices * sizeof(BlockStorageDevice));
        if (storageDevices == NULL)
        {
            // allocation failed
            goto cmd_executed;
        }

        devicesBlockInfos = (DeviceBlockInfo *)platform_malloc(numDevices * sizeof(DeviceBlockInfo));
        if (devicesBlockInfos == NULL)
        {
            // allocation failed
            goto cmd_executed;
        }

        // clear memory
        memset(storageDevices, 0, numDevices * sizeof(BlockStorageDevice));
        memset(devicesBlockInfos, 0, numDevices * sizeof(DeviceBlockInfo));

        for (unsigned int i = 0; i < numDevices; i++)
        {
            if (i == 0)
            {
                storageDevices[i] = *BlockStorageList_GetFirstDevice();
            }
            else
            {
                storageDevices[i] = *BlockStorageList_GetNextDevice(&storageDevices[i - 1]);
            }

            // sanity check
            if (&storageDevices[i] == NULL)
            {
                // failed
                goto cmd_executed;
            }

            devicesBlockInfos[i] = *BlockStorageDevice_GetDeviceInfo(&storageDevices[i]);
        }

        for (int cnt = 0; cnt < 2; cnt++)
        {
            if (cnt == 1)
            {
                size = rangeCount * sizeof(struct Flash_BlockRegionInfo);

                pData = (Flash_BlockRegionInfo *)platform_malloc(size);

                if (pData == NULL)
                {
                    goto cmd_executed;
                }

                // clear memory
                memset(pData, 0, size);
            }

            for (unsigned int i = 0; i < numDevices; i++)
            {
                for (unsigned int j = 0; j < devicesBlockInfos[i].NumRegions; j++)
                {
                    const BlockRegionInfo *pRegion = &devicesBlockInfos[i].Regions[j];

                    for (unsigned int k = 0; k < pRegion->NumBlockRanges; k++)
                    {

                        if (cnt == 0)
                        {
                            rangeCount++;
                        }
                        else
                        {
                            pData[rangeIndex].StartAddress =
                                BlockRegionInfo_BlockAddress(pRegion, pRegion->BlockRanges[k].StartBlock);
                            pData[rangeIndex].NumBlocks = BlockRange_GetBlockCount(pRegion->BlockRanges[k]);
                            pData[rangeIndex].BytesPerBlock = pRegion->BytesPerBlock;
                            pData[rangeIndex].Flags = pRegion->BlockRanges[k].RangeType & BlockRange_USAGE_MASK;
                            // add the media attributes to the flags
                            pData[rangeIndex].Flags |= pRegion->Attributes & BlockRegionAttributes_MASK;

                            rangeIndex++;
                        }
                    }
                }
            }

            // done here
            success = true;
        }
    }

cmd_executed:
    WP_ReplyToCommand(msg, success, false, (void *)pData, size);

    // free memory, when needed
    if (pData)
    {
        platform_free(pData);
    }

    if (storageDevices)
    {
        platform_free(storageDevices);
    }

    if (devicesBlockInfos)
    {
        platform_free(devicesBlockInfos);
    }

    return true;
}

bool CLR_DBG_Debugger::Monitor_TargetInfo(WP_Message *msg)
{
    Monitor_TargetInfo_Reply cmdReply;

    bool fOK = nanoBooter_GetTargetInfo(&cmdReply.m_TargetInfo) == true;

    WP_ReplyToCommand(msg, fOK, false, &cmdReply, sizeof(Monitor_TargetInfo_Reply));

    return true;
}

//--//

bool CLR_DBG_Debugger::CheckPermission(ByteAddress address, int mode)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    bool hasPermission = false;
    unsigned int regionIndex, rangeIndex;

    BlockStorageDevice_FindRegionFromAddress(m_deploymentStorageDevice, address, &regionIndex, &rangeIndex);
    const BlockRange &range =
        BlockStorageDevice_GetDeviceInfo(m_deploymentStorageDevice)->Regions[regionIndex].BlockRanges[rangeIndex];

    switch (mode)
    {
        case AccessMemory_Check:
            hasPermission = true;
            break;
        case AccessMemory_Read:
#if defined(BUILD_RTM)
            if (!DebuggerPort_IsUsingSsl(HalSystemConfig.DebuggerPort))
                break;
#endif
            switch (range.RangeType)
            {
                // fall through
                case BlockRange_BLOCKTYPE_CONFIG:
                case BlockRange_BLOCKTYPE_BOOTSTRAP:
                case BlockRange_BLOCKTYPE_DIRTYBIT:
                case BlockRange_BLOCKTYPE_FILESYSTEM:
                case BlockRange_BLOCKTYPE_STORAGE_A:
                case BlockRange_BLOCKTYPE_STORAGE_B:
                case BlockRange_BLOCKTYPE_SIMPLE_A:
                case BlockRange_BLOCKTYPE_SIMPLE_B:
                case BlockRange_BLOCKTYPE_UPDATE:

                    hasPermission = true;
                    break;
            }
            break;
        case AccessMemory_Write:
#if defined(BUILD_RTM)
            if (!DebuggerPort_IsUsingSsl(HalSystemConfig.DebuggerPort))
                break;
#endif
            if (BlockRange_IsDeployment(range) || BlockRange_IsConfig(range))
            {
                hasPermission = true;
            }
            else
            {
                hasPermission = DebuggerPort_IsUsingSsl(HalSystemConfig.DebuggerPort) == true;
            }
            break;
        case AccessMemory_Erase:
#if defined(BUILD_RTM)
            if (!DebuggerPort_IsUsingSsl(HalSystemConfig.DebuggerPort))
                break;
#endif
            switch (range.RangeType)
            {
                case BlockRange_BLOCKTYPE_DEPLOYMENT:
                case BlockRange_BLOCKTYPE_FILESYSTEM:
                case BlockRange_BLOCKTYPE_STORAGE_A:
                case BlockRange_BLOCKTYPE_STORAGE_B:
                case BlockRange_BLOCKTYPE_SIMPLE_A:
                case BlockRange_BLOCKTYPE_SIMPLE_B:
                case BlockRange_BLOCKTYPE_UPDATE:
                case BlockRange_BLOCKTYPE_CONFIG:
                    hasPermission = true;
                    break;
            }
            break;
        default:
            hasPermission = false;
            break;
    }

    return hasPermission;
}

void CLR_DBG_Debugger::AccessMemory(
    uint32_t location,
    uint32_t lengthInBytes,
    uint8_t *buf,
    uint32_t mode,
    uint32_t *errorCode)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    bool proceed = false;

    // reset error code
    *errorCode = AccessMemoryErrorCode_RequestedOperationFailed;

    //--//
    unsigned int iRegion, iRange;

    if (BlockStorageDevice_FindRegionFromAddress(m_deploymentStorageDevice, location, &iRegion, &iRange))
    {
        const DeviceBlockInfo *deviceInfo = BlockStorageDevice_GetDeviceInfo(m_deploymentStorageDevice);

        // start from the block where the sector sits.
        ByteAddress accessAddress = location;

        uint8_t *bufPtr = buf;
        signed int accessLenInBytes = lengthInBytes;
        signed int blockOffset =
            BlockRegionInfo_OffsetFromBlock(((BlockRegionInfo *)(&deviceInfo->Regions[iRegion])), accessAddress);

        for (; iRegion < deviceInfo->NumRegions; iRegion++)
        {
            const BlockRegionInfo *pRegion = &deviceInfo->Regions[iRegion];

            unsigned int blockIndex = BlockRegionInfo_BlockIndexFromAddress(pRegion, accessAddress);
            unsigned int accessMaxLength = pRegion->BytesPerBlock - blockOffset;

            blockOffset = 0;

            for (; blockIndex < pRegion->NumBlocks; blockIndex++)
            {
                // accessMaxLength =the current largest number of bytes can be read from the block from the address to
                // its block boundary.
                unsigned int NumOfBytes = __min(accessMaxLength, (unsigned int)accessLenInBytes);

                accessMaxLength = pRegion->BytesPerBlock;

                if (blockIndex > pRegion->BlockRanges[iRange].EndBlock)
                {
                    iRange++;

                    if (iRange >= pRegion->NumBlockRanges)
                    {
                        ASSERT(false);
                        break;
                    }
                }

                // since AccessMemory_Check is always true and will not break from here, no need to check
                // AccessMemory_Check to free memory.
                if (!CheckPermission(accessAddress, mode))
                {
                    // Permission check failed

                    // set error code
                    *errorCode = AccessMemoryErrorCode_PermissionDenied;

                    // done here
                    return;
                }

                switch (mode)
                {
                    case AccessMemory_Check:
                    case AccessMemory_Read:
                        if (deviceInfo->Attribute & MediaAttribute_SupportsXIP)
                        {
                            // memory block support XIP, OK to read directly from address
                            if (mode == AccessMemory_Check)
                            {
                                // compute CRC32 of the memory segment
                                *(CLR_DBG_Commands_Monitor_CheckMemory_Reply *)buf =
                                    SUPPORT_ComputeCRC((const void *)accessAddress, NumOfBytes, 0);
                            }
                            else
                            {
                                // copy memory segment to buffer
                                memcpy((unsigned char *)bufPtr, (const void *)accessAddress, NumOfBytes);
                            }

                            // done here
                            proceed = true;
                        }
                        else
                        {
                            // need to use driver to access storage block
                            if (mode == AccessMemory_Check)
                            {
                                bufPtr = (unsigned char *)CLR_RT_Memory::Allocate(lengthInBytes, true);

                                if (!bufPtr)
                                {
                                    // Failed to allocate data buffer

                                    // set error code
                                    *errorCode = AccessMemoryErrorCode_FailedToAllocateReadBuffer;

                                    // done here
                                    return;
                                }
                            }

                            proceed = BlockStorageDevice_Read(
                                m_deploymentStorageDevice,
                                accessAddress,
                                NumOfBytes,
                                (unsigned char *)bufPtr);

                            if (mode == AccessMemory_Check)
                            {
                                // compute CRC32 of the memory segment
                                *(CLR_DBG_Commands_Monitor_CheckMemory_Reply *)buf =
                                    SUPPORT_ComputeCRC(bufPtr, NumOfBytes, 0);

                                CLR_RT_Memory::Release(bufPtr);
                            }
                        }
                        break;

                    case AccessMemory_Write:
                        proceed = BlockStorageDevice_Write(
                            m_deploymentStorageDevice,
                            accessAddress,
                            NumOfBytes,
                            (unsigned char *)bufPtr,
                            false);
                        break;

                    case AccessMemory_Erase:
                        if (BlockStorageDevice_IsBlockErased(m_deploymentStorageDevice, accessAddress, NumOfBytes))
                        {
                            // block is already erased, we are good
                            proceed = true;
                        }
                        else
                        {
                            // need to erase block
                            proceed = BlockStorageDevice_EraseBlock(m_deploymentStorageDevice, accessAddress);
                        }
                        break;

                    default:
                        break;
                }

                if (!proceed)
                {
                    return;
                }

                accessLenInBytes -= NumOfBytes;

                if (accessLenInBytes <= 0)
                {
                    break;
                }

                bufPtr += NumOfBytes;
                accessAddress += NumOfBytes;
            }

            blockIndex = 0;
            iRange = 0;

            if ((accessLenInBytes <= 0) || (!proceed))
            {
                break;
            }
        }
    }
    else
    {
        //--// RAM write
        ByteAddress sectAddr = location;

#if defined(_WIN32)

        bool proceed = false;
        void *temp;
        temp = (void *)sectAddr;

        switch (mode)
        {
            case AccessMemory_Check:
                break;

            case AccessMemory_Read:
                proceed = IsBadReadPtr(temp, lengthInBytes) == false;
                break;

            case AccessMemory_Write:
                proceed = IsBadWritePtr(temp, lengthInBytes) == false;
                break;
        }

        if (proceed)
#else

        unsigned int sectAddrEnd = sectAddr + lengthInBytes;
        unsigned int ramStartAddress = HalSystemConfig.RAM1.Base;
        unsigned int ramEndAddress = ramStartAddress + HalSystemConfig.RAM1.Size;

        if ((sectAddr < ramStartAddress) || (sectAddr >= ramEndAddress) || (sectAddrEnd > ramEndAddress))
        {
            // Invalid address and range
            return;
        }
        else
#endif
        {
            switch (mode)
            {
                case AccessMemory_Check:
                    break;

                case AccessMemory_Read:
                    memcpy(buf, (const void *)sectAddr, lengthInBytes);
                    break;

                case AccessMemory_Write:
                    unsigned char *memPtr;
                    memPtr = (unsigned char *)sectAddr;
                    memcpy(memPtr, buf, lengthInBytes);
                    break;

                case AccessMemory_Erase:
                    memPtr = (unsigned char *)sectAddr;
                    if (lengthInBytes != 0)
                        memset(memPtr, 0xFF, lengthInBytes);
                    break;

                default:
                    break;
            }
        }
    }

    *errorCode = AccessMemoryErrorCode_NoError;
}

bool CLR_DBG_Debugger::Monitor_ReadMemory(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands_Monitor_ReadMemory *cmd = (CLR_DBG_Commands_Monitor_ReadMemory *)msg->m_payload;
    uint8_t *buffer;
    uint32_t errorCode;

    uint32_t allocationSize = 0;
    uint32_t len = cmd->length;

    // adjust length, if bigger than the WP packet size
    if (len > WP_PACKET_SIZE)
    {
        len = WP_PACKET_SIZE;
    }

    // start computing allocation size, fist the ErrorCode field...
    allocationSize = sizeof(uint32_t);
    // ... and the buffer
    allocationSize += len;

    // allocate memory
    buffer = (uint8_t *)platform_malloc(allocationSize);

    // sanity check
    if (buffer != NULL)
    {
        // clear allocated memory
        memset(buffer, 0, allocationSize);

        g_CLR_DBG_Debugger->AccessMemory(cmd->address, len, (buffer + sizeof(uint32_t)), AccessMemory_Read, &errorCode);

        WP_ReplyToCommand(msg, true, false, buffer, allocationSize);

        // free allocated memory
        platform_free(buffer);

        // done here
        return true;
    }

    return false;
}

bool CLR_DBG_Debugger::Monitor_WriteMemory(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands_Monitor_WriteMemory *cmd = (CLR_DBG_Commands_Monitor_WriteMemory *)msg->m_payload;
    CLR_DBG_Commands_Monitor_WriteMemory_Reply cmdReply;

    g_CLR_DBG_Debugger->AccessMemory(cmd->address, cmd->length, cmd->data, AccessMemory_Write, &cmdReply);

    WP_ReplyToCommand(msg, true, false, &cmdReply, sizeof(cmdReply));

    return true;
}

bool CLR_DBG_Debugger::Monitor_CheckMemory(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands_Monitor_CheckMemory *cmd = (CLR_DBG_Commands_Monitor_CheckMemory *)msg->m_payload;
    CLR_DBG_Commands_Monitor_CheckMemory_Reply cmdReply;
    uint32_t errorCode;

    g_CLR_DBG_Debugger
        ->AccessMemory(cmd->address, cmd->length, (unsigned char *)&cmdReply, AccessMemory_Check, &errorCode);

    WP_ReplyToCommand(msg, errorCode == AccessMemoryErrorCode_NoError, false, &cmdReply, sizeof(cmdReply));

    return true;
}

bool CLR_DBG_Debugger::Monitor_EraseMemory(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands_Monitor_EraseMemory *cmd = (CLR_DBG_Commands_Monitor_EraseMemory *)msg->m_payload;
    CLR_DBG_Commands_Monitor_EraseMemory_Reply cmdReply;

    g_CLR_DBG_Debugger->AccessMemory(cmd->address, cmd->length, NULL, AccessMemory_Erase, &cmdReply);

    WP_ReplyToCommand(msg, true, false, &cmdReply, sizeof(cmdReply));

    return true;
}

bool CLR_DBG_Debugger::Monitor_Execute(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Monitor_Execute *cmd = (CLR_DBG_Commands::Monitor_Execute *)msg->m_payload;

#if defined(BUILD_RTM)
    if (!DebuggerPort_IsUsingSsl(HalSystemConfig.DebuggerPort))
        return false;
#endif

    WP_ReplyToCommand(msg, true, false, NULL, 0);

    ((void (*)())(size_t)cmd->m_address)();

    return true;
}

bool CLR_DBG_Debugger::Monitor_Reboot(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    bool success = true;

    CLR_DBG_Commands::Monitor_Reboot *cmd = (CLR_DBG_Commands::Monitor_Reboot *)msg->m_payload;

    if (CLR_DBG_Commands::Monitor_Reboot::c_EnterNanoBooter ==
        (cmd->m_flags & CLR_DBG_Commands::Monitor_Reboot::c_EnterNanoBooter))
    {
        success = RequestToLaunchNanoBooter();
    }
    else if (
        CLR_DBG_Commands::Monitor_Reboot::c_EnterProprietaryBooter ==
        (cmd->m_flags & CLR_DBG_Commands::Monitor_Reboot::c_EnterProprietaryBooter))
    {
        success = RequestToLaunchProprietaryBootloader();
    }

    WP_ReplyToCommand(msg, success, false, NULL, 0);

    // on success, apply reboot options and set reboot pending flag
    if (success)
    {
        g_CLR_RT_ExecutionEngine.m_iReboot_Options = cmd->m_flags;

        CLR_EE_DBG_SET(RebootPending);
    }

    return true;
}

bool CLR_DBG_Debugger::Monitor_MemoryMap(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    MemoryMap_Range map[2];

    map[0].m_address = HalSystemConfig.RAM1.Base;
    map[0].m_length = HalSystemConfig.RAM1.Size;
    map[0].m_flags = Monitor_MemoryMap_c_RAM;

    map[1].m_address = HalSystemConfig.FLASH1.Base;
    map[1].m_length = HalSystemConfig.FLASH1.Size;
    map[1].m_flags = Monitor_MemoryMap_c_FLASH;

    WP_ReplyToCommand(msg, true, false, map, sizeof(map));

    return true;
}

bool CLR_DBG_Debugger::Monitor_DeploymentMap(WP_Message *msg)
{
    (void)msg;

    WP_ReplyToCommand(msg, true, false, NULL, 0);

    return true;
}

bool CLR_DBG_Debugger::Monitor_QueryConfiguration(WP_Message *message)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    // include handling of configuration block only if feature is available
#if (HAS_CONFIG_BLOCK == TRUE)

    Monitor_QueryConfiguration_Command *cmd = (Monitor_QueryConfiguration_Command *)message->m_payload;
    int size = 0;
    int sizeOfBlock = 0;

    HAL_Configuration_NetworkInterface *configNetworkInterface;
    HAL_Configuration_Wireless80211 *configWireless80211NetworkInterface;
    HAL_Configuration_X509CaRootBundle *x509Certificate;
    HAL_Configuration_X509DeviceCertificate *x509DeviceCertificate;

    switch ((DeviceConfigurationOption)cmd->Configuration)
    {
        case DeviceConfigurationOption_Network:

            configNetworkInterface =
                (HAL_Configuration_NetworkInterface *)platform_malloc(sizeof(HAL_Configuration_NetworkInterface));

            // check allocation
            if (configNetworkInterface != NULL)
            {

                memset(configNetworkInterface, 0, sizeof(HAL_Configuration_NetworkInterface));

                if (ConfigurationManager_GetConfigurationBlock(
                        configNetworkInterface,
                        (DeviceConfigurationOption)cmd->Configuration,
                        cmd->BlockIndex) == true)
                {
                    size = sizeof(HAL_Configuration_NetworkInterface);

                    WP_ReplyToCommand(message, true, false, (uint8_t *)configNetworkInterface, size);
                }

                platform_free(configNetworkInterface);

                // done here
                return true;
            }
            break;

        case DeviceConfigurationOption_Wireless80211Network:

            configWireless80211NetworkInterface =
                (HAL_Configuration_Wireless80211 *)platform_malloc(sizeof(HAL_Configuration_Wireless80211));

            // check allocation
            if (configWireless80211NetworkInterface != NULL)
            {
                memset(configWireless80211NetworkInterface, 0, sizeof(HAL_Configuration_Wireless80211));

                if (ConfigurationManager_GetConfigurationBlock(
                        configWireless80211NetworkInterface,
                        (DeviceConfigurationOption)cmd->Configuration,
                        cmd->BlockIndex) == true)
                {
                    size = sizeof(HAL_Configuration_Wireless80211);

                    WP_ReplyToCommand(message, true, false, (uint8_t *)configWireless80211NetworkInterface, size);
                }

                platform_free(configWireless80211NetworkInterface);

                // done here
                return true;
            }
            break;

        case DeviceConfigurationOption_X509CaRootBundle:

            // check if cert store is empty OR request index doesn't exist
            // (mind the zero based index)
            if (g_TargetConfiguration.CertificateStore->Count == 0 ||
                g_TargetConfiguration.CertificateStore->Count < cmd->BlockIndex + 1)
            {
                // we are done here, just send an empty reply
                return true;
            }
            else
            {
                // because X509 certificate has a variable length need to compute the block size in two steps
                sizeOfBlock = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
                sizeOfBlock += g_TargetConfiguration.CertificateStore->Certificates[cmd->BlockIndex]->CertificateSize;

                x509Certificate = (HAL_Configuration_X509CaRootBundle *)platform_malloc(sizeOfBlock);

                // check allocation
                if (x509Certificate != NULL)
                {
                    memset(x509Certificate, 0, sizeof(sizeOfBlock));

                    if (ConfigurationManager_GetConfigurationBlock(
                            x509Certificate,
                            (DeviceConfigurationOption)cmd->Configuration,
                            cmd->BlockIndex) == true)
                    {
                        size = sizeOfBlock;

                        WP_ReplyToCommand(message, true, false, (uint8_t *)x509Certificate, size);
                    }

                    platform_free(x509Certificate);

                    // done here
                    return true;
                }
            }

            break;

        case DeviceConfigurationOption_X509DeviceCertificates:

            // check if device cert store is empty OR request index doesn't exist
            // (mind the zero based index)
            if (g_TargetConfiguration.DeviceCertificates->Count == 0 ||
                g_TargetConfiguration.DeviceCertificates->Count < cmd->BlockIndex + 1)
            {
                // we are done here, just send an empty reply
                return true;
            }
            else
            {
                // because X509 certificate has a variable length need to compute the block size in two steps
                sizeOfBlock = offsetof(HAL_Configuration_X509DeviceCertificate, Certificate);
                sizeOfBlock += g_TargetConfiguration.DeviceCertificates->Certificates[cmd->BlockIndex]->CertificateSize;

                x509DeviceCertificate = (HAL_Configuration_X509DeviceCertificate *)platform_malloc(sizeOfBlock);
                memset(x509DeviceCertificate, 0, sizeof(sizeOfBlock));

                if (ConfigurationManager_GetConfigurationBlock(
                        x509DeviceCertificate,
                        (DeviceConfigurationOption)cmd->Configuration,
                        cmd->BlockIndex) == true)
                {
                    size = sizeOfBlock;

                    WP_ReplyToCommand(message, true, false, (uint8_t *)x509DeviceCertificate, size);
                }

                platform_free(x509DeviceCertificate);

                // done here
                return true;
            }

            break;

        case DeviceConfigurationOption_WirelessNetworkAP:
            // TODO missing implementation for now
            break;

        default:
            break;
    }

#else

    (void)message;

#endif // (HAS_CONFIG_BLOCK == TRUE)

    // got here, something wrong
    return false;
}

bool CLR_DBG_Debugger::Monitor_UpdateConfiguration(WP_Message *message)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    bool success = false;

    // include handling of configuration block only if feature is available
#if (HAS_CONFIG_BLOCK == TRUE)

    Monitor_UpdateConfiguration_Command *cmd = (Monitor_UpdateConfiguration_Command *)message->m_payload;
    Monitor_UpdateConfiguration_Reply cmdReply;

    switch ((DeviceConfigurationOption)cmd->Configuration)
    {
        case DeviceConfigurationOption_Network:
        case DeviceConfigurationOption_Wireless80211Network:
        case DeviceConfigurationOption_X509CaRootBundle:
        case DeviceConfigurationOption_X509DeviceCertificates:
        case DeviceConfigurationOption_All:
            if (ConfigurationManager_StoreConfigurationBlock(
                    cmd->Data,
                    (DeviceConfigurationOption)cmd->Configuration,
                    cmd->BlockIndex,
                    cmd->Length,
                    cmd->Offset,
                    cmd->Done) == true)
            {
                cmdReply.ErrorCode = 0;
                success = true;
            }
            else
            {
                cmdReply.ErrorCode = 100;
            }
            break;

        default:
            cmdReply.ErrorCode = 10;
    }

    WP_ReplyToCommand(message, success, false, &cmdReply, sizeof(cmdReply));

#else

    (void)message;

#endif // (HAS_CONFIG_BLOCK == TRUE)

    return success;
}

//--//

bool CLR_DBG_Debugger::Debugging_Execution_BasePtr(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Execution_BasePtr::Reply cmdReply;

    cmdReply.m_EE = (CLR_UINT32)(size_t)&g_CLR_RT_ExecutionEngine;

    WP_ReplyToCommand(msg, true, false, &cmdReply, sizeof(CLR_DBG_Commands::Debugging_Execution_BasePtr::Reply));

    return true;
}

bool CLR_DBG_Debugger::Debugging_Execution_ChangeConditions(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Execution_ChangeConditions *cmd =
        (CLR_DBG_Commands::Debugging_Execution_ChangeConditions *)msg->m_payload;

    // save current value
    int32_t newConditions = g_CLR_RT_ExecutionEngine.m_iDebugger_Conditions;

    // apply received flags
    newConditions |= cmd->FlagsToSet;
    newConditions &= ~cmd->FlagsToReset;

    // send confirmation reply
    if ((msg->m_header.m_flags & WP_Flags_c_NonCritical) == 0)
    {
        CLR_DBG_Commands::Debugging_Execution_ChangeConditions::Reply cmdReply;

        cmdReply.CurrentState = (CLR_UINT32)newConditions;

        WP_ReplyToCommand(
            msg,
            true,
            false,
            &cmdReply,
            sizeof(CLR_DBG_Commands::Debugging_Execution_ChangeConditions::Reply));
    }

    // if there is anything to change, apply the changes
    if (cmd->FlagsToSet || cmd->FlagsToReset)
    {
        // update conditions
        g_CLR_RT_ExecutionEngine.m_iDebugger_Conditions = newConditions;
    }

    return true;
}

//--//

static void GetClrReleaseInfo(CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::ClrInfo &clrInfo)
{
    NFReleaseInfo::Init(
        clrInfo.m_clrReleaseInfo,
        VERSION_MAJOR,
        VERSION_MINOR,
        VERSION_BUILD,
        VERSION_REVISION,
        OEMSYSTEMINFOSTRING,
        ARRAYSIZE(OEMSYSTEMINFOSTRING),
        TARGETNAMESTRING,
        ARRAYSIZE(TARGETNAMESTRING),
        PLATFORMNAMESTRING,
        ARRAYSIZE(PLATFORMNAMESTRING));

    if (g_CLR_RT_TypeSystem.m_assemblyMscorlib && g_CLR_RT_TypeSystem.m_assemblyMscorlib->m_header)
    {
        const CLR_RECORD_VERSION *mscorlibVer = &(g_CLR_RT_TypeSystem.m_assemblyMscorlib->m_header->version);
        NFVersion::Init(
            clrInfo.m_TargetFrameworkVersion,
            mscorlibVer->iMajorVersion,
            mscorlibVer->iMinorVersion,
            mscorlibVer->iBuildNumber,
            mscorlibVer->iRevisionNumber);
    }
    else
    {
        NFVersion::Init(clrInfo.m_TargetFrameworkVersion, 0, 0, 0, 0);
    }
}

static bool GetInteropNativeAssemblies(uint8_t *&data, uint32_t *size, uint32_t startIndex, uint32_t count)
{
    uint32_t index = 0;

    CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::NativeAssemblyDetails *interopNativeAssemblies;

    // sanity checks on the requested size
    // - if 0, adjust to the assemblies count to make the execution backwards compatible
    // - trim if over the available assembly count
    // (max possible page size is 255)
    if (startIndex == 0 && count == 0)
    {
        // adjust to the assemblies count to make the execution backwards compatible
        count = g_CLR_InteropAssembliesCount;
    }
    else if (count > 255 || count + startIndex > g_CLR_InteropAssembliesCount)
    {
        // adjust to the assemblies count so it doesn't overflow
        count = g_CLR_InteropAssembliesCount - startIndex;
    }

    // set buffer size which matches the allocation size
    *size = (sizeof(CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::NativeAssemblyDetails) * count);

    // alloc buffer to hold the requested number of assemblies
    data = (uint8_t *)platform_malloc(*size);

    // check for malloc failure
    if (data == NULL)
    {
        return false;
    }

    // copy pointer to ease handling ahead
    interopNativeAssemblies = (CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::NativeAssemblyDetails *)data;

    // clear buffer memory
    memset(interopNativeAssemblies, 0, (uint32_t)*size);

    // fill the array
    for (uint32_t i = 0; i < g_CLR_InteropAssembliesCount; i++)
    {
        // check if the assembly at this position it's on the requested range
        if (i >= startIndex && i < (startIndex + count))
        {
            interopNativeAssemblies[index].CheckSum = g_CLR_InteropAssembliesNativeData[i]->m_checkSum;
            hal_strcpy_s(
                (char *)interopNativeAssemblies[index].AssemblyName,
                ARRAYSIZE(interopNativeAssemblies[index].AssemblyName),
                g_CLR_InteropAssembliesNativeData[i]->m_szAssemblyName);

            NFVersion::Init(
                interopNativeAssemblies[index].Version,
                g_CLR_InteropAssembliesNativeData[i]->m_Version.iMajorVersion,
                g_CLR_InteropAssembliesNativeData[i]->m_Version.iMinorVersion,
                g_CLR_InteropAssembliesNativeData[i]->m_Version.iBuildNumber,
                g_CLR_InteropAssembliesNativeData[i]->m_Version.iRevisionNumber);

            index++;
        }
    }

    return true;
}

//--//

bool CLR_DBG_Debugger::Debugging_Execution_QueryCLRCapabilities(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities *cmd =
        (CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities *)msg->m_payload;

    CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::ReplyUnion reply;
    reply.u_capsFlags = 0;

    CLR_UINT8 *data = NULL;
    uint32_t size = 0;
    bool fSuccess = true;
    bool freeAllocFlag = false;
    uint32_t startIndex = 0;
    uint32_t count = 0xFF;

    // set the compiler info string here
#if defined(__GNUC__)
#define COMPILER_INFO "GNU ARM GCC"
    const size_t len = MIN(sizeof(COMPILER_INFO), sizeof(reply.u_SoftwareVersion.CompilerInfo) - 1);
#else
#define COMPILER_INFO "UNKNOWN"
    const size_t len = 0;
#endif

    memset(&reply, 0, sizeof(reply));

    // need to mask the command to get only the lower byte with the command code
    uint32_t cmdCode = 0x000000FF & cmd->m_cmd;

    switch (cmdCode)
    {
        case CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::c_CapabilityFlags:

#if !defined(NANOCLR_EMULATED_FLOATINGPOINT)
            reply.u_capsFlags |=
                CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::c_CapabilityFlags_FloatingPoint;
#endif

            reply.u_capsFlags |=
                CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::c_CapabilityFlags_ExceptionFilters;

            // the current version ONLY suports incremental deployment
            reply.u_capsFlags |=
                CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::c_CapabilityFlags_IncrementalDeployment;

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
            reply.u_capsFlags |=
                CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::c_CapabilityFlags_SourceLevelDebugging;

            reply.u_capsFlags |=
                CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::c_CapabilityFlags_ThreadCreateEx;
#endif

#if defined(NANOCLR_PROFILE_NEW)
            reply.u_capsFlags |=
                CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::c_CapabilityFlags_Profiling;
#if defined(NANOCLR_PROFILE_NEW_CALLS)
            reply.u_capsFlags |=
                CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::c_CapabilityFlags_Profiling_Calls;
#endif
#if defined(NANOCLR_PROFILE_NEW_ALLOCATIONS)
            reply.u_capsFlags |=
                CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::c_CapabilityFlags_Profiling_Allocations;
#endif
#endif

#if defined(NANOCLR_APPDOMAINS)
            reply.u_capsFlags |=
                CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::c_CapabilityFlags_AppDomains;
#endif

            if (CPU_IsSoftRebootSupported())
            {
                reply.u_capsFlags |=
                    CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::c_CapabilityFlags_SoftReboot;
            }

            if (::Target_ConfigUpdateRequiresErase())
            {
                reply.u_capsFlags |= CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::
                    c_CapabilityFlags_ConfigBlockRequiresErase;
            }

            if (::Target_HasNanoBooter())
            {
                reply.u_capsFlags |=
                    CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::c_CapabilityFlags_HasNanoBooter;
            }

            reply.u_capsFlags |=
                (::GetPlatformCapabilities() &
                 CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::c_CapabilityFlags_PlatformCapabiliy_Mask);

            reply.u_capsFlags |=
                (::GetTargetCapabilities() &
                 CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::c_CapabilityFlags_TargetCapabiliy_Mask);

            data = (CLR_UINT8 *)&reply.u_capsFlags;
            size = sizeof(reply.u_capsFlags);
            break;

        case CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::c_CapabilityVersion:
#if defined(__GNUC__)
            // this is returning the GNU GCC compiler version in coded format: MAJOR x 10000 + MINOR x 100 + PATCH
            // example: v6.3.1 shows as 6 x 10000 + 3 x 100 + 1 = 60301
            reply.u_SoftwareVersion.CompilerVersion = (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__);
#else
            reply.u_SoftwareVersion.CompilerVersion = -1;
#endif

#if defined(__DATE__)
            hal_strncpy_s(
                reply.u_SoftwareVersion.BuildDate,
                sizeof(reply.u_SoftwareVersion.BuildDate),
                __DATE__,
                hal_strlen_s(__DATE__));
#else
#error "__DATE__ with build timestamp needs to be  defined"
#endif

            hal_strncpy_s(
                (char *)&reply.u_SoftwareVersion.CompilerInfo[0],
                sizeof(reply.u_SoftwareVersion.CompilerInfo),
                COMPILER_INFO,
                len);

            data = (CLR_UINT8 *)&reply.u_SoftwareVersion;
            size = sizeof(reply.u_SoftwareVersion);
            break;

        case CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::c_HalSystemInfo:
            if (GetHalSystemInfo(reply.u_HalSystemInfo) == true)
            {
                data = (CLR_UINT8 *)&reply.u_HalSystemInfo;
                size = sizeof(reply.u_HalSystemInfo);
            }
            else
            {
                fSuccess = false;
            }
            break;

        case CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::c_ClrInfo:
            GetClrReleaseInfo(reply.u_ClrInfo);
            data = (CLR_UINT8 *)&reply.u_ClrInfo;
            size = sizeof(reply.u_ClrInfo);
            break;

        case CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::c_TargetReleaseInfo:
            if (Target_GetReleaseInfo(reply.u_TargetReleaseInfo) == true)
            {
                data = (CLR_UINT8 *)&reply.u_TargetReleaseInfo;
                size = sizeof(reply.u_TargetReleaseInfo);
            }
            else
            {
                fSuccess = false;
            }
            break;

        case CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::c_InteropNativeAssemblies:

            // the start index and count are encoded on the MSBytes of the command, like this: 0xCCSSxxxx
            // SS: index of the 1st assembly to list
            // CC: how many assemblies to add to the list

            // getting the upper bytes (to save on the rotate operations)
            startIndex = (cmd->m_cmd >> 16);
            // get the count from the above
            count = startIndex >> 8;
            // now fix the start index by masking the count byte
            startIndex &= 0x000000FF;

            if (GetInteropNativeAssemblies(data, &size, startIndex, count) == true)
            {
                // signal need to free memory
                freeAllocFlag = true;
            }
            else
            {
                fSuccess = false;
            }
            break;

        case CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::c_InteropNativeAssembliesCount:
            // reusing the struct field to return the assemblies count
            // because GCC optimizations prevent using the variable directly
            reply.u_capsFlags = g_CLR_InteropAssembliesCount;
            data = (CLR_UINT8 *)&reply.u_capsFlags;
            size = sizeof(reply.u_capsFlags);
            break;

        default:
            fSuccess = false;
            break;
    }

    WP_ReplyToCommand(msg, fSuccess, false, data, size);

    // check if we need to free data pointer
    if (freeAllocFlag && data != NULL)
    {
        platform_free(data);
    }

    return true;
}

bool CLR_DBG_Debugger::Debugging_Execution_Allocate(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Execution_Allocate *cmd =
        (CLR_DBG_Commands::Debugging_Execution_Allocate *)msg->m_payload;
    CLR_DBG_Commands::Debugging_Execution_Allocate::Reply reply;

    reply.m_address = (CLR_UINT32)(size_t)CLR_RT_Memory::Allocate(cmd->m_size, CLR_RT_HeapBlock::HB_CompactOnFailure);

    if (!reply.m_address)
    {
        return false;
    }

    WP_ReplyToCommand(msg, true, false, &reply, sizeof(CLR_DBG_Commands::Debugging_Execution_Allocate::Reply));

    return true;
}

bool CLR_DBG_Debugger::Debugging_UpgradeToSsl(WP_Message *msg)
{

    CLR_DBG_Commands::Debugging_UpgradeToSsl *cmd = (CLR_DBG_Commands::Debugging_UpgradeToSsl *)msg->m_payload;
    CLR_DBG_Commands::Debugging_UpgradeToSsl::Reply reply;

    if (!DebuggerPort_IsSslSupported(HalSystemConfig.DebuggerPort))
    {
        return false;
    }

    reply.m_success = 1;

    WP_ReplyToCommand(msg, true, true, &reply, sizeof(CLR_DBG_Commands::Debugging_UpgradeToSsl::Reply));

    Events_WaitForEvents(0, 300);

    return true == DebuggerPort_UpgradeToSsl(HalSystemConfig.DebuggerPort, cmd->m_flags);
}

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

static bool FillValues(
    CLR_RT_HeapBlock *ptr,
    CLR_DBG_Commands::Debugging_Value *&array,
    size_t num,
    CLR_RT_HeapBlock *reference,
    CLR_RT_TypeDef_Instance *pTD)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    if (!ptr)
        return true;

    if (!array || num == 0)
        return false;

    CLR_DBG_Commands::Debugging_Value *dst = array++;
    num--;
    CLR_RT_TypeDescriptor desc;

    memset(dst, 0, sizeof(*dst));

    dst->m_referenceID = (reference != NULL) ? reference : ptr;
    dst->m_dt = ptr->DataType();
    dst->m_flags = ptr->DataFlags();
    dst->m_size = ptr->DataSize();

    if (pTD != NULL)
    {
        dst->m_td = *pTD;
    }
    else if (SUCCEEDED(desc.InitializeFromObject(*ptr)))
    {
        dst->m_td = desc.m_handlerCls;
    }

    switch (dst->m_dt)
    {
        case DATATYPE_BOOLEAN:
        case DATATYPE_I1:
        case DATATYPE_U1:

        case DATATYPE_CHAR:
        case DATATYPE_I2:
        case DATATYPE_U2:

        case DATATYPE_I4:
        case DATATYPE_U4:
        case DATATYPE_R4:

        case DATATYPE_I8:
        case DATATYPE_U8:
        case DATATYPE_R8:
        case DATATYPE_DATETIME:
        case DATATYPE_TIMESPAN:
        case DATATYPE_REFLECTION:
            //
            // Primitives or optimized value types.
            //
            memcpy(dst->m_builtinValue, (void *)&ptr->NumericByRefConst().u1, 8);
            break;

        case DATATYPE_STRING:
        {
            const char *text = ptr->StringText();

            if (text != NULL)
            {
                dst->m_charsInString = text;
                dst->m_bytesInString = (CLR_UINT32)hal_strlen_s(text);

                hal_strncpy_s(
                    (char *)dst->m_builtinValue,
                    ARRAYSIZE(dst->m_builtinValue),
                    text,
                    __min(dst->m_bytesInString, MAXSTRLEN(dst->m_builtinValue)));
            }
            else
            {
                dst->m_charsInString = NULL;
                dst->m_bytesInString = 0;
                dst->m_builtinValue[0] = 0;
            }
        }
        break;

        case DATATYPE_OBJECT:
        case DATATYPE_BYREF:
            return FillValues(ptr->Dereference(), array, num, NULL, pTD);

        case DATATYPE_CLASS:
        case DATATYPE_VALUETYPE:
            dst->m_td = ptr->ObjectCls();
            break;

        case DATATYPE_SZARRAY:
        {
            CLR_RT_HeapBlock_Array *ptr2 = (CLR_RT_HeapBlock_Array *)ptr;

            dst->m_array_numOfElements = ptr2->m_numOfElements;
            dst->m_array_depth = ptr2->ReflectionDataConst().m_levels;
            dst->m_array_typeIndex = ptr2->ReflectionDataConst().m_data.m_type;
        }
        break;

            ////////////////////////////////////////

        case DATATYPE_WEAKCLASS:
            break;

        case DATATYPE_ARRAY_BYREF:
            dst->m_arrayref_referenceID = ptr->Array();
            dst->m_arrayref_index = ptr->ArrayIndex();

            break;

        case DATATYPE_DELEGATE_HEAD:
            break;

        case DATATYPE_DELEGATELIST_HEAD:
            break;

        case DATATYPE_FREEBLOCK:
        case DATATYPE_CACHEDBLOCK:
        case DATATYPE_ASSEMBLY:
        case DATATYPE_OBJECT_TO_EVENT:
        case DATATYPE_BINARY_BLOB_HEAD:

        case DATATYPE_THREAD:
        case DATATYPE_SUBTHREAD:
        case DATATYPE_STACK_FRAME:
        case DATATYPE_TIMER_HEAD:
        case DATATYPE_LOCK_HEAD:
        case DATATYPE_LOCK_OWNER_HEAD:
        case DATATYPE_LOCK_REQUEST_HEAD:
        case DATATYPE_WAIT_FOR_OBJECT_HEAD:
        case DATATYPE_FINALIZER_HEAD:
        case DATATYPE_MEMORY_STREAM_HEAD:
        case DATATYPE_MEMORY_STREAM_DATA:

        case DATATYPE_SERIALIZER_HEAD:
        case DATATYPE_SERIALIZER_DUPLICATE:
        case DATATYPE_SERIALIZER_STATE:

        case DATATYPE_ENDPOINT_HEAD:

#if defined(NANOCLR_APPDOMAINS)
        case DATATYPE_APPDOMAIN_HEAD:
        case DATATYPE_TRANSPARENT_PROXY:
        case DATATYPE_APPDOMAIN_ASSEMBLY:
#endif

            break;
    }

    return true;
}

bool CLR_DBG_Debugger::GetValue(
    WP_Message *msg,
    CLR_RT_HeapBlock *ptr,
    CLR_RT_HeapBlock *reference,
    CLR_RT_TypeDef_Instance *pTD)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    CLR_DBG_Commands::Debugging_Value reply[4];
    CLR_DBG_Commands::Debugging_Value *array = reply;

    if (FillValues(ptr, array, ARRAYSIZE(reply), reference, pTD))
    {
        WP_ReplyToCommand(msg, true, false, reply, (int)((size_t)array - (size_t)reply));

        return true;
    }

    WP_ReplyToCommand(msg, false, false, NULL, 0);

    return false;
}

//--//

bool CLR_DBG_Debugger::Debugging_Execution_SetCurrentAppDomain(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
#if defined(NANOCLR_APPDOMAINS)

    CLR_DBG_Commands::Debugging_Execution_SetCurrentAppDomain *cmd =
        (CLR_DBG_Commands::Debugging_Execution_SetCurrentAppDomain *)msg->m_payload;
    CLR_RT_AppDomain *appDomain = g_CLR_DBG_Debugger->GetAppDomainFromID(cmd->m_id);

    if (appDomain)
    {
        g_CLR_RT_ExecutionEngine.SetCurrentAppDomain(appDomain);
    }

    WP_ReplyToCommand(msg, appDomain != NULL, false, NULL, 0);

    return true;
#else
    (void)msg;
    return false;
#endif
}

bool CLR_DBG_Debugger::Debugging_Execution_Breakpoints(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Execution_Breakpoints *cmd =
        (CLR_DBG_Commands::Debugging_Execution_Breakpoints *)msg->m_payload;

    g_CLR_RT_ExecutionEngine.InstallBreakpoints(
        cmd->m_data,
        (msg->m_header.m_size - sizeof(cmd->m_flags)) / sizeof(CLR_DBG_Commands::Debugging_Execution_BreakpointDef));

    WP_ReplyToCommand(msg, true, false, NULL, 0);

    return true;
}

bool CLR_DBG_Debugger::Debugging_Execution_BreakpointStatus(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Execution_BreakpointStatus::Reply reply;

    if (g_CLR_RT_ExecutionEngine.DequeueActiveBreakpoint(reply.m_lastHit) == false)
    {
        memset(&reply.m_lastHit, 0, sizeof(reply.m_lastHit));
    }

    WP_ReplyToCommand(msg, true, false, &reply, sizeof(CLR_DBG_Commands::Debugging_Execution_BreakpointStatus::Reply));

    return true;
}

//--//

CLR_RT_Assembly *CLR_DBG_Debugger::IsGoodAssembly(CLR_IDX idxAssm)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    NANOCLR_FOREACH_ASSEMBLY(g_CLR_RT_TypeSystem)
    {
        if (pASSM->m_idx == idxAssm)
            return pASSM;
    }
    NANOCLR_FOREACH_ASSEMBLY_END();

    return NULL;
}

bool CLR_DBG_Debugger::CheckTypeDef(const CLR_RT_TypeDef_Index &td, CLR_RT_TypeDef_Instance &inst)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    CLR_RT_Assembly *assm = IsGoodAssembly(td.Assembly());

    if (assm && td.Type() < assm->m_pTablesSize[TBL_TypeDef])
    {
        return inst.InitializeFromIndex(td);
    }

    return false;
}

bool CLR_DBG_Debugger::CheckFieldDef(const CLR_RT_FieldDef_Index &fd, CLR_RT_FieldDef_Instance &inst)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    CLR_RT_Assembly *assm = IsGoodAssembly(fd.Assembly());

    if (assm && fd.Field() < assm->m_pTablesSize[TBL_FieldDef])
    {
        return inst.InitializeFromIndex(fd);
    }

    return false;
}

bool CLR_DBG_Debugger::CheckMethodDef(const CLR_RT_MethodDef_Index &md, CLR_RT_MethodDef_Instance &inst)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    CLR_RT_Assembly *assm = IsGoodAssembly(md.Assembly());

    if (assm && md.Method() < assm->m_pTablesSize[TBL_MethodDef])
    {
        return inst.InitializeFromIndex(md);
    }

    return false;
}

CLR_RT_StackFrame *CLR_DBG_Debugger::CheckStackFrame(CLR_INT32 pid, CLR_UINT32 depth, bool &isInline)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    CLR_RT_Thread *th = GetThreadFromPid(pid);

    isInline = false;

    if (th)
    {
        NANOCLR_FOREACH_NODE(CLR_RT_StackFrame, call, th->m_stackFrames)
        {
#ifndef CLR_NO_IL_INLINE
            if (call->m_inlineFrame)
            {
                if (depth-- == 0)
                {
                    isInline = true;
                    return call;
                }
            }
#endif

            if (depth-- == 0)
                return call;
        }
        NANOCLR_FOREACH_NODE_END();
    }

    return NULL;
}

//--//

static HRESULT Debugging_Thread_Create_Helper(CLR_RT_MethodDef_Index &md, CLR_RT_Thread *&th, CLR_INT32 pid)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock ref;
    ref.SetObjectReference(NULL);
    CLR_RT_ProtectFromGC gc(ref);
    CLR_RT_Thread *realThread = (pid != 0) ? CLR_DBG_Debugger::GetThreadFromPid(pid) : NULL;

    th = NULL;

    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Delegate::CreateInstance(ref, md, NULL));

    NANOCLR_CHECK_HRESULT(
        g_CLR_RT_ExecutionEngine.NewThread(th, ref.DereferenceDelegate(), ThreadPriority::Highest, -1));

    if (realThread)
    {
        th->m_realThread = realThread;
    }

    {
        CLR_RT_StackFrame *stack = th->CurrentFrame();
        const CLR_RECORD_METHODDEF *target = stack->m_call.m_target;
        CLR_UINT8 numArgs = target->numArgs;

        if (numArgs)
        {
            CLR_RT_SignatureParser parser;
            parser.Initialize_MethodSignature(stack->m_call.m_assm, target);
            CLR_RT_SignatureParser::Element res;
            CLR_RT_HeapBlock *args = stack->m_arguments;

            if (parser.m_flags & PIMAGE_CEE_CS_CALLCONV_HASTHIS)
            {
                args->SetObjectReference(NULL);

                numArgs--;
                args++;
            }

            //
            // Skip return value.
            //
            NANOCLR_CHECK_HRESULT(parser.Advance(res));

            //
            // None of the arguments can be ByRef.
            //
            {
                CLR_RT_SignatureParser parser2 = parser;

                for (; parser2.Available() > 0;)
                {
                    NANOCLR_CHECK_HRESULT(parser2.Advance(res));

                    if (res.m_fByRef)
                    {
                        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
                    }
                }
            }

            for (CLR_UINT8 i = 0; i < numArgs; i++, args++)
            {
                NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.InitializeReference(*args, parser));
            }
        }
    }

    NANOCLR_CLEANUP();

    if (FAILED(hr))
    {
        if (th)
        {
            th->Terminate();
            th = NULL;
        }
    }

    NANOCLR_CLEANUP_END();
}

bool CLR_DBG_Debugger::Debugging_Thread_CreateEx(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Thread_CreateEx *cmd = (CLR_DBG_Commands::Debugging_Thread_CreateEx *)msg->m_payload;
    CLR_DBG_Commands::Debugging_Thread_CreateEx::Reply cmdReply;
    CLR_RT_Thread *th;

    if (SUCCEEDED(Debugging_Thread_Create_Helper(cmd->m_md, th, cmd->m_pid)))
    {
        th->m_scratchPad = cmd->m_scratchPad;

        cmdReply.m_pid = th->m_pid;
    }
    else
    {
        cmdReply.m_pid = -1;
    }

    WP_ReplyToCommand(msg, true, false, &cmdReply, sizeof(CLR_DBG_Commands::Debugging_Thread_CreateEx::Reply));

    return true;
}

bool CLR_DBG_Debugger::Debugging_Thread_List(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Thread_List::Reply *cmdReply = NULL;
    int len = 0;

    if (FAILED(g_CLR_DBG_Debugger->CreateListOfThreads(cmdReply, len)))
    {
        return false;
    }
    else
    {
        WP_ReplyToCommand(msg, true, false, cmdReply, len);
    }

    CLR_RT_Memory::Release(cmdReply);

    return true;
}

bool CLR_DBG_Debugger::Debugging_Thread_Stack(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Thread_Stack *cmd = (CLR_DBG_Commands::Debugging_Thread_Stack *)msg->m_payload;
    CLR_DBG_Commands::Debugging_Thread_Stack::Reply *cmdReply = NULL;
    int len = 0;

    if (FAILED(g_CLR_DBG_Debugger->CreateListOfCalls(cmd->m_pid, cmdReply, len)))
    {
        return false;
    }
    else
    {
        WP_ReplyToCommand(msg, true, false, cmdReply, len);
    }

    CLR_RT_Memory::Release(cmdReply);

    return true;
}

bool CLR_DBG_Debugger::Debugging_Thread_Kill(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Thread_Kill *cmd = (CLR_DBG_Commands::Debugging_Thread_Kill *)msg->m_payload;
    CLR_DBG_Commands::Debugging_Thread_Kill::Reply cmdReply;
    CLR_RT_Thread *th = g_CLR_DBG_Debugger->GetThreadFromPid(cmd->m_pid);

    if (th)
    {
        th->Terminate();

        cmdReply.m_result = 1;
    }
    else
    {
        cmdReply.m_result = 0;
    }

    WP_ReplyToCommand(msg, true, false, &cmdReply, sizeof(CLR_DBG_Commands::Debugging_Thread_Kill::Reply));

    return true;
}

bool CLR_DBG_Debugger::Debugging_Thread_Suspend(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Thread_Suspend *cmd = (CLR_DBG_Commands::Debugging_Thread_Suspend *)msg->m_payload;
    CLR_RT_Thread *th = g_CLR_DBG_Debugger->GetThreadFromPid(cmd->m_pid);

    if (th)
    {
        th->Suspend();
    }

    WP_ReplyToCommand(msg, th != NULL, false, NULL, 0);

    return true;
}

bool CLR_DBG_Debugger::Debugging_Thread_Resume(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Thread_Resume *cmd = (CLR_DBG_Commands::Debugging_Thread_Resume *)msg->m_payload;
    CLR_RT_Thread *th = g_CLR_DBG_Debugger->GetThreadFromPid(cmd->m_pid);

    if (th)
    {
        th->Resume();
    }

    WP_ReplyToCommand(msg, th != NULL, false, NULL, 0);

    return true;
}

bool CLR_DBG_Debugger::Debugging_Thread_Get(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    CLR_DBG_Debugger *dbg = g_CLR_DBG_Debugger;
    CLR_DBG_Commands::Debugging_Thread_Get *cmd = (CLR_DBG_Commands::Debugging_Thread_Get *)msg->m_payload;
    CLR_RT_Thread *th = dbg->GetThreadFromPid(cmd->m_pid);
    CLR_RT_HeapBlock *pThread;
    bool fFound = false;

    if (th == NULL)
    {
        return false;
    }

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
    // If we are a thread spawned by the debugger to perform evaluations,
    // return the thread object that correspond to thread that has focus in debugger.
    th = th->m_realThread;
#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

    // Find an existing managed thread, if it exists
    // making sure to only return the managed object association with the current appdomain
    // to prevent leaking of managed Thread objects across AD boundaries.

    NANOCLR_FOREACH_NODE(CLR_RT_ObjectToEvent_Source, src, th->m_references)
    {
        CLR_RT_HeapBlock *pManagedThread = src->m_objectPtr;
        _ASSERTE(pManagedThread != NULL);

#if defined(NANOCLR_APPDOMAINS)
        {
            CLR_RT_ObjectToEvent_Source *appDomainSrc = CLR_RT_ObjectToEvent_Source::ExtractInstance(
                pManagedThread[Library_corlib_native_System_Threading_Thread::FIELD___appDomain]);

            if (appDomainSrc == NULL)
            {
                break;
            }

            fFound = (appDomainSrc->m_eventPtr == g_CLR_RT_ExecutionEngine.GetCurrentAppDomain());
        }
#else
        fFound = true;
#endif

        if (fFound)
        {
            pThread = pManagedThread;

            break;
        }
    }
    NANOCLR_FOREACH_NODE_END();

    if (!fFound)
    {
        pThread = (CLR_RT_HeapBlock *)platform_malloc(sizeof(CLR_RT_HeapBlock));

        // Create the managed thread.
        // This implies that there is no state in the managed object.  This is not exactly true, as the managed thread
        // contains the priority as well as the delegate to start.  However, that state is really just used as a
        // placeholder for the data before the thread is started.  Once the thread is started, they are copied over to
        // the unmanaged thread object and no longer used.  The managed object is then used simply as a wrapper for the
        // unmanaged thread.  Therefore, it is safe to simply make another managed thread here.
        if (SUCCEEDED(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*pThread, g_CLR_RT_WellKnownTypes.m_Thread)))
        {
            CLR_RT_HeapBlock *pRes = pThread->Dereference();

            int pri = th->GetThreadPriority();

            pRes[Library_corlib_native_System_Threading_Thread::FIELD___priority].NumericByRef().s4 = pri;

            if (SUCCEEDED(CLR_RT_ObjectToEvent_Source::CreateInstance(
                    th,
                    *pRes,
                    pRes[Library_corlib_native_System_Threading_Thread::FIELD___thread])))
            {
#if defined(NANOCLR_APPDOMAINS)
                CLR_RT_ObjectToEvent_Source::CreateInstance(
                    g_CLR_RT_ExecutionEngine.GetCurrentAppDomain(),
                    *pRes,
                    pRes[Library_corlib_native_System_Threading_Thread::FIELD___appDomain]);
#endif
                fFound = true;
            }
        }
    }

    if (!fFound)
    {
        return false;
    }

    // try to get value
    WP_ReplyToCommand(msg, g_CLR_DBG_Debugger->GetValue(msg, pThread, NULL, NULL), false, NULL, 0);

    return true;
}

bool CLR_DBG_Debugger::Debugging_Thread_GetException(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Thread_GetException *cmd =
        (CLR_DBG_Commands::Debugging_Thread_GetException *)msg->m_payload;
    CLR_RT_Thread *th = g_CLR_DBG_Debugger->GetThreadFromPid(cmd->m_pid);
    CLR_RT_HeapBlock *blk = NULL;

    if (th)
    {
        blk = &th->m_currentException;
    }

    // try to get value
    WP_ReplyToCommand(msg, g_CLR_DBG_Debugger->GetValue(msg, blk, NULL, NULL), false, NULL, 0);

    return true;
}

bool CLR_DBG_Debugger::Debugging_Thread_Unwind(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Thread_Unwind *cmd = (CLR_DBG_Commands::Debugging_Thread_Unwind *)msg->m_payload;
    CLR_RT_StackFrame *call;
    CLR_RT_Thread *th;
    bool isInline = false;

    if ((call = g_CLR_DBG_Debugger->CheckStackFrame(cmd->m_pid, cmd->m_depth, isInline)) != NULL)
    {
        _ASSERTE((call->m_flags & CLR_RT_StackFrame::c_MethodKind_Native) == 0);

        th = call->m_owningThread;
        _ASSERTE(th->m_nestedExceptionsPos);

        CLR_RT_Thread::UnwindStack &us = th->m_nestedExceptions[th->m_nestedExceptionsPos - 1];
        _ASSERTE(th->m_currentException.Dereference() == us.m_exception);
        _ASSERTE(us.m_flags & CLR_RT_Thread::UnwindStack::c_ContinueExceptionHandler);

        us.m_handlerStack = call;
        us.m_flags |= CLR_RT_Thread::UnwindStack::c_MagicCatchForInteceptedException;

        us.SetPhase(CLR_RT_Thread::UnwindStack::p_2_RunningFinallys_0);
    }

    WP_ReplyToCommand(msg, true, false, NULL, 0);

    return true;
}

//--//

bool CLR_DBG_Debugger::Debugging_Stack_Info(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Stack_Info *cmd = (CLR_DBG_Commands::Debugging_Stack_Info *)msg->m_payload;
    CLR_DBG_Commands::Debugging_Stack_Info::Reply cmdReply;
    CLR_RT_StackFrame *call;
    bool isInline = false;

    if ((call = g_CLR_DBG_Debugger->CheckStackFrame(cmd->m_pid, cmd->m_depth, isInline)) != NULL)
    {
#ifndef CLR_NO_IL_INLINE
        if (isInline)
        {
            cmdReply.m_md = call->m_inlineFrame->m_frame.m_call;
            cmdReply.m_IP = (CLR_UINT32)(call->m_inlineFrame->m_frame.m_IP - call->m_inlineFrame->m_frame.m_IPStart);
            cmdReply.m_numOfArguments = call->m_inlineFrame->m_frame.m_call.m_target->numArgs;
            cmdReply.m_numOfLocals = call->m_inlineFrame->m_frame.m_call.m_target->numLocals;
            cmdReply.m_depthOfEvalStack = (CLR_UINT32)(call->m_evalStack - call->m_inlineFrame->m_frame.m_evalStack);
        }
        else
#endif
        {
            cmdReply.m_md = call->m_call;
            cmdReply.m_IP = (CLR_UINT32)(call->m_IP - call->m_IPstart);
            cmdReply.m_numOfArguments = call->m_call.m_target->numArgs;
            cmdReply.m_numOfLocals = call->m_call.m_target->numLocals;
            cmdReply.m_depthOfEvalStack = (CLR_UINT32)call->TopValuePosition();
        }

        WP_ReplyToCommand(msg, true, false, &cmdReply, sizeof(CLR_DBG_Commands::Debugging_Stack_Info::Reply));

        return true;
    }

    return false;
}

bool CLR_DBG_Debugger::Debugging_Stack_SetIP(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Stack_SetIP *cmd = (CLR_DBG_Commands::Debugging_Stack_SetIP *)msg->m_payload;
    CLR_RT_StackFrame *call;
    bool isInline = false;

    if ((call = g_CLR_DBG_Debugger->CheckStackFrame(cmd->m_pid, cmd->m_depth, isInline)) != NULL)
    {
#ifndef CLR_NO_IL_INLINE
        if (isInline)
        {
            return false;
        }
        else
#endif
        {
            call->m_IP = call->m_IPstart + cmd->m_IP;
            call->m_evalStackPos = call->m_evalStack + cmd->m_depthOfEvalStack;
        }

        call->m_flags &= ~CLR_RT_StackFrame::c_InvalidIP;

        WP_ReplyToCommand(msg, true, false, NULL, 0);

        return true;
    }

    return false;
}

//--//

static bool IsBlockEnumMaybe(CLR_RT_HeapBlock *blk)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    const CLR_UINT32 c_MaskForPrimitive = CLR_RT_DataTypeLookup::c_Integer | CLR_RT_DataTypeLookup::c_Numeric;

    CLR_RT_TypeDescriptor desc;

    if (FAILED(desc.InitializeFromObject(*blk)))
        return false;

    const CLR_RT_DataTypeLookup &dtl = c_CLR_RT_DataTypeLookup[desc.m_handlerCls.m_target->dataType];

    return (dtl.m_flags & c_MaskForPrimitive) == c_MaskForPrimitive;
}

static bool SetBlockHelper(CLR_RT_HeapBlock *blk, CLR_DataType dt, CLR_UINT8 *builtinValue)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    bool fCanAssign = false;

    if (blk)
    {
        CLR_DataType dtDst;
        CLR_RT_HeapBlock src;

        dtDst = blk->DataType();

        src.SetDataId(CLR_RT_HEAPBLOCK_RAW_ID(dt, 0, 1));
        memcpy((void *)&src.NumericByRef().u1, builtinValue, sizeof(CLR_UINT64));

        if (dtDst == dt)
        {
            fCanAssign = true;
        }
        else
        {
            if (dt == DATATYPE_REFLECTION)
            {
                fCanAssign = (dtDst == DATATYPE_OBJECT && blk->Dereference() == NULL);
            }
            else if (dt == DATATYPE_OBJECT)
            {
                fCanAssign = (src.Dereference() == NULL && dtDst == DATATYPE_REFLECTION);
            }
            else
            {
                _ASSERTE(c_CLR_RT_DataTypeLookup[dtDst].m_flags & CLR_RT_DataTypeLookup::c_Numeric);

                if (c_CLR_RT_DataTypeLookup[dtDst].m_sizeInBytes == sizeof(CLR_INT32) &&
                    c_CLR_RT_DataTypeLookup[dt].m_sizeInBytes < sizeof(CLR_INT32))
                {
                    dt = dtDst;
                    fCanAssign = true;
                }
            }
        }

        if (fCanAssign)
        {
            blk->ChangeDataType(dt);
            memcpy((void *)&blk->NumericByRef().u1, builtinValue, sizeof(CLR_UINT64));
        }
    }

    return fCanAssign;
}

static CLR_RT_HeapBlock *GetScratchPad_Helper(int idx)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    CLR_RT_HeapBlock_Array *array = g_CLR_RT_ExecutionEngine.m_scratchPadArray;
    CLR_RT_HeapBlock tmp;
    CLR_RT_HeapBlock ref;

    tmp.SetObjectReference(array);

    if (SUCCEEDED(ref.InitializeArrayReference(tmp, idx)))
    {
        return (CLR_RT_HeapBlock *)array->GetElement(idx);
    }

    return NULL;
}

//--//

bool CLR_DBG_Debugger::Debugging_Value_ResizeScratchPad(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Value_ResizeScratchPad *cmd =
        (CLR_DBG_Commands::Debugging_Value_ResizeScratchPad *)msg->m_payload;
    CLR_RT_HeapBlock ref;

    if (cmd->m_size == 0)
    {
        g_CLR_RT_ExecutionEngine.m_scratchPadArray = NULL;
    }
    else
    {
        if (SUCCEEDED(CLR_RT_HeapBlock_Array::CreateInstance(ref, cmd->m_size, g_CLR_RT_WellKnownTypes.m_Object)))
        {
            CLR_RT_HeapBlock_Array *pOld = g_CLR_RT_ExecutionEngine.m_scratchPadArray;
            CLR_RT_HeapBlock_Array *pNew = ref.DereferenceArray();

            if (pOld)
            {
                memcpy(
                    pNew->GetFirstElement(),
                    pOld->GetFirstElement(),
                    sizeof(CLR_RT_HeapBlock) * __min(pNew->m_numOfElements, pOld->m_numOfElements));
            }

            g_CLR_RT_ExecutionEngine.m_scratchPadArray = pNew;

            // done
            WP_ReplyToCommand(msg, true, false, NULL, 0);
        }
        else
        {
            // failed to create instance
        }
    }

    return false;
}

bool CLR_DBG_Debugger::Debugging_Value_GetStack(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Value_GetStack *cmd = (CLR_DBG_Commands::Debugging_Value_GetStack *)msg->m_payload;
    CLR_RT_StackFrame *call;
    bool isInline = false;

    if ((call = g_CLR_DBG_Debugger->CheckStackFrame(cmd->m_pid, cmd->m_depth, isInline)) != NULL)
    {
        CLR_RT_HeapBlock *array;
        CLR_UINT32 num;
#ifndef CLR_NO_IL_INLINE
        CLR_RT_MethodDef_Instance &md = isInline ? call->m_inlineFrame->m_frame.m_call : call->m_call;
#else
        CLR_RT_MethodDef_Instance &md = call->m_call;
#endif

        switch (cmd->m_kind)
        {
            case CLR_DBG_Commands::Debugging_Value_GetStack::c_Argument:
#ifndef CLR_NO_IL_INLINE
                array = isInline ? call->m_inlineFrame->m_frame.m_args : call->m_arguments;
                num = isInline ? md.m_target->numArgs : md.m_target->numArgs;
#else
                array = call->m_arguments;
                num = call->m_call.m_target->numArgs;
#endif
                break;

            case CLR_DBG_Commands::Debugging_Value_GetStack::c_Local:
#ifndef CLR_NO_IL_INLINE
                array = isInline ? call->m_inlineFrame->m_frame.m_locals : call->m_locals;
                num = isInline ? md.m_target->numLocals : md.m_target->numLocals;
#else
                array = call->m_locals;
                num = call->m_call.m_target->numLocals;
#endif
                break;

            case CLR_DBG_Commands::Debugging_Value_GetStack::c_EvalStack:
#ifndef CLR_NO_IL_INLINE
                array = isInline ? call->m_inlineFrame->m_frame.m_evalStack : call->m_evalStack;
                num = isInline ? (CLR_UINT32)(call->m_evalStack - call->m_inlineFrame->m_frame.m_evalStack)
                               : (CLR_UINT32)call->TopValuePosition();
#else
                array = call->m_evalStack;
                num = (CLR_UINT32)call->TopValuePosition();
#endif
                break;

            default:
                return false;
        }

        if (cmd->m_index >= num)
        {
            return false;
        }

        CLR_RT_HeapBlock *blk = &array[cmd->m_index];
        CLR_RT_HeapBlock *reference = NULL;
        CLR_RT_HeapBlock tmp;
        CLR_RT_TypeDef_Instance *pTD = NULL;
        CLR_RT_TypeDef_Instance td;

        if (cmd->m_kind != CLR_DBG_Commands::Debugging_Value_GetStack::c_EvalStack && IsBlockEnumMaybe(blk))
        {
            CLR_UINT32 iElement = cmd->m_index;
            CLR_RT_SignatureParser parser;
            CLR_RT_SignatureParser::Element res;
            CLR_RT_TypeDescriptor desc;

            if (cmd->m_kind == CLR_DBG_Commands::Debugging_Value_GetStack::c_Argument)
            {
                parser.Initialize_MethodSignature(md.m_assm, md.m_target);

                iElement++; // Skip the return value, always at the head of the signature.

                if (parser.m_flags & PIMAGE_CEE_CS_CALLCONV_HASTHIS)
                {
                    if (iElement == 0)
                    {
                        // The requested argument is the "this" argument, it can never be a primitive.
                        return false;
                    }

                    iElement--;
                }
            }
            else
            {
                parser.Initialize_MethodLocals(md.m_assm, md.m_target);
            }

            do
            {
                parser.Advance(res);
            } while (iElement--);

            //
            // Arguments to a methods come from the eval stack and we don't fix up the eval stack for each call.
            // So some arguments have the wrong datatype, since an eval stack push always promotes to 32 bits.
            //
            if (c_CLR_RT_DataTypeLookup[blk->DataType()].m_sizeInBytes == sizeof(CLR_INT32) &&
                c_CLR_RT_DataTypeLookup[res.m_dt].m_sizeInBytes < sizeof(CLR_INT32))
            {
                tmp.Assign(*blk);
                tmp.ChangeDataType(res.m_dt);

                reference = blk;
                blk = &tmp;
            }

            //
            // Check for enum.
            //
            desc.InitializeFromType(res.m_cls);

            if (desc.m_handlerCls.m_target->IsEnum())
            {
                td = desc.m_handlerCls;
                pTD = &td;
            }
        }

        WP_ReplyToCommand(msg, g_CLR_DBG_Debugger->GetValue(msg, blk, reference, pTD), false, NULL, 0);

        return true;
    }

    return false;
}

bool CLR_DBG_Debugger::Debugging_Value_GetField(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Value_GetField *cmd = (CLR_DBG_Commands::Debugging_Value_GetField *)msg->m_payload;
    CLR_RT_HeapBlock *blk = cmd->m_heapblock;
    CLR_RT_HeapBlock *reference = NULL;
    CLR_RT_HeapBlock tmp;
    CLR_RT_TypeDescriptor desc;
    CLR_RT_TypeDef_Instance td;
    CLR_RT_TypeDef_Instance *pTD = NULL;
    CLR_RT_FieldDef_Instance inst;
    CLR_UINT32 offset;

    if (blk != NULL && cmd->m_offset > 0)
    {
        if (FAILED(desc.InitializeFromObject(*blk)))
        {
            return false;
        }

        td = desc.m_handlerCls;
        offset = cmd->m_offset - 1;

        while (true)
        {
            CLR_UINT32 iFields = td.m_target->iFields_Num;
            CLR_UINT32 totalFields = td.CrossReference().m_totalFields;
            CLR_UINT32 dFields = totalFields - iFields;

            if (offset >= dFields)
            {
                offset -= dFields;
                break;
            }

            if (!td.SwitchToParent())
            {
                return false;
            }
        }

        cmd->m_fd.Set(td.Assembly(), td.m_target->iFields_First + offset);
    }

    if (!g_CLR_DBG_Debugger->CheckFieldDef(cmd->m_fd, inst))
    {
        return false;
    }

    if (blk == NULL)
    {
        blk = CLR_RT_ExecutionEngine::AccessStaticField(cmd->m_fd);
    }
    else
    {
        if (cmd->m_offset == 0)
        {
            cmd->m_offset = inst.CrossReference().m_offset;
        }

        if (cmd->m_offset == 0)
        {
            return false;
        }

        switch (blk->DataType())
        {
            case DATATYPE_CLASS:
            case DATATYPE_VALUETYPE:
                break;

            default:
                if (FAILED(blk->EnsureObjectReference(blk)))
                    return false;
                break;
        }

        switch (blk->DataType())
        {
            case DATATYPE_DATETIME: // Special case.
            case DATATYPE_TIMESPAN: // Special case.
                tmp.SetInteger((CLR_INT64)blk->NumericByRefConst().s8);
                reference = blk;
                blk = &tmp;
                break;

            default:
                blk = &blk[cmd->m_offset];
                break;
        }
    }

    if (IsBlockEnumMaybe(blk))
    {
        if (SUCCEEDED(desc.InitializeFromFieldDefinition(inst)))
        {
            if (desc.m_handlerCls.m_target->IsEnum())
            {
                pTD = &desc.m_handlerCls;
            }
        }
    }

    WP_ReplyToCommand(msg, g_CLR_DBG_Debugger->GetValue(msg, blk, reference, pTD), false, NULL, 0);

    return true;
}

bool CLR_DBG_Debugger::Debugging_Value_GetArray(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Value_GetArray *cmd = (CLR_DBG_Commands::Debugging_Value_GetArray *)msg->m_payload;
    CLR_RT_HeapBlock *blk = NULL;
    CLR_RT_HeapBlock *reference = NULL;
    CLR_RT_HeapBlock tmp;
    CLR_RT_HeapBlock ref;
    CLR_RT_TypeDef_Instance *pTD = NULL;
    CLR_RT_TypeDef_Instance td;

    tmp.SetObjectReference(cmd->m_heapblock);

    if (SUCCEEDED(ref.InitializeArrayReference(tmp, cmd->m_index)))
    {
        CLR_RT_HeapBlock_Array *array = ref.Array();

        if (array->m_fReference)
        {
            blk = (CLR_RT_HeapBlock *)array->GetElement(cmd->m_index);
        }
        else
        {
            if (FAILED(tmp.LoadFromReference(ref)))
            {
                return false;
            }

            blk = &tmp;
            reference = (CLR_RT_HeapBlock *)-1;
        }

        if (IsBlockEnumMaybe(blk))
        {
            if (td.InitializeFromIndex(array->ReflectionDataConst().m_data.m_type))
            {
                if (td.m_target->IsEnum())
                {
                    pTD = &td;
                }
            }
        }
    }

    WP_ReplyToCommand(msg, g_CLR_DBG_Debugger->GetValue(msg, blk, reference, pTD), false, NULL, 0);

    return true;
}

bool CLR_DBG_Debugger::Debugging_Value_GetBlock(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Value_GetBlock *cmd = (CLR_DBG_Commands::Debugging_Value_GetBlock *)msg->m_payload;
    CLR_RT_HeapBlock *blk = cmd->m_heapblock;

    WP_ReplyToCommand(msg, g_CLR_DBG_Debugger->GetValue(msg, blk, NULL, NULL), false, NULL, 0);

    return true;
}

bool CLR_DBG_Debugger::Debugging_Value_GetScratchPad(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Value_GetScratchPad *cmd =
        (CLR_DBG_Commands::Debugging_Value_GetScratchPad *)msg->m_payload;
    CLR_RT_HeapBlock *blk = GetScratchPad_Helper(cmd->m_idx);

    WP_ReplyToCommand(msg, g_CLR_DBG_Debugger->GetValue(msg, blk, NULL, NULL), false, NULL, 0);

    return true;
}

bool CLR_DBG_Debugger::Debugging_Value_SetBlock(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Value_SetBlock *cmd = (CLR_DBG_Commands::Debugging_Value_SetBlock *)msg->m_payload;
    CLR_RT_HeapBlock *blk = cmd->m_heapblock;

    WP_ReplyToCommand(msg, SetBlockHelper(blk, (CLR_DataType)cmd->m_dt, cmd->m_builtinValue), false, NULL, 0);

    return true;
}

bool CLR_DBG_Debugger::Debugging_Value_SetArray(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Value_SetArray *cmd = (CLR_DBG_Commands::Debugging_Value_SetArray *)msg->m_payload;
    CLR_RT_HeapBlock_Array *array = cmd->m_heapblock;
    CLR_RT_HeapBlock tmp;

    tmp.SetObjectReference(cmd->m_heapblock);

    //
    // We can only set values in arrays of primitive types.
    //
    if (array != NULL && !array->m_fReference)
    {
        CLR_RT_HeapBlock ref;

        if (SUCCEEDED(ref.InitializeArrayReference(tmp, cmd->m_index)))
        {
            if (SUCCEEDED(tmp.LoadFromReference(ref)))
            {
                if (SetBlockHelper(&tmp, tmp.DataType(), cmd->m_builtinValue))
                {
                    if (SUCCEEDED(tmp.StoreToReference(ref, 0)))
                    {
                        WP_ReplyToCommand(msg, true, false, NULL, 0);
                    }
                }
            }
        }
    }

    return false;
}

//--//

bool CLR_DBG_Debugger::Debugging_Value_AllocateObject(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Value_AllocateObject *cmd =
        (CLR_DBG_Commands::Debugging_Value_AllocateObject *)msg->m_payload;
    CLR_RT_HeapBlock *blk = NULL;
    CLR_RT_HeapBlock *ptr = GetScratchPad_Helper(cmd->m_index);

    if (ptr)
    {
        if (SUCCEEDED(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*ptr, cmd->m_td)))
        {
            blk = ptr;
        }
    }

    WP_ReplyToCommand(msg, g_CLR_DBG_Debugger->GetValue(msg, blk, NULL, NULL), false, NULL, 0);

    return true;
}

bool CLR_DBG_Debugger::Debugging_Value_AllocateString(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Value_AllocateString *cmd =
        (CLR_DBG_Commands::Debugging_Value_AllocateString *)msg->m_payload;
    CLR_RT_HeapBlock *blk = NULL;
    CLR_RT_HeapBlock *ptr = GetScratchPad_Helper(cmd->m_index);

    if (ptr)
    {
        CLR_RT_HeapBlock_String *str = CLR_RT_HeapBlock_String::CreateInstance(*ptr, cmd->m_size);

        if (str)
        {
            char *dst = (char *)str->StringText();

            //
            // Fill the string with spaces, it will be set at a later stage.
            //
            memset(dst, ' ', cmd->m_size);
            dst[cmd->m_size] = 0;

            blk = ptr;
        }
    }

    WP_ReplyToCommand(msg, g_CLR_DBG_Debugger->GetValue(msg, blk, NULL, NULL), false, NULL, 0);

    return true;
}

bool CLR_DBG_Debugger::Debugging_Value_AllocateArray(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Value_AllocateArray *cmd =
        (CLR_DBG_Commands::Debugging_Value_AllocateArray *)msg->m_payload;
    CLR_RT_HeapBlock *blk = NULL;
    CLR_RT_HeapBlock *ptr = GetScratchPad_Helper(cmd->m_index);

    if (ptr)
    {
        CLR_RT_ReflectionDef_Index reflex;

        reflex.m_kind = REFLECTION_TYPE;
        reflex.m_levels = cmd->m_depth;
        reflex.m_data.m_type = cmd->m_td;

        if (SUCCEEDED(CLR_RT_HeapBlock_Array::CreateInstance(*ptr, cmd->m_numOfElements, reflex)))
        {
            blk = ptr;
        }
    }

    WP_ReplyToCommand(msg, g_CLR_DBG_Debugger->GetValue(msg, blk, NULL, NULL), false, NULL, 0);

    return true;
}

#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

#if defined(NANOCLR_PROFILE_NEW)
bool CLR_DBG_Debugger::Profiling_Command(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    CLR_DBG_Debugger *dbg = g_CLR_DBG_Debugger;
    CLR_DBG_Commands::Profiling_Command *cmd = (CLR_DBG_Commands::Profiling_Command *)msg->m_payload;
    CLR_UINT8 command = cmd->m_command;

    switch (command)
    {
        case CLR_DBG_Commands::Profiling_Command::c_Command_ChangeConditions:
            WP_ReplyToCommand(msg, dbg->Profiling_ChangeConditions(msg), false, NULL, 0);
            break;

        case CLR_DBG_Commands::Profiling_Command::c_Command_FlushStream:
            WP_ReplyToCommand(msg, dbg->Profiling_FlushStream(msg), false, NULL, 0);
            break;

        default:
            return false;
    }
}

bool CLR_DBG_Debugger::Profiling_ChangeConditions(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    CLR_DBG_Commands::Profiling_Command *parent_cmd = (CLR_DBG_Commands::Profiling_Command *)msg->m_payload;
    CLR_DBG_Commands::Profiling_ChangeConditions *cmd = (CLR_DBG_Commands::Profiling_ChangeConditions *)&parent_cmd[1];

    g_CLR_RT_ExecutionEngine.m_iProfiling_Conditions |= cmd->m_set;
    g_CLR_RT_ExecutionEngine.m_iProfiling_Conditions &= ~cmd->m_reset;

    CLR_DBG_Commands::Profiling_Command::Reply cmdReply;

    cmdReply.m_raw = g_CLR_RT_ExecutionEngine.m_iProfiling_Conditions;

    WP_ReplyToCommand(msg, true, false, &cmdReply, sizeof(cmdReply));

    _ASSERTE(FIMPLIES(CLR_EE_PRF_IS(Allocations), CLR_EE_PRF_IS(Enabled)));
    _ASSERTE(FIMPLIES(CLR_EE_PRF_IS(Calls), CLR_EE_PRF_IS(Enabled)));

    if ((cmd->m_set & CLR_RT_ExecutionEngine::c_fProfiling_Enabled) != 0)
    {
        g_CLR_PRF_Profiler.SendMemoryLayout();
    }

    return true;
}

bool CLR_DBG_Debugger::Profiling_FlushStream(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    CLR_DBG_Commands::Profiling_Command::Reply cmdReply;

    g_CLR_PRF_Profiler.Stream_Flush();

    cmdReply.m_raw = 0;

    WP_ReplyToCommand(msg, true, false, &cmdReply, sizeof(CLR_DBG_Commands::Profiling_Command::Reply));

    return true;
}

#endif //#if defined(NANOCLR_PROFILE_NEW)

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

//--//

struct AnalyzeObject
{
    CLR_RT_HeapBlock *m_ptr;
    bool m_fNull;
    bool m_fBoxed;
    bool m_fCanBeNull;
    CLR_RT_TypeDescriptor m_desc;
    CLR_RT_HeapBlock m_value;
};

static HRESULT AnalyzeObject_Helper(CLR_RT_HeapBlock *ptr, AnalyzeObject &ao)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    NANOCLR_HEADER();

    if (ptr && ptr->DataType() == DATATYPE_BYREF)
        ptr = ptr->Dereference();

    ao.m_ptr = ptr;

    if (ptr == NULL || (ptr->DataType() == DATATYPE_OBJECT && ptr->Dereference() == NULL))
    {
        ao.m_fNull = true;
        ao.m_fBoxed = false;
        ao.m_fCanBeNull = true;
    }
    else
    {
        NANOCLR_CHECK_HRESULT(ao.m_desc.InitializeFromObject(*ptr));

        ao.m_fNull = false;
        ao.m_fBoxed = (ptr->DataType() == DATATYPE_OBJECT && ptr->Dereference()->IsBoxed());

        switch (ao.m_desc.m_flags & CLR_RT_DataTypeLookup::c_SemanticMask2 & ~CLR_RT_DataTypeLookup::c_SemanticMask)
        {
            case CLR_RT_DataTypeLookup::c_Array:
            case CLR_RT_DataTypeLookup::c_ArrayList:
                ao.m_fCanBeNull = true;
                break;
            default:
            {
                switch (ao.m_desc.m_flags & CLR_RT_DataTypeLookup::c_SemanticMask)
                {
                    case CLR_RT_DataTypeLookup::c_Primitive:
                    case CLR_RT_DataTypeLookup::c_ValueType:
                    case CLR_RT_DataTypeLookup::c_Enum:
                        ao.m_fCanBeNull =
                            ao.m_fBoxed || (ao.m_desc.m_handlerCls.m_data == g_CLR_RT_WellKnownTypes.m_String.m_data);
                        break;

                    default:
                        ao.m_fCanBeNull = true;
                        break;
                }

                break;
            }
        }
    }

    NANOCLR_NOCLEANUP();
}

static HRESULT Assign_Helper(CLR_RT_HeapBlock *blkDst, CLR_RT_HeapBlock *blkSrc)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    NANOCLR_HEADER();

    AnalyzeObject aoDst;
    AnalyzeObject aoSrc;
    CLR_RT_HeapBlock srcVal;
    srcVal.SetObjectReference(NULL);
    CLR_RT_ProtectFromGC gc(srcVal);

    NANOCLR_CHECK_HRESULT(AnalyzeObject_Helper(blkDst, aoDst));
    NANOCLR_CHECK_HRESULT(AnalyzeObject_Helper(blkSrc, aoSrc));

    if (aoSrc.m_fNull)
    {
        if (aoDst.m_fCanBeNull == false)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
        }

        NANOCLR_CHECK_HRESULT(srcVal.StoreToReference(*blkDst, 0));
    }
    else
    {
        NANOCLR_CHECK_HRESULT(srcVal.LoadFromReference(*blkSrc));

        if (aoDst.m_fNull)
        {
            if (aoSrc.m_fCanBeNull == false)
            {
                NANOCLR_CHECK_HRESULT(srcVal.PerformBoxing(aoSrc.m_desc.m_handlerCls));
            }

            blkDst->Assign(srcVal);
        }
        else
        {
            if (srcVal.IsAValueType())
            {
                if (blkDst->IsAValueType() == false)
                {
                    NANOCLR_CHECK_HRESULT(srcVal.PerformBoxing(aoSrc.m_desc.m_handlerCls));
                }
            }
            else
            {
                if (blkDst->IsAValueType() == true)
                {
                    NANOCLR_CHECK_HRESULT(srcVal.PerformUnboxing(aoSrc.m_desc.m_handlerCls));
                }
            }

            NANOCLR_CHECK_HRESULT(blkDst->Reassign(srcVal));
        }
    }

    NANOCLR_NOCLEANUP();
}

bool CLR_DBG_Debugger::Debugging_Value_Assign(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Value_Assign *cmd = (CLR_DBG_Commands::Debugging_Value_Assign *)msg->m_payload;
    CLR_RT_HeapBlock *blkDst = cmd->m_heapblockDst;
    CLR_RT_HeapBlock *blkSrc = cmd->m_heapblockSrc;

    if (blkDst && FAILED(Assign_Helper(blkDst, blkSrc)))
    {
        blkDst = NULL;
    }

    WP_ReplyToCommand(msg, g_CLR_DBG_Debugger->GetValue(msg, blkDst, NULL, NULL), false, NULL, 0);

    return true;
}

//--//

bool CLR_DBG_Debugger::Debugging_TypeSys_Assemblies(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_RT_Assembly_Index assemblies[CLR_RT_TypeSystem::c_MaxAssemblies];
    int num = 0;

    NANOCLR_FOREACH_ASSEMBLY(g_CLR_RT_TypeSystem)
    {
        assemblies[num++].Set(pASSM->m_idx);
    }
    NANOCLR_FOREACH_ASSEMBLY_END();

    WP_ReplyToCommand(msg, true, false, assemblies, sizeof(CLR_RT_Assembly_Index) * num);

    return true;
}

bool CLR_DBG_Debugger::Debugging_TypeSys_AppDomains(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
#if defined(NANOCLR_APPDOMAINS)

    int num = 0;
    CLR_UINT32 appDomainIDs[256];

    NANOCLR_FOREACH_NODE(CLR_RT_AppDomain, appDomain, g_CLR_RT_ExecutionEngine.m_appDomains)
    {
        appDomainIDs[num++] = appDomain->m_id;

        if (num >= ARRAYSIZE(appDomainIDs))
        {
            break;
        }
    }
    NANOCLR_FOREACH_NODE_END();

    WP_ReplyToCommand(msg, true, false, appDomainIDs, sizeof(CLR_UINT32) * num);

    return true;
#else
    (void)msg;
    return false;
#endif
}

//--//

bool CLR_DBG_Debugger::Debugging_Resolve_AppDomain(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
#if defined(NANOCLR_APPDOMAINS)

    CLR_DBG_Commands::Debugging_Resolve_AppDomain *cmd =
        (CLR_DBG_Commands::Debugging_Resolve_AppDomain *)msg->m_payload;
    CLR_RT_AppDomain *appDomain = g_CLR_DBG_Debugger->GetAppDomainFromID(cmd->m_id);
    CLR_UINT32 numAssemblies = 0;
    CLR_DBG_Commands::Debugging_Resolve_AppDomain::Reply *cmdReply;
    CLR_UINT8
    buf[sizeof(CLR_DBG_Commands::Debugging_Resolve_AppDomain::Reply) +
        sizeof(CLR_RT_Assembly_Index) * CLR_RT_TypeSystem::c_MaxAssemblies];
    size_t count;
    const char *name;
    CLR_RT_Assembly_Index *pAssemblyIndex;

    if (appDomain)
    {
        cmdReply = (CLR_DBG_Commands::Debugging_Resolve_AppDomain::Reply *)&buf;

        cmdReply->m_state = appDomain->m_state;

        name = appDomain->m_strName->StringText();
        count = __min(hal_strlen_s(name) + 1, sizeof(cmdReply->m_szName) - 1);

        hal_strncpy_s(cmdReply->m_szName, ARRAYSIZE(cmdReply->m_szName), name, count);

        pAssemblyIndex = (CLR_RT_Assembly_Index *)(&cmdReply->m_assemblies);

        NANOCLR_FOREACH_ASSEMBLY_IN_APPDOMAIN(appDomain)
        {
            pAssemblyIndex->Set(pASSM->m_idx);
            pAssemblyIndex++;
            numAssemblies++;
        }
        NANOCLR_FOREACH_ASSEMBLY_IN_APPDOMAIN_END();

        WP_ReplyToCommand(
            msg,
            true,
            false,
            cmdReply,
            sizeof(*cmdReply) + sizeof(CLR_RT_Assembly_Index) * (numAssemblies - 1));
    }
    else
    {
        // no app
        return false;
    }

    return true;
#else
    (void)msg;
    return false;
#endif
}

bool CLR_DBG_Debugger::Debugging_Resolve_Assembly(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Resolve_Assembly::Reply *cmdReply;

    CLR_DBG_Commands::Debugging_Resolve_Assembly *cmd = (CLR_DBG_Commands::Debugging_Resolve_Assembly *)msg->m_payload;
    CLR_RT_Assembly *assm = g_CLR_DBG_Debugger->IsGoodAssembly(cmd->m_idx.Assembly());

#ifdef TARGET_RAM_CONSTRAINED
    // try allocating memory for reply struct
    cmdReply = (CLR_DBG_Commands::Debugging_Resolve_Assembly::Reply *)platform_malloc(
        sizeof(CLR_DBG_Commands::Debugging_Resolve_Assembly::Reply));
#else
    CLR_DBG_Commands::Debugging_Resolve_Assembly::Reply reply;
    cmdReply = &reply;
#endif

    // sanity check
    if (cmdReply != NULL)
    {
        if (assm)
        {
#if defined(_WIN32)
            // append path
            if (assm->m_strPath != NULL)
            {
                sprintf_s(
                    cmdReply->m_szName,
                    ARRAYSIZE(cmdReply->m_szName),
                    "%s,%s",
                    assm->m_szName,
                    assm->m_strPath->c_str());
            }
            else
#endif
            {
                hal_strncpy_s(
                    cmdReply->m_szName,
                    ARRAYSIZE(cmdReply->m_szName),
                    assm->m_szName,
                    MAXSTRLEN(cmdReply->m_szName));
            }

            cmdReply->m_flags = assm->m_flags;
            cmdReply->m_version = assm->m_header->version;

            WP_ReplyToCommand(msg, true, false, cmdReply, sizeof(CLR_DBG_Commands::Debugging_Resolve_Assembly::Reply));

#ifdef TARGET_RAM_CONSTRAINED
            // free memory
            platform_free(cmdReply);
#endif

            return true;
        }
    }

    return false;
}

bool CLR_DBG_Debugger::Debugging_Resolve_Type(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_RT_TypeDef_Instance inst;
    CLR_DBG_Commands::Debugging_Resolve_Type::Reply *cmdReply;

    CLR_DBG_Commands::Debugging_Resolve_Type *cmd = (CLR_DBG_Commands::Debugging_Resolve_Type *)msg->m_payload;

#ifdef TARGET_RAM_CONSTRAINED
    // try allocating memory for reply struct
    cmdReply = (CLR_DBG_Commands::Debugging_Resolve_Type::Reply *)platform_malloc(
        sizeof(CLR_DBG_Commands::Debugging_Resolve_Type::Reply));
#else
    CLR_DBG_Commands::Debugging_Resolve_Type::Reply reply;
    cmdReply = &reply;
#endif

    // sanity check
    if (cmdReply != NULL)
    {
        if (g_CLR_DBG_Debugger->CheckTypeDef(cmd->m_td, inst))
        {
            char *szBuffer = cmdReply->m_type;
            size_t iBuffer = MAXSTRLEN(cmdReply->m_type);

            if (SUCCEEDED(g_CLR_RT_TypeSystem.BuildTypeName(inst, szBuffer, iBuffer)))
            {
                WP_ReplyToCommand(msg, true, false, cmdReply, sizeof(CLR_DBG_Commands::Debugging_Resolve_Type::Reply));

#ifdef TARGET_RAM_CONSTRAINED
                // free memory
                platform_free(cmdReply);
#endif

                return true;
            }
        }
    }

    return false;
}

bool CLR_DBG_Debugger::Debugging_Resolve_Field(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Resolve_Field::Reply *cmdReply;
    CLR_RT_FieldDef_Instance inst;

    CLR_DBG_Commands::Debugging_Resolve_Field *cmd = (CLR_DBG_Commands::Debugging_Resolve_Field *)msg->m_payload;

#ifdef TARGET_RAM_CONSTRAINED
    // try allocating memory for reply struct
    cmdReply = (CLR_DBG_Commands::Debugging_Resolve_Field::Reply *)platform_malloc(
        sizeof(CLR_DBG_Commands::Debugging_Resolve_Field::Reply));
#else
    CLR_DBG_Commands::Debugging_Resolve_Field::Reply reply;
    cmdReply = &reply;
#endif

    // sanity check
    if (cmdReply != NULL)
    {

        if (g_CLR_DBG_Debugger->CheckFieldDef(cmd->m_fd, inst))
        {
            char *szBuffer = cmdReply->m_name;
            size_t iBuffer = MAXSTRLEN(cmdReply->m_name);

            if (SUCCEEDED(g_CLR_RT_TypeSystem.BuildFieldName(inst, szBuffer, iBuffer)))
            {
                CLR_RT_TypeDef_Instance instClass;
                instClass.InitializeFromField(inst);

                cmdReply->m_td = instClass;
                cmdReply->m_index = inst.CrossReference().m_offset;

                WP_ReplyToCommand(msg, true, false, cmdReply, sizeof(CLR_DBG_Commands::Debugging_Resolve_Field::Reply));

#ifdef TARGET_RAM_CONSTRAINED
                // free memory
                platform_free(cmdReply);
#endif

                return true;
            }
        }
    }

    return false;
}

bool CLR_DBG_Debugger::Debugging_Resolve_Method(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Resolve_Method::Reply *cmdReply;
    CLR_RT_MethodDef_Instance inst;
    CLR_RT_TypeDef_Instance instOwner;

    CLR_DBG_Commands::Debugging_Resolve_Method *cmd = (CLR_DBG_Commands::Debugging_Resolve_Method *)msg->m_payload;

#ifdef TARGET_RAM_CONSTRAINED
    // try allocating memory for reply struct
    cmdReply = (CLR_DBG_Commands::Debugging_Resolve_Method::Reply *)platform_malloc(
        sizeof(CLR_DBG_Commands::Debugging_Resolve_Method::Reply));
#else
    CLR_DBG_Commands::Debugging_Resolve_Method::Reply reply;
    cmdReply = &reply;
#endif

    // sanity check
    if (cmdReply != NULL)
    {

        if (g_CLR_DBG_Debugger->CheckMethodDef(cmd->m_md, inst) && instOwner.InitializeFromMethod(inst))
        {
            char *szBuffer = cmdReply->m_method;
            size_t iBuffer = MAXSTRLEN(cmdReply->m_method);

            cmdReply->m_td = instOwner;

            CLR_SafeSprintf(szBuffer, iBuffer, "%s", inst.m_assm->GetString(inst.m_target->name));

            WP_ReplyToCommand(msg, true, false, cmdReply, sizeof(CLR_DBG_Commands::Debugging_Resolve_Method::Reply));

#ifdef TARGET_RAM_CONSTRAINED
            // free memory
            platform_free(cmdReply);
#endif
            return true;
        }
    }

    return false;
}

bool CLR_DBG_Debugger::Debugging_Resolve_VirtualMethod(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Resolve_VirtualMethod *cmd =
        (CLR_DBG_Commands::Debugging_Resolve_VirtualMethod *)msg->m_payload;
    CLR_DBG_Commands::Debugging_Resolve_VirtualMethod::Reply cmdReply;
    CLR_RT_TypeDef_Index cls;
    CLR_RT_MethodDef_Index md;

    cmdReply.m_md.Clear();

    if (SUCCEEDED(CLR_RT_TypeDescriptor::ExtractTypeIndexFromObject(*cmd->m_obj, cls)))
    {
        if (g_CLR_RT_EventCache.FindVirtualMethod(cls, cmd->m_md, md))
        {
            cmdReply.m_md = md;
        }
    }

    WP_ReplyToCommand(msg, true, false, &cmdReply, sizeof(cmdReply));

    return true;
}

#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

//--//

bool CLR_DBG_Debugger::Debugging_Deployment_Status(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Deployment_Status::Reply *cmdReply;
    CLR_UINT32 totLength;
    CLR_UINT32 deploySectorsNum = 0;
    CLR_UINT32 deploySectorStart = 0;
    CLR_UINT32 deployLength = 0;

    // find the first device in list with DEPLOYMENT blocks
    if (m_deploymentStorageDevice != NULL)
    {
        BlockStorageStream stream;

        if (BlockStorageStream_InitializeWithBlockStorageDevice(
                &stream,
                BlockUsage_DEPLOYMENT,
                m_deploymentStorageDevice))
        {
            do
            {
                if (deploySectorsNum == 0)
                {
                    deploySectorStart = BlockStorageStream_CurrentAddress(&stream);
                }

                deployLength += stream.Length;
                deploySectorsNum++;

            } while (BlockStorageStream_NextStream(&stream) &&
                     stream.BaseAddress == (deploySectorStart + deployLength));
        }

        totLength = sizeof(CLR_DBG_Commands::Debugging_Deployment_Status::Reply);

        cmdReply = (CLR_DBG_Commands::Debugging_Deployment_Status::Reply *)CLR_RT_Memory::Allocate(totLength, true);

        if (!cmdReply)
        {
            return false;
        }

        CLR_RT_Memory::ZeroFill(cmdReply, totLength);

        cmdReply->EntryPoint = g_CLR_RT_TypeSystem.m_entryPoint.m_data;
        cmdReply->StorageStart = deploySectorStart;
        cmdReply->StorageLength = deployLength;

        WP_ReplyToCommand(msg, true, false, cmdReply, totLength);

        CLR_RT_Memory::Release(cmdReply);

        WP_ReplyToCommand(msg, true, false, NULL, 0);

        return true;
    }
    else
    {
        return false;
    }
}

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

bool CLR_DBG_Debugger::Debugging_Info_SetJMC_Method(const CLR_RT_MethodDef_Index &idx, bool fJMC)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    CLR_RT_MethodDef_Instance inst;

    if (!CheckMethodDef(idx, inst))
    {
        return false;
    }

    if (inst.m_target->RVA == CLR_EmptyIndex)
    {
        return false;
    }

    inst.DebuggingInfo().SetJMC(fJMC);

    return true;
}

bool CLR_DBG_Debugger::Debugging_Info_SetJMC_Type(const CLR_RT_TypeDef_Index &idx, bool fJMC)
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    const CLR_RECORD_TYPEDEF *td;
    CLR_RT_TypeDef_Instance inst;
    int totMethods;
    CLR_RT_MethodDef_Index md;

    if (!CheckTypeDef(idx, inst))
        return false;

    td = inst.m_target;
    totMethods = td->vMethods_Num + td->iMethods_Num + td->sMethods_Num;

    for (int i = 0; i < totMethods; i++)
    {
        md.Set(idx.Assembly(), td->methods_First + i);

        Debugging_Info_SetJMC_Method(md, fJMC);
    }

    return true;
}

bool CLR_DBG_Debugger::Debugging_Info_SetJMC(WP_Message *msg)
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    CLR_DBG_Commands::Debugging_Info_SetJMC *cmd = (CLR_DBG_Commands::Debugging_Info_SetJMC *)msg->m_payload;
    bool fJMC = (cmd->m_fIsJMC != 0);

    switch (cmd->m_kind)
    {
        case REFLECTION_ASSEMBLY:
        {
            CLR_RT_Assembly *assm = g_CLR_DBG_Debugger->IsGoodAssembly(cmd->m_data.m_assm.Assembly());

            if (!assm)
            {
                return false;
            }

            for (int i = 0; i < assm->m_pTablesSize[TBL_TypeDef]; i++)
            {
                CLR_RT_TypeDef_Index idx;

                idx.Set(cmd->m_data.m_assm.Assembly(), i);

                g_CLR_DBG_Debugger->Debugging_Info_SetJMC_Type(idx, fJMC);
            }

            WP_ReplyToCommand(msg, true, false, NULL, 0);

            return true;
        }

        case REFLECTION_TYPE:
            WP_ReplyToCommand(
                msg,
                g_CLR_DBG_Debugger->Debugging_Info_SetJMC_Type(cmd->m_data.m_type, fJMC),
                false,
                NULL,
                0);
            return true;

        case REFLECTION_METHOD:
            WP_ReplyToCommand(
                msg,
                g_CLR_DBG_Debugger->Debugging_Info_SetJMC_Method(cmd->m_data.m_method, fJMC),
                false,
                NULL,
                0);
            return true;

        default:
            return false;
    }
}

#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
