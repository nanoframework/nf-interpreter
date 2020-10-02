//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2001, 2002 Georges Menie. All rights reserved.
// Portions Copyright (c) 2009-2013 Daniel D Miller. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef __TFP_PRINTF__
#define __TFP_PRINTF__

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

int tiny_sprintf (char *out, const char *format, ...);
int tiny_snprintf(char *out, unsigned int max_len, const char *format, ...);
int tiny_vsnprintf(char *out, unsigned int max_len, const char *format, va_list va);

#ifdef __cplusplus
}
#endif

// the defines below allow using the regular calls to sprintf
#define sprintf tiny_sprintf 
#define snprintf tiny_snprintf
#define vsnprintf tiny_vsnprintf

#endif
