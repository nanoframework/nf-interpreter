# Silabs Gecko SDK

## Configuration for USB devices and classes

### USB serial number

The USB serial number string is defined with `SL_USBD_DEVICE_SERIAL_NUMBER_STRING` in sl_usbd_device_config.h.
It can be hardcoded by setting it to a string like `"1234567890ABCDEF"`.
As an alternative it can be generated from the device unique ID. For this the define has to be set to `(const char *)&UsbSerialNumber` and the following declaration has to be added `extern char *UsbSerialNumber[];`. Please check the example [here](SL_STK3701A/config/sl_usbd_device_config.h).

### WinUSB devices

In order to add a WinUSB device configuration, the follow is required:

1. Set build option `GECKO_FEATURE_USBD_WINUSB` to `ON`.

1. Add the following files to the target folder (mind the location)
    [sl_usbd_class_vendor_instances.c](SL_STK3701A/autogen/sl_usbd_class_vendor_instances.c)

    [sl_usbd_class_vendor_instances.h](SL_STK3701A/autogen/sl_usbd_class_vendor_instances.h)

    [sl_usbd_class_winusb_config.h](SL_STK3701A/config/sl_usbd_class_winusb_config.h)

1. Add the following compiler definition to the target CMakelists.txt `NANO_SL_USBD_CLASS_VENDOR_DESCRIPTION=\"Description-of-your-deice-goes-here\"` (mind the maximum length of 32).

1. Define a GUID for the device class and set define `DEVICE_CLASS_GUID_PROPERTY` with it. Check [sl_usbd_class_winusb_config.h](SL_STK3701A/config/sl_usbd_class_winusb_config.h).
For details check [Microsoft documentation](https://learn.microsoft.com/en-us/windows-hardware/drivers/usbcon/automatic-installation-of-winusb#registering-a-device-interface-guid) about setting a WinUSB device that does not require an INF file to install.
