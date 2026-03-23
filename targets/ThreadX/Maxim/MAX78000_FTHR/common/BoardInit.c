//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2017 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoCLR_Headers.h>

#include <platformHAL.h>
#include <nanoRingBuffer.h>
#include <platform_UARTDriver.h>

#include <tx_api.h>

#include <mxc_device.h>
#include <mxc_sys.h>
#include <mxc_delay.h>
#include <simo_regs.h>
#include <gpio.h>
#include <led.h>
#include <dma.h>
#include <rtc.h>
#include <pb.h>

const mxc_gpio_cfg_t pb_pin[] = {
    {MXC_GPIO0, MXC_GPIO_PIN_2, MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_PULL_UP, MXC_GPIO_VSSEL_VDDIOH},
    {MXC_GPIO1, MXC_GPIO_PIN_7, MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_PULL_UP, MXC_GPIO_VSSEL_VDDIOH},
};
const unsigned int num_pbs = (sizeof(pb_pin) / sizeof(mxc_gpio_cfg_t));

const mxc_gpio_cfg_t led_pin[] = {
    {MXC_GPIO2, MXC_GPIO_PIN_0, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIOH},
    {MXC_GPIO2, MXC_GPIO_PIN_1, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIOH},
    {MXC_GPIO2, MXC_GPIO_PIN_2, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIOH},
};
const unsigned int num_leds = (sizeof(led_pin) / sizeof(mxc_gpio_cfg_t));

NanoRingBuffer WPRingBuffer;
uint8_t rxBuffer[WIRE_PROTOCOL_UART_BUFFER_SIZE];

void RTC_IRQHandler(void)
{
}

void UART0_IRQHandler(void)
{
    _tx_thread_context_save();

    NanoUART_AsyncHandler(MXC_UART_GET_UART(WIRE_PROTOCOL_UART));

    _tx_thread_context_restore();
}

void StartRTC()
{
    // enable RTC IRQ
    NVIC_EnableIRQ(RTC_IRQn);

    // Use RTC as Unix time(years since 1970)
    // TODO
    // check if RTC does already have a valid value
    // MXC_RTC_Init(0, 0);

    // start it
    MXC_RTC_Start();
}

void StartWireProtocolUART()
{
    int32_t error;
    mxc_uart_regs_t *uart = MXC_UART_GET_UART(WIRE_PROTOCOL_UART);

    // Initialize the UART
    if ((error = MXC_UART_Init(uart, 115200, MXC_UART_IBRO_CLK)) != E_NO_ERROR)
    {
        while (1)
        {
        }
    }

    // Initilize the RX ring buffer
    NanoRingBuffer_Initialize(&WPRingBuffer, rxBuffer, WIRE_PROTOCOL_UART_BUFFER_SIZE);

    NVIC_ClearPendingIRQ(MXC_UART_GET_IRQ(WIRE_PROTOCOL_UART));
    NVIC_EnableIRQ(MXC_UART_GET_IRQ(WIRE_PROTOCOL_UART));

    // set threshold
    MXC_UART_SetRXThreshold(uart, 1);

    NanoUART_EnableInt(uart);
}

void BoardInit()
{
    int32_t error;

    // Enable GPIO
    MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_GPIO0);
    MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_GPIO1);
    MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_GPIO2);

    // UART
    StartWireProtocolUART();

    // RTC
    StartRTC();

    if ((error = LED_Init()) != E_NO_ERROR)
    {
        MXC_ASSERT_FAIL();
        return;
    }

    if ((error = PB_Init()) != E_NO_ERROR)
    {
        MXC_ASSERT_FAIL();
        return;
    }

    MXC_SIMO->vrego_c = 0x43; // Set CNN voltage

    // Wait for PMIC 1.8V to become available, about 180ms after power up.
    MXC_Delay(200000);
}
