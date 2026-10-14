# Deployment Confirmation Timeout

A countdown that resets the device when a test-swapped deployment (image 1) isn't confirmed
in time. After the reset MCUboot reverts the deployment to the previous one.

Code: `MCUboot/include/MCUboot_StartupPolicy.h`, `MCUboot/common/MCUboot_StartupPolicy.c`
(policy), `targets/ChibiOS/_mcuboot/mcuboot_confirm_timer.c` (ChibiOS timer, compiled into nanoCLR), `src/CLR/Startup/CLRStartup.cpp`
(arm point), `src/CLR/Debugger/Debugger.cpp` and
`src/nanoFramework.Runtime.InFieldUpdate/` (confirm paths).

## Why

A deployment is only kept if the application confirms it
(`UpdateManager.ConfirmDeploymentImage()`). If the new application hangs, deadlocks or never
gets to that call, nothing reboots the device, so MCUboot never gets the chance to revert it.
The device stays stuck on the broken application until someone power-cycles it.

## Configuration

The application sets the timeout with an attribute on `Main`:

```csharp
using nanoFramework.Runtime.InFieldUpdate;

[DeploymentConfirmationTimeout(120)]
public static void Main() { ... }
```

The value is in seconds. `0`, or no attribute at all, leaves the countdown disarmed. The setting
lives in the deployment itself, so each deployment chooses its own value with no CLR rebuild
and no Kconfig option.

The CLR looks the attribute type up by name
(`nanoFramework.Runtime.InFieldUpdate.DeploymentConfirmationTimeoutAttribute`) and reads the
constructor argument from the attribute blob. It has no build dependency on the managed API.

## Life cycle

| Event | Effect |
|---|---|
| `Load()` succeeds, image 1 unconfirmed, attribute > 0 | armed, before `Execute()` (static constructors and the wait for a debugger are covered) |
| image 1 already confirmed, or not test-swapped | never armed, and the attribute isn't even parsed |
| `ConfirmDeploymentImage()` / `Monitor_ImageConfirm` for image 1 | stopped before the trailer write; re-armed with the same timeout if the confirm fails |
| debugger enables source-level debugging | cancelled for the rest of the power cycle |
| CLR soft reboot | keeps running, not re-armed and not extended |
| timer expires | `NVIC_SystemReset()`, and MCUboot reverts image 1 |

The watchdog arms at most once per power cycle. Once armed or cancelled it never arms again
until the next hard reset.

All confirmations of image 1 go through `nf_mcuboot_deploy_ok()`, so no confirm path can leave
the timer running. The timer is stopped *before* the trailer write, so the reset can't land in
the middle of it.

A plain `Monitor_Ping` (nanoff, Device Explorer) doesn't cancel the countdown. Only a real debug
session does. The image is still unconfirmed after a cancel, so it reverts on the next reboot
unless it gets confirmed.

## Deployment that can't start

When image 1 is unconfirmed and the deployment fails to load (bad record, native contract hash
mismatch, ...) or has no entry point, there is no `Main` to read the attribute from. The CLR
resets straight away so MCUboot reverts it. This doesn't depend on the attribute. A confirmed
deployment that fails to load keeps the old behaviour: it falls through to the debugger loop.

"Unconfirmed" is read from the live trailer (`nf_mcuboot_deploy_needs_confirmation()`), not
from the power-on snapshot. That way a confirmation made later in the same power cycle is seen,
and a soft reboot after a confirm can't trigger a reset. After a revert MCUboot sets `image_ok`
on the restored image, so this can't loop.

## Platform hooks

```c
bool nf_mcuboot_confirm_timer_start(uint32_t seconds); // one-shot, resets the CPU on expiry
void nf_mcuboot_confirm_timer_stop(void);              // thread context only
```

Weak defaults in `MCUboot_StartupPolicy.c` return `false` / do nothing, so on a platform without
an implementation the attribute is ignored.

ChibiOS: a `virtual_timer_t` that is `platform_malloc`'d only while armed (no RAM cost when the
attribute is absent). Its callback runs in ISR context and calls `NVIC_SystemReset()`. The timeout
is clamped to what `sysinterval_t` can hold.

Other RTOSes can map these hooks onto FreeRTOS `xTimerCreate` or ThreadX `tx_timer_create` once
MCUboot is enabled there.

## Paired confirmation

The CLR normally confirms image 0 at startup, before loading the deployment. When image 0 and
image 1 were updated together (both unconfirmed in the power-on snapshot,
`nf_mcuboot_clr_confirm_paired()`), that would let a failing new deployment revert alone, back to
an old deployment that then fails the native contract-hash check against the new CLR, with no
reset to follow because the old deployment is confirmed.

So in that case image 0 is not confirmed at startup. `nf_mcuboot_deploy_ok()` confirms it right
after image 1, and reports an error if that fails. Every outcome that leaves the deployment
unconfirmed (failed load, no entry point, timeout, reset, power cycle) reverts both images.

An update of image 0 alone (deployment already confirmed, or a Wire Protocol deployment with no
MCUboot trailer) is confirmed at startup as before.

No flag is needed to tell a paired update from two unrelated ones: the trailers are the flag. It
does not matter what staged the images - an update bundle on SD/USB, the managed
`UpdateManager.RequestUpdate()` (see "Requesting both images together" in
[session-update.md](session-update.md)), or two separate completions that reached the same reboot.
What matters is that both are pending at the same reboot, which `RequestUpdate()` and the bundle
importer guarantee and two separate completions don't.

Note for debugging: after a paired update the CLR stays unconfirmed until the deployment is
confirmed. A debug session cancels the countdown, but a reset before the application confirms
reverts both images.
