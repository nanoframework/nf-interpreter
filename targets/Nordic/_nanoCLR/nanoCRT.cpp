//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>

//--//
/* STDIO stubs                                                              */
//--//

#if !defined(BUILD_RTM)

void hal_fprintf_SetLoggingCallback( LOGGING_CALLBACK fpn )
{
    (void)fpn;
    NATIVE_PROFILE_PAL_CRT();

}

#endif

// because debug_printf needs to be called in both C and C++ we need a proxy to allow it to be called in 'C'
extern "C" {

#if !defined(BUILD_RTM)
    
    void debug_printf(const char* format, ...)
    {
        char buffer[256];
        va_list arg_ptr;
    
        va_start( arg_ptr, format );
    
        int len = vsnprintf( buffer, sizeof(buffer)-1, format, arg_ptr );
   
        DebuggerPort_Write( HalSystemConfig.stdio, buffer, len, 0 ); // skip null terminator
    
        va_end( arg_ptr );
    }

#else
    __inline void debug_printf( const char *format, ... ) {}
#endif  // !defined(BUILD_RTM)
}

int hal_strcpy_s ( char* strDst, size_t sizeInBytes, const char* strSrc )
{
    NATIVE_PROFILE_PAL_CRT();
#undef strcpy

    size_t len;
    if(strDst == NULL || strSrc == NULL || sizeInBytes == 0) {_ASSERTE(FALSE); return 1;}
    
    len = hal_strlen_s(strSrc);
    if(sizeInBytes < len + 1) {_ASSERTE(FALSE); return 1;}

    strcpy( strDst, strSrc );
    return 0;

#define strcpy DoNotUse_*strcpy []
}

int hal_strncpy_s ( char* strDst, size_t sizeInBytes, const char* strSrc, size_t count )
{
    NATIVE_PROFILE_PAL_CRT();
#undef strncpy
    if(strDst == NULL || strSrc == NULL || sizeInBytes == 0) {_ASSERTE(FALSE); return 1;}
    
    if (sizeInBytes < count + 1)
    {
        _ASSERTE(FALSE);
        strDst[0] = 0;
        return 1;
    }

    strDst[count] = 0;
    strncpy( strDst, strSrc, count );
    return 0;

#define strncpy DoNotUse_*strncpy []
}

size_t hal_strlen_s (const char * str)
{
    NATIVE_PROFILE_PAL_CRT();

    const char *eos = str;
    while( *eos++ ) ;
    return( eos - str - 1 );
}

int hal_strncmp_s ( const char* str1, const char* str2, size_t num )
{
    NATIVE_PROFILE_PAL_CRT();
#undef strncmp
    if(str1 == NULL || str2 == NULL) {_ASSERTE(FALSE); return 1;}
    
    return strncmp( str1, str2, num );

#define strncmp DoNotUse_*strncmp []
}

// Compares 2 ASCII strings case insensitive. Does not take locale into account.
int hal_stricmp( const char * dst, const char * src )
{
    int f = 0, l = 0;

    do
    {
        if ( ((f = (unsigned char)(*(dst++))) >= 'A') && (f <= 'Z') )
        {
            f -= 'A' - 'a';
        }
        if ( ((l = (unsigned char)(*(src++))) >= 'A') && (l <= 'Z') )
        {
            l -= 'A' - 'a';
        }
    }
    while ( f && (f == l) );

    return(f - l);
}
