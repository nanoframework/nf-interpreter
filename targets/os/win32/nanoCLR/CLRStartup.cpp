//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "stdafx.h"

#if defined(_WIN32)


#endif

////////////////////////////////////////////////////////////////////////////////

struct Settings
#if defined(_WIN32)
: CLR_RT_ParseOptions
#endif
{
    CLR_SETTINGS m_clrOptions;
#if defined(_WIN32)
    CLR_RT_ParseOptions::BufferMap m_assemblies;
#endif
    bool m_fInitialized;

    //--//
    
    HRESULT Initialize(CLR_SETTINGS params)
    {
        NANOCLR_HEADER();

        m_clrOptions = params;

#if defined(PLATFORM_WINDOWS_EMULATOR)
        g_CLR_RT_ExecutionEngine.m_fPerformGarbageCollection = params.PerformGarbageCollection;
        g_CLR_RT_ExecutionEngine.m_fPerformHeapCompaction    = params.PerformHeapCompaction;

        CLR_UINT32 clockFrequencyBaseline = 27000000;
        CLR_UINT32 clockFrequency         = CPU_SystemClock();
        double     clockFrequencyRatio    = 1;

        if(clockFrequency > 0)
        {
            clockFrequencyRatio = (double)clockFrequencyBaseline / (double)clockFrequency;
        }

        g_HAL_Configuration_Windows.ProductType              = HAL_Configuration_Windows::Product_Aux;
        g_HAL_Configuration_Windows.SlowClockPerSecond       = 32768;
        g_HAL_Configuration_Windows.TicksPerMethodCall       = (CLR_UINT64)(45.0*clockFrequencyRatio);
        g_HAL_Configuration_Windows.TicksPerOpcode           = (CLR_UINT64)( 5.0*clockFrequencyRatio);
        g_HAL_Configuration_Windows.GraphHeapEnabled         = false;
#endif

        NANOCLR_CHECK_HRESULT(CLR_RT_ExecutionEngine::CreateInstance());
#if !defined(BUILD_RTM)
        CLR_Debug::Printf( "Created EE.\r\n" );
#endif

#if !defined(BUILD_RTM)
        if(params.WaitForDebugger)
        {
#if defined(_WIN32)
            CLR_EE_DBG_SET( Enabled );
#endif
            CLR_EE_DBG_SET( Stopped );
        }
#endif

        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.StartHardware());
#if !defined(BUILD_RTM)
        CLR_Debug::Printf( "Started Hardware.\r\n" );
#endif

        CLR_DBG_Debugger::Debugger_Discovery();

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

        // clear flag (in case EE wasn't restarted)
        CLR_EE_DBG_CLR(StateResolutionFailed);

#if defined(_WIN32)
        CLR_RT_StringVector vec;

        vec.push_back(L"-load");
#if defined _DEBUG
		vec.push_back(L"..\\netnf\\TestApplication\\bin\\Debug\\TestApplication.pe");
#else
		vec.push_back(L"..\\netnf\\TestApplication\\bin\\Release\\TestApplication.pe");
#endif

		// grab mscorlib.pe from the packages folder (it has to be there because the NF.TestApplication has just build)
		// ************************************************************************* //
		// just need to update the path on the package folder as the version changes //
		// ************************************************************************* //
		vec.push_back(L"-load");
		vec.push_back(L"..\\packages\\nanoFramework.CoreLibrary.1.0.0-preview038\\lib\\mscorlib.pe");
		
		// grab Windows.Devices.Gpio.pe from the packages folder (it has to be there because the NF.TestApplication has just build)
		// ************************************************************************* //
		// just need to update the path on the package folder as the version changes //
		// ************************************************************************* //
		vec.push_back(L"-load");
		vec.push_back(L"..\\packages\\nanoFramework.Windows.Devices.Gpio.1.0.0-preview164\\lib\\Windows.Devices.Gpio.pe");

		// grab nanoFramework.Runtime.Events.pe from the packages folder (it has to be there because the NF.TestApplication has just build)
		// ************************************************************************* //
		// just need to update the path on the package folder as the version changes //
		// ************************************************************************* //
		vec.push_back(L"-load");
		vec.push_back(L"..\\packages\\nanoFramework.Runtime.Events.1.0.0-preview165\\lib\\nanoFramework.Runtime.Events.pe");

		// grab Windows.Devices.Spi.pe from the packages folder (it has to be there because the NF.TestApplication has just build)
		// ************************************************************************* //
		// just need to update the path on the package folder as the version changes //
		// ************************************************************************* //
		vec.push_back(L"-load");
		vec.push_back(L"..\\packages\\nanoFramework.Windows.Devices.Spi.1.0.0-preview164\\lib\\Windows.Devices.Spi.pe");


        //vec.push_back(L"-load");
        //vec.push_back(L"C:\\Program Files (x86)\\Microsoft .NET Micro Framework\\v4.4\\Assemblies\\le\\Microsoft.SPOT.Native.pe");

        //wchar_t* pContext = NULL;
        //wchar_t* pch = wcstok_s(emulatorArgs, L" ", &pContext); // UNDONE: FIXME: wcstok_s(this->m_clrOptions.EmulatorArgs, L" ", &pContext);

        //while (pch != NULL)
        //{
        //    //printf ("%s\n",pch);
        //    vec.push_back(pch);

        //    pch = wcstok_s(NULL, L" ", &pContext);
        //}

        ProcessOptions(vec);

        for(CLR_RT_ParseOptions::BufferMapIter it = m_assemblies.begin(); it != m_assemblies.end(); it++)
        {
            CLR_RT_Assembly* assm;
            const CLR_RT_Buffer* buffer = (const CLR_RT_Buffer*)it->second;
            const CLR_RECORD_ASSEMBLY* header = (CLR_RECORD_ASSEMBLY*)&(*buffer)[0];

            // Creates instance of assembly, sets pointer to native functions, links to g_CLR_RT_TypeSystem 
            NANOCLR_CHECK_HRESULT( LoadAssembly( header, assm ) );
        }
#else     

#if !defined(BUILD_RTM)
        CLR_Debug::Printf( "Create Type System.\r\n" );
#endif

        NANOCLR_CHECK_HRESULT(LoadKnownAssemblies( nanoCLR_Dat_Start, nanoCLR_Dat_End ));

#endif // defined(PLATFORM_WINDOWS_EMULATOR)

#if !defined(BUILD_RTM)
        CLR_Debug::Printf( "Loading Deployment Assemblies.\r\n" );
#endif

        LoadDeploymentAssemblies( BlockUsage_DEPLOYMENT );

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

            if(hr == CLR_E_TYPE_UNAVAILABLE)
            {
                // exception occurred during type resolution
                CLR_EE_DBG_SET(StateResolutionFailed);
            }
        }
#endif

        NANOCLR_CLEANUP_END();
    }

    HRESULT CheckKnownAssembliesForNonXIP( char** start, char** end )
    {
        //--//
        NANOCLR_HEADER();

		BlockStorageDevice *device;
        ByteAddress datByteAddress;
        unsigned int datSize = ROUNDTOMULTIPLE((unsigned int)(*end)- (unsigned int)(*start), CLR_UINT32);

        if (BlockStorageList_FindDeviceForPhysicalAddress( &device, (unsigned int)(*start), &datByteAddress ) && device != NULL)
        {    
            const DeviceBlockInfo * deviceInfo= BlockStorageDevice_GetDeviceInfo(device);

            if (deviceInfo->Attribute & MediaAttribute_SupportsXIP)
            {
                unsigned char * datAssembliesBuffer = (unsigned char*)CLR_RT_Memory::Allocate_And_Erase( datSize, CLR_RT_HeapBlock ::HB_Unmovable );  CHECK_ALLOCATION(datAssembliesBuffer);

                if ( !BlockStorageDevice_Read(device, datByteAddress, datSize, datAssembliesBuffer ))
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
                }
                *start = (char *)datAssembliesBuffer;
                *end = (char *)((unsigned int) datAssembliesBuffer + (unsigned int)datSize);

            }
        }

        // else data in RAM
        NANOCLR_NOCLEANUP();
    }


    HRESULT LoadKnownAssemblies( char* start, char* end )
    {
        //--//
        NANOCLR_HEADER();
        char *assStart = start;
        char *assEnd = end;
        const CLR_RECORD_ASSEMBLY* header;

        NANOCLR_CHECK_HRESULT(CheckKnownAssembliesForNonXIP( &assStart, &assEnd ));
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


    HRESULT ContiguousBlockAssemblies( BlockStorageStream stream, bool isXIP ) 
    {
        NANOCLR_HEADER();

        const CLR_RECORD_ASSEMBLY* header;
        unsigned char * assembliesBuffer ;
        signed int  headerInBytes = sizeof(CLR_RECORD_ASSEMBLY);
        unsigned char * headerBuffer  = NULL;

        if(!isXIP)
        {
            headerBuffer = (unsigned char*)CLR_RT_Memory::Allocate( headerInBytes, true );  CHECK_ALLOCATION(headerBuffer);
            CLR_RT_Memory::ZeroFill( headerBuffer, headerInBytes );
        }

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

            if(!isXIP)
            {
                // read the assemblies
                assembliesBuffer = (unsigned char*)CLR_RT_Memory::Allocate_And_Erase( AssemblySizeInByte, CLR_RT_HeapBlock ::HB_Unmovable );
                
                if (!assembliesBuffer) 
                {
                    // release the headerbuffer which has being used and leave
                    CLR_RT_Memory::Release( headerBuffer );
                    
                    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
                }
            }

			BlockStorageStream_Seek(&stream, -headerInBytes, BlockStorageStream_SeekCurrent);

            if(!BlockStorageStream_Read(&stream, &assembliesBuffer, AssemblySizeInByte )) break;

            header = (const CLR_RECORD_ASSEMBLY*)assembliesBuffer;

            if(!header->GoodAssembly())
            {
                if(!isXIP) CLR_RT_Memory::Release( assembliesBuffer );
                break;
            }
                
            // we have good Assembly 
            CLR_RT_Assembly* assm;

            // Creates instance of assembly, sets pointer to native functions, links to g_CLR_RT_TypeSystem 
            if (FAILED(LoadAssembly( header, assm ) ))
            {   
                if(!isXIP) CLR_RT_Memory::Release( assembliesBuffer );
                break;
            }
            assm->m_flags |= CLR_RT_Assembly::Deployed;
        }
        if(!isXIP) CLR_RT_Memory::Release( headerBuffer );
        
        NANOCLR_NOCLEANUP();
    }

    HRESULT LoadDeploymentAssemblies( unsigned int memoryUsage )
    {
        NANOCLR_HEADER();

        BlockStorageStream      stream;
        const DeviceBlockInfo* deviceInfo;

        // find the block            
        if (!BlockStorageStream_Initialize(&stream, memoryUsage ))
        {
#if !defined(BUILD_RTM)
            CLR_Debug::Printf( "ERROR: Could not find device for DEPLOYMENT usage\r\n" );
#endif            
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
        }

        do
        {
            deviceInfo = BlockStorageDevice_GetDeviceInfo(stream.Device);
            
            ContiguousBlockAssemblies( stream, (deviceInfo->Attribute & MediaAttribute_SupportsXIP));
        }
        while(BlockStorageStream_NextStream(&stream));
        
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
#if defined(_WIN32)
        BuildOptions();
#endif
    }

#if defined(_WIN32)
    ~Settings()
    {
        for(CLR_RT_ParseOptions::BufferMapIter it = m_assemblies.begin(); it != m_assemblies.end(); it++)
        {
            delete it->second;
        }

        m_assemblies.clear();                            // CLR_RT_ParseOptions::BufferMap m_assemblies;
    }

    struct Command_Call : CLR_RT_ParseOptions::Command
    {
        typedef HRESULT (Settings::*FPN)( CLR_RT_ParseOptions::ParameterList* params );

        Settings& m_parent;
        FPN       m_call;

        Command_Call( Settings& parent, FPN call, const wchar_t* szName, const wchar_t* szDescription ) 
            : CLR_RT_ParseOptions::Command( szName, szDescription ), m_parent(parent), m_call(call)
        {
        }

        virtual HRESULT Execute()
        {
            return (m_parent.*m_call)( &m_params );
        }
    };

#define PARAM_GENERIC(parm1Name,parm1Desc) param = new CLR_RT_ParseOptions::Parameter_Generic( parm1Name, parm1Desc ); cmd->m_params.push_back( param )
#define OPTION_CALL(fpn,optName,optDesc)   cmd   = new Command_Call( *this, &Settings::fpn, optName, optDesc ); m_commands.push_back( cmd )
#define PARAM_EXTRACT_STRING(lst,idx)    ((CLR_RT_ParseOptions::Parameter_Generic*)(*lst)[idx])->m_data.c_str()

    void BuildOptions()
    {
        CLR_RT_ParseOptions::Command*   cmd;
        CLR_RT_ParseOptions::Parameter* param;

        OPTION_CALL( Cmd_Load, L"-load", L"Loads an assembly formatted for nanoCLR" );
        PARAM_GENERIC( L"<file>", L"File to load"                                   );
        
        OPTION_CALL( Cmd_LoadDatabase, L"-loadDatabase", L"Loads a set of assemblies" );
        PARAM_GENERIC( L"<file>", L"Image to load"                                    );

        OPTION_CALL( Cmd_Resolve, L"-resolve", L"Tries to resolve cross-assembly references" );
    }    

    HRESULT CheckAssemblyFormat( CLR_RECORD_ASSEMBLY* header, const wchar_t* src )
    {
        NANOCLR_HEADER();

        if(header->GoodAssembly() == false)
        {
            wprintf( L"Invalid assembly format for '%s': ", src );
            for(int i=0; i<sizeof(header->marker); i++)
            {
                wprintf( L"%02x", header->marker[i] );
            }
            wprintf( L"\n" );

            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }

        NANOCLR_NOCLEANUP();
    }

    HRESULT Cmd_Load( CLR_RT_ParseOptions::ParameterList* params = NULL )
    {
        NANOCLR_HEADER();

        const wchar_t*              szName = PARAM_EXTRACT_STRING( params, 0 );
        CLR_RT_Buffer*       buffer = new CLR_RT_Buffer(); 
        CLR_RECORD_ASSEMBLY* header;

        NANOCLR_CHECK_HRESULT(CLR_RT_FileStore::LoadFile( szName, *buffer ));

        header = (CLR_RECORD_ASSEMBLY*)&(*buffer)[0]; NANOCLR_CHECK_HRESULT(CheckAssemblyFormat( header, szName ));

        m_assemblies[szName] = buffer;

        NANOCLR_CLEANUP();

        if(FAILED(hr))
        {
            delete buffer;
        }

        NANOCLR_CLEANUP_END();
    }


    HRESULT Cmd_LoadDatabase( CLR_RT_ParseOptions::ParameterList* params = NULL )
    {
        NANOCLR_HEADER();

        if(!m_fInitialized)
        {
            CLR_RT_ExecutionEngine::CreateInstance();
        }

        {
            const wchar_t*              szFile = PARAM_EXTRACT_STRING( params, 0 );
            CLR_RT_Buffer        buffer;
            CLR_RECORD_ASSEMBLY* header;
            CLR_RECORD_ASSEMBLY* headerEnd;
            std::wstring         strName;
            
            NANOCLR_CHECK_HRESULT(CLR_RT_FileStore::LoadFile( szFile, buffer ));

            header    = (CLR_RECORD_ASSEMBLY*)&buffer[0              ];
            headerEnd = (CLR_RECORD_ASSEMBLY*)&buffer[buffer.size()-1];
            
            while(header + 1 <= headerEnd && header->GoodAssembly())
            {
                CLR_RT_Buffer*       bufferSub = new CLR_RT_Buffer(); 
                CLR_RECORD_ASSEMBLY* headerSub;
                CLR_RT_Assembly*     assm;
            
                bufferSub->resize( header->TotalSize() );
            
                headerSub = (CLR_RECORD_ASSEMBLY*)&(*bufferSub)[0]; 
            
                if((CLR_UINT8*)header + header->TotalSize() > (CLR_UINT8*)headerEnd)
                {
                    //checksum passed, but not enough data in assembly
                    _ASSERTE(FALSE);
                    delete bufferSub;
                    break;
                }
                memcpy( headerSub, header, header->TotalSize() );
            
                m_assemblies[strName] = bufferSub;
            
                if(FAILED(hr = CLR_RT_Assembly::CreateInstance( headerSub, assm )))
                {
                    delete bufferSub;
                    break;
                }
            
                CLR_RT_UnicodeHelper::ConvertFromUTF8( assm->m_szName, strName ); m_assemblies[strName] = bufferSub;
            
                assm->DestroyInstance();
            
                header = (CLR_RECORD_ASSEMBLY*)ROUNDTOMULTIPLE( (size_t)header + header->TotalSize(), CLR_UINT32 );
            }
        }

        NANOCLR_CLEANUP();

        if(!m_fInitialized)
        {
            CLR_RT_ExecutionEngine::DeleteInstance();
        }

        NANOCLR_CLEANUP_END();
    }

    HRESULT Cmd_Resolve( CLR_RT_ParseOptions::ParameterList* params = NULL )
    {
        NANOCLR_HEADER();

        bool fError = false;

        NANOCLR_FOREACH_ASSEMBLY(g_CLR_RT_TypeSystem)
        {
            const CLR_RECORD_ASSEMBLYREF* src = (const CLR_RECORD_ASSEMBLYREF*)pASSM->GetTable( TBL_AssemblyRef );
            for(int i=0; i<pASSM->m_pTablesSize[TBL_AssemblyRef]; i++, src++)
            {
                const char* szName = pASSM->GetString( src->name );

                if(g_CLR_RT_TypeSystem.FindAssembly( szName, &src->version, true ) == NULL)
                {
                    printf( "Missing assembly: %s (%d.%d.%d.%d)\n", szName, src->version.iMajorVersion, src->version.iMinorVersion, src->version.iBuildNumber, src->version.iRevisionNumber );

                    fError = true;
                }
            }
        }
        NANOCLR_FOREACH_ASSEMBLY_END();

        if(fError) NANOCLR_SET_AND_LEAVE(CLR_E_ENTRY_NOT_FOUND);

        NANOCLR_CHECK_HRESULT(g_CLR_RT_TypeSystem.ResolveAll());

        NANOCLR_NOCLEANUP();
    }
#endif //#if defined(_WIN32)

};

static Settings s_ClrSettings;

//--//

#if defined(_WIN32)
HRESULT ClrLoadPE( const wchar_t* szPeFilePath )
{
    CLR_RT_StringVector vec;

    vec.push_back(L"-load");

    vec.push_back(szPeFilePath);

    return s_ClrSettings.ProcessOptions(vec);
}

HRESULT ClrLoadDAT( const wchar_t* szDatFilePath )
{
    CLR_RT_StringVector vec;

    vec.push_back(L"-loadDatabase");

    vec.push_back(szDatFilePath);

    return s_ClrSettings.ProcessOptions(vec);
}
#endif


void ClrStartup( CLR_SETTINGS params )
{
    NATIVE_PROFILE_CLR_STARTUP();
    //Settings settings;
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

//#if defined(_WIN32)
//                (void)g_CLR_RT_ExecutionEngine.Execute( params.EmulatorArgs, params.MaxContextSwitches );
//#else
                (void)g_CLR_RT_ExecutionEngine.Execute( NULL, params.MaxContextSwitches );
//#endif

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
            CLR_EE_DBG_EVENT_BROADCAST(CLR_DBG_Commands::c_Monitor_ProgramExit, 0, NULL,WP_Flags_c_NonCritical);
#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

            if(params.EnterDebuggerLoopAfterExit)
            {
                CLR_DBG_Debugger::Debugger_WaitForCommands();
            }
        }

        // DO NOT USE 'ELSE IF' here because the state can change in Debugger_WaitForCommands() call
        
        if( CLR_EE_DBG_IS( RebootPending ))
        {
            if(CLR_EE_REBOOT_IS( ClrOnly ))
            {
                softReboot = true;

                params.WaitForDebugger = CLR_EE_REBOOT_IS(WaitForDebugger);

                s_ClrSettings.Cleanup();

                //nanoHAL_Uninitialize();

                //re-init the hal for the reboot (initially it is called in bootentry)
                //nanoHAL_Initialize();
            }
            else
            {
                CPU_Reset();
            }
        }
    } while( softReboot );
}
