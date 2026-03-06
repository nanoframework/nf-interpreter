//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <iostream>
#include <string_view>

namespace
{
void PrintBanner()
{
    std::cout << ".NET nanoFramework nanoCLR OSX v" << NANOCLR_OSX_VERSION_STRING << "\n";
    std::cout << "Copyright (c) .NET Foundation and Contributors\n\n";
}

void PrintUsage(const char *executableName)
{
    std::cout << "Usage: " << executableName << " [--help] [--version]\n";
    std::cout << "  --help      Show this help text.\n";
    std::cout << "  --version   Show version and exit.\n";
}
} // namespace

int main(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        const std::string_view arg(argv[i]);

        if (arg == "--help" || arg == "-h")
        {
            PrintBanner();
            PrintUsage(argv[0]);
            return 0;
        }

        if (arg == "--version" || arg == "-v")
        {
            std::cout << NANOCLR_OSX_VERSION_STRING << "\n";
            return 0;
        }
    }

    PrintBanner();

#if NANO_OSX_ENABLE_SMOKE
    // TODO: Replace with real CLR startup flow once shared runtime portability shims are in place.
    std::cout << "Smoke mode is enabled. Runtime startup is not wired yet.\n";
    return 0;
#else
    std::cerr << "Smoke mode is disabled and runtime startup is not available yet.\n";
    return 2;
#endif
}
