//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <iostream>

namespace
{
void PrintBanner()
{
    std::cout << ".NET nanoFramework nanoCLR OSX v" << NANOCLR_OSX_VERSION_STRING << "\n";
    std::cout << "Copyright (c) .NET Foundation and Contributors\n\n";
}
} // namespace

int main()
{
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
