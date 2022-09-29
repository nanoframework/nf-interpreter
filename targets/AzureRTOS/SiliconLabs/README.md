# Silabs Gecko SDK

## Configuration for USB devices and classes

### WinUSB devices

In order to add a WinUSB device configuration, the follow is required:

1. Set build option `GECKO_FEATURE_USBD_WINUSB` to `ON`.
1. Add the following files to the target folder (mind the location)
    [sl_usbd_class_vendor_instances.c](SL_STK3701A/autogen/sl_usbd_class_vendor_instances.c)

    [sl_usbd_class_vendor_instances.h](SL_STK3701A/autogen/sl_usbd_class_vendor_instances.h)

    [sl_usbd_class_winusb_config.h](SL_STK3701A/config/sl_usbd_class_winusb_config.h)

1. Add the following compiler definition to the target CMakelists.txt `NANO_SL_USBD_CLASS_VENDOR_DESCRIPTION=\"Description-of-your-deice-goes-here\"` (mind the maximum length of 32).
