//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <Target_Network.h>

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#if NANOCLR_HOST_APPLE
#include <SystemConfiguration/SystemConfiguration.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <sys/sysctl.h>
#else
#include <net/if_arp.h>
#include <netpacket/packet.h>
#endif
#include <net/route.h>

#include <algorithm>
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <map>

namespace
{
bool ReadDns(std::array<uint32_t, 2> &dns)
{
#if NANOCLR_HOST_APPLE
    // resolv.conf is not authoritative on macOS; use the active system configuration.
    auto store = SCDynamicStoreCreate(nullptr, CFSTR("nanoCLR"), nullptr, nullptr);
    if (store == nullptr)
    {
        fprintf(stderr, "nanoCLR DNS configuration: %s\n", SCErrorString(SCError()));
        return true;
    }
    auto configuration = SCDynamicStoreCopyValue(store, CFSTR("State:/Network/Global/DNS"));
    int error = SCError();
    CFRelease(store);
    if (configuration == nullptr)
    {
        if (error == kSCStatusNoKey)
        {
            return true;
        }
        fprintf(stderr, "nanoCLR DNS configuration: %s\n", SCErrorString(error));
        return false;
    }
    bool success = CFGetTypeID(configuration) == CFDictionaryGetTypeID();
    if (success)
    {
        auto servers = CFDictionaryGetValue(static_cast<CFDictionaryRef>(configuration), kSCPropNetDNSServerAddresses);
        success = servers == nullptr || CFGetTypeID(servers) == CFArrayGetTypeID();
        if (servers != nullptr && success)
        {
            auto addresses = static_cast<CFArrayRef>(servers);
            size_t count = 0;
            for (CFIndex i = 0; i < CFArrayGetCount(addresses) && count < dns.size(); ++i)
            {
                auto value = CFArrayGetValueAtIndex(addresses, i);
                char text[256];
                if (CFGetTypeID(value) != CFStringGetTypeID() ||
                    !CFStringGetCString(static_cast<CFStringRef>(value), text, sizeof(text), kCFStringEncodingUTF8))
                {
                    success = false;
                    break;
                }
                in_addr native;
                if (inet_pton(AF_INET, text, &native) == 1)
                {
                    dns[count++] = native.s_addr;
                }
            }
        }
    }
    CFRelease(configuration);
    if (!success)
    {
        fprintf(stderr, "nanoCLR DNS configuration: invalid server addresses\n");
    }
    return success;
#else
    FILE *file = fopen("/etc/resolv.conf", "r");
    if (file == nullptr)
    {
        if (errno == ENOENT)
        {
            // No resolver configuration: report no DNS servers (0.0.0.0).
            return true;
        }
        perror("nanoCLR read /etc/resolv.conf");
        return false;
    }
    char line[1024];
    size_t count = 0;
    while (fgets(line, sizeof(line), file) != nullptr && count < dns.size())
    {
        char keyword[32];
        char address[256];
        in_addr native;
        if (sscanf(line, "%31s %255s", keyword, address) == 2 && strcmp(keyword, "nameserver") == 0 &&
            inet_pton(AF_INET, address, &native) == 1)
        {
            dns[count++] = native.s_addr;
        }
    }
    bool success = !ferror(file);
    if (!success)
    {
        perror("nanoCLR read /etc/resolv.conf");
    }
    fclose(file);
    return success;
#endif
}

bool ReadGateways(std::vector<HostNetworkInterface> &interfaces)
{
#if NANOCLR_HOST_APPLE
    int mib[] = {CTL_NET, PF_ROUTE, 0, AF_INET, NET_RT_DUMP, 0};
    std::vector<uint8_t> routes;
    size_t length = 0;
    bool loaded = false;
    for (int attempt = 0; attempt < 3; ++attempt)
    {
        if (sysctl(mib, 6, nullptr, &length, nullptr, 0) < 0)
        {
            break;
        }
        routes.resize(length);
        if (length == 0 || sysctl(mib, 6, routes.data(), &length, nullptr, 0) == 0)
        {
            routes.resize(length);
            loaded = true;
            break;
        }
        if (errno != ENOMEM)
        {
            break;
        }
    }
    if (!loaded)
    {
        perror("nanoCLR read routing table");
        return false;
    }
    for (size_t offset = 0; offset < routes.size();)
    {
        rt_msghdr header;
        if (routes.size() - offset < sizeof(header))
        {
            fprintf(stderr, "nanoCLR routing table: truncated header\n");
            return false;
        }
        memcpy(&header, routes.data() + offset, sizeof(header));
        if (header.rtm_msglen < sizeof(header) || header.rtm_msglen > routes.size() - offset ||
            header.rtm_version != RTM_VERSION)
        {
            fprintf(stderr, "nanoCLR routing table: invalid route message\n");
            return false;
        }
        size_t end = offset + header.rtm_msglen;
        size_t position = offset + sizeof(header);
        std::array<sockaddr_in, RTAX_MAX> addresses = {};
        for (int i = 0; i < RTAX_MAX; ++i)
        {
            if ((header.rtm_addrs & (1 << i)) == 0)
            {
                continue;
            }
            if (end - position < 2)
            {
                fprintf(stderr, "nanoCLR routing table: truncated address\n");
                return false;
            }
            size_t size = routes[position];
            // Darwin route sockaddrs are padded to 32-bit boundaries, even on arm64.
            size_t padded = size == 0 ? sizeof(uint32_t) : (size + 3) & ~size_t(3);
            if (padded > end - position)
            {
                fprintf(stderr, "nanoCLR routing table: invalid address length\n");
                return false;
            }
            memcpy(&addresses[i], routes.data() + position, std::min(size, sizeof(sockaddr_in)));
            position += padded;
        }
        if ((header.rtm_flags & (RTF_UP | RTF_GATEWAY)) == (RTF_UP | RTF_GATEWAY) &&
            (header.rtm_addrs & (RTA_DST | RTA_GATEWAY)) == (RTA_DST | RTA_GATEWAY) &&
            addresses[RTAX_DST].sin_family == AF_INET && addresses[RTAX_DST].sin_len >= sizeof(sockaddr_in) &&
            addresses[RTAX_DST].sin_addr.s_addr == 0 && addresses[RTAX_NETMASK].sin_addr.s_addr == 0 &&
            addresses[RTAX_GATEWAY].sin_family == AF_INET && addresses[RTAX_GATEWAY].sin_len >= sizeof(sockaddr_in))
        {
            for (auto &interface : interfaces)
            {
                if (interface.index == header.rtm_index && interface.gateway == 0)
                {
                    interface.gateway = addresses[RTAX_GATEWAY].sin_addr.s_addr;
                }
            }
        }
        offset = end;
    }
    return true;
#else
    FILE *file = fopen("/proc/net/route", "r");
    if (file == nullptr)
    {
        perror("nanoCLR read /proc/net/route");
        return false;
    }
    std::map<std::string, unsigned long> metrics;
    char line[512];
    while (fgets(line, sizeof(line), file) != nullptr)
    {
        char name[IFNAMSIZ];
        unsigned long destination, gateway, metric, mask;
        unsigned flags;
        if (sscanf(line, "%15s %lx %lx %x %*u %*u %lu %lx", name, &destination, &gateway, &flags, &metric, &mask) !=
                6 ||
            destination != 0 || mask != 0 || (flags & (RTF_UP | RTF_GATEWAY)) != (RTF_UP | RTF_GATEWAY))
        {
            continue;
        }
        auto previous = metrics.find(name);
        if (previous != metrics.end() && previous->second <= metric)
        {
            continue;
        }
        metrics[name] = metric;
        for (auto &interface : interfaces)
        {
            if (interface.name == name)
            {
                interface.gateway = static_cast<uint32_t>(gateway);
            }
        }
    }
    bool success = !ferror(file);
    if (!success)
    {
        perror("nanoCLR read /proc/net/route");
    }
    fclose(file);
    return success;
#endif
}
} // namespace

int HostNetwork_ListInterfaces(std::vector<HostNetworkInterface> &interfaces)
{
    interfaces.clear();
    ifaddrs *addresses;
    if (getifaddrs(&addresses) < 0)
    {
        perror("nanoCLR enumerate interfaces");
        return -1;
    }
    std::map<uint32_t, HostNetworkInterface> entries;
#if NANOCLR_HOST_APPLE
    uint32_t loopbackIndex = 0;
#endif
    bool success = true;
    for (auto current = addresses; current != nullptr; current = current->ifa_next)
    {
#if NANOCLR_HOST_APPLE
        constexpr int linkFamily = AF_LINK;
#else
        constexpr int linkFamily = AF_PACKET;
#endif
        if (current->ifa_addr == nullptr ||
            (current->ifa_addr->sa_family != AF_INET && current->ifa_addr->sa_family != linkFamily))
        {
            continue;
        }
        uint32_t index = if_nametoindex(current->ifa_name);
        if (index == 0)
        {
            if (errno == ENODEV || errno == ENXIO)
            {
                continue;
            }
            perror("nanoCLR interface index");
            success = false;
            break;
        }
        auto &entry = entries[index];
        entry.index = index;
        entry.name = current->ifa_name;
        if (current->ifa_addr->sa_family == AF_INET)
        {
            bool useAddress = entry.address == 0;
#if NANOCLR_HOST_APPLE
            if ((current->ifa_flags & IFF_LOOPBACK) != 0 &&
                reinterpret_cast<sockaddr_in *>(current->ifa_addr)->sin_addr.s_addr == htonl(INADDR_LOOPBACK))
            {
                loopbackIndex = index;
                useAddress = true;
            }
#endif
            // The managed API has room for one IPv4 address per interface.
            if (useAddress)
            {
                entry.address = reinterpret_cast<sockaddr_in *>(current->ifa_addr)->sin_addr.s_addr;
                if (current->ifa_netmask != nullptr)
                {
                    entry.netmask = reinterpret_cast<sockaddr_in *>(current->ifa_netmask)->sin_addr.s_addr;
                }
            }
            entry.available = entry.address != 0 &&
                              (current->ifa_flags & (IFF_UP | IFF_RUNNING)) == (IFF_UP | IFF_RUNNING) &&
                              (current->ifa_flags & (IFF_LOOPBACK | IFF_POINTOPOINT)) == 0;
        }
        else
        {
#if NANOCLR_HOST_APPLE
            auto link = reinterpret_cast<sockaddr_dl *>(current->ifa_addr);
            size_t offset = offsetof(sockaddr_dl, sdl_data) + link->sdl_nlen;
            if (offset > link->sdl_len || link->sdl_alen > link->sdl_len - offset)
            {
                fprintf(stderr, "nanoCLR interface %s: invalid hardware address length\n", current->ifa_name);
                success = false;
                break;
            }
            auto mac = reinterpret_cast<const uint8_t *>(link) + offset;
            entry.mac.assign(mac, mac + link->sdl_alen);
            entry.type = link->sdl_type == IFT_ETHER ? 6 : 1;
#else
            auto link = reinterpret_cast<sockaddr_ll *>(current->ifa_addr);
            if (link->sll_halen > sizeof(link->sll_addr))
            {
                fprintf(
                    stderr,
                    "nanoCLR interface %s: unsupported hardware address length %u\n",
                    current->ifa_name,
                    link->sll_halen);
                success = false;
                break;
            }
            entry.mac.assign(link->sll_addr, link->sll_addr + link->sll_halen);
            entry.type = link->sll_hatype == ARPHRD_ETHER ? 6 : 1;
#endif
        }
    }
    freeifaddrs(addresses);
    if (!success)
    {
        return -1;
    }
    for (auto &[index, entry] : entries)
    {
#if NANOCLR_HOST_APPLE
        if (index != loopbackIndex && (entry.type != 6 || !entry.available))
        {
            continue;
        }
#endif
        interfaces.push_back(std::move(entry));
    }
    std::array<uint32_t, 2> dns = {};
    if (!ReadDns(dns) || !ReadGateways(interfaces))
    {
        interfaces.clear();
        return -1;
    }
    for (auto &entry : interfaces)
    {
        entry.dns = dns;
    }
    return 0;
}
