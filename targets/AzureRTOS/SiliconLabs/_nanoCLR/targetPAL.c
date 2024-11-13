//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <em_device.h>
#include <em_gpio.h>

#include <targetHAL.h>
#include <tx_api.h>

// need to declare these as extern instead of including the header files
extern void usb_device_hid_app_init(void);
extern void sli_usbd_configuration_config0_init(void);
extern void sli_usbd_hid_hid0_init(void);
extern void sli_usbd_init(void);
extern void sli_usbd_configuration_config0_init(void);
extern void sli_usbd_cdc_acm_acm0_init(void);
extern void usb_device_cdc_acm_app_init(void);

// required for Azure RTOS TX_INTERRUPT_SAVE_AREA implementation at platform level
unsigned int interrupt_save;

// common code used to start USB stack and various components
void UsbStackInit()
{

#if GECKO_FEATURE_USBD_HID == TRUE || HAL_WP_USE_USB_CDC == TRUE || GECKO_FEATURE_USBD_WINUSB == TRUE
    // wait a couple of seconds to allow all other threads to init
    tx_thread_sleep(TX_TICKS_PER_MILLISEC(2000));

    // can't call USBD init twice
    sli_usbd_init();
    sli_usbd_configuration_config0_init();
#endif

#if GECKO_FEATURE_USBD_HID == TRUE
    sli_usbd_hid_hid0_init();
    usb_device_hid_app_init();
#endif

#if HAL_WP_USE_USB_CDC == TRUE
    sli_usbd_cdc_acm_acm0_init();
    usb_device_cdc_acm_app_init();
#endif
}
