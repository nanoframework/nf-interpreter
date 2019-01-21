//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_hardware_stm32_native.h"
#include <nanoCLR_Hardware.h>

/////////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH nanoFramework.Hardware.Stm32.Power.WakeupPin (in managed code) !!!    //
/////////////////////////////////////////////////////////////////////////////////////////////////
enum WakeupPin
{
    WakeupPin_Pin1 = 1,
    WakeupPin_Pin2,
    WakeupPin_Pin3
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH nanoFramework.Hardware.Stm32.Power.WakeupReason (in managed code) !!!   //
///////////////////////////////////////////////////////////////////////////////////////////////////
enum WakeupReason
{
    WakeupReason_Undetermined = 0,
    WakeupReason_FromStandby,
    WakeupReason_FromPin
}; 

HRESULT Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Power::get_WakeupReason___STATIC__nanoFrameworkHardwareStm32PowerWakeupReasonType( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    stack.SetResult_U4( WakeupReasonStore );

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Power::DisableWakeupPin___STATIC__VOID__nanoFrameworkHardwareStm32PowerWakeupPin( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    uint32_t wakeUpPin;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    switch((WakeupPin)stack.Arg0().NumericByRef().u4)
    {
      #if defined(PWR_CSR_EWUP)
        case WakeupPin_Pin1:
            wakeUpPin = PWR_CSR_EWUP;
            break;
      #elif defined(PWR_CSR_EWUP1)
        case WakeupPin_Pin1:
            wakeUpPin = PWR_CSR_EWUP1;
            break;
      #elif defined(PWR_CSR2_EWUP1)
        case WakeupPin_Pin1:
            wakeUpPin = PWR_CSR2_EWUP1;
            break;
      #else
        #error "missing include for STM32 target PAL"
      #endif

      #if defined(PWR_CSR_EWUP2)
        case WakeupPin_Pin2:
            wakeUpPin = PWR_CSR_EWUP2;
            break;
      #elif defined(PWR_CSR2_EWUP2)
        case WakeupPin_Pin2:
            wakeUpPin = PWR_CSR2_EWUP2;
            break;
      #endif

      #if defined(PWR_CSR_EWUP3)
        case WakeupPin_Pin3:
            wakeUpPin = PWR_CSR_EWUP3;
            break;      
      #elif defined(PWR_CSR2_EWUP3)
        case WakeupPin_Pin3:
            wakeUpPin = PWR_CSR2_EWUP3;
            break;
      #endif

        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            break;
    }

    // disable the wake up pin
  #if defined(STM32F0XX) || defined(STM32F1XX) || defined(STM32F2XX) || \
  defined(STM32F3XX) ||defined(STM32F4XX) || defined(STM32L0XX) || defined(STM32L1XX)

    CLEAR_BIT(PWR->CSR, wakeUpPin);

  #endif

  #if defined(STM32F7XX) || defined(STM32H7XX) || defined(STM32L4XX)

    CLEAR_BIT(PWR->CSR2, wakeUpPin);

  #endif    
 
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Power::EnableWakeupPin___STATIC__VOID__nanoFrameworkHardwareStm32PowerWakeupPin( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    uint32_t wakeUpPin;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    switch((WakeupPin)stack.Arg0().NumericByRef().u4)
    {
      #if defined(PWR_CSR_EWUP)

        case WakeupPin_Pin1:
            wakeUpPin = PWR_CSR_EWUP;

            // need to config the respective EXTI line (PA0)
            EXTI->IMR |= EXTI_IMR_IM0;
            EXTI->RTSR |= EXTI_RTSR_TR0;

            break;

      #elif defined(PWR_CSR_EWUP1)

        case WakeupPin_Pin1:
            wakeUpPin = PWR_CSR_EWUP1;
            // need to config the respective EXTI line (PA0)
            EXTI->IMR |= EXTI_IMR_IM0;
            EXTI->RTSR |= EXTI_RTSR_TR0;
            break;

      #elif defined(PWR_CSR2_EWUP1)

        case WakeupPin_Pin1:
            wakeUpPin = PWR_CSR2_EWUP1;

            // need to config the respective EXTI line (PA0)
            EXTI->IMR |= EXTI_IMR_IM0;
            EXTI->RTSR |= EXTI_RTSR_TR0;

            break;

      #else
        #error "missing include for STM32 target PAL"
      #endif

      #if defined(PWR_CSR_EWUP2)

        case WakeupPin_Pin2:
            wakeUpPin = PWR_CSR_EWUP2;

            // need to config the respective EXTI line (PA2)
            EXTI->IMR |= EXTI_IMR_IM2;
            EXTI->RTSR |= EXTI_RTSR_TR2;
            break;      

      #elif defined(PWR_CSR2_EWUP2)

        case WakeupPin_Pin2:
            wakeUpPin = PWR_CSR2_EWUP2;

            // need to config the respective EXTI line (PA2)
            EXTI->IMR |= EXTI_IMR_IM2;
            EXTI->RTSR |= EXTI_RTSR_TR2;

            break;

      #endif

      #if defined(PWR_CSR_EWUP3)

        case WakeupPin_Pin3:
            wakeUpPin = PWR_CSR_EWUP3;

            // need to config the respective EXTI line (PC1)
            EXTI->IMR |= EXTI_IMR_IM1;
            EXTI->RTSR |= EXTI_RTSR_TR1;

            break;      

      #elif defined(PWR_CSR2_EWUP3)

        case WakeupPin_Pin3:
            wakeUpPin = PWR_CSR2_EWUP3;

            // need to config the respective EXTI line (PC1)
            EXTI->IMR |= EXTI_IMR_IM1;
            EXTI->RTSR |= EXTI_RTSR_TR1;

            break;

      #endif

        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            break;
    }

    // according to the STM32 documentation it's highly recommended to first disable and then enable the wake-up source

  #if defined(STM32F0XX) || defined(STM32F1XX) || defined(STM32F2XX) || \
  defined(STM32F3XX) ||defined(STM32F4XX) || defined(STM32L0XX) || defined(STM32L1XX)
    // clear PWR wake up Flag
    PWR->CR |=  PWR_CSR_WUF;

    // disable the wake up pin
    CLEAR_BIT(PWR->CSR, wakeUpPin);

    // enable the wake up pin
    SET_BIT(PWR->CSR, wakeUpPin);

  #endif

  #if defined(STM32F7XX) || defined(STM32H7XX) || defined(STM32L4XX)
    // clear PWR wake up Flag
    PWR->CR1 |=  PWR_CR1_CSBF;

    // disable the wake up pin
    CLEAR_BIT(PWR->CSR2, wakeUpPin);

    // enable the wake up pin
    SET_BIT(PWR->CSR2, wakeUpPin);

  #endif
    
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Power::NativeEnterStandbyMode___STATIC__VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    (void)stack;

    // set flags to stop debug...
    CLR_EE_DBG_SET( RebootPending );
    // ... reboot CLR so execution ends gracefully ...
    CLR_EE_REBOOT_CLR;
    // ... and set power level to OFF
    g_CLR_HW_Hardware.m_powerLevel = PowerLevel__Off;

    //////////////////////////////////////////////////////////
    // developer notes:
    // - the "goto sleep" is actually handled in ClrStartup() loop when calling CPU_SetPowerMode()
    // - the execution engine has to exit the current thread execution to be able to check the reboot flags set above and shutdown gracefully
    // - a clean approach to accomplish this is to add a Thread.Sleep(Inifinite) in the caller
    //////////////////////////////////////////////////////////

    NANOCLR_NOCLEANUP_NOLABEL();
}
