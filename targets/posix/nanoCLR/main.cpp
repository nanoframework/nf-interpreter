//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Test harness / standalone runner for nanoFramework.nanoCLR.dylib / .so.
// Calls through the public exported API — no direct CLR symbols.
//
// Usage:
//   nanoFramework.nanoCLR.test [--assemblies] file1.pe [file2.pe ...]
//
// All positional arguments (and arguments after --assemblies) that end in
// .pe are treated as managed assembly files to load before running the CLR.
// The CLR resolves references and executes the entry-point assembly.
// Assemblies must be provided in dependency order (mscorlib.pe first).

#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

#include "nanoCLR_native.h"

namespace
{
    void PrintBanner()
    {
        std::cout << ".NET nanoFramework nanoCLR " << NANOCLR_PLATFORM_NAME
                  << " v" << NANOCLR_POSIX_VERSION_STRING << "\n";
        std::cout << "Copyright (c) .NET Foundation and Contributors\n\n";
    }

    // Read a file into a byte vector. Returns false on error.
    bool ReadFile(const char *path, std::vector<uint8_t> &out)
    {
        std::ifstream in(path, std::ios::binary | std::ios::ate);
        if (!in.is_open())
        {
            std::cerr << "error: cannot open '" << path << "'\n";
            return false;
        }

        auto size = in.tellg();
        if (size <= 0)
        {
            std::cerr << "error: empty file '" << path << "'\n";
            return false;
        }

        in.seekg(0);
        out.resize(static_cast<size_t>(size));
        const auto bytesToRead = static_cast<std::streamsize>(out.size());
        if (!in.read(reinterpret_cast<char *>(out.data()), bytesToRead))
        {
            std::cerr << "error: failed to read '" << path << "'\n";
            return false;
        }

        return true;
    }

    // Convert a narrow (ASCII) path to a char16_t string.
    // char16_t matches the 2-byte UTF-16 LE units that nanoCLR_LoadAssembly expects
    // (same layout as CharSet.Unicode marshalling from the C# host).
    std::u16string ToChar16(const char *s)
    {
        return std::u16string(s, s + std::strlen(s));
    }

} // namespace

int main(int argc, char **argv)
{
    PrintBanner();

    // ── Collect .pe paths from command line ──────────────────────────────────
    // Accepts:  [--assemblies] file.pe file2.pe ...
    // The --assemblies flag is accepted but not required (mirrors nanoclr CLI).
    std::vector<std::string> peFiles;

    for (int i = 1; i < argc; i++)
    {
        if (std::strcmp(argv[i], "--assemblies") == 0)
            continue; // skip the flag itself

        std::string arg(argv[i]);
        if (arg.size() < 3 || arg.compare(arg.size() - 3, 3, ".pe") != 0)
        {
            std::cerr << "error: unexpected argument '" << arg << "' (expected a .pe file)\n";
            return 1;
        }

        peFiles.push_back(arg);
    }

    // ── Load assemblies into the CLR before starting ─────────────────────────
    for (const auto &path : peFiles)
    {
        std::vector<uint8_t> data;
        if (!ReadFile(path.c_str(), data))
            return 1;

        std::u16string wname = ToChar16(path.c_str());
        int hr = nanoCLR_LoadAssembly(wname.c_str(), data.data(), data.size());
        if (hr != 0)
        {
            std::cerr << "error: failed to load '" << path
                      << "' (hr=0x" << std::hex << hr << ")\n";
            return 1;
        }

        std::cout << "Loaded: " << path << "\n";
    }

    if (!peFiles.empty())
        std::cout << "\n";

    // ── Run the CLR ──────────────────────────────────────────────────────────
    NANO_CLR_SETTINGS settings{};
    settings.MaxContextSwitches         = 50;
    settings.WaitForDebugger            = false;
    settings.EnterDebuggerLoopAfterExit = false;
    settings.PerformGarbageCollection   = false;
    settings.PerformHeapCompaction      = false;

    nanoCLR_Run(settings);

    return 0;
}
