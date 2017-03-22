//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoCLR_Application.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Types.h>
#include <CLRStartup.h>

// FIXME 
// these are dummy data, we'll have to figure out how to fill these after the build happens
char nanoCLR_Dat_Start[100 ];
char nanoCLR_Dat_End  [1 ];

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

        // UNDONE: FIXME: CLR_DBG_Debugger::Debugger_Discovery();

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
                CLR_Debug::Printf("***********************************************************************\r\n");
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

#if !defined(BUILD_RTM)
        CLR_Debug::Printf( "Create TS.\r\n" );
#endif
        // UNDONE: FIXME: NANOCLR_CHECK_HRESULT(LoadKnownAssemblies( 0 /*nanoCLR_Dat_Start*/, 0 /*nanoCLR_Dat_End*/ ));

#if !defined(BUILD_RTM)
        CLR_Debug::Printf( "Loading Deployment Assemblies.\r\n" );
#endif

        LoadDeploymentAssemblies();

        //--//

#if !defined(BUILD_RTM)
        CLR_Debug::Printf( "Resolving.\r\n" );
#endif
        NANOCLR_CHECK_HRESULT(g_CLR_RT_TypeSystem.ResolveAll());

        g_CLR_RT_Persistence_Manager.Initialize();

        // UNDONE: FIXME: NANOCLR_CHECK_HRESULT(g_CLR_RT_TypeSystem.PrepareForExecution());

#if defined(NANOCLR_PROFILE_HANDLER)
        CLR_PROF_Handler::Calibrate();
#endif

        NANOCLR_CLEANUP();

#if !defined(BUILD_RTM)
        if(FAILED(hr)) CLR_Debug::Printf( "Error: %08x\r\n", hr );
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
        unsigned char * assembliesBuffer ;
        signed int  headerInBytes = sizeof(CLR_RECORD_ASSEMBLY);
        unsigned char * headerBuffer  = NULL;

        while(TRUE)
        {
            if(!BlockStorageStream_Read(&stream, &headerBuffer, headerInBytes )) break;

            header = (const CLR_RECORD_ASSEMBLY*)headerBuffer;

            // check header first before read
            if(!header->GoodHeader())
            {
                break;
            }

            unsigned int AssemblySizeInByte = ROUNDTOMULTIPLE(header->TotalSize(), CLR_UINT32);

            BlockStorageStream_Seek(&stream, -headerInBytes, BlockStorageStream_SeekCurrent);

            if(!BlockStorageStream_Read(&stream, &assembliesBuffer, AssemblySizeInByte)) break;

            header = (const CLR_RECORD_ASSEMBLY*)assembliesBuffer;

            if(!header->GoodAssembly())
            {
                break;
            }
                
            // we have good Assembly 

            CLR_RT_Assembly* assm;

            CLR_Debug::Printf( "Attaching deployed file.\r\n" );

            // Creates instance of assembly, sets pointer to native functions, links to g_CLR_RT_TypeSystem 
            if (FAILED(LoadAssembly(header, assm)))
            {   
                break;
            }
            assm->m_flags |= CLR_RT_Assembly::c_Deployed;
        }
                
        NANOCLR_NOCLEANUP();
    }

    HRESULT LoadDeploymentAssemblies()
    {
        NANOCLR_HEADER();

        // perform initialization of BlockStorageStream structure
        BlockStorageStream stream;

        // init the stream for deployment storage
        if (!BlockStorageStream_Initialize(&stream, StorageUsage_DEPLOYMENT))
        {
#if !defined(BUILD_RTM)
            CLR_Debug::Printf( "ERROR: failed to initialize DEPLOYMENT storage\r\n" );
#endif            
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
        }

        ContiguousBlockAssemblies(stream);
        
        NANOCLR_NOCLEANUP();
    }

    void Cleanup()
    {
        g_CLR_RT_Persistence_Manager.Uninitialize();

        // UNDONE: FIXME: CLR_RT_ExecutionEngine::DeleteInstance();

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

        if( CLR_EE_DBG_IS_NOT( RebootPending ))
        {
#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
            CLR_EE_DBG_SET_MASK(State_ProgramExited, State_Mask);
            CLR_EE_DBG_EVENT_BROADCAST(CLR_DBG_Commands::c_Monitor_ProgramExit, 0, NULL, WP_Flags::c_NonCritical);
#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

            if(params.EnterDebuggerLoopAfterExit)
            {
                // UNDONE: FIXME: CLR_DBG_Debugger::Debugger_WaitForCommands();
            }
        }

        // DO NOT USE 'ELSE IF' here because the state can change in Debugger_WaitForCommands() call
        
        if( CLR_EE_DBG_IS( RebootPending ))
        {
            if(CLR_EE_REBOOT_IS( ClrOnly ))
            {
                softReboot = true;

                params.WaitForDebugger = CLR_EE_REBOOT_IS(ClrOnlyStopDebugger);
                
                s_ClrSettings.Cleanup();

                // UNDONE: FIXME: HAL_Uninitialize();

                // UNDONE: FIXME: SmartPtr_IRQ::ForceDisabled();

                //re-init the hal for the reboot (initially it is called in bootentry)
                // UNDONE: FIXME: HAL_Initialize();

                // make sure interrupts are back on
                // UNDONE: FIXME: SmartPtr_IRQ::ForceEnabled();
            }
            else
            {
                CPU_Reset();
            }
        }

    } while( softReboot );

}
