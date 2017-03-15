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

        // UNDONE: FIXME: NANOCLR_CHECK_HRESULT(LoadKnownAssemblies( nanoCLR_Dat_Start, nanoCLR_Dat_End ));

#if !defined(BUILD_RTM)
        CLR_Debug::Printf( "Loading Deployment Assemblies.\r\n" );
#endif

        // UNDONE: FIXME: LoadDeploymentAssemblies( BlockUsage::DEPLOYMENT );

        //--//

#if !defined(BUILD_RTM)
        CLR_Debug::Printf( "Resolving.\r\n" );
#endif
        // UNDONE: FIXME: NANOCLR_CHECK_HRESULT(g_CLR_RT_TypeSystem.ResolveAll());

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

    HRESULT CheckKnownAssembliesForNonXIP( char** start, char** end )
    {
        //--//
        NANOCLR_HEADER();

        BlockStorageDevice *device;
        ByteAddress datByteAddress;
        UINT32 datSize = ROUNDTOMULTIPLE((UINT32)(*end)- (UINT32)(*start), CLR_UINT32);

        if (BlockStorageList::FindDeviceForPhysicalAddress( &device, (UINT32)(*start), datByteAddress ) && device != NULL)
        {    
            const BlockDeviceInfo * deviceInfo=device->GetDeviceInfo();

            if (!deviceInfo->Attribute.SupportsXIP)
            {
                BYTE * datAssembliesBuffer = (BYTE*)CLR_RT_Memory::Allocate_And_Erase( datSize, CLR_RT_HeapBlock ::HB_Unmovable );  CHECK_ALLOCATION(datAssembliesBuffer);

                if ( !device->Read( datByteAddress, datSize, datAssembliesBuffer ))
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
                }
                *start = (char *)datAssembliesBuffer;
                *end = (char *)((UINT32) datAssembliesBuffer + (UINT32)datSize);

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
        CLR_Debug::Printf(" Loading start at %x, end %x\r\n", (UINT32)assStart, (UINT32)assEnd);
#endif 

        g_buildCRC = SUPPORT_ComputeCRC( assStart, (UINT32)assEnd -(UINT32) assStart, 0 );


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


    HRESULT ContiguousBlockAssemblies( BlockStorageStream stream, BOOL isXIP ) 
    {
        NANOCLR_HEADER();

        const CLR_RECORD_ASSEMBLY* header;
        BYTE * assembliesBuffer ;
        INT32  headerInBytes = sizeof(CLR_RECORD_ASSEMBLY);
        BYTE * headerBuffer  = NULL;

        if(!isXIP)
        {
            headerBuffer = (BYTE*)CLR_RT_Memory::Allocate( headerInBytes, true );  CHECK_ALLOCATION(headerBuffer);
            CLR_RT_Memory::ZeroFill( headerBuffer, headerInBytes );
        }

        while(TRUE)
        {
            if(!stream.Read( &headerBuffer, headerInBytes )) break;

            header = (const CLR_RECORD_ASSEMBLY*)headerBuffer;

            // check header first before read
            if(!header->GoodHeader())
            {
                break;
            }

            UINT32 AssemblySizeInByte = ROUNDTOMULTIPLE(header->TotalSize(), CLR_UINT32);

            if(!isXIP)
            {
                // read the assemblies
                assembliesBuffer = (BYTE*)CLR_RT_Memory::Allocate_And_Erase( AssemblySizeInByte, CLR_RT_HeapBlock ::HB_Unmovable );
                
                if (!assembliesBuffer) 
                {
                    // release the headerbuffer which has being used and leave
                    CLR_RT_Memory::Release( headerBuffer );
                    
                    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
                }
            }

            stream.Seek( -headerInBytes );

            if(!stream.Read( &assembliesBuffer, AssemblySizeInByte )) break;

            header = (const CLR_RECORD_ASSEMBLY*)assembliesBuffer;

            if(!header->GoodAssembly())
            {
                if(!isXIP) CLR_RT_Memory::Release( assembliesBuffer );
                break;
            }
                
            // we have good Assembly 

            CLR_RT_Assembly* assm;

            CLR_Debug::Printf( "Attaching deployed file.\r\n" );

            // Creates instance of assembly, sets pointer to native functions, links to g_CLR_RT_TypeSystem 
            if (FAILED(LoadAssembly( header, assm ) ))
            {   
                if(!isXIP) CLR_RT_Memory::Release( assembliesBuffer );
                break;
            }
            assm->m_flags |= CLR_RT_Assembly::c_Deployed;
        }
        if(!isXIP) CLR_RT_Memory::Release( headerBuffer );
        
        NANOCLR_NOCLEANUP();
    }


    HRESULT LoadDeploymentAssemblies( UINT32 memoryUsage )
    {
        NANOCLR_HEADER();

        BlockStorageStream      stream;
        const BlockDeviceInfo* deviceInfo;

        // find the block            
        if (!stream.Initialize( memoryUsage ))
        {
#if !defined(BUILD_RTM)
            CLR_Debug::Printf( "ERROR: Could not find device for DEPLOYMENT usage\r\n" );
#endif            
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
        }

        do
        {
            deviceInfo = stream.Device->GetDeviceInfo();
            
            ContiguousBlockAssemblies( stream, deviceInfo->Attribute.SupportsXIP );
        }
        while(stream.NextStream());
        
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
