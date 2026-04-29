//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Stub for ChibiOS HAL community init not needed by MCUboot.
//
// halCommunityInit(): MCUboot uses only the nf-overlay flash driver and does not
//   include ChibiOS-Contrib. hal.c calls halCommunityInit() when HAL_USE_COMMUNITY
//   is TRUE; this no-op stub satisfies the linker without pulling in Contrib.

void halCommunityInit(void)
{
}
