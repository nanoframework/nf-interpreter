//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef LAUNCHCLR_H
#define LAUNCHCLR_H

#include <nanoCLR_Headers.h>

void LaunchCLR(uint32_t address);
bool CheckValidCLRImage(uint32_t address);

#endif // LAUNCHCLR_H
