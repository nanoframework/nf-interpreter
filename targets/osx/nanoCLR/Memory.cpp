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

unsigned char *s_MemoryStart = nullptr;
unsigned char *s_CustomHeapStart = nullptr;
std::once_flag s_DefaultHeapOnce;
std::once_flag s_CustomHeapOnce;
} // namespace

void HeapLocation(unsigned char *&BaseAddress, unsigned int &SizeInBytes)
{
    std::call_once(s_DefaultHeapOnce, []() {
        // TODO: evaluate mmap() if we need stronger parity with win32 VirtualAlloc behavior.
        s_MemoryStart = static_cast<unsigned char *>(std::malloc(c_DefaultHeapSizeBytes));

        if (s_MemoryStart != nullptr)
        {
            std::memset(s_MemoryStart, 0xEA, c_DefaultHeapSizeBytes);
        }
    });

    BaseAddress = s_MemoryStart;
    SizeInBytes = static_cast<unsigned int>(c_DefaultHeapSizeBytes);
}

void CustomHeapLocation(unsigned char *&BaseAddress, unsigned int &SizeInBytes)
{
    std::call_once(s_CustomHeapOnce, []() {
        // TODO: revisit custom heap ownership model when CLR runtime is wired.
        s_CustomHeapStart = static_cast<unsigned char *>(std::malloc(c_DefaultHeapSizeBytes));

        if (s_CustomHeapStart != nullptr)
        {
            std::memset(s_CustomHeapStart, 0xEA, c_DefaultHeapSizeBytes);
        }
    });

    BaseAddress = s_CustomHeapStart;
    SizeInBytes = static_cast<unsigned int>(c_DefaultHeapSizeBytes);
}
