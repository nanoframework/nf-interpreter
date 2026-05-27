//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <hal_nf_community.h>
#include <cmsis_os.h>

#include <targetHAL.h>
#include <nanoHAL_v2.h>
#include <nanoCLR_Application.h>
#include <nanoPAL_BlockStorage.h>

void AssertBlockStorage()
{
    // if the target Device_BlockStorageConfig (and collections) and the linker file have an address mismatch
    // the assertions below will cause an exception
    // these checks are only included when RTM build option isn't specified
    BlockStorageStream stream;
    memset(&stream, 0, sizeof(BlockStorageStream));

    // DEPLOYMENT block
    // this one lives in a single block so we are OK with checking start, end and length
    BlockStorageStream_Initialize(&stream, BlockUsage_DEPLOYMENT);
    ASSERT(stream.BaseAddress == (uint32_t)&__deployment_start__);
    // ASSERT(stream.Length == ((uint32_t)&__deployment_end__) - ((uint32_t)&__deployment_start__));

    // CODE block (CLR)
    // this one can be spread accross several blocks so we can only check the start address
    memset(&stream, 0, sizeof(BlockStorageStream));
    BlockStorageStream_Initialize(&stream, BlockUsage_CODE);
    ASSERT(stream.BaseAddress == (uint32_t)&__nanoImage_start__);
    // ASSERT(stream.Length == ((uint32_t)&__nanoImage_end__) - ((uint32_t)&__nanoImage_start__));

    // CONFIG block
    // this one lives in a single block so we are OK with checking start, end and length
    memset(&stream, 0, sizeof(BlockStorageStream));
    BlockStorageStream_Initialize(&stream, BlockUsage_CONFIG);
    ASSERT(stream.BaseAddress == (uint32_t)&__nanoConfig_start__);
    ASSERT(stream.Length == ((uint32_t)&__nanoConfig_end__) - ((uint32_t)&__nanoConfig_start__));
}

__attribute__((noreturn)) void CLRStartupThread(void const *argument)
{
    CLR_SETTINGS *clrSettings = (CLR_SETTINGS *)argument;

    // initialize nanoHAL
    nanoHAL_Initialize_C();

    ///////////////////////////////////////////
    // sanity check of block storage addresses
#if !defined(BUILD_RTM)
    AssertBlockStorage();
#endif
    ///////////////////////////////////////////

    ClrStartup(*clrSettings);

    // loop until thread receives a request to terminate
    while (1)
    {
        osDelay(500);
    }

    // this function never returns
}
