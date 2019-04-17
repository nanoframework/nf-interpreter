//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "CLRStartup.h"
#include <nanoHAL.h>
#include <nanoCLR_Application.h>
#include <nanoCLR_Hardware.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Types.h>

void ClrExit()
{
    NATIVE_PROFILE_CLR_STARTUP();
    CLR_EE_DBG_SET(ExitPending);
}

void ClrReboot()
{
    NATIVE_PROFILE_CLR_STARTUP();
    CLR_EE_REBOOT_CLR;
    CLR_EE_DBG_SET(RebootPending);
}

// the CLR Startup code on Windows version is different
#ifndef WIN32

struct Settings
{
    CLR_SETTINGS m_clrOptions;
    bool m_fInitialized;

    //--//
    
    HRESULT Initialize(CLR_SETTINGS params)
    {
        NANOCLR_HEADER();

        m_clrOptions = params;

        NANOCLR_CHECK_HRESULT(CLR_RT_ExecutionEngine::CreateInstance());
#if !defined(BUILD_RTM)
        CLR_Debug::Printf( "Created EE.\r\n" );
#endif

#if !defined(BUILD_RTM)
        if(params.WaitForDebugger)
        {
            CLR_EE_DBG_SET( Stopped );
        }
#endif

        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.StartHardware());
#if !defined(BUILD_RTM)
        CLR_Debug::Printf( "Started Hardware.\r\n" );
#endif

        m_fInitialized = true;

        NANOCLR_NOCLEANUP();
    }

    HRESULT LoadAssembly( const CLR_RECORD_ASSEMBLY* header, CLR_RT_Assembly*& assm )
    {   
        NANOCLR_HEADER();
        
        const CLR_RT_NativeAssemblyData *pNativeAssmData;
        
        NANOCLR_CHECK_HRESULT(CLR_RT_Assembly::CreateInstance( header, assm ));
        
        // Get handlers for native functions in assembly
        pNativeAssmData = GetAssemblyNativeData( assm->m_szName );
        
        // If pNativeAssmData not NULL- means this assembly has native calls and there is pointer to table with native calls.
        if ( pNativeAssmData != NULL )
        {   
            // First verify that check sum in assembly object matches hardcoded check sum. 
            if ( assm->m_header->nativeMethodsChecksum != pNativeAssmData->m_checkSum )
            {
                CLR_Debug::Printf("\r\n\r\n***********************************************************************\r\n");
                CLR_Debug::Printf("*                                                                     *\r\n");
                CLR_Debug::Printf("* ERROR!!!!  Firmware version does not match managed code version!!!! *\r\n");
                CLR_Debug::Printf("*                                                                     *\r\n");
                CLR_Debug::Printf("*                                                                     *\r\n");
                CLR_Debug::Printf("* Invalid native checksum: %s 0x%08X!=0x%08X *\r\n",
                                    assm->m_szName,
                                    assm->m_header->nativeMethodsChecksum,
                                    pNativeAssmData->m_checkSum
                                 );
                CLR_Debug::Printf("*                                                                     *\r\n");
                CLR_Debug::Printf("***********************************************************************\r\n");

                NANOCLR_SET_AND_LEAVE(CLR_E_ASSM_WRONG_CHECKSUM);
            }

            // Assembly has valid pointer to table with native methods. Save it.
            assm->m_nativeCode = (const CLR_RT_MethodHandler *)pNativeAssmData->m_pNativeMethods;
        }
        g_CLR_RT_TypeSystem.Link( assm );
        NANOCLR_NOCLEANUP();
    }


    HRESULT Load()
    {
        NANOCLR_HEADER();

        // cler flag, in case EE wasn't restarted
        CLR_EE_DBG_CLR(StateResolutionFailed);

#if !defined(BUILD_RTM)
        CLR_Debug::Printf( "Create TS.\r\n" );
#endif
        //NANOCLR_CHECK_HRESULT(LoadKnownAssemblies( (char*)&__deployment_start__, (char*)&__deployment_end__ ));

#if !defined(BUILD_RTM)
        CLR_Debug::Printf( "Loading Deployment Assemblies.\r\n" );
#endif

        NANOCLR_CHECK_HRESULT(LoadDeploymentAssemblies());

        //--//

#if !defined(BUILD_RTM)
        CLR_Debug::Printf( "Resolving.\r\n" );
#endif
        NANOCLR_CHECK_HRESULT(g_CLR_RT_TypeSystem.ResolveAll());

        NANOCLR_CHECK_HRESULT(g_CLR_RT_TypeSystem.PrepareForExecution());

#if defined(NANOCLR_PROFILE_HANDLER)
        CLR_PROF_Handler::Calibrate();
#endif

        NANOCLR_CLEANUP();

#if !defined(BUILD_RTM)
        if(FAILED(hr))
        {
            CLR_Debug::Printf( "Error: %08x\r\n", hr );

            // exception occurred in assembly loading or type resolution
            CLR_EE_DBG_SET(StateResolutionFailed);
        }
#endif

        NANOCLR_CLEANUP_END();
    }

    HRESULT LoadKnownAssemblies( char* start, char* end )
    {
        //--//
        NANOCLR_HEADER();
        char *assStart = start;
        char *assEnd = end;
        const CLR_RECORD_ASSEMBLY* header;

#if !defined(BUILD_RTM)
        CLR_Debug::Printf(" Loading start at %x, end %x\r\n", (unsigned int)assStart, (unsigned int)assEnd);
#endif 

        g_buildCRC = SUPPORT_ComputeCRC( assStart, (unsigned int)assEnd -(unsigned int) assStart, 0 );


        header = (const CLR_RECORD_ASSEMBLY*)assStart;

        while((char*)header + sizeof(CLR_RECORD_ASSEMBLY) < assEnd && header->GoodAssembly())
        {
            CLR_RT_Assembly* assm;

            // Creates instance of assembly, sets pointer to native functions, links to g_CLR_RT_TypeSystem 
            NANOCLR_CHECK_HRESULT(LoadAssembly( header, assm ));
            
            header = (const CLR_RECORD_ASSEMBLY*)ROUNDTOMULTIPLE((size_t)header + header->TotalSize(), CLR_UINT32);
        }
        
        NANOCLR_NOCLEANUP();
    }


    HRESULT ContiguousBlockAssemblies(BlockStorageStream stream) 
    {
        NANOCLR_HEADER();

        const CLR_RECORD_ASSEMBLY* header;
        unsigned char* assembliesBuffer;
        uint32_t  headerInBytes = sizeof(CLR_RECORD_ASSEMBLY);
        unsigned char* headerBuffer  = NULL;

        // for the context it's being used (read the assemblies) 
        // XIP and memory mapped block regions are equivalent so they can be ORed
        bool isXIP = (stream.Flags & BLOCKSTORAGESTREAM_c_BlockStorageStream__XIP) ||
                     (stream.Flags & BLOCKSTORAGESTREAM_c_BlockStorageStream__MemoryMapped);

        if(!isXIP)
        {
            headerBuffer = (unsigned char*)platform_malloc(headerInBytes);

            if (headerBuffer == NULL) 
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
            }            

            // clear the buffer
            memset(headerBuffer, 0, headerInBytes);
        }

        while(stream.CurrentIndex < stream.Length)
        {
            // check if there is enough stream length to continue
            if((stream.Length - stream.CurrentIndex ) < headerInBytes)
            {
                // not enough stream to read, leave now
                break;
            }

            if(!BlockStorageStream_Read(&stream, &headerBuffer, headerInBytes ))
            {
                // failed to read 
                break;
            }

            header = (const CLR_RECORD_ASSEMBLY*)headerBuffer;

            // check header first before read
            if(!header->GoodHeader())
            {
                // check failed, try to continue to the next 
                continue;
            }

            unsigned int assemblySizeInByte = ROUNDTOMULTIPLE(header->TotalSize(), CLR_UINT32);

            if(!isXIP)
            {
                // setup buffer for assembly
                /////////////////////////////////////////////////////////////////////////////////////
                // because we need to map to the functions, the assembly needs to remain on memory //
                // so platform_free can't be called for this                                       //
                /////////////////////////////////////////////////////////////////////////////////////
                assembliesBuffer = (unsigned char*)platform_malloc(assemblySizeInByte);

                if (assembliesBuffer == NULL) 
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
                }

                // clear the buffer
                memset(assembliesBuffer, 0, assemblySizeInByte);
            }

            // advance stream beyond header
            BlockStorageStream_Seek(&stream, -headerInBytes, BlockStorageStream_SeekCurrent);

            // read the assembly
            if(!BlockStorageStream_Read(&stream, &assembliesBuffer, assemblySizeInByte))
            {
                // something wrong with the read, can't continue
                break;
            }

            header = (const CLR_RECORD_ASSEMBLY*)assembliesBuffer;

            if(!header->GoodAssembly())
            {
                // check failed, try to continue to the next

                if(!isXIP && assembliesBuffer != NULL)
                {
                    // release the assembliesBuffer
                    platform_free(assembliesBuffer);
                }

                continue;
            }
                
            // we have good Assembly 
            CLR_RT_Assembly* assm;

#if !defined(BUILD_RTM)            
            CLR_Debug::Printf( "Attaching deployed file.\r\n" );
#endif
            
            // Creates instance of assembly, sets pointer to native functions, links to g_CLR_RT_TypeSystem 
            if (FAILED(LoadAssembly(header, assm)))
            {
                // load failed, try to continue to the next

                if(!isXIP && assembliesBuffer != NULL)
                {
                    // release the assembliesBuffer which has being used and leave
                    platform_free(assembliesBuffer);
                }

                continue;
            }

            // load successfull, mark as deployed
            assm->m_flags |= CLR_RT_Assembly::Deployed;
        }

        if(!isXIP && headerBuffer != NULL)
        {
            // release the headerbuffer which has being used and leave
            platform_free(headerBuffer);
        }
                
        NANOCLR_NOCLEANUP();
    }

    HRESULT LoadDeploymentAssemblies()
    {
        NANOCLR_HEADER();

        // perform initialization of BlockStorageStream structure
        BlockStorageStream stream;
        memset(&stream, 0, sizeof(BlockStorageStream));

        // init the stream for deployment storage
        if (!BlockStorageStream_Initialize(&stream, BlockUsage_DEPLOYMENT))
        {
#if !defined(BUILD_RTM)
            CLR_Debug::Printf( "ERROR: failed to initialize DEPLOYMENT storage\r\n" );
#endif            
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
        }

        NANOCLR_CHECK_HRESULT(ContiguousBlockAssemblies(stream));
        
        NANOCLR_NOCLEANUP();
    }

    void Cleanup()
    {
        CLR_RT_ExecutionEngine::DeleteInstance();
    
        memset( &g_CLR_RT_ExecutionEngine, 0, sizeof(g_CLR_RT_ExecutionEngine));
        memset( &g_CLR_RT_WellKnownTypes, 0, sizeof(g_CLR_RT_WellKnownTypes));
        memset( &g_CLR_RT_WellKnownMethods, 0, sizeof(g_CLR_RT_WellKnownMethods));
        memset( &g_CLR_RT_TypeSystem, 0, sizeof(g_CLR_RT_TypeSystem));
        memset( &g_CLR_RT_EventCache, 0, sizeof(g_CLR_RT_EventCache));
        memset( &g_CLR_RT_GarbageCollector, 0, sizeof(g_CLR_RT_GarbageCollector));
        memset( &g_CLR_HW_Hardware, 0, sizeof(g_CLR_HW_Hardware));        

        m_fInitialized = false;
    }

    Settings()
    {
        m_fInitialized = false;
    }

};


static Settings s_ClrSettings;

void ClrStartup(CLR_SETTINGS params)
{
    NATIVE_PROFILE_CLR_STARTUP();
    Settings settings;
    ASSERT(sizeof(CLR_RT_HeapBlock_Raw) == sizeof(CLR_RT_HeapBlock));
    bool softReboot;

    do
    {
        softReboot = false;

        CLR_RT_Assembly::InitString();

#if !defined(BUILD_RTM)
        CLR_Debug::Printf( "\r\nnanoCLR (Build %d.%d.%d.%d)\r\n\r\n", VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD, VERSION_REVISION );
        CLR_Debug::Printf( "\r\n%s\r\n\r\n", OEMSYSTEMINFOSTRING );
#endif

        CLR_RT_Memory::Reset();
        
#if !defined(BUILD_RTM)
        CLR_Debug::Printf( "Starting...\r\n" );
#endif


        HRESULT hr;

        if(SUCCEEDED(hr = s_ClrSettings.Initialize(params)))
        {
            if(SUCCEEDED(hr = s_ClrSettings.Load()))
            {
#if !defined(BUILD_RTM)
                CLR_Debug::Printf( "Ready.\r\n" );
#endif

                (void)g_CLR_RT_ExecutionEngine.Execute( NULL, params.MaxContextSwitches );

#if !defined(BUILD_RTM)
                CLR_Debug::Printf( "Done.\r\n" );
#endif
            }
        }

        // process setting of power mode, if reboot was requested along with a power mode "higher" then PowerLevel__Active
        if(CLR_EE_REBOOT_IS( ClrOnly ) && g_CLR_HW_Hardware.m_powerLevel > PowerLevel__Active)
        {
            CPU_SetPowerMode(g_CLR_HW_Hardware.m_powerLevel);
        }

        if( CLR_EE_DBG_IS_NOT( RebootPending ))
        {
#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
            CLR_EE_DBG_SET_MASK(StateProgramExited, StateMask);
            CLR_EE_DBG_EVENT_BROADCAST(CLR_DBG_Commands::c_Monitor_ProgramExit, 0, NULL, WP_Flags_c_NonCritical);
#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

#if !defined(BUILD_RTM)
            if(params.EnterDebuggerLoopAfterExit)
            {
                CLR_DBG_Debugger::Debugger_WaitForCommands();
            }
#endif
        }

        // DO NOT USE 'ELSE IF' here because the state can change in Debugger_WaitForCommands() call
        
        if( CLR_EE_DBG_IS( RebootPending ))
        {
            if(CLR_EE_REBOOT_IS( ClrOnly ))
            {
                softReboot = true;

                params.WaitForDebugger = CLR_EE_REBOOT_IS(WaitForDebugger);
                
                s_ClrSettings.Cleanup();

                nanoHAL_Uninitialize();

                //re-init the hal for the reboot (initially it is called in bootentry)
                nanoHAL_Initialize();
            }
            else
            {
                CPU_Reset();
            }
        }

    } while( softReboot );

  #if !defined(BUILD_RTM)
    CLR_Debug::Printf( "Exiting.\r\n" );
  #endif

    CPU_Reset();
}

#endif // WIN32
