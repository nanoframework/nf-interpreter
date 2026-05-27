//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef TOUCHINTERFACE_H
#define TOUCHINTERFACE_H

#include "nanoCLR_Types.h"

struct TouchInterface
{
public:
    bool Initialize();
    CLR_UINT8* Write_Read(CLR_UINT8* valuesToSend, CLR_UINT16 numberOfValuesToSend, CLR_UINT16 numberValuesExpected);
};

#endif // TOUCHINTERFACE_H


