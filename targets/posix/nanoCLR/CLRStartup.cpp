//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

// POSIX-specific CLRStartup implementation.
// Replaces src/CLR/Startup/CLRStartup.cpp in the POSIX build so that
// assembly loading from in-memory buffers is supported (virtual-device style)
// without requiring VIRTUAL_DEVICE to be defined (which would pull in Windows
// headers through nf_errors_exceptions.h and nanoCLR_PlatformDef.h).

#include <nanoCLR_Runtime.h>
#include <nanoCLR_Application.h>
#include <nanoCLR_Hardware.h>
#include "nanoCLR_native.h"

#include <map>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// CLR_RT_Buffer / CLR_RT_StringVector are normally defined inside #if _WIN32
// in nanoCLR_Runtime.h.  Provide the POSIX equivalents locally.
#if !defined(_WIN32)
typedef std::vector<uint8_t> CLR_RT_Buffer;
#endif

// ── Startup helpers (always needed) ─────────────────────────────────────────

extern "C" void ClrExit()
{
    NATIVE_PROFILE_CLR_STARTUP();
    CLR_EE_DBG_SET(ExitPending);
}

extern "C" void ClrReboot()
{
    NATIVE_PROFILE_CLR_STARTUP();
    CLR_EE_REBOOT_CLR;
    CLR_EE_DBG_SET(RebootPending);
}

// ── Forward declarations ────────────────────────────────────────────────────

static std::string Char16ToString(const char16_t *w);

// ── Settings struct ──────────────────────────────────────────────────────────

struct Settings
{
    CLR_SETTINGS m_clrOptions;
    // Map keyed by assembly name (ASCII string from binary header, or converted from char16_t name)
    std::map<std::string, CLR_RT_Buffer *> m_assemblies;
    ConfigureRuntimeCallback m_configureRuntimeCallback;
    bool m_fInitialized;
    bool m_configured;

    //--//

    HRESULT Initialize(CLR_SETTINGS const &params)
    {
        NANOCLR_HEADER();

        m_clrOptions = params;

        NANOCLR_CHECK_HRESULT(CLR_RT_ExecutionEngine::CreateInstance());
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

    // Load one assembly from a CLR record header (already-validated binary).
    HRESULT LoadAssembly(const CLR_RECORD_ASSEMBLY *header, CLR_RT_Assembly *&assm)
    {
        NANOCLR_HEADER();

        const CLR_RT_NativeAssemblyData *pNativeAssmData;

        NANOCLR_CHECK_HRESULT(CLR_RT_Assembly::CreateInstance(header, assm));

        pNativeAssmData = GetAssemblyNativeData(assm->m_szName);
        if (pNativeAssmData != NULL)
        {
            if (assm->m_header->nativeMethodsChecksum != pNativeAssmData->m_checkSum)
            {
                CLR_Debug::Printf(
                    "\r\n\r\n***********************************************************************\r\n");
                CLR_Debug::Printf("*                                                                     *\r\n");
                CLR_Debug::Printf("* ERROR!!!!  Firmware version does not match managed code version!!!! *\r\n");
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

            assm->m_nativeCode = (const CLR_RT_MethodHandler *)pNativeAssmData->m_pNativeMethods;
        }

        g_CLR_RT_TypeSystem.Link(assm);
        NANOCLR_NOCLEANUP();
    }

    // Called by nanoCLR_LoadAssembly – validates header and adds buffer to map.
    HRESULT LoadAssembly(const char16_t *name, const uint8_t *data, size_t size)
    {
        NANOCLR_HEADER();

        CLR_RT_Buffer *buffer = new CLR_RT_Buffer(data, data + size);
        CLR_RECORD_ASSEMBLY *header = (CLR_RECORD_ASSEMBLY *)&(*buffer)[0];
        std::string key;

        if (!header->GoodAssembly())
        {
            delete buffer;
            buffer = nullptr;
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }

        key = Char16ToString(name);
        m_assemblies[key] = buffer;
        buffer = nullptr; // ownership transferred

        NANOCLR_CLEANUP();

        if (FAILED(hr) && buffer != nullptr)
        {
            delete buffer;
        }

        NANOCLR_CLEANUP_END();
    }

    // Called by nanoCLR_LoadAssembliesSet – parses a concatenated .pe blob.
    HRESULT LoadAssembliesSet(const uint8_t *data, size_t size)
    {
        NANOCLR_HEADER();

        CLR_RT_Buffer bulk(data, data + size);
        CLR_RECORD_ASSEMBLY *header = (CLR_RECORD_ASSEMBLY *)&bulk[0];
        CLR_RECORD_ASSEMBLY *headerEnd = (CLR_RECORD_ASSEMBLY *)(&bulk[bulk.size() - 1]);

        while (header + 1 <= headerEnd && header->GoodAssembly())
        {
            size_t asmSize = header->TotalSize();

            if ((uint8_t *)header + asmSize > (uint8_t *)headerEnd + 1)
                break;

            CLR_RT_Buffer *bufferSub = new CLR_RT_Buffer((uint8_t *)header, (uint8_t *)header + asmSize);
            CLR_RECORD_ASSEMBLY *headerSub = (CLR_RECORD_ASSEMBLY *)&(*bufferSub)[0];
            CLR_RT_Assembly *assm;

            if (FAILED(hr = CLR_RT_Assembly::CreateInstance(headerSub, assm)))
            {
#if !defined(BUILD_RTM)
                CLR_Debug::Printf("LoadAssembliesSet: CreateInstance failed (hr=0x%08X) — stopping parse.\r\n", hr);
#endif
                delete bufferSub;
                break;
            }

            std::string key(assm->m_szName);
            assm->DestroyInstance();

            m_assemblies[key] = bufferSub;

            header =
                (CLR_RECORD_ASSEMBLY *)ROUNDTOMULTIPLE((uintptr_t)header + asmSize, CLR_UINT32);
        }

        // Return S_OK even if some assemblies failed to parse – same behaviour
        // as the Windows implementation.
        hr = S_OK;
        NANOCLR_CLEANUP();
        NANOCLR_CLEANUP_END();
    }

    // Resolve all assembly references.
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

    // Load assemblies from block storage (deployment).  Allowed to fail.
    HRESULT LoadDeploymentAssemblies(unsigned int memoryUsage)
    {
        NANOCLR_HEADER();

        BlockStorageStream stream;
        memset(&stream, 0, sizeof(BlockStorageStream));

        if (!BlockStorageStream_Initialize(&stream, memoryUsage))
        {
            // No block storage on POSIX — deployment from flash is not available.
            // Assemblies are loaded directly via nanoCLR_LoadAssembly(); this is not an error.
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
        }

        do
        {
            const DeviceBlockInfo *deviceInfo = BlockStorageDevice_GetDeviceInfo(stream.Device);
            ContiguousBlockAssemblies(stream, (deviceInfo->Attribute & MediaAttribute_SupportsXIP));
        } while (BlockStorageStream_NextStream(&stream));

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

            if (!header->GoodHeader())
                break;

            unsigned int AssemblySizeInByte = ROUNDTOMULTIPLE(header->TotalSize(), CLR_UINT32);

            if (!isXIP)
            {
                assembliesBuffer = (unsigned char *)CLR_RT_Memory::Allocate_And_Erase(
                    AssemblySizeInByte,
                    CLR_RT_HeapBlock::HB_Unmovable);

                if (!assembliesBuffer)
                {
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
                    CLR_RT_Memory::Release(assembliesBuffer);
                break;
            }

            CLR_RT_Assembly *assm;
            if (FAILED(LoadAssembly(header, assm)))
            {
                if (!isXIP)
                    CLR_RT_Memory::Release(assembliesBuffer);
                break;
            }

            assm->m_flags |= CLR_RT_Assembly::Deployed;
        }

        if (!isXIP)
            CLR_RT_Memory::Release(headerBuffer);

        NANOCLR_NOCLEANUP();
    }

    HRESULT Load()
    {
        NANOCLR_HEADER();

        CLR_EE_DBG_CLR(StateResolutionFailed);

        if (!m_configured && m_configureRuntimeCallback)
        {
            if ((hr = m_configureRuntimeCallback()) != S_OK)
            {
                NANOCLR_LEAVE();
            }
            m_configured = true;
        }

#if !defined(BUILD_RTM)
        CLR_Debug::Printf("Loading Assemblies.\r\n");
#endif

        for (auto &it : m_assemblies)
        {
            const CLR_RT_Buffer *buf = it.second;
            const CLR_RECORD_ASSEMBLY *header = (const CLR_RECORD_ASSEMBLY *)&(*buf)[0];
            CLR_RT_Assembly *assm;
            NANOCLR_CHECK_HRESULT(LoadAssembly(header, assm));
        }

#if !defined(BUILD_RTM)
        CLR_Debug::Printf("Loading Deployment Assemblies.\r\n");
#endif

        // Deployment assemblies – allowed to fail on POSIX (no flash storage).
        (void)LoadDeploymentAssemblies(BlockUsage_DEPLOYMENT);

#if !defined(BUILD_RTM)
        CLR_Debug::Printf("Resolving.\r\n");
#endif

        NANOCLR_CHECK_HRESULT(g_CLR_RT_TypeSystem.ResolveAll());
        NANOCLR_CHECK_HRESULT(g_CLR_RT_TypeSystem.PrepareForExecution());

        NANOCLR_CLEANUP();

#if !defined(BUILD_RTM)
        if (FAILED(hr))
        {
            CLR_Debug::Printf("Error: %08x\r\n", hr);
            if (hr == CLR_E_TYPE_UNAVAILABLE)
            {
                CLR_EE_DBG_SET(StateResolutionFailed);
            }
        }
#endif

        NANOCLR_CLEANUP_END();
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

        for (auto &it : m_assemblies)
            delete it.second;
        m_assemblies.clear();

        m_fInitialized = false;
    }

    Settings() : m_configureRuntimeCallback(nullptr), m_fInitialized(false), m_configured(false)
    {
    }

    ~Settings()
    {
        Cleanup();
    }
};

static Settings s_ClrSettings;

// ── Helper: char16_t* (UTF-16 LE from C# CharSet.Unicode) → std::string ────
// Assembly names are always ASCII; just take the low byte of each code unit.
static std::string Char16ToString(const char16_t *w)
{
    if (!w)
        return std::string();

    std::string s;
    while (*w)
    {
#if !defined(BUILD_RTM)
        // Assembly names should be ASCII only
        if ((*w & ~0x7F) != 0)
        {
            CLR_Debug::Printf("Warning: non-ASCII character in assembly name\r\n");
        }
#endif
        s += (char)(*w & 0xFF);
        ++w;
    }
    return s;
}

// ── Public assembly-loading API (called from nanoCLR_native_posix.cpp) ──────

HRESULT nanoCLR_LoadAssemblyImpl(const char16_t *name, const uint8_t *data, size_t size)
{
    return s_ClrSettings.LoadAssembly(name, data, size);
}

HRESULT nanoCLR_LoadAssembliesSetImpl(const uint8_t *data, size_t size)
{
    return s_ClrSettings.LoadAssembliesSet(data, size);
}

HRESULT nanoCLR_ResolveImpl()
{
    return s_ClrSettings.Resolve();
}

void nanoCLR_SetConfigureCallbackImpl(ConfigureRuntimeCallback cb)
{
    s_ClrSettings.m_configureRuntimeCallback = cb;
}

// ── ClrStartup ──────────────────────────────────────────────────────────────

extern "C" void ClrStartup(CLR_SETTINGS params)
{
    NATIVE_PROFILE_CLR_STARTUP();
    ASSERT(sizeof(CLR_RT_HeapBlock_Raw) == sizeof(struct CLR_RT_HeapBlock));
    bool softReboot;

    do
    {
        softReboot = false;

        CLR_RT_Assembly::InitString();
        CLR_RT_Memory::Reset();

        HRESULT hr;

        if (SUCCEEDED(hr = s_ClrSettings.Initialize(params)))
        {
            if (SUCCEEDED(hr = s_ClrSettings.Load()))
            {
#if !defined(BUILD_RTM)
                CLR_Debug::Printf("Ready.\r\n");
                fflush(stdout);
#endif

                hr = g_CLR_RT_ExecutionEngine.Execute(NULL, params.MaxContextSwitches);

#if !defined(BUILD_RTM)
                CLR_Debug::Printf("Done. hr=0x%08X\r\n", hr);
                fflush(stdout);
#endif
            }
        }

        if (CLR_EE_REBOOT_IS(ClrOnly) && g_CLR_HW_Hardware.m_powerLevel > PowerLevel__Active)
        {
            CPU_SetPowerMode(g_CLR_HW_Hardware.m_powerLevel);
        }

        if (CLR_EE_DBG_IS_NOT(RebootPending))
        {
#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
            CLR_EE_DBG_SET_MASK(StateProgramExited, StateMask);
            CLR_EE_DBG_EVENT_BROADCAST(CLR_DBG_Commands_c_Monitor_ProgramExit, 0, NULL, WP_Flags_c_NonCritical);
#endif

            if (params.EnterDebuggerLoopAfterExit)
            {
                CLR_DBG_Debugger::Debugger_WaitForCommands();
            }
        }

        if (CLR_EE_DBG_IS(RebootPending))
        {
            if (CLR_EE_REBOOT_IS(ClrOnly))
            {
                softReboot = true;

                params.WaitForDebugger = CLR_EE_REBOOT_IS(WaitForDebugger);

                s_ClrSettings.Cleanup();

                nanoHAL_Uninitialize(false);
                nanoHAL_Initialize();
            }
            else if (CLR_EE_REBOOT_IS(EnterNanoBooter))
            {
                CPU_Reset();
            }
            else if (CLR_EE_REBOOT_IS(EnterProprietaryBooter))
            {
                LaunchProprietaryBootloader();
            }
        }

    } while (softReboot);

#if !defined(BUILD_RTM)
    CLR_Debug::Printf("Exiting.\r\n");
#endif
}
//
