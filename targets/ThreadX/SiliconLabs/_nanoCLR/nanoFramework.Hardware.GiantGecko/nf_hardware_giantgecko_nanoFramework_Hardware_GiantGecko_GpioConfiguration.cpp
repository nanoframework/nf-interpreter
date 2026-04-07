//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_hardware_giantgecko.h"
#include <CPU_GPIO_decl.h>
#include <em_device.h>
#include <em_gpio.h>
#include <sl_assert.h>

// need these declared as extern here because they belong to GPIO driver
extern bool CPU_GPIO_PinIsBusy(GPIO_PIN pinNumber);
extern void GetIoLine(int16_t pinNumber, GPIO_Port_TypeDef *port, uint32_t *portPin);

HRESULT Library_nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_GpioConfiguration::
    SetSlewRate___STATIC__VOID__I4__U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_CHECK_HRESULT(SetSlewRate(stack, false));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_GpioConfiguration::
    SetSlewRateAlternate___STATIC__VOID__I4__U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_CHECK_HRESULT(SetSlewRate(stack, true));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_GpioConfiguration::
    SetDriveStrenght___STATIC__VOID__I4__nanoFrameworkHardwareGiantGeckoGpioConfigurationDriveStrenght(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_CHECK_HRESULT(SetDriveStrenght(stack, false));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_GpioConfiguration::
    SetDriveStrenghtAlternate___STATIC__VOID__I4__nanoFrameworkHardwareGiantGeckoGpioConfigurationDriveStrenght(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_CHECK_HRESULT(SetDriveStrenght(stack, true));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_GpioConfiguration::SetSlewRate(
    CLR_RT_StackFrame &stack,
    bool isAlternate)
{
    NANOCLR_HEADER();

    GPIO_PIN pinNumber;
    uint8_t slewRate;
    GPIO_Port_TypeDef *port;
    uint32_t *portPin;

    // get the pin number
    pinNumber = (GPIO_PIN)stack.Arg0().NumericByRef().s4;

    // check if this is a valid GPIO pin
    if (!CPU_GPIO_PinIsBusy(pinNumber))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    // get the slew rate
    slewRate = (uint8_t)stack.Arg1().NumericByRef().u1;

    // check if the slew rate is valid
    // OK to use the NON alternate mask here because the value is the same for both
    if (slewRate > (_GPIO_P_CTRL_SLEWRATE_MASK >> _GPIO_P_CTRL_SLEWRATE_SHIFT))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // get the port and pin
    GetIoLine(pinNumber, port, portPin);

    // replicate the code from the Gecko SDK
    if (isAlternate)
    {
        GPIO->P[*port].CTRL =
            (GPIO->P[*port].CTRL & ~(_GPIO_P_CTRL_SLEWRATEALT_MASK)) | (slewRate << _GPIO_P_CTRL_SLEWRATEALT_SHIFT);
    }
    else
    {
        GPIO->P[*port].CTRL =
            (GPIO->P[*port].CTRL & ~(_GPIO_P_CTRL_SLEWRATE_MASK)) | (slewRate << _GPIO_P_CTRL_SLEWRATE_SHIFT);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_GpioConfiguration::SetDriveStrenght(
    CLR_RT_StackFrame &stack,
    bool isAlternate)
{
    NANOCLR_HEADER();

    GPIO_PIN pinNumber;
    GPIO_DriveStrength_TypeDef strength;
    GPIO_Port_TypeDef *port;
    uint32_t *portPin;

    // get the pin number
    pinNumber = (GPIO_PIN)stack.Arg0().NumericByRef().s4;

    // check if this is a valid GPIO pin
    if (!CPU_GPIO_PinIsBusy(pinNumber))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    // get the drive strenght
    strength = (GPIO_DriveStrength_TypeDef)stack.Arg1().NumericByRef().u1;

    // get the port and pin
    GetIoLine(pinNumber, port, portPin);

    // replicate the code from the Gecko SDK
    if (isAlternate)
    {
        BUS_RegMaskedWrite(
            &GPIO->P[*port].CTRL,
            _GPIO_P_CTRL_DRIVESTRENGTHALT_MASK,
            strength << _GPIO_P_CTRL_DRIVESTRENGTHALT_SHIFT);
    }
    else
    {
        BUS_RegMaskedWrite(&GPIO->P[*port].CTRL, _GPIO_P_CTRL_DRIVESTRENGTH_MASK, strength);
    }

    NANOCLR_NOCLEANUP();
}
