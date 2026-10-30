//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#pragma once

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#endif

#include <nanoHAL.h>
#include <nanoPAL_Events.h>
#if defined(_WIN32)
#include <nanoPAL_Sockets.h>
#else
#include <nanoPAL.h>
#endif
#include <array>
#include <string>
#include <vector>

struct HostNetworkInterface
{
    uint32_t index = 0;
    std::string name;
    uint32_t address = 0;
    uint32_t netmask = 0;
    uint32_t gateway = 0;
    std::array<uint32_t, 2> dns = {};
    std::vector<uint8_t> mac;
    int type = 1;
    bool available = false;
};

int HostNetwork_ListInterfaces(std::vector<HostNetworkInterface> &interfaces);

int HostNetwork_StartResolve(const char *name);
int HostNetwork_CompleteResolve(int request, SOCK_addrinfo **result);
void HostNetwork_CancelResolve(int request);
int HostNetwork_IsAvailable();

struct CLR_RT_StackFrame;
HRESULT HostNetwork_Resolve(CLR_RT_StackFrame &stack);
