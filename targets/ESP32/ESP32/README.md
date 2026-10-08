# ESP32

This reference target _fits_ all ESP32 boards carrying an ESP32 chip. Depending on the build options and SDK CONFIG passed to the build it can build images for revision 1 or revision 3 chips, PICO, supporting Ethernet connection and BLE (experimental).

Check the details at the documentation website [here](http://docs.nanoframework.net/content/reference-targets/esp32.html)

Getting started guides and build instructions can also be found at the documentation website:

- [Build instructions](https://docs.nanoframework.net/content/building/build-esp32.html)
- [Getting started with managed code (C#)](https://docs.nanoframework.net/content/getting-started-guides/getting-started-managed.html)

## Camera and PWM resources

The camera clock reserves low-speed LEDC timer 3 and channel 7 while the camera is
initialized. Camera initialization fails if PWM already owns either resource.
While the camera is active, PWM cannot allocate low-speed timer 3 (PWM timer ID 7
on ESP32) or channel 7, but other timers and channels remain available, including
high-speed timer 3. Stopping PWM does not release its reservation; dispose the PWM
channel before initializing the camera.

Disposing the camera or PWM channel releases its LEDC resources. Soft reboot also
cleans up both APIs and releases their reservations.
