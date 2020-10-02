//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _NANOCLR_ERRORCODES_H_
#define _NANOCLR_ERRORCODES_H_
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <nf_errors_exceptions.h>

//
// PARSER error codes.
//
#define CLR_E_PARSER_BAD_TEXT_SIGNATURE             MAKE_HRESULT(SEVERITY_ERROR, 0x0101, 0x0001)
#define CLR_E_PARSER_BAD_CUSTOM_ATTRIBUTE           MAKE_HRESULT(SEVERITY_ERROR, 0x0101, 0x0002)
#define CLR_E_PARSER_UNSUPPORTED_MULTIDIM_ARRAY     MAKE_HRESULT(SEVERITY_ERROR, 0x0101, 0x0003)
#define CLR_E_PARSER_UNKNOWN_MEMBER_REF             MAKE_HRESULT(SEVERITY_ERROR, 0x0101, 0x0004)
#define CLR_E_PARSER_MISSING_FIELD                  MAKE_HRESULT(SEVERITY_ERROR, 0x0101, 0x0005)
#define CLR_E_PARSER_MISSING_METHOD                 MAKE_HRESULT(SEVERITY_ERROR, 0x0101, 0x0006)
#define CLR_E_PARSER_MISSING_INTERFACE              MAKE_HRESULT(SEVERITY_ERROR, 0x0101, 0x0007)
#define CLR_E_PARSER_MISSING_TOKEN                  MAKE_HRESULT(SEVERITY_ERROR, 0x0101, 0x0008)
#define CLR_E_PARSER_UNSUPPORTED_GENERICS           MAKE_HRESULT(SEVERITY_ERROR, 0x0101, 0x0009)


#endif // _NANOCLR_ERRORCODES_H_

