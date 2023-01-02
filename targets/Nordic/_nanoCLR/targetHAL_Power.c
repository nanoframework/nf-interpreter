//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <zephyr.h>
#include <nanoHAL_v2.h>

#include <hal/nrf_power.h>

int sys_reboot(int);

inline void CPU_Reset()
{
    printk("CPU_Reset called\n");
    // Reboot the system
#ifdef CONFIG_LOG
    LOG_WRN("Reboot device...");
    LOG_PANIC();
#else
#if CONFIG_LTE
    printf("\nPowering down modem...\n");
    adb_lte_power_off();
#if 0
	nrf_modem_lib_shutdown_wait();
#else
    nrf_modem_lib_shutdown();
#endif
#endif

#endif
    printf("\nRebooting device...\n");
    sys_reboot(0);

    printk("reboot failed. Halting\n");
    while (1)
    {
        k_sleep(K_MSEC(30000));
    }
}

// CPU sleep is not currently implemented in this target
inline void CPU_Sleep(SLEEP_LEVEL_type level, uint64_t wakeEvents)
{
    (void)level;
    (void)wakeEvents;
};

inline bool CPU_IsSoftRebootSupported()
{
    return true;
};

void CPU_SetPowerMode(PowerLevel_type powerLevel)
{
    // TODO power modes support
    (void)powerLevel;
}
