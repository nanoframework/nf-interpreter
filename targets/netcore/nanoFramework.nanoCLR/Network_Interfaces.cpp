//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <Host_Network.h>
#include <iphlpapi.h>
#include <algorithm>

int HostNetwork_ListInterfaces(std::vector<HostNetworkInterface> &interfaces)
{
    interfaces.clear();
    ULONG length = 16 * 1024;
    std::vector<uint8_t> buffer(length);
    ULONG result;
    for (unsigned attempt = 0;; ++attempt)
    {
        result = GetAdaptersAddresses(
            AF_INET,
            GAA_FLAG_INCLUDE_GATEWAYS | GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST,
            nullptr,
            reinterpret_cast<IP_ADAPTER_ADDRESSES *>(buffer.data()),
            &length);
        if (result != ERROR_BUFFER_OVERFLOW || attempt == 2)
        {
            break;
        }
        buffer.resize(length);
    }
    if (result == ERROR_NO_DATA)
    {
        return 0;
    }
    if (result != NO_ERROR)
    {
        fprintf(stderr, "nanoCLR enumerate Windows interfaces failed (%lu)\n", result);
        return -1;
    }
    for (auto adapter = reinterpret_cast<IP_ADAPTER_ADDRESSES *>(buffer.data()); adapter != nullptr;
         adapter = adapter->Next)
    {
        if (adapter->IfIndex == 0)
        {
            continue;
        }
        HostNetworkInterface entry;
        entry.index = adapter->IfIndex;
        entry.name = adapter->AdapterName;
        entry.type = adapter->IfType == IF_TYPE_ETHERNET_CSMACD ? 6
                     : adapter->IfType == IF_TYPE_IEEE80211    ? 71
                                                              : 1;
        entry.mac.assign(adapter->PhysicalAddress, adapter->PhysicalAddress + adapter->PhysicalAddressLength);
        for (auto address = adapter->FirstUnicastAddress; address != nullptr; address = address->Next)
        {
            if (address->Address.lpSockaddr == nullptr || address->Address.lpSockaddr->sa_family != AF_INET)
            {
                continue;
            }
            entry.address = reinterpret_cast<sockaddr_in *>(address->Address.lpSockaddr)->sin_addr.s_addr;
            unsigned prefix = address->OnLinkPrefixLength;
            if (prefix > 32)
            {
                fprintf(stderr, "nanoCLR interface %s: invalid IPv4 prefix %u\n", entry.name.c_str(), prefix);
                interfaces.clear();
                return -1;
            }
            entry.netmask = htonl(prefix == 0 ? 0 : UINT32_MAX << (32 - prefix));
            break;
        }
        for (auto gateway = adapter->FirstGatewayAddress; gateway != nullptr; gateway = gateway->Next)
        {
            if (gateway->Address.lpSockaddr != nullptr && gateway->Address.lpSockaddr->sa_family == AF_INET)
            {
                entry.gateway = reinterpret_cast<sockaddr_in *>(gateway->Address.lpSockaddr)->sin_addr.s_addr;
                break;
            }
        }
        size_t count = 0;
        for (auto dns = adapter->FirstDnsServerAddress; dns != nullptr && count < entry.dns.size(); dns = dns->Next)
        {
            if (dns->Address.lpSockaddr != nullptr && dns->Address.lpSockaddr->sa_family == AF_INET)
            {
                entry.dns[count++] = reinterpret_cast<sockaddr_in *>(dns->Address.lpSockaddr)->sin_addr.s_addr;
            }
        }
        entry.available = entry.address != 0 && adapter->OperStatus == IfOperStatusUp &&
                          adapter->IfType != IF_TYPE_SOFTWARE_LOOPBACK && adapter->IfType != IF_TYPE_PPP &&
                          adapter->IfType != IF_TYPE_TUNNEL;
        interfaces.push_back(std::move(entry));
    }
    std::sort(interfaces.begin(), interfaces.end(), [](const auto &left, const auto &right) {
        return left.index < right.index;
    });
    return 0;
}
