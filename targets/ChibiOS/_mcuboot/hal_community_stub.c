//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Stubs for ChibiOS HAL functions not needed by MCUboot.
//
// halCommunityInit(): MCUboot uses only the nf-overlay flash driver and does not
//   include ChibiOS-Contrib. hal.c calls halCommunityInit() when HAL_USE_COMMUNITY
//   is TRUE; this no-op stub satisfies the linker without pulling in Contrib.
//
// irqInit() / irqDeinit(): Normally provided by stm32_isr.c, which configures EXTI
//   interrupt priorities and ISR handlers. MCUboot does not use GPIO external
//   interrupts, so an empty stub is sufficient. Including stm32_isr.c would require
//   every TIM and USART IRQ priority to be defined in mcuconf.h — unnecessary overhead
//   for a minimal bootloader.

void halCommunityInit(void)
{
}

void irqInit(void)
{
}

void irqDeinit(void)
{
}
