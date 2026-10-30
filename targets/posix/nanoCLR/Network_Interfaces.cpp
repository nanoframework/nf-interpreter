//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <Host_Network.h>

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <net/route.h>
#include <netpacket/packet.h>

#include <cerrno>
#include <cstdio>
#include <map>

namespace
{
bool ReadDns(std::array<uint32_t, 2> &dns)
{
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
}

bool ReadGateways(std::vector<HostNetworkInterface> &interfaces)
{
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
    bool success = true;
    for (auto current = addresses; current != nullptr; current = current->ifa_next)
    {
        if (current->ifa_addr == nullptr ||
            (current->ifa_addr->sa_family != AF_INET && current->ifa_addr->sa_family != AF_PACKET))
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
            // The managed API has room for one IPv4 address per interface.
            if (entry.address == 0)
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
        }
    }
    freeifaddrs(addresses);
    if (!success)
    {
        return -1;
    }
    for (auto &[index, entry] : entries)
    {
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
