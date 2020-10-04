//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _NANOCLR_PLATFORMDEF_H_
#define _NANOCLR_PLATFORMDEF_H_

//#include <CLR_Defines.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
// DEFINITIONS
#define NANOCLR_VALIDATE_HEAP_0_None                0 // No Trace
#define NANOCLR_VALIDATE_HEAP_1_HeapBlocksAndUnlink 1 // Validate HeapBlocks and Unlink.
#define NANOCLR_VALIDATE_HEAP_2_DblLinkedList       2 // Validate a DblLinkedList before each operation.
#define NANOCLR_VALIDATE_HEAP_3_Compaction          3 // Trace Compaction
#define NANOCLR_VALIDATE_HEAP_4_CompactionPlus      4 // Trace Compaction Plus

#define NANOCLR_MAX_ASSEMBLY_NAME 128

////////////////////////////////////////////////////////////////////////////////////////////////////
// FEATURES


#if defined(PLATFORM_EMULATED_FLOATINGPOINT)
#define NANOCLR_EMULATED_FLOATINGPOINT    // use the fixed point floating point notation in the clr codes
#endif

#if defined(NANOCLR_USE_APPDOMAINS)
#define NANOCLR_APPDOMAINS           // enables application doman support
#endif
#define NANOCLR_TRACE_EXCEPTIONS     // enables exception dump support
#define NANOCLR_TRACE_ERRORS         // enables rich exception dump support
#if defined(DEBUG) || defined(_DEBUG)
#define NANOCLR_TRACE_STACK          // enables rich eval stack tracing  
#endif
//#define TINYCLR_TRACE_INSTRUCTIONS 1    // enables tracing of instructions execution
//#define NANOCLR_TRACE_HRESULT        // enable tracing of HRESULTS from interop libraries 

//-o-//-o-//-o-//-o-//-o-//-o-//
// PLATFORMS
//-o-//-o-//-o-//-o-//-o-//-o-//


//--//
// Setting the threshold value to start Garbagge collector 
// PLATFORM_DEPENDENT_HEAP_SIZE_THRESHOLD should set in the file platform.settings file, eg sam7x_ek.settings. 
// defaults are 32Kb and 48 kb for lower and upper threshold respectively

#ifdef PLATFORM_DEPENDENT_HEAP_SIZE_THRESHOLD
#define HEAP_SIZE_THRESHOLD   PLATFORM_DEPENDENT_HEAP_SIZE_THRESHOLD
#else
#define HEAP_SIZE_THRESHOLD   48 * 1024
#endif

#ifdef PLATFORM_DEPENDENT_HEAP_SIZE_THRESHOLD_UPPER
#define HEAP_SIZE_THRESHOLD_UPPER   PLATFORM_DEPENDENT_HEAP_SIZE_THRESHOLD_UPPER
#else
#define HEAP_SIZE_THRESHOLD_UPPER   HEAP_SIZE_THRESHOLD + 16 * 1024
#endif

//--//

////////////////////////////////////////////////////////////////////////////////////////////////////
// WINDOWS
#if defined(_WIN32)

#define NANOCLR_GC_VERBOSE
#define NANOCLR_TRACE_MEMORY_STATS
#define NANOCLR_PROFILE_NEW
#define NANOCLR_PROFILE_NEW_CALLS
#define NANOCLR_PROFILE_NEW_ALLOCATIONS
#if defined(DEBUG) || defined(_DEBUG)
#define NANOCLR_VALIDATE_HEAP                   NANOCLR_VALIDATE_HEAP_2_DblLinkedList
//#define NANOCLR_TRACE_MALLOC
#define NANOCLR_FILL_MEMORY_WITH_DIRTY_PATTERN
#define NANOCLR_TRACE_EARLYCOLLECTION
#define NANOCLR_DELEGATE_PRESERVE_STACK
//#define NANOCLR_VALIDATE_APPDOMAIN_ISOLATION
#define NANOCLR_TRACE_HRESULT        // enable tracing of HRESULTS from interop libraries 
#else //RELEASE
#define NANOCLR_VALIDATE_HEAP                   NANOCLR_VALIDATE_HEAP_0_None
#endif
#define NANOCLR_ENABLE_SOURCELEVELDEBUGGING
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// ARM & ESP32
#if defined(PLATFORM_ARM) || defined(PLATFORM_ESP32)
// #define NANOCLR_STRESS_GC
// #define NANOCLR_GC_VERBOSE
// #define NANOCLR_PROFILE_NEW
// #define NANOCLR_PROFILE_NEW_CALLS
// #define NANOCLR_PROFILE_NEW_ALLOCATIONS
// #define NANOCLR_TRACE_MEMORY_STATS
// #define NANOCLR_FORCE_GC_BEFORE_EVERY_ALLOCATION
#define NANOCLR_VALIDATE_HEAP                   NANOCLR_VALIDATE_HEAP_0_CompactionPlus
#endif

//-o-//-o-//-o-//-o-//-o-//-o-//
// RULES AND DEPENDENCIES
//-o-//-o-//-o-//-o-//-o-//-o-//

////////////////////////////////////////////////////////////////////////////////////////////////////
// GENERAL RTM RULES
#if defined(BUILD_RTM) || defined(PLATFORM_NO_CLR_TRACE)
#undef NANOCLR_TRACE_MEMORY_STATS
#undef NANOCLR_TRACE_EXCEPTIONS 
#undef NANOCLR_TRACE_ERRORS
#undef NANOCLR_TRACE_EARLYCOLLECTION
#undef NANOCLR_VALIDATE_HEAP
#undef NANOCLR_FILL_MEMORY_WITH_DIRTY_PATTERN
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// TRACE DEPENDENCIES
#if defined(_WIN32)
#define NANOCLR_OPCODE_NAMES
#define NANOCLR_OPCODE_PARSER
#define NANOCLR_OPCODE_STACKCHANGES
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(NANOCLR_VALIDATE_HEAP)
#define      NANOCLR_VALIDATE_HEAP  NANOCLR_VALIDATE_HEAP_0_None
#endif

#if defined(NANOCLR_PROFILE_NEW_CALLS) && !defined(NANOCLR_PROFILE_HANDLER)
#define NANOCLR_PROFILE_HANDLER
#endif

#if defined(NANOCLR_PROFILE_NEW_CALLS) && !defined(NANOCLR_PROFILE_NEW)
#define NANOCLR_PROFILE_NEW
#endif

#if defined(NANOCLR_PROFILE_NEW_ALLOCATIONS) && !defined(NANOCLR_PROFILE_NEW)
#define NANOCLR_PROFILE_NEW
#endif

//-o-//-o-//-o-//-o-//-o-//-o-//
// CODE
//-o-//-o-//-o-//-o-//-o-//-o-//

////////////////////////////////////////////////////////////////////////////////////////////////////
// LANGUAGE
#if defined(_WIN32)
#define PROHIBIT_ALL_CONSTRUCTORS(cls)   \
    private:                             \
        cls();                           \
        cls( cls& );                     \
        cls& operator=( const cls& )

#define PROHIBIT_COPY_CONSTRUCTORS(cls)  \
    public:                              \
        cls() {}                         \
    private:                             \
        cls( cls& );                     \
        cls& operator=( const cls& )

#define PROHIBIT_COPY_CONSTRUCTORS2(cls) \
    private:                             \
        cls( cls& );                     \
        cls& operator=( const cls& )

#define LONGLONGCONSTANT(v) (v##I64)
#define ULONGLONGCONSTANT(v) (v##UI64)
#endif

#if defined(PLATFORM_ARM) | defined(PLATFORM_ESP32)
#define PROHIBIT_ALL_CONSTRUCTORS(cls)   \
    private:                             \
        cls();                           \
        cls( cls& );                     \
        cls& operator=( const cls& )

#define PROHIBIT_COPY_CONSTRUCTORS(cls)  \
    public:                              \
        cls() {}                         \
    private:                             \
        cls( cls& );                     \
        cls& operator=( const cls& )

#define PROHIBIT_COPY_CONSTRUCTORS2(cls) \
    private:                             \
        cls( cls& );                     \
        cls& operator=( const cls& )

#define LONGLONGCONSTANT(v) (v##ll)
#define ULONGLONGCONSTANT(v) (v##ull)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// INCLUDES
#if defined(_WIN32)

#define _WIN32_WINNT 0x0501

//Unsafe string functions be avoided, but there isn't a safe crt for the arm, so 
//a bunch of macros, cleanup code needs to be done first

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>

#include <crtdbg.h>

#include <string>
#include <list>
#include <vector>
#include <map>

#else

#include <nanoHAL_Types.h>

#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef MAKE_HRESULT
#define MAKE_HRESULT(sev,fac,code) \
      ((HRESULT) (((unsigned long)(sev)<<31) | ((unsigned long)(fac)<<16) | ((unsigned long)(code))) )
#endif

#ifndef SEVERITY_SUCCESS
#define SEVERITY_SUCCESS    0
#endif

#ifndef SEVERITY_ERROR
#define SEVERITY_ERROR      1
#endif

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////

#include <nanoHAL.h>

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _NANOCLR_PLATFORMDEF_H_
