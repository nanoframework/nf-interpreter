//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef SSL_TYPES_H
#define SSL_TYPES_H


#define NANOCLR_SSL_STRLEN              hal_strlen_s
#define TINYCLR_SSL_STRNCPY(a,b,c)      hal_strncpy_s(a,c+1,b,c)
#define NANOCLR_SSL_STRNCPY(a,b)        hal_strcpy_s(a,hal_strlen_s(b)+1,b)


// TODO clean up these
// possibly replacing with standard CRT calls 
//#define NANOCLR_SSL_FPRINTF             hal_fprintf_ssl
#define NANOCLR_SSL_PRINTF              debug_printf
#define NANOCLR_SSL_MEMCPY              memcpy
#define NANOCLR_SSL_MEMSET              memset


#endif 