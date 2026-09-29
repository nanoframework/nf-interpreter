//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <target_platform.h>
#include <esp32_idf.h>
#include <nanoHAL_v2.h>
#include <WireProtocol_Transport.h>

// the classic ESP32 is absent because it has no FORCE_DOWNLOAD_BOOT bit
#if defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
#include <soc/rtc_cntl_reg.h>
#elif defined(CONFIG_IDF_TARGET_ESP32C5) || defined(CONFIG_IDF_TARGET_ESP32C6) ||                                      \
    defined(CONFIG_IDF_TARGET_ESP32C61) || defined(CONFIG_IDF_TARGET_ESP32H2)
#include <soc/lp_aon_reg.h>
#elif defined(CONFIG_IDF_TARGET_ESP32P4)
#include <soc/lp_system_reg.h>
#endif

inline void CPU_Reset()
{
#if CONFIG_IDF_TARGET_ESP32C3
    if (g_WP_Transport == WP_TRANSPORT_USB_JTAG)
    {
        SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_SW_SYS_RST);
        while (true)
        {
        }
    }
#endif
    esp_restart();
};

// The proprietary bootloader of an ESP32 is the ROM download mode. The bit lives in the always-on
// domain, so it survives the reset that the caller performs next; a power cycle clears it.
#if defined(RTC_CNTL_OPTION1_REG) || defined(LP_AON_SYS_CFG_REG) || defined(LP_SYSTEM_REG_SYS_CTRL_REG)
bool RequestToLaunchProprietaryBootloader()
{
#if defined(RTC_CNTL_OPTION1_REG)
    REG_WRITE(RTC_CNTL_OPTION1_REG, RTC_CNTL_FORCE_DOWNLOAD_BOOT);
#elif defined(LP_AON_SYS_CFG_REG)
    // on C5 this is a two bit field where 01 is the UART and USB download
    REG_SET_FIELD(LP_AON_SYS_CFG_REG, LP_AON_FORCE_DOWNLOAD_BOOT, 1);
#else
    REG_SET_BIT(LP_SYSTEM_REG_SYS_CTRL_REG, LP_SYSTEM_REG_FORCE_DOWNLOAD_BOOT);
#endif

    return true;
}
#endif

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
