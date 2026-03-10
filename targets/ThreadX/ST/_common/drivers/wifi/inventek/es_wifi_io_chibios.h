//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2021 STMicroelectronics. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef WIFI_IO_H
#define WIFI_IO_H

#include <hal.h>
#include <targetHAL.h>

// event flags for Wi-Fi
#define WIFI_RX_EVENT_FLAG          0x80000000
#define WIFI_TX_EVENT_FLAG          0x40000000
#define WIFI_CMDDATA_RDY_EVENT_FLAG 0x20000000

#ifdef __cplusplus
extern "C"
{
#endif

#define WIFI_RESET_MODULE()                                                                                            \
    do                                                                                                                 \
    {                                                                                                                  \
        palClearLine(LINE_ISM43362_RST);                                                                               \
        HAL_Delay(20);                                                                                                 \
        palSetLine(LINE_ISM43362_RST);                                                                                 \
        HAL_Delay(500);                                                                                                \
    } while (0);

#define WIFI_IS_CMDDATA_READY() (palReadLine(LINE_ISM43362_CMDTRDY) == PAL_HIGH)

    void SPI_WIFI_MspInit(SPI_HandleTypeDef *hspi);
    int8_t SPI_WIFI_DeInit(void);
    int8_t SPI_WIFI_Init(uint16_t mode);
    int8_t SPI_WIFI_ResetModule(void);
    int16_t SPI_WIFI_ReceiveData(uint8_t *data, uint16_t len, uint32_t timeout);
    int16_t SPI_WIFI_SendData(uint8_t *data, uint16_t len, uint32_t timeout);
    void SPI_WIFI_Delay(uint32_t Delay);
    void SPI_WIFI_Callback(SPIDriver *spip);

#ifdef __cplusplus
}
#endif

#endif // WIFI_IO_H
