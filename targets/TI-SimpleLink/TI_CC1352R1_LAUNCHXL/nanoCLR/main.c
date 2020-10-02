//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <stdint.h>
#include <nanoCLR_Application.h>
#include <nanoHAL_v2.h>

// RTOS header files
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

// board Header files
#include <ti_drivers_config.h>

#include <ti/drivers/gpio/GPIOCC26XX.h>
// clang-format off
#include DeviceFamily_constructPath(inc/hw_prcm.h)
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)
// clang-format on

//////////////////////////////

// Stack size in bytes
#define THREADSTACKSIZE 2048

Task_Handle receiverHandle;
Task_Handle clrHandle;

CLR_SETTINGS clrSettings;

// this define has to match the one in cpu_gpio.cpp
#define GPIO_MAX_PINS 16

// these are declared in cpu_gpio.cpp
extern GPIO_PinConfig gpioPinConfigs[GPIO_MAX_PINS];
extern GPIO_CallbackFxn gpioCallbackFunctions[GPIO_MAX_PINS];

// this has to be define in a C file, otherwise the linker can't replace the weak one declared in the SDK driver library
const GPIOCC26XX_Config GPIOCC26XX_config = {
    .pinConfigs = (GPIO_PinConfig *)gpioPinConfigs,
    .callbacks = (GPIO_CallbackFxn *)gpioCallbackFunctions,
    .numberOfPinConfigs = GPIO_MAX_PINS,
    .numberOfCallbacks = GPIO_MAX_PINS,
    .intPriority = (~0)};

extern void ReceiverThread(UArg arg0, UArg arg1);
extern void CLRStartupThread(UArg arg0, UArg arg1);

int main(void)
{
    Task_Params taskParams;

    // get and store reset reason
    // must be called before PIN_init()
    WakeupReasonStore = SysCtrlResetSourceGet();

    // Call board init functions
    Board_init();

    // setup Task thread
    Task_Params_init(&taskParams);
    taskParams.stackSize = THREADSTACKSIZE;
    taskParams.priority = 4;

    // create Receiver
    receiverHandle = Task_create((Task_FuncPtr)ReceiverThread, &taskParams, Error_IGNORE);
    if (receiverHandle == NULL)
    {
        while (1)
            ;
    }

    // CLR settings to launch CLR thread
    (void)memset(&clrSettings, 0, sizeof(CLR_SETTINGS));

    clrSettings.MaxContextSwitches = 50;
    clrSettings.WaitForDebugger = false;
    clrSettings.EnterDebuggerLoopAfterExit = true;

    // setup CLR task
    taskParams.arg0 = (UArg)&clrSettings;
    taskParams.stackSize = THREADSTACKSIZE;
    taskParams.priority = 4;
    clrHandle = Task_create(CLRStartupThread, &taskParams, Error_IGNORE);
    if (clrHandle == NULL)
    {
        while (1)
            ;
    }

    GPIO_init();
    ADC_init();
    ConfigUART();

    BIOS_start();

    return (0);
}

///////////////////////////////////////////////////////////////////////
// need this dummy implementation here (started with SDK 4.20.01.04) //
///////////////////////////////////////////////////////////////////////
void __attribute__((naked)) _exit(int code)
{
    (void)code;

    for (;;)
        ;
}
