//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef SSL_TYPES_H
#define SSL_TYPES_H


#define NANOCLR_SSL_STRLEN              hal_strlen_s
#define TINYCLR_SSL_STRNCPY(a,b,c)      hal_strncpy_s(a,c+1,b,c)
#define NANOCLR_SSL_STRNCPY(a,b)        hal_strcpy_s(a,hal_strlen_s(b)+1,b)

#endif // SSL_TYPES_H
