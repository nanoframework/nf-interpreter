//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "stdafx.h"
#include <iostream>

void HAL_Windows_Debug_Print(char *szText)
{
    std::string str = (std::string)szText;

    // clear trailing LR & CR
    int pos;
    if ((pos = str.find_last_of('\n')) != std::string::npos)
    {
        str.erase(pos);
    }
    if ((pos = str.find_last_of('\r')) != std::string::npos)
    {
        str.erase(pos);
    }

    std::cout << (std::string)str + "\n" << std::flush;
}
