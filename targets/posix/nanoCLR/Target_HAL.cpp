//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <iostream>
#include <string>

void HAL_Windows_Debug_Print(const char *szText)
{
    if (szText == nullptr)
    {
        return;
    }

    std::string text(szText);

    while (!text.empty() && (text.back() == '\r' || text.back() == '\n'))
    {
        text.pop_back();
    }

    std::cout << text << "\n" << std::flush;
}
