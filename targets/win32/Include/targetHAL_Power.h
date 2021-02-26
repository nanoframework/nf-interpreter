//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _TARGET_HAL_POWER_H_
#define _TARGET_HAL_POWER_H_ 1

inline void CPU_Reset(){ ::ExitProcess( 0 ); };
inline bool CPU_IsSoftRebootSupported() { return true; };

#endif //_TARGET_HAL_POWER_H_
