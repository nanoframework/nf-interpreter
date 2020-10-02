//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _TOUCHINTERFACE_H_
#define _TOUCHINTERFACE_H_ 1

#include "nanoCLR_Types.h"

struct TouchInterface
{
public:
    bool Initialize();
    CLR_UINT8* Write_Read(CLR_UINT8* valuesToSend, CLR_UINT16 numberOfValuesToSend, CLR_UINT16 numberValuesExpected);
};

#endif // _TOUCHINTERFACE_H_


