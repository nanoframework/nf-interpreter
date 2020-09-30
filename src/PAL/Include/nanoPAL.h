//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _NANOPAL_H_
#define _NANOPAL_H_ 1

//#include <nanoHAL_Types.h>

#include <nanoHAL.h>
#include <nanoPAL_Time.h>
#include <nanoPAL_GPIO.h>
#include <nanoHAL_v2.h>
#include <nanoPAL_BlockStorage.h>
#include <nanoPAL_Events.h>
#include <nanoPAL_AsyncProcCalls_decl.h>
#include <nanoPAL_PerformanceCounters.h>





#include <nanoPAL_COM.h>
// 
#include <nanoPAL_Sockets.h>










//
//#include <Display_decl.h>

//#include <Power_decl.h> (must be before events_decl.h)














//#include <events_decl.h>
































//#include <palevent_decl.h>

//#include <heap_decl.h>

// This function returns location of the CLR heap.
void HeapLocation(unsigned char*& BaseAddress, unsigned int& SizeInBytes);




//
//#include <graphics_decl.h>
//
//#include <I2C_decl.h>
//#include <io_decl.h>
//#include <instrumentation_decl.h>
//
//#include <FS_decl.h>
//


//

//#include <Power_decl.h>

//
// !!! KEEP IN SYNC WITH Microsoft.SPOT.Hardware.PowerLevel !!!
//
enum POWER_LEVEL
{
    POWER_LEVEL__HIGH_POWER = 0x10,
    POWER_LEVEL__MID_POWER  = 0x20,
    POWER_LEVEL__LOW_POWER  = 0x30,
};


//#include <Security_decl.h>
//#include <Sockets_decl.h>

//#include <Time_decl.h>














































































//#include <TimeService_decl.h>


//#include <nanocrt_decl.h>

#if defined(PLATFORM_ARM) || defined(PLATFORM_ESP32)

#define strcpy    DoNotUse_*strcpy  []
#define strncpy   DoNotUse_*strcpy  []
#define strlen    DoNotUse_*strlen  []
#define strncmp   DoNotUse_*strncmp  []

int hal_strcpy_s ( char* strDst, size_t sizeInBytes, const char* strSrc );
int hal_strncpy_s( char* strDst, size_t sizeInBytes, const char* strSrc, size_t count );
size_t hal_strlen_s (const char * str);
int hal_strncmp_s( const char* str1, const char* str2, size_t num );

#elif defined(_WIN32)

int hal_vprintf( const char* format, va_list arg );
int hal_vfprintf( COM_HANDLE stream, const char* format, va_list arg );
int hal_snprintf( char* buffer, size_t len, const char* format, ... );
int hal_vsnprintf( char* buffer, size_t len, const char* format, va_list arg );

#define hal_strcpy_s(strDst, sizeInBytes, strSrc) strcpy_s(strDst, sizeInBytes, strSrc)
#define hal_strncpy_s(strDst, sizeInBytes, strSrc, count) strncpy_s(strDst, sizeInBytes, strSrc, count)
#define hal_strlen_s(str) strlen(str)
#define hal_strncmp_s(str1, str2, num) strncmp(str1, str2, num)

#else
!ERROR
#endif


//#include <USART_decl.h>
//#include <USB_decl.h>
//#include <PWM_decl.h>
//#include <Analog_decl.h>
//#include <AnalogOut_decl.h>
//
#include "TargetPAL_BlockStorage.h"

//#include <SD_decl.h>

//#include "..\pal\Diagnostics\Native_Profiler.h"
// native profiler
#define NATIVE_PROFILE_PAL_HEAP()
#define NATIVE_PROFILE_PAL_CRT()

#endif // _NANOPAL_H_
