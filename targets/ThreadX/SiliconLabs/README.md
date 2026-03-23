# Silabs Gecko SDK

## Configuration for USB devices and classes

### USB serial number

The USB serial number string is defined with `SL_USBD_DEVICE_SERIAL_NUMBER_STRING` in sl_usbd_device_config.h.
It can be hardcoded by setting it to a string like `"1234567890ABCDEF"`.
As an alternative it can be generated from the device unique ID. For this the define has to be set to `(const char *)&UsbSerialNumber` and the following declaration has to be added `extern char *UsbSerialNumber[];`. Please check the example [here](SL_STK3701A/config/sl_usbd_device_config.h).

### WinUSB devices

In order to add a WinUSB device configuration, the follow is required:

1. Set build option `GECKO_FEATURE_USBD_WINUSB` to `ON`. As an alternative, if you plan to use the UsbStream # class, the latter is not required. Just set to `ON` the respective build option `API_System.Device.UsbStream`.

1. Add the following files to the target folder (mind the location)
    [sl_usbd_class_vendor_instances.c](SL_STK3701A/autogen/sl_usbd_class_vendor_instances.c)

    [sl_usbd_class_vendor_instances.h](SL_STK3701A/autogen/sl_usbd_class_vendor_instances.h)

    [sl_usbd_class_winusb_config.h](SL_STK3701A/config/sl_usbd_class_winusb_config.h)

1. If defining the WinUSB device before compile time: define a GUID for the device class and set define `DEVICE_CLASS_GUID_PROPERTY` with it. Check [sl_usbd_class_winusb_config.h](SL_STK3701A/config/sl_usbd_class_winusb_config.h).
For details check [Microsoft documentation](https://learn.microsoft.com/en-us/windows-hardware/drivers/usbcon/automatic-installation-of-winusb#registering-a-device-interface-guid) about setting a WinUSB device that does not require an INF file to install.

1. In case the `System.Device.UsbStream` is being used, the device description and class GUID are defined in the C# code, therefore no other configuration is required at build time.

> **Warning** When performing a debug session from Visual Studio, if the deployment occurs on a blank device, when calling `UsbClient.CreateUsbStream()` despite the WinUSB device is added, it won't show up as no USB enumeration takes place. This will NOT happen on subsequent debug sessions nor on a device running the application standalone. The reason for this is that when that call is executed, a new USB interface and associated end points are added to the USB device configuration in order to add that new WinUSB device supporting the `UsbStream`. Following this, it would be expected that an USB re-enumeration would occur. This won't happen when there is a debugger connected in order to prevent the debug session from being abruptly interrupted.
There a limitation of the Gecko SDK as it doesn't allow adding a new interface after the USB device core is running. Stopping the USB device entirely to perform this action is not possible (or better, not desirable) because the CDC device on the same USB composite is being used to connect to Visual Studio. Stopping it would cease communication with Visual Studio, thus ending the debug session.
Because of this limitation the call to `UsbClient.CreateUsbStream()` has to be done **AS EARLY AS POSSIBLE** in the C# application. Preferably in the very first line of `Main()` thus ensuring that the descriptors are added to the USB configuration before the USB Core is initialized.
