/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
//*****************************************************************************
/*!
\addtogroup EasyLink Easylink

@{
*/
//*****************************************************************************
#ifndef EASYLINK_CONFIG_H_
#define EASYLINK_CONFIG_H_

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//! \brief Defines the address that will accompany each packet sent from this
//! device when EASYLINK_USE_DEFAULT_ADDR is true. It can also be thought of as 
//! this device's own address. Organized in little endian format (e.g.an address
//! of 0xABCD should be defined as {0xCD,0xAB})
#define EASYLINK_DEFAULT_ADDR {0xAA}

//! \brief When false, the Easylink stack expects the application to provide an
//! accompanying address for each EasyLink_TxPacket passed to the
//! EasyLink_transmit(), EasyLink_transmitAsync(), and
//! EasyLink_transmitCcaAsync() APIs. Otherwise, these APIs will use the address
//! defined in EASYLINK_DEFAULT_ADDR
#define EASYLINK_USE_DEFAULT_ADDR false

//! \brief Defines the largest Tx/Rx payload that the interface can support
#define EASYLINK_MAX_DATA_LENGTH            128

//! \brief Defines the default address size for rx and tx operations
#define EASYLINK_ADDR_SIZE                  1

//! \brief Enables of disables address filtering by default
#define EASYLINK_ENABLE_ADDR_FILTERING      true

//! \brief The table for setting the Rx Address Filters
#define EASYLINK_NUM_ADDR_FILTER            1

//! \brief The table for setting the Rx Address Filters
#define EASYLINK_ADDR_FILTER_TABLE          {0xAA}

//! \brief Defines the time for the radio to return to idle after inactivity
#define EASYLINK_IDLE_TIMEOUT               EasyLink_ms_To_RadioTime(1)

//! \brief Enables or disables the application to use multiple RF clients
#define EASYLINK_ENABLE_MULTI_CLIENT        false

//! \brief Defines the relative time from async RX start to timeout. A value of 
//! 0 means no timeout
#define EASYLINK_ASYNC_RX_TIMEOUT           EasyLink_ms_To_RadioTime(0)

//! \brief Minimum CCA back-off window in units of
//! EASYLINK_CCA_BACKOFF_TIMEUNITS, as a power of 2
#define EASYLINK_MIN_CCA_BACKOFF_WINDOW     5

//! \brief  Maximum CCA back-off window in units of
//! EASYLINK_CCA_BACKOFF_TIMEUNITS, as a power of 2
#define EASYLINK_MAX_CCA_BACKOFF_WINDOW     8

//! \brief The back-off time units in microseconds
#define EASYLINK_CCA_BACKOFF_TIMEUNITS      250

//! \brief RSSI threshold for Clear Channel Assessment (CCA)
#define EASYLINK_CS_RSSI_THRESHOLD_DBM      -80

//! \brief Time for which the channel RSSI must remain below the specified
//! threshold for the channel to be considered idle
#define EASYLINK_CHANNEL_IDLE_TIME_US       5000

//! \brief EasyLink default parameter configuration
#define EASYLINK_PARAM_CONFIG  \
    {.ui32ModType            = EasyLink_Phy_Custom,      \
     .pClientEventCb         = NULL,                     \
     .nClientEventMask       = 0,                        \
     .pGrnFxn                = (EasyLink_GetRandomNumber)rand\
    }

#ifdef __cplusplus
}
#endif


#endif //EASYLINK_CONFIG_H_

//*****************************************************************************
//
//! Close the Doxygen group.
//! @}
//
//*****************************************************************************
