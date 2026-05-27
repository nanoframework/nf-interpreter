//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright 2018 Silicon Laboratories Inc. All rights reserved.
// See LICENSE file in the project root for full license information.
//

// Need a strong implementation (despite empty) as this one is declared as WEAK in Gecko SDK which is causing
// issues when linking statically.
// Reported to Silabs here:
// (https://community.silabs.com/s/question/0D58Y0000AUTvt2SQD/declaration-of-slisleeptimersetpmemrequirement)

#if !defined(SL_CATALOG_POWER_MANAGER_PRESENT)
void sli_sleeptimer_set_pm_em_requirement(void)
{
}
#endif

#if !defined(SL_CATALOG_POWER_MANAGER_NO_DEEPSLEEP_PRESENT) &&                                                         \
    !defined(SL_CATALOG_POWER_MANAGER_DEEPSLEEP_BLOCKING_HFXO_RESTORE_PRESENT)
void sli_sleeptimer_hal_power_manager_integration_init(void)
{
}
#endif
