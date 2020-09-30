//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "stdafx.h"

#include "platform_selector.h"
//--//

//using namespace System;
//using namespace System::Diagnostics;
//using namespace Microsoft::SPOT::Emulator;

bool HAL_Windows_IsShutdownPending()
{
    return false; //return EmulatorNative::GetEmulatorNative()->IsShuttingDown();
}

void HAL_Windows_AcquireGlobalLock()
{
    // UNDONE: FIXME: EmulatorNative::GetEmulatorNative()->DisableInterrupts();
}

void HAL_Windows_ReleaseGlobalLock()
{
    // UNDONE: FIXME: EmulatorNative::GetEmulatorNative()->EnableInterrupts();
}

bool HAL_Windows_HasGlobalLock()
{
    return false; // UNDONE: FIXME: !EmulatorNative::GetEmulatorNative()->AreInterruptsEnabled();
}

uint64_t HAL_Windows_GetPerformanceTicks()
{
    return 0; // UNDONE: FIXME: return EmulatorNative::GetEmulatorNative()->GetCurrentTicks();
}

void HAL_Windows_Debug_Print( char* szText )
{
    // UNDONE: FIXME: System::Diagnostics::Debug::Print( gcnew System::String(szText) );
}


//HAL_Configuration_Windows g_HAL_Configuration_Windows;

//unsigned int LOAD_IMAGE_CalcCRC;

//OEM_MODEL_SKU OEM_Model_SKU = { 1, 2, 3 };

HAL_SYSTEM_CONFIG HalSystemConfig =
{
    { true }, // HAL_DRIVER_CONFIG_HEADER Header;

    //--//

                                                   // unsigned int      DebuggerPorts[MAX_DEBUGGERS];
    DEBUGGER_PORT,

    DEBUG_TEXT_PORT,
    921600,
    0,  // STDIO = COM2 or COM1

    { 0, 0 },   // { SRAM1_MEMORY_Base, SRAM1_MEMORY_Size },
    { 0, 0 },   // { FLASH_MEMORY_Base, FLASH_MEMORY_Size },
};

bool ConfigurationManager_GetConfigurationBlock(void* configurationBlock, DeviceConfigurationOption configuration, uint32_t configurationIndex)
{
	return true;
}

bool ConfigurationManager_StoreConfigurationBlock(void* configurationBlock, DeviceConfigurationOption configuration, uint32_t configurationIndex, uint32_t blockSize, bool done)
{
	return true;
}

int mbedtls_base64_encode(unsigned char *dst, size_t dlen, size_t *olen,
	const unsigned char *src, size_t slen)
{
	return 0;
}

int mbedtls_base64_decode(unsigned char *dst, size_t dlen, size_t *olen,
	const unsigned char *src, size_t slen)
{
	return 0;
}
__nfweak void NFReleaseInfo::Init(
	NFReleaseInfo& NFReleaseInfo,
	unsigned short int major,
	unsigned short int minor,
	unsigned short int build,
	unsigned short int revision,
	const char *info,
	size_t infoLen,
	const char *target,
	size_t targetLen,
	const char *platform,
	size_t platformLen)
{

}
//const ConfigurationSector g_ConfigurationSector =
//{
//    // ConfigurationLength
//    offsetof(ConfigurationSector, FirstConfigBlock),
//
//    //CONFIG_SECTOR_VERSION
//    {
//        ConfigurationSector::c_CurrentVersionMajor,
//        ConfigurationSector::c_CurrentVersionMinor,
//        ConfigurationSector::c_CurrentVersionBooter,
//        0, // extra
//    },
//
//    // backwards compatibility buffer (88 bytes to put booterflagarray at offset 96)
//    {
//        0x0,
//    },
//
//    // BooterFlagArray - determines if we enter the bootloader or not
//    {
//        0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
//        0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
//        0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
//        0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
//        0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
//    },
//
//    // unsigned int SectorSignatureCheck[9*8]; // 287 sectors max * 8 changes before erase
//    {
//        0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
//        0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
//        0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
//        0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
//        0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
//        0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
//        0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
//        0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
//    },
//
//    //NANOBOOTER_KEY_CONFIG DeploymentKey =
//    { 
//        {// ODM key configuration for programming firmware (non deployment sectors)
//            { // ODM public key for firware sectors
//                // exponent length
//                0xFF,0xFF,0xFF, 0xFF,
//
//                // module
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   
//
//                // exponent
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   
//            }
//        },
//        {// OEM key configuration for programming Deployment sector
//            { // OEM public key for Deployment sector
//                // exponent length
//                0xFF,0xFF,0xFF, 0xFF,
//
//                // module
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   
//
//                // exponent
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,
//                0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   
//            }
//        }
//    },
//
//    //--//--//--//
//
//    // OEM_MODEL_SKU OEM_Model_SKU;
//    {
//        1,     // unsigned char   OEM;
//        2,     // unsigned char   Model;
//        3,     // unsigned short int  SKU;
//    },
//
//    //--//--//--//
//
//    // OEM_SERIAL_NUMBERS OemSerialNumbers
//    {
//        { 0, },
//        { 0, }    // unsigned char system_serial_number[16];
//    },
//
//    // CLR Config Data
//    {
//        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
//        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
//        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
//        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
//        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
//        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
//        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
//        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
//        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
//        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
//
//        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
//        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
//        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
//        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
//        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
//        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
//        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
//        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
//        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
//        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
//
//        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
//        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
//        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
//        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
//        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
//        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
//        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
//    },
//
//    //--//--//--//
//
//    // HAL_CONFIG_BLOCK FirstConfigBlock;
//    {
//        HAL_CONFIG_BLOCK::c_Version_V2, // unsigned int Signature;
//        0x8833794c,                     // unsigned int HeaderCRC;
//        0x00000000,                     // unsigned int DataCRC;
//        0x00000000,                     // unsigned int Size;
//                                        // char   DriverName[64];
//    },
//};

////////////////////////////////////////////////////////////////////////////////////////////////////

static ON_SOFT_REBOOT_HANDLER s_rebootHandlers[5] = {NULL, NULL, NULL, NULL, NULL};

void __cdecl HAL_AddSoftRebootHandler(ON_SOFT_REBOOT_HANDLER handler)
{
    for(int i=0; i<ARRAYSIZE(s_rebootHandlers); i++)
    {
        if(s_rebootHandlers[i] == NULL)
        {
            s_rebootHandlers[i] = handler;
            return;
        }
        else if(s_rebootHandlers[i] == handler)
        {
            return;
        }
    }
}

bool g_fDoNotUninitializeDebuggerPort = false;

void __cdecl nanoHAL_Initialize(void)
{
	HAL_CONTINUATION::InitializeList();
	HAL_COMPLETION::InitializeList();

	Events_Initialize();
}

void __cdecl nanoHAL_Uninitialize(void)
{
    int i;

    // UNDONE: FIXME: CPU_GPIO_Uninitialize();

    for(i=0; i<ARRAYSIZE(s_rebootHandlers); i++)
    {
        if(s_rebootHandlers[i] != NULL)
        {
            s_rebootHandlers[i]();
            return;
        }
    }       
}

void HAL_EnterBooterMode()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

//#if !defined(BUILD_RTM)
//void __cdecl HARD_Breakpoint()
//{
//    if(::IsDebuggerPresent())
//    {
//        ::DebugBreak();
//    }
//}
//#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

//HAL_Mutex::HAL_Mutex()
//{
//    ::InitializeCriticalSection( &m_data );
//}
//
//HAL_Mutex::~HAL_Mutex()
//{
//    ::DeleteCriticalSection( &m_data );
//}
//
//void HAL_Mutex::Lock()
//{
//    ::EnterCriticalSection( &m_data );
//}
//
//void HAL_Mutex::Unlock()
//{
//    ::LeaveCriticalSection( &m_data );
//}

////////////////////////////////////////////////////////////////////////////////////////////////////

void CPU_ChangePowerLevel(POWER_LEVEL level)
{
    switch(level)
    {
        case POWER_LEVEL__MID_POWER:
            break;

        case POWER_LEVEL__LOW_POWER:
            break;

        case POWER_LEVEL__HIGH_POWER:
        default:
            break;
    }
}

void CPU_Hibernate()
{
    int64_t start = ::HAL_Time_CurrentTime();

    while(true)
    {
        //wait on SYSTEM_EVENT_FLAG_DEBUGGER_ACTIVITY as well??
        unsigned int mask = ::Events_WaitForEvents( SLEEP_LEVEL__SLEEP, SYSTEM_EVENT_FLAG_COM_IN | SYSTEM_EVENT_HW_INTERRUPT /*| SYSTEM_EVENT_FLAG_BUTTON */, 1000);

        if(mask)
        {
            break;
        }
    }
}

void CPU_Shutdown()
{
    ClrExit();
}

void CPU_Reset()
{
    ::ExitProcess( 0 );
}

bool CPU_IsSoftRebootSupported ()
{
    return TRUE;
}

void CPU_SetPowerMode(PowerLevel_type powerLevel)
{
	(void)powerLevel;
}

char nanoCLR_Dat_Start[512*1024];
char nanoCLR_Dat_End  [1       ];

////////////////////////////////////////////////////////////////////////////////////////////////////

void SecurityKey_Copy( unsigned char KeyCopy[], signed int BytesToCopy )
{
    memset( KeyCopy, 0, BytesToCopy );
}

void SecurityKey_LowLevelCopy( unsigned char KeyCopy[], signed int BytesToCopy )
{
    memset( KeyCopy, 0, BytesToCopy );
}

void SecurityKey_Print()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_Windows_FastSleep( signed __int64 ticks )
{
    LARGE_INTEGER frequency;
    LARGE_INTEGER countStart;
    LARGE_INTEGER countEnd;

    if(ticks > 0)
    {
        ::QueryPerformanceFrequency( &frequency  );

        ::QueryPerformanceCounter  ( &countStart );

        double ratio = (double)TIME_CONVERSION__TO_SECONDS / (double)frequency.QuadPart;

        countEnd.QuadPart = (signed __int64)(((countStart.QuadPart * ratio) + ticks) / ratio);

        while(countStart.QuadPart < countEnd.QuadPart)
        {
            ::QueryPerformanceCounter( &countStart );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma managed(push, off)


int hal_printf( const char* format, ... )
{
    va_list arg_ptr;

    va_start(arg_ptr, format);

    int chars = hal_vprintf( format, arg_ptr );

    va_end( arg_ptr );

    return chars;
}

int hal_vprintf( const char* format, va_list arg )
{
    return vprintf( format, arg );
}


int hal_fprintf( COM_HANDLE stream, const char* format, ... )
{
    va_list arg_ptr;
    int     chars;

    va_start( arg_ptr, format );

    chars = hal_vfprintf( stream, format, arg_ptr );

    va_end( arg_ptr );

    return chars;
}

int hal_vfprintf( COM_HANDLE stream, const char* format, va_list arg )
{
    char buffer[512];
    int chars = 0;

    chars = hal_vsnprintf( buffer, sizeof(buffer), format, arg );

    //switch(ExtractTransport(stream))
    //{
    //default:
        DebuggerPort_Write( stream, buffer, chars); // skip null terminator
    //    break;

    //case FLASH_WRITE_TRANSPORT:
    //    _ASSERTE(FALSE);
    //}

    return chars;
}

int hal_snprintf( char* buffer, size_t len, const char* format, ... )
{
    va_list arg_ptr;
    int     chars;

    va_start( arg_ptr, format );

    chars = hal_vsnprintf( buffer, len, format, arg_ptr );

    va_end( arg_ptr );

    return chars;
}

int hal_vsnprintf( char* buffer, size_t len, const char* format, va_list arg )
{
    return _vsnprintf_s( buffer, len, len-1/* force space for trailing zero*/, format, arg );
}


// Compares 2 ASCII strings case insensitive. Does not take locale into account.
int hal_stricmp(const char * dst, const char * src)
{
	int f = 0, l = 0;

	do
	{
		if (((f = (unsigned char)(*(dst++))) >= 'A') && (f <= 'Z'))
		{
			f -= 'A' - 'a';
		}
		if (((l = (unsigned char)(*(src++))) >= 'A') && (l <= 'Z'))
		{
			l -= 'A' - 'a';
		}
	} while (f && (f == l));

	return(f - l);
}

#pragma managed(pop)

/////////////////////////////////////////////////////////////////////////////
//
// CRC 32 table for use under ZModem protocol, IEEE 802
// G(x) = x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1
//
static const unsigned int c_CRCTable[256] =
{
	0x00000000, 0x04C11DB7, 0x09823B6E, 0x0D4326D9, 0x130476DC, 0x17C56B6B, 0x1A864DB2, 0x1E475005,
	0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6, 0x2B4BCB61, 0x350C9B64, 0x31CD86D3, 0x3C8EA00A, 0x384FBDBD,
	0x4C11DB70, 0x48D0C6C7, 0x4593E01E, 0x4152FDA9, 0x5F15ADAC, 0x5BD4B01B, 0x569796C2, 0x52568B75,
	0x6A1936C8, 0x6ED82B7F, 0x639B0DA6, 0x675A1011, 0x791D4014, 0x7DDC5DA3, 0x709F7B7A, 0x745E66CD,
	0x9823B6E0, 0x9CE2AB57, 0x91A18D8E, 0x95609039, 0x8B27C03C, 0x8FE6DD8B, 0x82A5FB52, 0x8664E6E5,
	0xBE2B5B58, 0xBAEA46EF, 0xB7A96036, 0xB3687D81, 0xAD2F2D84, 0xA9EE3033, 0xA4AD16EA, 0xA06C0B5D,
	0xD4326D90, 0xD0F37027, 0xDDB056FE, 0xD9714B49, 0xC7361B4C, 0xC3F706FB, 0xCEB42022, 0xCA753D95,
	0xF23A8028, 0xF6FB9D9F, 0xFBB8BB46, 0xFF79A6F1, 0xE13EF6F4, 0xE5FFEB43, 0xE8BCCD9A, 0xEC7DD02D,
	0x34867077, 0x30476DC0, 0x3D044B19, 0x39C556AE, 0x278206AB, 0x23431B1C, 0x2E003DC5, 0x2AC12072,
	0x128E9DCF, 0x164F8078, 0x1B0CA6A1, 0x1FCDBB16, 0x018AEB13, 0x054BF6A4, 0x0808D07D, 0x0CC9CDCA,
	0x7897AB07, 0x7C56B6B0, 0x71159069, 0x75D48DDE, 0x6B93DDDB, 0x6F52C06C, 0x6211E6B5, 0x66D0FB02,
	0x5E9F46BF, 0x5A5E5B08, 0x571D7DD1, 0x53DC6066, 0x4D9B3063, 0x495A2DD4, 0x44190B0D, 0x40D816BA,
	0xACA5C697, 0xA864DB20, 0xA527FDF9, 0xA1E6E04E, 0xBFA1B04B, 0xBB60ADFC, 0xB6238B25, 0xB2E29692,
	0x8AAD2B2F, 0x8E6C3698, 0x832F1041, 0x87EE0DF6, 0x99A95DF3, 0x9D684044, 0x902B669D, 0x94EA7B2A,
	0xE0B41DE7, 0xE4750050, 0xE9362689, 0xEDF73B3E, 0xF3B06B3B, 0xF771768C, 0xFA325055, 0xFEF34DE2,
	0xC6BCF05F, 0xC27DEDE8, 0xCF3ECB31, 0xCBFFD686, 0xD5B88683, 0xD1799B34, 0xDC3ABDED, 0xD8FBA05A,
	0x690CE0EE, 0x6DCDFD59, 0x608EDB80, 0x644FC637, 0x7A089632, 0x7EC98B85, 0x738AAD5C, 0x774BB0EB,
	0x4F040D56, 0x4BC510E1, 0x46863638, 0x42472B8F, 0x5C007B8A, 0x58C1663D, 0x558240E4, 0x51435D53,
	0x251D3B9E, 0x21DC2629, 0x2C9F00F0, 0x285E1D47, 0x36194D42, 0x32D850F5, 0x3F9B762C, 0x3B5A6B9B,
	0x0315D626, 0x07D4CB91, 0x0A97ED48, 0x0E56F0FF, 0x1011A0FA, 0x14D0BD4D, 0x19939B94, 0x1D528623,
	0xF12F560E, 0xF5EE4BB9, 0xF8AD6D60, 0xFC6C70D7, 0xE22B20D2, 0xE6EA3D65, 0xEBA91BBC, 0xEF68060B,
	0xD727BBB6, 0xD3E6A601, 0xDEA580D8, 0xDA649D6F, 0xC423CD6A, 0xC0E2D0DD, 0xCDA1F604, 0xC960EBB3,
	0xBD3E8D7E, 0xB9FF90C9, 0xB4BCB610, 0xB07DABA7, 0xAE3AFBA2, 0xAAFBE615, 0xA7B8C0CC, 0xA379DD7B,
	0x9B3660C6, 0x9FF77D71, 0x92B45BA8, 0x9675461F, 0x8832161A, 0x8CF30BAD, 0x81B02D74, 0x857130C3,
	0x5D8A9099, 0x594B8D2E, 0x5408ABF7, 0x50C9B640, 0x4E8EE645, 0x4A4FFBF2, 0x470CDD2B, 0x43CDC09C,
	0x7B827D21, 0x7F436096, 0x7200464F, 0x76C15BF8, 0x68860BFD, 0x6C47164A, 0x61043093, 0x65C52D24,
	0x119B4BE9, 0x155A565E, 0x18197087, 0x1CD86D30, 0x029F3D35, 0x065E2082, 0x0B1D065B, 0x0FDC1BEC,
	0x3793A651, 0x3352BBE6, 0x3E119D3F, 0x3AD08088, 0x2497D08D, 0x2056CD3A, 0x2D15EBE3, 0x29D4F654,
	0xC5A92679, 0xC1683BCE, 0xCC2B1D17, 0xC8EA00A0, 0xD6AD50A5, 0xD26C4D12, 0xDF2F6BCB, 0xDBEE767C,
	0xE3A1CBC1, 0xE760D676, 0xEA23F0AF, 0xEEE2ED18, 0xF0A5BD1D, 0xF464A0AA, 0xF9278673, 0xFDE69BC4,
	0x89B8FD09, 0x8D79E0BE, 0x803AC667, 0x84FBDBD0, 0x9ABC8BD5, 0x9E7D9662, 0x933EB0BB, 0x97FFAD0C,
	0xAFB010B1, 0xAB710D06, 0xA6322BDF, 0xA2F33668, 0xBCB4666D, 0xB8757BDA, 0xB5365D03, 0xB1F740B4
};

uint32_t SUPPORT_ComputeCRC(const void* rgBlock, const uint32_t nLength, const uint32_t crc)
{
    const unsigned char* ptr = (const unsigned char*)rgBlock;
	int32_t lenght = nLength;
	uint32_t newCrc = crc;

    while(lenght-- > 0)
    {
        newCrc = c_CRCTable[((newCrc >> 24) ^ (*ptr++)) & 0xFF] ^ (newCrc << 8);
    }

    return newCrc;
};

///////////////////////////////////////////////////////////////

size_t CPU_GetCachableAddress( size_t address )
{
    return address;
}

//--//

size_t CPU_GetUncachableAddress( size_t address )
{
    return address;
}

///////////////////////////////////////////////////////////////
