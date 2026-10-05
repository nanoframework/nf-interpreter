# ESP32

This reference target _fits_ all ESP32 boards carrying an ESP32 chip. Depending on the build options and SDK CONFIG passed to the build it can build images for revision 1 or revision 3 chips, PICO, supporting Ethernet connection and BLE (experimental).

Check the details at the documentation website [here](http://docs.nanoframework.net/content/reference-targets/esp32.html)

Getting started guides and build instructions can also be found at the documentation website:

- [Build instructions](https://docs.nanoframework.net/content/building/build-esp32.html)
- [Getting started with managed code (C#)](https://docs.nanoframework.net/content/getting-started-guides/getting-started-managed.html)

## Camera native contract and initialization

Enable `CONFIG_API_NANOFRAMEWORK_ESP32_CAMERA` to include the production camera
implementation. The native registration for `nanoFramework.Esp32.Camera` uses
checksum `0x775EFC2E`, version `100.0.0.0`, and method slots 76-85. Compare both
the registration table and header against the managed generated stubs when
updating the assembly: matching the checksum alone does not verify method slots.
The managed framebuffer-location values are PSRAM=0 and DRAM=1.

Initialization logs `Production NativeInit`, the chip and camera configuration,
available internal/PSRAM heap, resource-reservation failures, and the result of
`esp_camera_init()` with its ESP-IDF error name. A missing entry log requires
checking the flashed firmware and native method table before debugging hardware.
Sensor-not-found, unsupported-sensor, allocation, timeout, and driver errors are
reported as distinct CLR errors; consult the accompanying ESP-IDF logs to
distinguish sensor/SCCB, clock, DMA, and framebuffer failures.

For initial AI Thinker ESP32-CAM validation, use JPEG, QVGA maximum frame size,
one framebuffer in DRAM, and `WhenEmpty` grab mode. The standard pin mapping is
PWDN=32, RESET=-1, XCLK=0, SCCB SDA=26/SCL=27, D0-D7=5/18/19/21/36/39/34/35,
VSYNC=25, HREF=23, and PCLK=22, with a 20 MHz XCLK. Confirm the log reports
`frame=6`, `buffers=1`, `location=DRAM`, `grab=0`, then
`esp_camera_init returned 0x0 (ESP_OK)` and `Initialization complete`.
Also verify capture, disposal/reinitialization, and rejection of a second live
camera instance on hardware. PSRAM is not required for this DRAM configuration.

## Camera and PWM resources

The camera clock reserves low-speed LEDC timer 3 and channel 7 while the camera is
initialized. Camera initialization fails if PWM already owns either resource.
While the camera is active, PWM cannot allocate low-speed timer 3 (PWM timer ID 7
on ESP32) or channel 7, but other timers and channels remain available, including
high-speed timer 3. Stopping PWM does not release its reservation; dispose the PWM
channel before initializing the camera.

Disposing the camera or PWM channel releases its LEDC resources. Soft reboot also
cleans up both APIs and releases their reservations.

## Camera capture scheduling

Frame acquisition runs in a native worker, so waiting for a camera frame suspends
only the calling managed thread. Concurrent capture calls wait their turn. Managed
array allocation and copying still run on the CLR thread; capture-to-buffer
reacquires its destination after resuming to allow garbage collection during the wait.

The driver can return no frame after its capture timeout (approximately four
seconds); both capture methods continue to report a timeout exception in that case.
Disposal waits cooperatively for an in-flight acquisition to finish before stopping
the worker and deinitializing the driver. Soft-reboot cleanup also joins the worker
before releasing frames, pins and LEDC resources.

The capture worker uses a 2048-byte stack. Stack headroom and managed-thread/timer
responsiveness should be verified on hardware at the largest supported resolution.
