//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <target_platform.h>
#include <esp32_idf.h>
#include <nanoHAL_v2.h>

#if CONFIG_IDF_TARGET_ESP32C3
#include <soc/rtc_cntl_reg.h>
#endif

inline void CPU_Reset()
{
#if CONFIG_IDF_TARGET_ESP32C3 && HAL_WP_USE_USB_CDC
    SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_SW_SYS_RST);
    while (true)
    {
    }
#else
    esp_restart();
#endif
};

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
    switch (powerLevel)
    {
        case PowerLevel__Off:
            // gracefully shutdown everything
            nanoHAL_Uninitialize_C(true);

            esp_deep_sleep_start();

            break;

        default:
            // all the other power modes are unsupported here
            break;
    }
}
