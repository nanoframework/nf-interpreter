//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <hal.h>

#include <tx_api.h>


#define TX_TICKS_PER_MILLISEC(milliSecs) ((milliSecs * TX_TIMER_TICKS_PER_SECOND) / 1000)

// threads definitions and configurations
// blink thread
#define BLINK_THREAD_STACK_SIZE 1024
#define BLINK_THREAD_PRIORITY   5
TX_THREAD blink_led_thread;
uint32_t blink_led_thread_stack[BLINK_THREAD_STACK_SIZE / sizeof(uint32_t)];

void blink_led_thread_entry(ULONG parameter)
{
    (void)parameter;

    char *message = "--- ... ---      ";
    int len = strlen(message);

    while (1)
    {
        for (int i = 0; i < len; i++)
        {
            char ch = message[i];
            if (ch == '.')
            {
                // BSP_LED_On(LED2);
                palSetPad(GPIOB, GPIOB_LD2);
                tx_thread_sleep(TX_TICKS_PER_MILLISEC(500));
                // BSP_LED_Off(LED2);
                palClearPad(GPIOB, GPIOB_LD2);
                tx_thread_sleep(TX_TICKS_PER_MILLISEC(500));
            }
            else if (ch == '-')
            {
                // BSP_LED_On(LED2);
                palSetPad(GPIOB, GPIOB_LD2);
                tx_thread_sleep(TX_TICKS_PER_MILLISEC(1000));
                // BSP_LED_Off(LED2);
                palClearPad(GPIOB, GPIOB_LD2);
                tx_thread_sleep(TX_TICKS_PER_MILLISEC(1000));
            }
            else if (ch == ' ')
            {
                // BSP_LED_Off(LED2);
                palClearPad(GPIOB, GPIOB_LD2);
                tx_thread_sleep(TX_TICKS_PER_MILLISEC(500));
            }
        }
    }
}

void blink_led_thread_define(ULONG parameter)
{
    (void)parameter;
    uint16_t status;
    // Create blink thread
    status = tx_thread_create(
        &blink_led_thread,
        "Blink Thread",
        blink_led_thread_entry,
        0,
        blink_led_thread_stack,
        BLINK_THREAD_STACK_SIZE,
        BLINK_THREAD_PRIORITY,
        BLINK_THREAD_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);

    if (status != TX_SUCCESS)
    {
        while (1)
        {
        }
    }
}
