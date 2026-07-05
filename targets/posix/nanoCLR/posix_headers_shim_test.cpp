//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// This translation unit verifies that the shared CLR headers parse cleanly
// under Clang/GCC with no Windows SDK dependency.  It contains no executable
// code and is compiled as part of the shared library purely as a header
// correctness check.

#include <nanoCLR_PlatformDef.h>
#include <nanoCLR_Types.h>
#include <nanoCLR_Runtime.h>
