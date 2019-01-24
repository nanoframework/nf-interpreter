//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) 2016 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#include <hal.h>
#include <hal_nf_community.h>

#if (HAL_USE_STM32_ONEWIRE == TRUE)

#include <target_nf_devices_onewire_config.h>
#include <string.h>

#if !NF_ONEWIRE_STM32_UART_USE_USART1 && !NF_ONEWIRE_STM32_UART_USE_USART2 &&  \
    !NF_ONEWIRE_STM32_UART_USE_USART3 && !NF_ONEWIRE_STM32_UART_USE_USART4 &&  \
    !NF_ONEWIRE_STM32_UART_USE_USART5 && !NF_ONEWIRE_STM32_UART_USE_USART6 &&  \
    !NF_ONEWIRE_STM32_UART_USE_USART7 && !NF_ONEWIRE_STM32_UART_USE_USART8

#error "1-Wire driver activated but no USART/UART peripheral assigned. Make sure to assign it @ target_nf_devices_onewire_config.h"
#endif

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/** @brief ONEWIRED1 driver identifier.*/
ONEWIREDriver ONEWIRED1;

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/


/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

void uartSetSpeed(uint32_t speed)
{
    // stop UART, better do this before changing configuration
    uartStop(ONEWIRED1.UartDriver);
    
    // default UART config
    ONEWIRED1.UartConfig.speed = speed;

    uartStart(ONEWIRED1.UartDriver, &ONEWIRED1.UartConfig);
}


/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

static void TxEnd(UARTDriver *uartp) 
{
    (void) uartp;

    //NATIVE_INTERRUPT_START

    //chSysLockFromISR();
    chBSemSignalI(&ONEWIRED1.TxCompleted);
    // chSysUnlockFromISR();

    //NATIVE_INTERRUPT_END
}

static void RxEnd(UARTDriver *uartp)
{  
    (void) uartp;

    //NATIVE_INTERRUPT_START

    // chSysLockFromISR();
    chBSemSignalI(&ONEWIRED1.RxCompleted);
    // chSysUnlockFromISR();

    // NATIVE_INTERRUPT_END
}

static void RxErr(UARTDriver *uartp, uartflags_t e)
{
  (void) uartp;
  (void) e;
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Low level ONEWIRE driver initialization.
 *
 * @notapi
 */
void oneWire_lld_init(void) {
    
    ONEWIRED1.State  = ONEWIRE_STOP;

    // reset UART config
    memset(&ONEWIRED1.UartConfig, 0, sizeof(ONEWIRED1.UartConfig));

    // set callbacks
    ONEWIRED1.UartConfig.txend1_cb = TxEnd;
    ONEWIRED1.UartConfig.rxend_cb = RxEnd;
    ONEWIRED1.UartConfig.rxerr_cb = RxErr;

    // UART config
    // half duplex mode
    ONEWIRED1.UartConfig.cr3 = USART_CR3_HDSEL;

    #if (ONEWIRE_USE_MUTUAL_EXCLUSION == TRUE)
    osalMutexObjectInit(&ONEWIRED1.Lock);
    #endif   

}

void oneWire_lld_start() {

    // set UART according to target config
    // configure UART pins
    // set buffers
#if NF_ONEWIRE_STM32_UART_USE_USART1
    ConfigPins_UART1();
    ONEWIRED1.UartDriver = &UARTD1;
    ONEWIRED1.TxBuffer = Uart1_TxBuffer;
    ONEWIRED1.RxBuffer = Uart1_RxBuffer;
#endif
#if NF_ONEWIRE_STM32_UART_USE_USART2
    ConfigPins_UART2();
    ONEWIRED1.UartDriver = &UARTD2;
    ONEWIRED1.TxBuffer = Uart2_TxBuffer;
    ONEWIRED1.RxBuffer = Uart2_RxBuffer;
#endif
#if NF_ONEWIRE_STM32_UART_USE_USART3
    ConfigPins_UART3();
    ONEWIRED1.UartDriver = &UARTD3;
    ONEWIRED1.TxBuffer = Uart3_TxBuffer;
    ONEWIRED1.RxBuffer = Uart3_RxBuffer;
#endif
#if NF_ONEWIRE_STM32_UART_USE_USART4
    ConfigPins_UART4();
    ONEWIRED1.UartDriver = &UARTD4;
    ONEWIRED1.TxBuffer = Uart4_TxBuffer;
    ONEWIRED1.RxBuffer = Uart4_RxBuffer;
#endif
#if NF_ONEWIRE_STM32_UART_USE_USART5
    ConfigPins_UART5();
    ONEWIRED1.UartDriver = &UARTD5;
    ONEWIRED1.TxBuffer = Uart5_TxBuffer;
    ONEWIRED1.RxBuffer = Uart5_RxBuffer;
#endif
#if NF_ONEWIRE_STM32_UART_USE_USART6
    ConfigPins_UART6();
    ONEWIRED1.UartDriver = &UARTD6;
    ONEWIRED1.TxBuffer = Uart6_TxBuffer;
    ONEWIRED1.RxBuffer = Uart6_RxBuffer;
#endif
#if NF_ONEWIRE_STM32_UART_USE_USART7
    ConfigPins_UART7();
    ONEWIRED1.UartDriver = &UARTD7;
    ONEWIRED1.TxBuffer = Uart7_TxBuffer;
    ONEWIRED1.RxBuffer = Uart7_RxBuffer;
#endif
#if NF_ONEWIRE_STM32_UART_USE_USART8
    ConfigPins_UART8();
    ONEWIRED1.UartDriver = &UARTD8;
    ONEWIRED1.TxBuffer = Uart8_TxBuffer;
    ONEWIRED1.RxBuffer = Uart8_RxBuffer;
#endif    

    uartSetSpeed(9600);

    // driver is ready
    ONEWIRED1.State = ONEWIRE_READY;
}

void oneWire_lld_stop() {
     
    // stop UART
    uartStop(ONEWIRED1.UartDriver);

    // driver is stopped
    ONEWIRED1.State = ONEWIRE_STOP;
}

uint8_t oneWire_lld_TouchReset(void) 
{
    ONEWIRED1.TxBuffer[0] = 0xf0;
    ONEWIRED1.RxBuffer[0] = 0;

    // flush DMA buffer to ensure cache coherency
    // (only required for Cortex-M7)
    cacheBufferFlush(&ONEWIRED1.TxBuffer[0], 1);

    // set UART baud rate to 9600bps (required to send the RESET condition to the 1-Wire bus)
    uartSetSpeed(9600);
        
    chBSemReset(&ONEWIRED1.TxCompleted, TRUE);
    chBSemReset(&ONEWIRED1.RxCompleted, TRUE);
    uartStartReceive (ONEWIRED1.UartDriver, 1 , &ONEWIRED1.RxBuffer[0]);	
    uartStartSend (ONEWIRED1.UartDriver, 1, &ONEWIRED1.TxBuffer[0]);
    chThdSleepMilliseconds(10);
    chBSemWait (&ONEWIRED1.TxCompleted);
    chBSemWait (&ONEWIRED1.RxCompleted);
  
    // invalidate cache over read buffer to ensure that content from DMA is read
    // (only required for Cortex-M7)
    cacheBufferInvalidate(&ONEWIRED1.RxBuffer[0], 1);

    // set UART baud rate to 115200bps (normal comm is performed at this baud rate)
    uartSetSpeed(115200);

    // check for presence pulse
    return (ONEWIRED1.TxBuffer[0] != ONEWIRED1.RxBuffer[0]);
}

bool oneWire_lld_TouchBit(bool sendbit)
{
    // need to send 1-Wire write 1 or 0 according to sendbit
    ONEWIRED1.TxBuffer[0] = sendbit ? IWIRE_WR1 : IWIRE_WR0;

    // flush DMA buffer to ensure cache coherency
    // (only required for Cortex-M7)
    cacheBufferFlush(&ONEWIRED1.TxBuffer[0], 1);

    chBSemReset(&ONEWIRED1.RxCompleted, TRUE);
    chBSemReset(&ONEWIRED1.TxCompleted, TRUE);
    uartStartReceive (ONEWIRED1.UartDriver, 1 , &ONEWIRED1.RxBuffer[0]);
    uartStartSend(ONEWIRED1.UartDriver, 1, &ONEWIRED1.TxBuffer[0]);
    chThdSleepMilliseconds(0.1);
    chBSemWait (&ONEWIRED1.TxCompleted);
    chBSemWait (&ONEWIRED1.RxCompleted);
  
    // invalidate cache over read buffer to ensure that content from DMA is read
    // (only required for Cortex-M7)
    cacheBufferInvalidate(&ONEWIRED1.RxBuffer[0], 1);

    // interpret 1-Wire reply
    return (ONEWIRED1.RxBuffer[0] == 0xff);
}

uint8_t oneWire_lld_TouchByte(uint8_t sendbyte)
{
    uint8_t send_mask = 0x01, result = 0;
    uint8_t i = 0;

    // send byte     
    while (send_mask) 
    {
        if (sendbyte & send_mask) 
        {
            // if transmit bit is 1
            ONEWIRED1.TxBuffer[i] = IWIRE_WR1;
        }  
        else 
        {
            // if transmit bit is 0
            ONEWIRED1.TxBuffer[i] = IWIRE_WR0;
        }

        i++;

        // rotates the position mask transmit bit
        send_mask<<=1;
    };


    // flush DMA buffer to ensure cache coherency
    // (only required for Cortex-M7)
    cacheBufferFlush(&ONEWIRED1.TxBuffer[0], 8);

    chBSemReset(&ONEWIRED1.RxCompleted, TRUE);
    chBSemReset(&ONEWIRED1.TxCompleted, TRUE);
    uartStartReceive (ONEWIRED1.UartDriver, 8 , &ONEWIRED1.RxBuffer[0]);
    uartStartSend (ONEWIRED1.UartDriver, 8, &ONEWIRED1.TxBuffer[0]);
    chThdSleepMilliseconds(0.1);
    chBSemWait (&ONEWIRED1.TxCompleted);
    chBSemWait (&ONEWIRED1.RxCompleted);

    // invalidate cache over read buffer to ensure that content from DMA is read
    // (only required for Cortex-M7)
    cacheBufferInvalidate(&ONEWIRED1.RxBuffer[0], 8);

    // reset send mask to interpret the reply
    send_mask = 0x01;

    for (uint32_t i = 0;  i < 8; i++) 
    {
        if (ONEWIRED1.RxBuffer[i] == 0xff) 
        {
            result |= send_mask;
        }

        send_mask<<=1;
    }

    return result;
}

#if (ONEWIRE_USE_MUTUAL_EXCLUSION == TRUE)

void oneWire_lld_aquire()
{
    osalMutexLock(&ONEWIRED1.Lock);
}

void oneWire_lld_release()
{
    osalMutexUnlock(&ONEWIRED1.Lock);
}

#endif

#endif /* HAL_USE_STM32_ONEWIRE */
