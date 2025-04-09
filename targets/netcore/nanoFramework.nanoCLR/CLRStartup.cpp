//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "stdafx.h"
#include "nanoCLR_native.h"

////////////////////////////////////////////////////////////////////////////////

struct Settings
{
    CLR_SETTINGS m_clrOptions;
    CLR_RT_ParseOptions::BufferMap m_assemblies;
    bool m_fInitialized;
#if defined(VIRTUAL_DEVICE)
    ConfigureRuntimeCallback m_configureRuntimeCallback;
    bool m_configured;
#endif

    //--//

    HRESULT Initialize(CLR_SETTINGS const &params)
    {
        NANOCLR_HEADER();

        m_clrOptions = params;

        NANOCLR_CHECK_HRESULT(CLR_RT_ExecutionEngine::CreateInstance(params));
#if !defined(BUILD_RTM)
        CLR_Debug::Printf("Created EE.\r\n");
#endif

#if !defined(BUILD_RTM)
        if (params.WaitForDebugger)
        {
            CLR_EE_DBG_SET(Stopped);
        }
#endif

        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.StartHardware());
#if !defined(BUILD_RTM)
        CLR_Debug::Printf("Started Hardware.\r\n");
#endif

        CLR_DBG_Debugger::Debugger_Discovery();

        m_fInitialized = true;

        NANOCLR_NOCLEANUP();
    }

    HRESULT LoadAssembly(const CLR_RECORD_ASSEMBLY *header, CLR_RT_Assembly *&assm)
    {
        NANOCLR_HEADER();

        const CLR_RT_NativeAssemblyData *pNativeAssmData;

        NANOCLR_CHECK_HRESULT(CLR_RT_Assembly::CreateInstance(header, assm));

        // Get handlers for native functions in assembly
        pNativeAssmData = GetAssemblyNativeData(assm->m_szName);

        // If pNativeAssmData not NULL- means this assembly has native calls and there is pointer to table with native
        // calls.
        if (pNativeAssmData != NULL)
        {
            // First verify that check sum in assembly object matches hardcoded check sum.
            if (assm->m_header->nativeMethodsChecksum != pNativeAssmData->m_checkSum)
            {
                CLR_Debug::Printf(
                    "\r\n\r\n***********************************************************************\r\n");
                CLR_Debug::Printf("*                                                                     *\r\n");
                CLR_Debug::Printf("* ERROR!!!!  Firmware version does not match managed code version!!!! *\r\n");
                CLR_Debug::Printf("*                                                                     *\r\n");
                CLR_Debug::Printf("*                                                                     *\r\n");
                CLR_Debug::Printf(
                    "* Invalid native checksum: %s 0x%08X!=0x%08X *\r\n",
                    assm->m_szName,
                    assm->m_header->nativeMethodsChecksum,
                    pNativeAssmData->m_checkSum);
                CLR_Debug::Printf("*                                                                     *\r\n");
                CLR_Debug::Printf("***********************************************************************\r\n");

                NANOCLR_SET_AND_LEAVE(CLR_E_ASSM_WRONG_CHECKSUM);
            }

            // Assembly has valid pointer to table with native methods. Save it.
            assm->m_nativeCode = (const CLR_RT_MethodHandler *)pNativeAssmData->m_pNativeMethods;
        }
        g_CLR_RT_TypeSystem.Link(assm);
        NANOCLR_NOCLEANUP();
    }

    HRESULT Load()
    {
        NANOCLR_HEADER();

        // clear flag (in case EE wasn't restarted)
        CLR_EE_DBG_CLR(StateResolutionFailed);

#if defined(VIRTUAL_DEVICE)
        if (!m_configured && m_configureRuntimeCallback)
        {
            NANOCLR_CHECK_HRESULT(m_configureRuntimeCallback());
            m_configured = true;
        }
#endif

#if !defined(BUILD_RTM)
        CLR_Debug::Printf("Loading Assemblies.\r\n");
#endif

        for (CLR_RT_ParseOptions::BufferMapIter it = m_assemblies.begin(); it != m_assemblies.end(); it++)
        {
            CLR_RT_Assembly *assm;
            const CLR_RT_Buffer *buffer = (const CLR_RT_Buffer *)it->second;
            const CLR_RECORD_ASSEMBLY *header = (CLR_RECORD_ASSEMBLY *)&(*buffer)[0];

            // Creates instance of assembly, sets pointer to native functions, links to g_CLR_RT_TypeSystem
            NANOCLR_CHECK_HRESULT(LoadAssembly(header, assm));
        }

        // don't load assemblies because we don't have any deployment storage
#if !defined(BUILD_RTM)
        CLR_Debug::Printf("Loading Deployment Assemblies.\r\n");
#endif

        LoadDeploymentAssemblies(BlockUsage_DEPLOYMENT);

        //--//

#if !defined(BUILD_RTM)
        CLR_Debug::Printf("Resolving.\r\n");
#endif
        NANOCLR_CHECK_HRESULT(g_CLR_RT_TypeSystem.ResolveAll());

        NANOCLR_CHECK_HRESULT(g_CLR_RT_TypeSystem.PrepareForExecution());

#if defined(NANOCLR_PROFILE_HANDLER)
        CLR_PROF_Handler::Calibrate();
#endif

        NANOCLR_CLEANUP();

#if !defined(BUILD_RTM)
        if (FAILED(hr))
        {
            CLR_Debug::Printf("Error: %08x\r\n", hr);

            if (hr == CLR_E_TYPE_UNAVAILABLE)
            {
                // exception occurred during type resolution
                CLR_EE_DBG_SET(StateResolutionFailed);
            }
        }
#endif

        NANOCLR_CLEANUP_END();
    }

    HRESULT CheckKnownAssembliesForNonXIP(char **start, char **end)
    {
        //--//
        NANOCLR_HEADER();

        BlockStorageDevice *device;
        ByteAddress datByteAddress;
        unsigned int datSize = ROUNDTOMULTIPLE((unsigned int)(*end) - (unsigned int)(*start), CLR_UINT32);

        if (BlockStorageList_FindDeviceForPhysicalAddress(&device, (unsigned int)(*start), &datByteAddress) &&
            device != NULL)
        {
            const DeviceBlockInfo *deviceInfo = BlockStorageDevice_GetDeviceInfo(device);

            if (deviceInfo->Attribute & MediaAttribute_SupportsXIP)
            {
                unsigned char *datAssembliesBuffer =
                    (unsigned char *)CLR_RT_Memory::Allocate_And_Erase(datSize, CLR_RT_HeapBlock ::HB_Unmovable);
                CHECK_ALLOCATION(datAssembliesBuffer);

                if (!BlockStorageDevice_Read(device, datByteAddress, datSize, datAssembliesBuffer))
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
                }
                *start = (char *)datAssembliesBuffer;
                *end = (char *)((unsigned int)datAssembliesBuffer + (unsigned int)datSize);
            }
        }

        // else data in RAM
        NANOCLR_NOCLEANUP();
    }

    HRESULT ContiguousBlockAssemblies(BlockStorageStream stream, bool isXIP)
    {
        NANOCLR_HEADER();

        const CLR_RECORD_ASSEMBLY *header;
        unsigned char *assembliesBuffer;
        signed int headerInBytes = sizeof(CLR_RECORD_ASSEMBLY);
        unsigned char *headerBuffer = NULL;

        if (!isXIP)
        {
            headerBuffer = (unsigned char *)CLR_RT_Memory::Allocate(headerInBytes, true);
            CHECK_ALLOCATION(headerBuffer);
            CLR_RT_Memory::ZeroFill(headerBuffer, headerInBytes);
        }

        while (TRUE)
        {
            if (!BlockStorageStream_Read(&stream, &headerBuffer, headerInBytes))
                break;

            header = (const CLR_RECORD_ASSEMBLY *)headerBuffer;

            // check header first before read
            if (!header->GoodHeader())
            {
                break;
            }

            unsigned int AssemblySizeInByte = ROUNDTOMULTIPLE(header->TotalSize(), CLR_UINT32);

            if (!isXIP)
            {
                // read the assemblies
                assembliesBuffer = (unsigned char *)CLR_RT_Memory::Allocate_And_Erase(
                    AssemblySizeInByte,
                    CLR_RT_HeapBlock ::HB_Unmovable);

                if (!assembliesBuffer)
                {
                    // release the headerbuffer which has being used and leave
                    CLR_RT_Memory::Release(headerBuffer);

                    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
                }
            }

            BlockStorageStream_Seek(&stream, -headerInBytes, BlockStorageStream_SeekCurrent);

            if (!BlockStorageStream_Read(&stream, &assembliesBuffer, AssemblySizeInByte))
                break;

            header = (const CLR_RECORD_ASSEMBLY *)assembliesBuffer;

            if (!header->GoodAssembly())
            {
                if (!isXIP)
                {

                    CLR_RT_Memory::Release(assembliesBuffer);
                }

                break;
            }

            // we have good Assembly
            CLR_RT_Assembly *assm;

            // Creates instance of assembly, sets pointer to native functions, links to g_CLR_RT_TypeSystem
            if (FAILED(LoadAssembly(header, assm)))
            {
                if (!isXIP)
                {
                    CLR_RT_Memory::Release(assembliesBuffer);
                }

                break;
            }
            assm->m_flags |= CLR_RT_Assembly::Deployed;
        }
        if (!isXIP)
            CLR_RT_Memory::Release(headerBuffer);

        NANOCLR_NOCLEANUP();
    }

    HRESULT LoadDeploymentAssemblies(unsigned int memoryUsage)
    {
        NANOCLR_HEADER();

        BlockStorageStream stream;
        memset(&stream, 0, sizeof(BlockStorageStream));
        const DeviceBlockInfo *deviceInfo;

        // find the block
        if (!BlockStorageStream_Initialize(&stream, memoryUsage))
        {
#if !defined(BUILD_RTM)
            CLR_Debug::Printf("ERROR: Could not find device for DEPLOYMENT usage\r\n");
#endif
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
        }

        do
        {
            deviceInfo = BlockStorageDevice_GetDeviceInfo(stream.Device);

            ContiguousBlockAssemblies(stream, (deviceInfo->Attribute & MediaAttribute_SupportsXIP));
        } while (BlockStorageStream_NextStream(&stream));

        NANOCLR_NOCLEANUP();
    }

    void Cleanup()
    {
        CLR_RT_ExecutionEngine::DeleteInstance();

        memset(&g_CLR_RT_ExecutionEngine, 0, sizeof(g_CLR_RT_ExecutionEngine));
        memset(&g_CLR_RT_WellKnownTypes, 0, sizeof(g_CLR_RT_WellKnownTypes));
        memset(&g_CLR_RT_WellKnownMethods, 0, sizeof(g_CLR_RT_WellKnownMethods));
        memset(&g_CLR_RT_TypeSystem, 0, sizeof(g_CLR_RT_TypeSystem));
        memset(&g_CLR_RT_EventCache, 0, sizeof(g_CLR_RT_EventCache));
        memset(&g_CLR_RT_GarbageCollector, 0, sizeof(g_CLR_RT_GarbageCollector));
        memset(&g_CLR_HW_Hardware, 0, sizeof(g_CLR_HW_Hardware));

        m_fInitialized = false;
    }

    Settings()
    {
        m_fInitialized = false;
#if defined(VIRTUAL_DEVICE)
        m_configureRuntimeCallback = NULL;
        m_configured = false;
#endif
    }

#if defined(VIRTUAL_DEVICE)
    ~Settings()
    {
        for (CLR_RT_ParseOptions::BufferMapIter it = m_assemblies.begin(); it != m_assemblies.end(); it++)
        {
            delete it->second;
        }

        m_assemblies.clear(); // CLR_RT_ParseOptions::BufferMap m_assemblies;
    }

    HRESULT CheckAssemblyFormat(CLR_RECORD_ASSEMBLY *header, const wchar_t *src)
    {
        NANOCLR_HEADER();

        if (header->GoodAssembly() == false)
        {
            wprintf(L"Invalid assembly format for '%s': ", src);
            for (int i = 0; i < sizeof(header->marker); i++)
            {
                wprintf(L"%02x", header->marker[i]);
            }
            wprintf(L"\n");

            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }

        NANOCLR_NOCLEANUP();
    }

    HRESULT LoadAssembly(const wchar_t *name, const CLR_UINT8 *data, size_t size)
    {
        NANOCLR_HEADER();

        CLR_RT_Buffer *buffer = new CLR_RT_Buffer(data, data + size);
        CLR_RECORD_ASSEMBLY *header;

        header = (CLR_RECORD_ASSEMBLY *)&(*buffer)[0];
        NANOCLR_CHECK_HRESULT(CheckAssemblyFormat(header, name));

        m_assemblies[name] = buffer;

        NANOCLR_CLEANUP();

        if (FAILED(hr))
        {
            delete buffer;
        }

        NANOCLR_CLEANUP_END();
    }

    HRESULT LoadAssembliesSet(const CLR_UINT8 *data, size_t size)
    {
        NANOCLR_HEADER();

        if (!m_fInitialized)
        {
            CLR_RT_ExecutionEngine::CreateInstance(m_clrOptions);
        }

        {
            CLR_RT_Buffer buffer(data, data + size);
            CLR_RECORD_ASSEMBLY *header;
            CLR_RECORD_ASSEMBLY *headerEnd;
            std::wstring strName;

            header = (CLR_RECORD_ASSEMBLY *)&buffer[0];
            headerEnd = (CLR_RECORD_ASSEMBLY *)&buffer[buffer.size() - 1];

            while (header + 1 <= headerEnd && header->GoodAssembly())
            {
                CLR_RT_Buffer *bufferSub = new CLR_RT_Buffer();
                CLR_RECORD_ASSEMBLY *headerSub;
                CLR_RT_Assembly *assm;

                bufferSub->resize(header->TotalSize());

                headerSub = (CLR_RECORD_ASSEMBLY *)&(*bufferSub)[0];

                if ((CLR_UINT8 *)header + header->TotalSize() > (CLR_UINT8 *)headerEnd)
                {
                    // checksum passed, but not enough data in assembly
                    _ASSERTE(FALSE);
                    delete bufferSub;
                    break;
                }
                memcpy(headerSub, header, header->TotalSize());

                m_assemblies[strName] = bufferSub;

                if (FAILED(hr = CLR_RT_Assembly::CreateInstance(headerSub, assm)))
                {
                    delete bufferSub;
                    break;
                }

                CLR_RT_UnicodeHelper::ConvertFromUTF8(assm->m_szName, strName);
                m_assemblies[strName] = bufferSub;

                assm->DestroyInstance();

                header = (CLR_RECORD_ASSEMBLY *)ROUNDTOMULTIPLE((size_t)header + header->TotalSize(), CLR_UINT32);
            }
        }

        NANOCLR_CLEANUP();

        if (!m_fInitialized)
        {
            CLR_RT_ExecutionEngine::DeleteInstance();
        }

        NANOCLR_CLEANUP_END();
    }

    HRESULT Resolve()
    {
        NANOCLR_HEADER();

        bool fError = false;

        NANOCLR_FOREACH_ASSEMBLY(g_CLR_RT_TypeSystem)
        {
            const CLR_RECORD_ASSEMBLYREF *src = (const CLR_RECORD_ASSEMBLYREF *)pASSM->GetTable(TBL_AssemblyRef);
            for (int i = 0; i < pASSM->m_pTablesSize[TBL_AssemblyRef]; i++, src++)
            {
                const char *szName = pASSM->GetString(src->name);

                if (g_CLR_RT_TypeSystem.FindAssembly(szName, &src->version, true) == NULL)
                {
                    printf(
                        "Missing assembly: %s (%d.%d.%d.%d)\n",
                        szName,
                        src->version.iMajorVersion,
                        src->version.iMinorVersion,
                        src->version.iBuildNumber,
                        src->version.iRevisionNumber);

                    fError = true;
                }
            }
        }
        NANOCLR_FOREACH_ASSEMBLY_END();

        if (fError)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_ENTRY_NOT_FOUND);
        }

        NANOCLR_CHECK_HRESULT(g_CLR_RT_TypeSystem.ResolveAll());

        NANOCLR_NOCLEANUP();
    }
#endif // #if defined(VIRTUAL_DEVICE)
};

static Settings s_ClrSettings;

//--//

#if defined(VIRTUAL_DEVICE)
HRESULT nanoCLR_LoadAssembly(const wchar_t *name, const CLR_UINT8 *data, size_t size)
{
    return s_ClrSettings.LoadAssembly(name, data, size);
}

HRESULT nanoCLR_LoadAssembliesSet(const CLR_UINT8 *data, size_t size)
{
    return s_ClrSettings.LoadAssembliesSet(data, size);
}

HRESULT nanoCLR_Resolve()
{
    return s_ClrSettings.Resolve();
}

void nanoCLR_SetConfigureCallback(ConfigureRuntimeCallback configureRuntimeCallback)
{
    s_ClrSettings.m_configureRuntimeCallback = configureRuntimeCallback;
}

#endif

void ClrStartup(CLR_SETTINGS params)
{
    NATIVE_PROFILE_CLR_STARTUP();
    ASSERT(sizeof(CLR_RT_HeapBlock_Raw) == sizeof(struct CLR_RT_HeapBlock));
    bool softReboot;

    do
    {
        softReboot = false;

        CLR_RT_Assembly::InitString();
        CLR_RT_Memory::Reset();

#if !defined(BUILD_RTM)
        CLR_Debug::Printf("Starting...\r\n");
#endif

        HRESULT hr;

        if (SUCCEEDED(hr = s_ClrSettings.Initialize(params)))
        {
            if (SUCCEEDED(hr = s_ClrSettings.Load()))
            {
#if !defined(BUILD_RTM)
                CLR_Debug::Printf("Ready.\r\n");
#endif

                (void)g_CLR_RT_ExecutionEngine.Execute(NULL, params.MaxContextSwitches);

#if !defined(BUILD_RTM)
                CLR_Debug::Printf("Done.\r\n");
#endif
            }
        }

        // process setting of power mode, if reboot was requested along with a power mode "higher" then
        // PowerLevel__Active
        if (CLR_EE_REBOOT_IS(ClrOnly) && g_CLR_HW_Hardware.m_powerLevel > PowerLevel__Active)
        {
            CPU_SetPowerMode(g_CLR_HW_Hardware.m_powerLevel);
        }

        if (CLR_EE_DBG_IS_NOT(RebootPending))
        {
#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
            CLR_EE_DBG_SET_MASK(StateProgramExited, StateMask);
            CLR_EE_DBG_EVENT_BROADCAST(CLR_DBG_Commands::c_Monitor_ProgramExit, 0, NULL, WP_Flags_c_NonCritical);
#endif // #if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

            if (params.EnterDebuggerLoopAfterExit)
            {
                CLR_DBG_Debugger::Debugger_WaitForCommands();
            }
        }

        // DO NOT USE 'ELSE IF' here because the state can change in Debugger_WaitForCommands() call
        if (CLR_EE_DBG_IS(RebootPending))
        {
            if (CLR_EE_REBOOT_IS(ClrOnly))
            {
                softReboot = true;

                params.WaitForDebugger = CLR_EE_REBOOT_IS(WaitForDebugger);

                s_ClrSettings.Cleanup();

                nanoHAL_Uninitialize(false);

                // re-init the hal for the reboot (initially it is called in bootentry)
                nanoHAL_Initialize();
            }
            else
            {
                // equivalent to CPU_Reset();
                break;
            }
        }

    } while (softReboot);
}
