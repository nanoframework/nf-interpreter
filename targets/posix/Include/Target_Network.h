//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#pragma once

#include <nanoHAL.h>
#include <nanoPAL_Events.h>
#include <nanoPAL.h>
#include <array>
#include <string>
#include <vector>

struct LinuxNetworkInterface
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

int LinuxNetwork_ListInterfaces(std::vector<LinuxNetworkInterface> &interfaces);

int LinuxNetwork_StartResolve(const char *name);
int LinuxNetwork_CompleteResolve(int request, SOCK_addrinfo **result);
void LinuxNetwork_CancelResolve(int request);
int LinuxNetwork_IsAvailable();

struct CLR_RT_StackFrame;
HRESULT LinuxNetwork_Resolve(CLR_RT_StackFrame &stack);
