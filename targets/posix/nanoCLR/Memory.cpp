//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <cstdlib>
#include <cstring>
#include <mutex>

namespace
{
    constexpr size_t c_DefaultHeapSizeBytes = 10 * 1024 * 1024;

    unsigned char *g_MemoryStart = nullptr;
    unsigned char *g_CustomHeapStart = nullptr;
    std::once_flag g_DefaultHeapOnce;
    std::once_flag g_CustomHeapOnce;
} // namespace

void HeapLocation(unsigned char *&BaseAddress, unsigned int &SizeInBytes)
{
    std::call_once(g_DefaultHeapOnce, []() {
        // TODO: evaluate mmap() if we need stronger parity with win32 VirtualAlloc behavior.
        g_MemoryStart = static_cast<unsigned char *>(std::malloc(c_DefaultHeapSizeBytes));

        if (g_MemoryStart != nullptr)
        {
            std::memset(g_MemoryStart, 0xEA, c_DefaultHeapSizeBytes);
        }
    });

    BaseAddress = g_MemoryStart;
    SizeInBytes = (g_MemoryStart != nullptr) ? static_cast<unsigned int>(c_DefaultHeapSizeBytes) : 0;
}

void CustomHeapLocation(unsigned char *&BaseAddress, unsigned int &SizeInBytes)
{
    std::call_once(g_CustomHeapOnce, []() {
        // TODO: revisit custom heap ownership model when CLR runtime is wired.
        g_CustomHeapStart = static_cast<unsigned char *>(std::malloc(c_DefaultHeapSizeBytes));

        if (g_CustomHeapStart != nullptr)
        {
            std::memset(g_CustomHeapStart, 0xEA, c_DefaultHeapSizeBytes);
        }
    });

    BaseAddress = g_CustomHeapStart;
    SizeInBytes = (g_CustomHeapStart != nullptr) ? static_cast<unsigned int>(c_DefaultHeapSizeBytes) : 0;
}
