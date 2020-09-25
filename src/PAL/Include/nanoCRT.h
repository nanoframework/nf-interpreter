//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _NANOCRT_DECL_H_
#define _NANOCRT_DECL_H_ 1

#if defined(PLATFORM_EMULATED_FLOATINGPOINT)
int hal_snprintf_float( char* buffer, size_t len, const char* format, int32_t f );
int hal_snprintf_double( char* buffer, size_t len, const char* format, int64_t& d );
#else
int hal_snprintf_float( char* buffer, size_t len, const char* format, float f );
int hal_snprintf_double( char* buffer, size_t len, const char* format, double d );
#endif

// Compares 2 ASCII strings case insensitive. Always defined in our code ( nanoCRT.cpp )
int hal_stricmp( const char * dst, const char * src );

#endif // _NANOCRT_DECL_H_
