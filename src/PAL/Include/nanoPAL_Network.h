//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _NETWORK_DECL_H_
#define _NETWORK_DECL_H_ 1

#ifdef PLATFORM_DEPENDENT__SOCKETS_MAX_COUNT
#define SOCKETS_MAX_COUNT PLATFORM_DEPENDENT__SOCKETS_MAX_COUNT
#else
// required to have at least one for array init
#error "PLATFORM_DEPENDENT__SOCKETS_MAX_COUNT no defined at target or platform level"
#endif

#ifdef PLATFORM_DEPENDENT__SOCKETS_MAX_SEND_LENGTH
#define SOCKETS_MAX_SEND_LENGTH PLATFORM_DEPENDENT__SOCKETS_MAX_SEND_LENGTH
#endif

#if !defined(DEBGGER_NETWORK_INTERFACE_INDEX)
#define DEBGGER_NETWORK_INTERFACE_INDEX 0
#endif

#endif //_NETWORK_DECL_H_
