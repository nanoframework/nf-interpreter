//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) 2001-2004 Swedish Institute of Computer Science. All rights reserved.
// Portions Copyright (c) 2013-2016, Freescale Semiconductor, Inc. All rights reserved.
// Portions Copyright 2016-2019 NXP All rights reserved.
// See LICENSE file in the project root for full license information.
//

/*
 * from the original source code file
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * Copyright (c) 2013-2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "netif/etharp.h"
#include "netif/ppp/pppoe.h"
#include "lwip/igmp.h"
#include "lwip/mld6.h"

#include "FreeRTOS.h"
#include "event_groups.h"

#include "ethernetif.h"

#include "fsl_enet.h"
#include "fsl_phy.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

#if defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL
    #if defined(FSL_FEATURE_L2CACHE_LINESIZE_BYTE) \
        && ((!defined(FSL_SDK_DISBLE_L2CACHE_PRESENT)) || (FSL_SDK_DISBLE_L2CACHE_PRESENT == 0))
        #if defined(FSL_FEATURE_L1DCACHE_LINESIZE_BYTE)
            #define FSL_CACHE_LINESIZE_MAX MAX(FSL_FEATURE_L1DCACHE_LINESIZE_BYTE, FSL_FEATURE_L2CACHE_LINESIZE_BYTE)
            #define FSL_ENET_BUFF_ALIGNMENT MAX(ENET_BUFF_ALIGNMENT, FSL_CACHE_LINESIZE_MAX)
        #else
            #define FSL_ENET_BUFF_ALIGNMENT MAX(ENET_BUFF_ALIGNMENT, FSL_FEATURE_L2CACHE_LINESIZE_BYTE)
        #endif
    #elif defined(FSL_FEATURE_L1DCACHE_LINESIZE_BYTE)
        #define FSL_ENET_BUFF_ALIGNMENT MAX(ENET_BUFF_ALIGNMENT, FSL_FEATURE_L1DCACHE_LINESIZE_BYTE)
    #else
        #define FSL_ENET_BUFF_ALIGNMENT ENET_BUFF_ALIGNMENT
    #endif
#else
    #define FSL_ENET_BUFF_ALIGNMENT ENET_BUFF_ALIGNMENT
#endif

#if defined(FSL_FEATURE_SOC_LPC_ENET_COUNT) && (FSL_FEATURE_SOC_LPC_ENET_COUNT > 0)
#define kENET_RxEvent kENET_RxIntEvent
#define kENET_TxEvent kENET_TxIntEvent
#endif

#define ENET_RING_NUM 1U

typedef uint8_t rx_buffer_t[SDK_SIZEALIGN(ENET_RXBUFF_SIZE, FSL_ENET_BUFF_ALIGNMENT)];
typedef uint8_t tx_buffer_t[SDK_SIZEALIGN(ENET_TXBUFF_SIZE, FSL_ENET_BUFF_ALIGNMENT)];

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 */
struct ethernetif
{
    ENET_Type *base;
    enet_handle_t handle;

    EventGroupHandle_t enetTransmitAccessEvent;
    EventBits_t txFlag;

    enet_rx_bd_struct_t *RxBuffDescrip;
    enet_tx_bd_struct_t *TxBuffDescrip;
    rx_buffer_t *RxDataBuff;
    tx_buffer_t *TxDataBuff;
    uint8_t txIdx;

};


/*******************************************************************************
 * Code
 ******************************************************************************/
#if defined(FSL_FEATURE_SOC_ENET_COUNT) && (FSL_FEATURE_SOC_ENET_COUNT > 0)
#if FSL_FEATURE_ENET_QUEUE > 1
static void ethernet_callback(ENET_Type *base, enet_handle_t *handle, uint32_t ringId, enet_event_t event, void *param)
#else
static void ethernet_callback(ENET_Type *base, enet_handle_t *handle, enet_event_t event, void *param)
#endif /* FSL_FEATURE_ENET_QUEUE */
#elif defined(FSL_FEATURE_SOC_LPC_ENET_COUNT) && (FSL_FEATURE_SOC_LPC_ENET_COUNT > 0)
static void ethernet_callback(ENET_Type *base, enet_handle_t *handle, enet_event_t event, uint8_t channel, void *param)
#endif /* FSL_FEATURE_SOC_*_ENET_COUNT */
{
    LWIP_UNUSED_ARG(base);
    LWIP_UNUSED_ARG(handle);
    struct netif *netif = (struct netif *)param;
    struct ethernetif *ethernetif = netif->state;
    BaseType_t xResult;
    

    switch (event)
    {
        case kENET_RxEvent:
            ethernetif_input(netif);
            break;
        case kENET_TxEvent:
        {
            portBASE_TYPE taskToWake = pdFALSE;

#ifdef __CA7_REV
            if (SystemGetIRQNestingLevel())
#else
            if (__get_IPSR())
#endif 
            {
                xResult = xEventGroupSetBitsFromISR(ethernetif->enetTransmitAccessEvent, ethernetif->txFlag, &taskToWake);
                if ((pdPASS == xResult) && (pdTRUE == taskToWake))
                {
                    portYIELD_FROM_ISR(taskToWake);
                }
            }
            else
            {
                xEventGroupSetBits(ethernetif->enetTransmitAccessEvent, ethernetif->txFlag);
            }
        }
        break;
        default:
            break;
    }
}


#if LWIP_IPV4 && LWIP_IGMP
static err_t ethernetif_igmp_mac_filter(struct netif *netif, const ip4_addr_t *group, enum netif_mac_filter_action action)
#if defined(FSL_FEATURE_SOC_LPC_ENET_COUNT) && (FSL_FEATURE_SOC_LPC_ENET_COUNT > 0)
{
    struct ethernetif *ethernetif = netif->state;
    err_t result;

    switch (action)
    {
        case IGMP_ADD_MAC_FILTER:
            /* LPC ENET does not accept multicast selectively,
             * so all multicast has to be passed through. */
            ENET_AcceptAllMulticast(ethernetif->base);
            result = ERR_OK;
            break;
        case IGMP_DEL_MAC_FILTER:
            /*
             * Moves the ENET device from a multicast group.
             * Since we don't keep track of which multicast groups
             * are still to enabled, the call is commented out.
             */
            /* ENET_RejectAllMulticast(ethernetif->base); */
            result = ERR_OK;
            break;
        default:
            result = ERR_IF;
            break;
    }

    return result;
}
#else
{
    struct ethernetif *ethernetif = netif->state;
    uint8_t multicastMacAddr[6];
    err_t result;

    multicastMacAddr[0] = 0x01U;
    multicastMacAddr[1] = 0x00U;
    multicastMacAddr[2] = 0x5EU;
    multicastMacAddr[3] = (group->addr >> 8) & 0x7FU;
    multicastMacAddr[4] = (group->addr >> 16) & 0xFFU;
    multicastMacAddr[5] = (group->addr >> 24) & 0xFFU;

    switch (action)
    {
        case IGMP_ADD_MAC_FILTER:
            /* Adds the ENET device to a multicast group.*/
            ENET_AddMulticastGroup(ethernetif->base, multicastMacAddr);
            result = ERR_OK;
            break;
        case IGMP_DEL_MAC_FILTER:
            /*
             * Moves the ENET device from a multicast group.
             * Since the ENET_LeaveMulticastGroup() could filter out also other
             * group addresses having the same hash, the call is commented out.
             */
            /* ENET_LeaveMulticastGroup(ethernetif->base, multicastMacAddr); */
            result = ERR_OK;
            break;
        default:
            result = ERR_IF;
            break;
    }

    return result;
}
#endif
#endif

#if LWIP_IPV6 && LWIP_IPV6_MLD
static err_t ethernetif_mld_mac_filter(struct netif *netif, const ip6_addr_t *group, enum netif_mac_filter_action action)
#if defined(FSL_FEATURE_SOC_LPC_ENET_COUNT) && (FSL_FEATURE_SOC_LPC_ENET_COUNT > 0)
{
    struct ethernetif *ethernetif = netif->state;
    err_t result;

    switch (action)
    {
        case NETIF_ADD_MAC_FILTER:
            /* LPC ENET does not accept multicast selectively,
             * so all multicast has to be passed through. */
            ENET_AcceptAllMulticast(ethernetif->base);
            result = ERR_OK;
            break;
        case NETIF_DEL_MAC_FILTER:
            /*
             * Moves the ENET device from a multicast group.
             * Since we don't keep track of which multicast groups
             * are still to enabled, the call is commented out.
             */
            /* ENET_RejectAllMulticast(ethernetif->base); */
            result = ERR_OK;
            break;
        default:
            result = ERR_IF;
            break;
    }

    return result;
}
#else
{
    struct ethernetif *ethernetif = netif->state;
    uint8_t multicastMacAddr[6];
    err_t result;

    multicastMacAddr[0] = 0x33U;
    multicastMacAddr[1] = 0x33U;
    multicastMacAddr[2] = (group->addr[3]) & 0xFFU;
    multicastMacAddr[3] = (group->addr[3] >> 8) & 0xFFU;
    multicastMacAddr[4] = (group->addr[3] >> 16) & 0xFFU;
    multicastMacAddr[5] = (group->addr[3] >> 24) & 0xFFU;

    switch (action)
    {
        case NETIF_ADD_MAC_FILTER:
            /* Adds the ENET device to a multicast group.*/
            ENET_AddMulticastGroup(ethernetif->base, multicastMacAddr);
            result = ERR_OK;
            break;
        case NETIF_DEL_MAC_FILTER:
            /*
             * Moves the ENET device from a multicast group.
             * Since the ENET_LeaveMulticastGroup() could filter out also other
             * group addresses having the same hash, the call is commented out.
             */
            /* ENET_LeaveMulticastGroup(ethernetif->base, multicastMacAddr); */
            result = ERR_OK;
            break;
        default:
            result = ERR_IF;
            break;
    }

    return result;
}
#endif
#endif

#if defined(FSL_FEATURE_SOC_LPC_ENET_COUNT) && (FSL_FEATURE_SOC_LPC_ENET_COUNT > 0)
static inline enet_rx_bd_struct_t *get_rx_desc(struct ethernetif *ethernetif, uint32_t index)
{
    return &(ethernetif->RxBuffDescrip[index]);
}

static inline enet_tx_bd_struct_t *get_tx_desc(struct ethernetif *ethernetif, uint32_t index)
{
    return &(ethernetif->TxBuffDescrip[index]);
}
#endif

static void ethernetif_phy_init(struct ethernetif *ethernetif,
                                const ethernetif_config_t *ethernetifConfig,
                                enet_config_t *config)
{
    uint32_t sysClock;
    status_t status;
    bool link = false;
    uint32_t count = 0;
    phy_speed_t speed;
    phy_duplex_t duplex;

    sysClock = CLOCK_GetFreq(ethernetifConfig->clockName);

    //Initializing PHY...

    while ((count < ENET_ATONEGOTIATION_TIMEOUT) && (!link))
    {
        status = PHY_Init(ethernetif->base, ethernetifConfig->phyAddress, sysClock);

        if (kStatus_Success == status)
        {
            PHY_GetLinkStatus(ethernetif->base, ethernetifConfig->phyAddress, &link);
        }
        else if (kStatus_PHY_AutoNegotiateFail == status)
        {
            //PHY Auto-negotiation failed. Please check the ENET cable connection and link partner setting.
        }
        else
        {
            LWIP_ASSERT("\r\nCannot initialize PHY.\r\n", 0);
        }

        count++;
    }

    if (link)
    {
        /* Get the actual PHY link speed. */
        PHY_GetLinkSpeedDuplex(ethernetif->base, ethernetifConfig->phyAddress, &speed, &duplex);
        /* Change the MII speed and duplex for actual link status. */
        config->miiSpeed = (enet_mii_speed_t)speed;
        config->miiDuplex = (enet_mii_duplex_t)duplex;
    }
#if 0 /* Disable assert. If initial auto-negation is timeout, \ \
         the ENET is set to default (100Mbs and full-duplex). */
    else
    {
        LWIP_ASSERT("\r\nGiving up PHY initialization. Please check the ENET cable connection and link partner setting and reset the board.\r\n", 0);
    }
#endif
}

/**
 * Initializes ENET driver.
 */
static void enet_init(struct netif *netif, struct ethernetif *ethernetif,
                      const ethernetif_config_t *ethernetifConfig)
{
    enet_config_t config;
    uint32_t sysClock;
    enet_buffer_config_t buffCfg[ENET_RING_NUM];

    /* prepare the buffer configuration. */
    buffCfg[0].rxBdNumber = ENET_RXBD_NUM;                      /* Receive buffer descriptor number. */
    buffCfg[0].txBdNumber = ENET_TXBD_NUM;                      /* Transmit buffer descriptor number. */
    buffCfg[0].rxBuffSizeAlign = sizeof(rx_buffer_t);           /* Aligned receive data buffer size. */
    buffCfg[0].txBuffSizeAlign = sizeof(tx_buffer_t);           /* Aligned transmit data buffer size. */
    buffCfg[0].rxBdStartAddrAlign = &(ethernetif->RxBuffDescrip[0]); /* Aligned receive buffer descriptor start address. */
    buffCfg[0].txBdStartAddrAlign = &(ethernetif->TxBuffDescrip[0]); /* Aligned transmit buffer descriptor start address. */
    buffCfg[0].rxBufferAlign = &(ethernetif->RxDataBuff[0][0]); /* Receive data buffer start address. */
    buffCfg[0].txBufferAlign = &(ethernetif->TxDataBuff[0][0]); /* Transmit data buffer start address. */

    sysClock = CLOCK_GetFreq(ethernetifConfig->clockName);

    ENET_GetDefaultConfig(&config);
    config.ringNum = ENET_RING_NUM;

    ethernetif_phy_init(ethernetif, ethernetifConfig, &config);

    uint32_t instance;
    static ENET_Type *const enetBases[] = ENET_BASE_PTRS;
    static const IRQn_Type enetTxIrqId[] = ENET_Transmit_IRQS;
    /*! @brief Pointers to enet receive IRQ number for each instance. */
    static const IRQn_Type enetRxIrqId[] = ENET_Receive_IRQS;
#if defined(ENET_ENHANCEDBUFFERDESCRIPTOR_MODE) && ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
    /*! @brief Pointers to enet timestamp IRQ number for each instance. */
    static const IRQn_Type enetTsIrqId[] = ENET_1588_Timer_IRQS;
#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */

    /* Create the Event for transmit busy release trigger. */
    ethernetif->enetTransmitAccessEvent = xEventGroupCreate();
    ethernetif->txFlag = 0x1;

    config.interrupt |= kENET_RxFrameInterrupt | kENET_TxFrameInterrupt | kENET_TxBufferInterrupt;

    for (instance = 0; instance < ARRAY_SIZE(enetBases); instance++)
    {
        if (enetBases[instance] == ethernetif->base)
        {
#ifdef __CA7_REV
            GIC_SetPriority(enetRxIrqId[instance], ENET_PRIORITY);
            GIC_SetPriority(enetTxIrqId[instance], ENET_PRIORITY);
#if defined(ENET_ENHANCEDBUFFERDESCRIPTOR_MODE) && ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
            GIC_SetPriority(enetTsIrqId[instance], ENET_1588_PRIORITY);
#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */
#else
            NVIC_SetPriority(enetRxIrqId[instance], ENET_PRIORITY);
            NVIC_SetPriority(enetTxIrqId[instance], ENET_PRIORITY);
#if defined(ENET_ENHANCEDBUFFERDESCRIPTOR_MODE) && ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
            NVIC_SetPriority(enetTsIrqId[instance], ENET_1588_PRIORITY);
#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */
#endif /* __CA7_REV */
            break;
        }
    }

    LWIP_ASSERT("Input Ethernet base error!", (instance != ARRAY_SIZE(enetBases)));


    /* Initialize the ENET module.*/
    ENET_Init(ethernetif->base, &ethernetif->handle, &config, &buffCfg[0], netif->hwaddr, sysClock);

    ENET_SetCallback(&ethernetif->handle, ethernet_callback, netif);

    ENET_ActiveRead(ethernetif->base);
}


/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void low_level_init(struct netif *netif, const uint8_t enetIdx,
                           const ethernetif_config_t *ethernetifConfig)
{
    LWIP_UNUSED_ARG(enetIdx);
    struct ethernetif *ethernetif = netif->state;

    /* set MAC hardware address length */
    netif->hwaddr_len = ETH_HWADDR_LEN;

    /* set MAC hardware address */
    memcpy(netif->hwaddr, ethernetifConfig->macAddress, NETIF_MAX_HWADDR_LEN);

    /* maximum transfer unit */
    netif->mtu = 1500; /* TODO: define a config */

    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

    /* ENET driver initialization.*/
    enet_init(netif, ethernetif, ethernetifConfig);

#if LWIP_IPV6 && LWIP_IPV6_MLD
    /*
     * For hardware/netifs that implement MAC filtering.
     * All-nodes link-local is handled by default, so we must let the hardware know
     * to allow multicast packets in.
     * Should set mld_mac_filter previously. */
    if (netif->mld_mac_filter != NULL)
    {
        ip6_addr_t ip6_allnodes_ll;
        ip6_addr_set_allnodes_linklocal(&ip6_allnodes_ll);
        netif->mld_mac_filter(netif, &ip6_allnodes_ll, NETIF_ADD_MAC_FILTER);
    }
#endif /* LWIP_IPV6 && LWIP_IPV6_MLD */
}

/**
 * Returns next buffer for TX.
 * Can wait if no buffer available.
 */
static unsigned char *enet_get_tx_buffer(struct ethernetif *ethernetif)
{
    LWIP_UNUSED_ARG(ethernetif);
    static unsigned char ucBuffer[ENET_FRAME_MAX_FRAMELEN];
    return ucBuffer;
}

/**
 * Sends frame via ENET.
 */
static err_t enet_send_frame(struct ethernetif *ethernetif, unsigned char *data, const uint32_t length)
{
    status_t result;

    do
    {
        result = ENET_SendFrame(ethernetif->base, &ethernetif->handle, data, length);

        if (result == kStatus_ENET_TxFrameBusy)
        {
            xEventGroupWaitBits(ethernetif->enetTransmitAccessEvent, ethernetif->txFlag, pdTRUE, (BaseType_t) false,
                                portMAX_DELAY);
        }

    } while (result == kStatus_ENET_TxFrameBusy);
    return ERR_OK;
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become available since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    err_t result;
    struct ethernetif *ethernetif = netif->state;
    struct pbuf *q;
    unsigned char *pucBuffer;
    unsigned char *pucChar;

    LWIP_ASSERT("Output packet buffer empty", p);

    pucBuffer = enet_get_tx_buffer(ethernetif);
    if (pucBuffer == NULL)
    {
        return ERR_BUF;
    }

/* Initiate transfer. */

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

    if (p->len == p->tot_len)
    {
        /* No pbuf chain, don't have to copy -> faster. */
        pucBuffer = (unsigned char *)p->payload;
    }
    else
    {
        /* pbuf chain, copy into contiguous ucBuffer. */
        if (p->tot_len >= ENET_FRAME_MAX_FRAMELEN)
        {
            return ERR_BUF;
        }
        else
        {
            pucChar = pucBuffer;

            for (q = p; q != NULL; q = q->next)
            {
                /* Send the data from the pbuf to the interface, one pbuf at a
                time. The size of the data in each pbuf is kept in the ->len
                variable. */
                /* send data from(q->payload, q->len); */
                memcpy(pucChar, q->payload, q->len);
                pucChar += q->len;
            }
        }
    }

    /* Send frame. */
    result = enet_send_frame(ethernetif, pucBuffer, p->tot_len);

    MIB2_STATS_NETIF_ADD(netif, ifoutoctets, p->tot_len);
    if (((u8_t *)p->payload)[0] & 1)
    {
        /* broadcast or multicast packet*/
        MIB2_STATS_NETIF_INC(netif, ifoutnucastpkts);
    }
    else
    {
        /* unicast packet */
        MIB2_STATS_NETIF_INC(netif, ifoutucastpkts);
    }
/* increase ifoutdiscards or ifouterrors on error */

#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

    LINK_STATS_INC(link.xmit);

    return result;
}

/**
 * Gets the length of received frame (if any).
 */
static status_t enet_get_rx_frame_size(struct ethernetif *ethernetif, uint32_t *length)
{
    return ENET_GetRxFrameSize(&ethernetif->handle, length);
}

/**
 * Reads frame from ENET.
 */
static void enet_read_frame(struct ethernetif *ethernetif, uint8_t *data, uint32_t length)
{
        ENET_ReadFrame(ethernetif->base, &ethernetif->handle, data, length);
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *low_level_input(struct netif *netif)
{
    struct ethernetif *ethernetif = netif->state;
    struct pbuf *p = NULL;
    struct pbuf *q;
    uint32_t len;
    status_t status;

    /* Obtain the size of the packet and put it into the "len"
       variable. */
    status = enet_get_rx_frame_size(ethernetif, &len);

    if (kStatus_ENET_RxFrameEmpty != status)
    {
        /* Call enet_read_frame when there is a received frame. */
        if (len != 0)
        {
#if ETH_PAD_SIZE
            len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

            /* We allocate a pbuf chain of pbufs from the pool. */
            p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

            if (p != NULL)
            {
#if ETH_PAD_SIZE
                pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif
                if (p->next == 0) /* One-chain buffer.*/
                {
                    enet_read_frame(ethernetif, p->payload, p->len);
                }
                else    /* Multi-chain buffer.*/
                {
                    uint8_t data_tmp[ENET_FRAME_MAX_FRAMELEN];
                    uint32_t data_tmp_len = 0;

                    enet_read_frame(ethernetif, data_tmp, p->tot_len);

                    /* We iterate over the pbuf chain until we have read the entire
                    * packet into the pbuf. */
                    for (q = p; (q != NULL) && ((data_tmp_len + q->len) <= sizeof(data_tmp)); q = q->next)
                    {
                        /* Read enough bytes to fill this pbuf in the chain. The
                        * available data in the pbuf is given by the q->len
                        * variable. */
                        memcpy(q->payload,  &data_tmp[data_tmp_len], q->len);
                        data_tmp_len += q->len;
                    }
                }

                MIB2_STATS_NETIF_ADD(netif, ifinoctets, p->tot_len);
                if (((u8_t *)p->payload)[0] & 1)
                {
                    /* broadcast or multicast packet*/
                    MIB2_STATS_NETIF_INC(netif, ifinnucastpkts);
                }
                else
                {
                    /* unicast packet*/
                    MIB2_STATS_NETIF_INC(netif, ifinucastpkts);
                }
#if ETH_PAD_SIZE
                pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

                LINK_STATS_INC(link.recv);
            }
            else
            {
                /* drop packet*/
                enet_read_frame(ethernetif, NULL, 0U);

                LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: Fail to allocate new memory space\n"));

                LINK_STATS_INC(link.memerr);
                LINK_STATS_INC(link.drop);
                MIB2_STATS_NETIF_INC(netif, ifindiscards);
            }
        }
        else
        {
            /* Update the received buffer when error happened. */
            if (status == kStatus_ENET_RxFrameError)
            {
#if 0 && defined(FSL_FEATURE_SOC_ENET_COUNT) && (FSL_FEATURE_SOC_ENET_COUNT > 0) /* Error statisctics */
        enet_data_error_stats_t eErrStatic;
        /* Get the error information of the received g_frame. */
        ENET_GetRxErrBeforeReadFrame(&ethernetif->handle, &eErrStatic);
#endif

                /* Update the receive buffer. */
                enet_read_frame(ethernetif, NULL, 0U);

                LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: RxFrameError\n"));

                LINK_STATS_INC(link.drop);
                MIB2_STATS_NETIF_INC(netif, ifindiscards);
            }
        }
    }
    return p;
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void ethernetif_input(struct netif *netif)
{
    struct pbuf *p;

    LWIP_ASSERT("netif != NULL", (netif != NULL));

    /* move received packet into a new pbuf */
    while ((p = low_level_input(netif)) != NULL)
    {
        /* pass all packets to ethernet_input, which decides what packets it supports */
        if (netif->input(p, netif) != ERR_OK)
        {
            LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
            pbuf_free(p);
            p = NULL;
        }
    }
}

static ENET_Type *get_enet_base(const uint8_t enetIdx)
{
    ENET_Type* enets[] = ENET_BASE_PTRS;
    unsigned int arrayIdx;
    int enetCount;

    for (arrayIdx = 0, enetCount = 0; arrayIdx < ARRAY_SIZE(enets); arrayIdx++)
    {
        if (enets[arrayIdx] != 0U)    /* process only defined positions */
        {                             /* (some SOC headers count ENETs from 1 instead of 0) */
            if (enetCount == enetIdx)
            {
                return enets[arrayIdx];
            }
            enetCount++;
        }
    }

    return NULL;
}

static err_t ethernetif_init(struct netif *netif, struct ethernetif *ethernetif,
                             const uint8_t enetIdx,
                             const ethernetif_config_t *ethernetifConfig)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));
    LWIP_ASSERT("ethernetifConfig != NULL", (ethernetifConfig != NULL));

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

    /*
     * Initialize the snmp variables and counters inside the struct netif.
     * The last argument should be replaced with your link speed, in units
     * of bits per second.
     */
    MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

    netif->state = ethernetif;
    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
/* We directly use etharp_output() here to save a function call.
 * You can instead declare your own function an call etharp_output()
 * from it if you have to do some checks before sending (e.g. if link
 * is available...) */
#if LWIP_IPV4
    netif->output = etharp_output;
#endif
#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */
    netif->linkoutput = low_level_output;

#if LWIP_IPV4 && LWIP_IGMP
    netif_set_igmp_mac_filter(netif, ethernetif_igmp_mac_filter);
    netif->flags |= NETIF_FLAG_IGMP;
#endif
#if LWIP_IPV6 && LWIP_IPV6_MLD
    netif_set_mld_mac_filter(netif, ethernetif_mld_mac_filter);
    netif->flags |= NETIF_FLAG_MLD6;
#endif

    /* Init ethernetif parameters.*/
    ethernetif->base = get_enet_base(enetIdx);
    LWIP_ASSERT("ethernetif->base != NULL", (ethernetif->base != NULL));

    /* initialize the hardware */
    low_level_init(netif, enetIdx, ethernetifConfig);

    return ERR_OK;
}

/**
 * Should be called at the beginning of the program to set up the
 * first network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ethernetif0_init(struct netif *netif)
{
    static struct ethernetif ethernetif_0;
    AT_NONCACHEABLE_SECTION_ALIGN(static enet_rx_bd_struct_t rxBuffDescrip_0[ENET_RXBD_NUM], FSL_ENET_BUFF_ALIGNMENT);
    AT_NONCACHEABLE_SECTION_ALIGN(static enet_tx_bd_struct_t txBuffDescrip_0[ENET_TXBD_NUM], FSL_ENET_BUFF_ALIGNMENT);
    SDK_ALIGN(static rx_buffer_t rxDataBuff_0[ENET_RXBD_NUM], FSL_ENET_BUFF_ALIGNMENT);
    SDK_ALIGN(static tx_buffer_t txDataBuff_0[ENET_TXBD_NUM], FSL_ENET_BUFF_ALIGNMENT);

    ethernetif_0.RxBuffDescrip = &(rxBuffDescrip_0[0]);
    ethernetif_0.TxBuffDescrip = &(txBuffDescrip_0[0]);
    ethernetif_0.RxDataBuff = &(rxDataBuff_0[0]);
    ethernetif_0.TxDataBuff = &(txDataBuff_0[0]);

    return ethernetif_init(netif, &ethernetif_0, 0U, (ethernetif_config_t *)netif->state);
}

#if (defined(FSL_FEATURE_SOC_ENET_COUNT) && (FSL_FEATURE_SOC_ENET_COUNT > 1)) \
 || (defined(FSL_FEATURE_SOC_LPC_ENET_COUNT) && (FSL_FEATURE_SOC_LPC_ENET_COUNT > 1))
/**
 * Should be called at the beginning of the program to set up the
 * second network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ethernetif1_init(struct netif *netif)
{
    static struct ethernetif ethernetif_1;
    AT_NONCACHEABLE_SECTION_ALIGN(static enet_rx_bd_struct_t rxBuffDescrip_1[ENET_RXBD_NUM], FSL_ENET_BUFF_ALIGNMENT);
    AT_NONCACHEABLE_SECTION_ALIGN(static enet_tx_bd_struct_t txBuffDescrip_1[ENET_TXBD_NUM], FSL_ENET_BUFF_ALIGNMENT);
    SDK_ALIGN(static rx_buffer_t rxDataBuff_1[ENET_RXBD_NUM], FSL_ENET_BUFF_ALIGNMENT);
    SDK_ALIGN(static tx_buffer_t txDataBuff_1[ENET_TXBD_NUM], FSL_ENET_BUFF_ALIGNMENT);

    ethernetif_1.RxBuffDescrip = &(rxBuffDescrip_1[0]);
    ethernetif_1.TxBuffDescrip = &(txBuffDescrip_1[0]);
    ethernetif_1.RxDataBuff = &(rxDataBuff_1[0]);
    ethernetif_1.TxDataBuff = &(txDataBuff_1[0]);

    return ethernetif_init(netif, &ethernetif_1, 1U, (ethernetif_config_t *)netif->state);
}
#endif /* FSL_FEATURE_SOC_*_ENET_COUNT */
