//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// Portions Copyright EBSNet , 2007
// See LICENSE file in the project root for full license information.
//

//
// enc28j60.C - device driver interface
//

#include <nanohal.h>

#include "enc28j60_lwip.h"

#include "lwip\netif.h"
#include "lwip\pbuf.h"
#include "lwip\mem.h"

extern void lwip_interrupt_continuation(void);

// FIXME
#define hal_printf(x)

/* ********************************************************************
   GLOBAL DATA
   ******************************************************************** */

extern HAL_TARGET_CONFIGURATION g_TargetConfiguration;

extern ENC28J60_LWIP_DEVICE_CONFIG g_ENC28J60_LWIP_Config;

/* Function Prototypes */

void enc28j60_lwip_soft_reset(uint32_t spiHandle);

void enc28j60_lwip_write_spi(
    uint32_t spiHandle,
    uint8_t opcode,
    uint8_t address,
    uint8_t perPacketCtrlByte,
    uint8_t *byteData,
    uint32_t numBytes);

void enc28j60_lwip_write_spi(uint32_t spiHandle, uint8_t opcode, uint8_t address, uint8_t byteData);

void enc28j60_lwip_read_spi(
    uint32_t spiHandle,
    uint8_t opcode,
    uint8_t address,
    uint8_t *byteData,
    uint32_t numBytes,
    uint8_t offset);

void enc28j60_lwip_select_bank(uint32_t spiHandle, uint8_t bankNumber);

void enc28j60_lwip_write_phy_register(uint32_t spiHandle, uint8_t registerAddress, unsigned short data);

bool enc28j60_lwip_setup_device(struct netif *pNetIF);

void enc28j60_lwip_destroy_device();

unsigned short enc28j60_lwip_read_phy_register(uint32_t spiHandle, uint8_t registerAddress);

/* keep track of the receive pointer */
static unsigned short s_ENC28J60_TRANSMIT_BUFFER_START = ENC28J60_TRANSMIT_BUFFER_START;
static unsigned short s_ENC28J60_RECEIVE_BUFFER_START = ENC28J60_RECEIVE_BUFFER_START;

/* ********************************************************************
   open the ENC28J60 driver interface.

   This routine opens a ENC28J60 device driver

   The address of this function must be placed into the "devices" table in
   iface.c either at compile time or before a device open is called.

  ******************************************************************** */
bool enc28j60_lwip_open(struct netif *pNetIF)

{
    NATIVE_PROFILE_HAL_DRIVERS_ETHERNET();

    return enc28j60_lwip_setup_device(pNetIF);
}

/* ********************************************************************
   close the packet driver interface.

   This routine is called when the device interface is no longer needed
   it should stop the driver from delivering packets to the upper levels
   and shut off packet delivery to the network.

   The address of this function must be placed into the "devices" table in
   iface.c either at compile time or before a device open is called.

 */
void enc28j60_lwip_close(struct netif *pNetIF)
{
    (void)pNetIF;

    NATIVE_PROFILE_HAL_DRIVERS_ETHERNET();

    enc28j60_lwip_destroy_device();
}

/* ********************************************************************
   Interrupt handling routine at the task level

   ******************************************************************** */
#define TRANSMIT_RETRIES 5
static uint8_t s_retriesTransmit = TRANSMIT_RETRIES;
#define RECEIVE_RETRIES 5
static uint8_t s_receiveRetries = 10;

void enc28j60_handle_recv_error(struct netif *pNetIF, uint32_t spiHandle)
{
    (void)pNetIF;

    uint8_t byteData;

    if (--s_receiveRetries <= 0)
    {
        s_receiveRetries = RECEIVE_RETRIES;
        enc28j60_lwip_setup_device(pNetIF);
        return;
    }

    /* Reset the receiver logic */
    byteData = (1 << ENC28J60_ECON1_RXRST_BIT);
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_BIT_FIELD_SET_OPCODE, ENC28J60_ECON1, byteData);
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_BIT_FIELD_CLEAR_OPCODE, ENC28J60_ECON1, byteData);

    enc28j60_lwip_read_spi(spiHandle, ENC28J60_SPI_READ_CONTROL_REGISTER_OPCODE, ENC28J60_ESTAT, &byteData, 1, 0);

    /* buffer is corrupted flush it */
    if (byteData & (1 << ENC28J60_ESTAT_BUFER))
    {
        enc28j60_lwip_setup_recv_buffer(pNetIF, spiHandle);
    }

    byteData = (1 << ENC28J60_EIR_RXERIF_BIT);
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_BIT_FIELD_CLEAR_OPCODE, ENC28J60_EIR, byteData);

    byteData = ((1 << ENC28J60_EIE_RXERIE_BIT) | (1 << ENC28J60_EIE_INTIE_BIT));
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_BIT_FIELD_SET_OPCODE, ENC28J60_EIE, byteData);

    byteData = (1 << ENC28J60_ECON1_RXEN_BIT);
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_BIT_FIELD_SET_OPCODE, ENC28J60_ECON1, byteData);

    /* Is there an interrupt pending */
    enc28j60_lwip_read_spi(spiHandle, ENC28J60_SPI_READ_CONTROL_REGISTER_OPCODE, ENC28J60_ESTAT, &byteData, 1, 0);

    s_ENC28J60_RECEIVE_BUFFER_START = ENC28J60_RECEIVE_BUFFER_START;
}

void enc28j60_handle_xmit_error(struct netif *pNetIF, uint32_t spiHandle)
{
    (void)pNetIF;

    uint8_t byteData;

    byteData = (1 << ENC28J60_ECON1_TXRST_BIT);
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_BIT_FIELD_SET_OPCODE, ENC28J60_ECON1, byteData);
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_BIT_FIELD_CLEAR_OPCODE, ENC28J60_ECON1, byteData);

    byteData = (1 << ENC28J60_EIR_TXERIF_BIT) | (1 << ENC28J60_EIR_TXIF_BIT);
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_BIT_FIELD_CLEAR_OPCODE, ENC28J60_EIR, byteData);

    byteData = ((1 << ENC28J60_EIE_TXERIE_BIT) | (1 << ENC28J60_EIE_INTIE_BIT));
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_BIT_FIELD_SET_OPCODE, ENC28J60_EIE, byteData);

    byteData = (1 << ENC28J60_ECON1_TXRTS_BIT);

    /* CLEAR the TXRTS bit to cancel the last transmission */
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_BIT_FIELD_CLEAR_OPCODE, ENC28J60_ECON1, byteData);

    if (s_retriesTransmit--)
    {
        /* SET the TXRTS bit to retry the last transmission */
        enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_BIT_FIELD_SET_OPCODE, ENC28J60_ECON1, byteData);
    }
    else
    {
        s_retriesTransmit = TRANSMIT_RETRIES;
    }
}

void enc28j60_lwip_pre_interrupt(GPIO_PIN Pin, bool PinState, void *pArg)
{
    (void)Pin;
    (void)PinState;

    NATIVE_PROFILE_HAL_DRIVERS_ETHERNET();

    struct netif *pNetIF = (struct netif *)pArg;
    uint8_t byteData;
    uint8_t eirData;

    uint32_t spiHandle = g_ENC28J60_LWIP_Config.DeviceConfigs[0].spiHandle;

    GLOBAL_LOCK();

    /* After an interrupt occurs, the host controller should
        clear the global enable bit for the interrupt pin before
        servicing the interrupt. Clearing the enable bit will
        cause the interrupt pin to return to the non-asserted
        state (high). Doing so will prevent the host controller
        from missing a falling edge should another interrupt
        occur while the immediate interrupt is being serviced.
     */

    byteData = (1 << ENC28J60_EIE_INTIE_BIT);
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_BIT_FIELD_CLEAR_OPCODE, ENC28J60_EIE, byteData);

    enc28j60_lwip_read_spi(spiHandle, ENC28J60_SPI_READ_CONTROL_REGISTER_OPCODE, ENC28J60_EIR, &eirData, 1, 0);

    /* recover from tx error */
    if (eirData & (1 << ENC28J60_EIR_TXERIF_BIT))
    {
        enc28j60_handle_xmit_error(pNetIF, spiHandle);
    }

    lwip_interrupt_continuation();

    GLOBAL_UNLOCK();
}

/* ********************************************************************
   Interrupt handling routine at the task level

   ******************************************************************** */
void enc28j60_lwip_interrupt(struct netif *pNetIF)

{
    uint8_t byteData;
    uint8_t cntPkts;
    uint8_t eirData;
    uint8_t status;
    int packetsLeft = 0;

    GLOBAL_LOCK();

    if (!pNetIF)
    {
        return;
    }

    uint32_t spiHandle = g_ENC28J60_LWIP_Config.DeviceConfigs[0].spiHandle;

    /* Is there an interrupt pending */
    enc28j60_lwip_read_spi(spiHandle, ENC28J60_SPI_READ_CONTROL_REGISTER_OPCODE, ENC28J60_ESTAT, &status, 1, 0);

    /*  FROM ERRATA
        Module: Interrupts The Receive Packet Pending Interrupt Flag (EIR.PKTIF) does not reliably/accurately report
        the status of pending packets. Work around In the Interrupt Service Routine, if it is unknown if
        a packet is pending and the source of the interrupt is unknown, switch to Bank 1 and check the value
        in EPKTCNT. If polling to see if a packet is pending, check the value in EPKTCNT.

        //enc28j60_lwip_read_spi(spiHandle,  ENC28J60_SPI_READ_CONTROL_REGISTER_OPCODE, ENC28J60_EIR, &byteData, 1, 0);
        //if (byteData & (1 << ENC28J60_EIR_PKTIF_BIT))
    */

    /* Read the number of packets remaining */
    enc28j60_lwip_select_bank(spiHandle, ENC28J60_CONTROL_REGISTER_BANK1);
    enc28j60_lwip_read_spi(spiHandle, ENC28J60_SPI_READ_BUFFER_MEMORY_OPCODE, ENC28J60_EPKTCNT, &cntPkts, 1, 0);

    if ((status & (1 << ENC28J60_ESTAT_INT)) || cntPkts)
    {
        enc28j60_lwip_read_spi(spiHandle, ENC28J60_SPI_READ_CONTROL_REGISTER_OPCODE, ENC28J60_EIR, &eirData, 1, 0);

        if (eirData & (1 << ENC28J60_EIR_TXERIF_BIT))
        {
            enc28j60_handle_xmit_error(pNetIF, spiHandle);
        }
        else if (eirData & (1 << ENC28J60_EIR_TXIF_BIT)) /* A packet is xmited */
        {
            s_retriesTransmit = TRANSMIT_RETRIES;

            /* signal IP task that xmit has completed */
            /* JJM Eventually this should invoke enc28j60_lwip_xmit_done(); */
        }

        /* recover from rx error */
        if (eirData & (1 << ENC28J60_EIR_RXERIF_BIT))
        {
            enc28j60_handle_recv_error(pNetIF, spiHandle);
        }
        else
        {
            s_receiveRetries = RECEIVE_RETRIES;
        }

        if (cntPkts)
        {
            // FIXME - Restores an interrupt to its previous state, as stored by the SmartPtr_IRQ.Acquire method.
            //			irq.Release();
            packetsLeft = enc28j60_lwip_recv(pNetIF);
            // FIXME  - Disables interrupts if they are currently enabled, storing the previous interrupt state.
            //			irq.Acquire();
        }
    }

    /* enable packet reception */
    enc28j60_lwip_read_spi(spiHandle, ENC28J60_SPI_READ_CONTROL_REGISTER_OPCODE, ENC28J60_ECON1, &byteData, 1, 0);

    /* Is the receiver or transmitter in reset state */
    if ((byteData & (1 << ENC28J60_ECON1_TXRST_BIT)) || (byteData & (1 << ENC28J60_ECON1_RXRST_BIT)))
    {
        /* If so, take it out of reset */
        byteData &= ~((1 << ENC28J60_ECON1_TXRST_BIT) | (1 << ENC28J60_ECON1_RXRST_BIT));
    }

    byteData |= (1 << ENC28J60_ECON1_RXEN_BIT);
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_ECON1, byteData);

    /* clear errors if there are any */
    byteData = 0xFF;
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_BIT_FIELD_CLEAR_OPCODE, ENC28J60_ESTAT, byteData);
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_BIT_FIELD_CLEAR_OPCODE, ENC28J60_EIR, byteData);

    /* re-enable interrupts when a packet is received and when transmit is done  */
    byteData = (1 << ENC28J60_EIE_INTIE_BIT);
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_BIT_FIELD_SET_OPCODE, ENC28J60_EIE, byteData);

    if (packetsLeft)
    {
        /* If there are more packets left re-queue the continuation */

        lwip_interrupt_continuation();
    }

    GLOBAL_UNLOCK();
}

/* ********************************************************************
   Receive. a packet over the packet driver interface.  This is called
   from the ISR or task ISR.

   This routine is called when a packet is received.

   Upon successful reading of received packet, the IP is
   signalled to process the packet.

   Returns the number of packets that remain to be processed

  ******************************************************************** */
int enc28j60_lwip_recv(struct netif *pNetIF)
{
    NATIVE_PROFILE_HAL_DRIVERS_ETHERNET();

    struct pbuf *pPBuf;
    uint8_t *dataRX;
    uint8_t nextPktAndRecvStatusVector[6];
    uint16_t length;
    uint8_t byteData;
    uint8_t packetsLeft;
    uint16_t lastReceiveBuffer;
    uint32_t spiHandle;

    int numPacketsProcessed = 0;

    if (!pNetIF)
    {
        return 1;
    }

    spiHandle = g_ENC28J60_LWIP_Config.DeviceConfigs[0].spiHandle;

    do
    {
        /* Disable interrupt for each loop and only each loop */
        GLOBAL_LOCK();

        /* Set the read buffer pointer to the beginning of the packet */
        enc28j60_lwip_select_bank(spiHandle, ENC28J60_CONTROL_REGISTER_BANK0);

        byteData = s_ENC28J60_RECEIVE_BUFFER_START & 0xFF;

        enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_ERDPTL, byteData);

        byteData = (s_ENC28J60_RECEIVE_BUFFER_START >> 8) & 0xFF;

        enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_ERDPTH, byteData);

        /* Get the next packet pointer */
        enc28j60_lwip_read_spi(
            spiHandle,
            ENC28J60_SPI_READ_BUFFER_MEMORY_OPCODE,
            ENC28J60_SPI_READ_BUFFER_MEMORY_ARGUMENT,
            nextPktAndRecvStatusVector,
            6,
            0);

        lastReceiveBuffer = (nextPktAndRecvStatusVector[1] << 8) | nextPktAndRecvStatusVector[0];
        length = (nextPktAndRecvStatusVector[3] << 8) | nextPktAndRecvStatusVector[2];

        /* corrupted */
        if (lastReceiveBuffer > ENC28J60_RECEIVE_BUFFER_END)
        {
            enc28j60_handle_recv_error(pNetIF, spiHandle);
            packetsLeft = 0;
            break;
        }
        else
        {
            s_ENC28J60_RECEIVE_BUFFER_START = lastReceiveBuffer;
        }

        if (length != 0)
        {
            // FIXME
            //				irq.Release();
            pPBuf = pbuf_alloc(PBUF_RAW, length, PBUF_RAM);
            // FIXME
            //				irq.Acquire();

            if (pPBuf)
            {
                dataRX = (uint8_t *)pPBuf->payload;

                // remove the checksum trailing bytes
                if (length > 63)
                {
                    length -= 4;
                }

                /* Get the packet */
                enc28j60_lwip_read_spi(
                    spiHandle,
                    ENC28J60_SPI_READ_BUFFER_MEMORY_OPCODE,
                    ENC28J60_SPI_READ_BUFFER_MEMORY_ARGUMENT,
                    dataRX,
                    length,
                    0);

                /* invoke stack ip input - the stack should free the buffer when it is done,
                                    so DON'T call pbuf_free on pPBuf!!!!!*/
                // FIXME
                //					irq.Release();
                pNetIF->input(pPBuf, pNetIF);
                // FIXME
                //					irq.Acquire();
            }
            else
            {
                hal_printf("enc28j60_lwip_recv: input alloc packet failed \r\n");
            }
        }
        else // no packets left - should we break??
        {
        }

        if (0 == (s_ENC28J60_RECEIVE_BUFFER_START % 2))
        {
            /* from errata rev.b5 - circular buffer doesn't handle even numbers well -> nextPkt is guarranteed to be
             * even */
            if (((s_ENC28J60_RECEIVE_BUFFER_START - 1) < ENC28J60_RECEIVE_BUFFER_START) ||
                ((s_ENC28J60_RECEIVE_BUFFER_START - 1) > ENC28J60_RECEIVE_BUFFER_END))
            {
                lastReceiveBuffer = ENC28J60_RECEIVE_BUFFER_END;
            }
            else
            {
                lastReceiveBuffer = s_ENC28J60_RECEIVE_BUFFER_START - 1;
            }
        }
        else
        {

            lastReceiveBuffer = s_ENC28J60_RECEIVE_BUFFER_START;
        }

        /* Free the packet from the ethernet */
        byteData = lastReceiveBuffer & 0xFF;
        enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_ERXRDPTL, byteData);

        byteData = lastReceiveBuffer >> 8;
        enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_ERXRDPTH, byteData);

        /* the host controller must write a 1 to the ECON2.PKTDEC bit.
                        Doing so will cause the EPKTCNT register to decrement by 1 */
        byteData = (1 << ENC28J60_ECON2_PKTDEC_BIT);
        enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_BIT_FIELD_SET_OPCODE, ENC28J60_ECON2, byteData);

        /* Read the number of packets remaining*/
        enc28j60_lwip_select_bank(spiHandle, ENC28J60_CONTROL_REGISTER_BANK1);
        enc28j60_lwip_read_spi(spiHandle, ENC28J60_SPI_READ_BUFFER_MEMORY_OPCODE, ENC28J60_EPKTCNT, &packetsLeft, 1, 0);

        if ((++numPacketsProcessed > CFG_MAX_PACKETS_PROCESSED) && packetsLeft)
        {
            break;
        }

        GLOBAL_UNLOCK();
    } while (packetsLeft);

    return packetsLeft;
}

/* ********************************************************************
   Transmit. a packet over the packet driver interface.

   This routine is called when a packet needs sending. The packet contains a
   full ethernet frame to be transmitted. The length of the packet is
   provided.

   Returns 0 if successful or errno if unsuccessful

 */

err_t enc28j60_lwip_xmit(struct netif *pNetIF, struct pbuf *pPBuf)
{
    NATIVE_PROFILE_HAL_DRIVERS_ETHERNET();
    uint16_t length = 0;
    uint8_t perPacketControlByte;
    uint8_t dataByte;
    uint32_t spiHandle;
    struct pbuf *pTmp;
    uint8_t *pTx;
    int idx;
    int retries = 100;

    GLOBAL_LOCK();

    if (!pNetIF)
    {
        return ERR_ARG;
    }

    spiHandle = g_ENC28J60_LWIP_Config.DeviceConfigs[0].spiHandle;

    enc28j60_lwip_read_spi(spiHandle, ENC28J60_SPI_READ_CONTROL_REGISTER_OPCODE, ENC28J60_ECON1, &dataByte, 1, 0);

    length = pPBuf->tot_len;

    if (length > ETHERSIZE) // (ETHERSIZE+4))
    {
        debug_printf("xmit - length is too large, truncated \r\n");
        ASSERT(FALSE);
        length = ETHERSIZE; // ETHERSIZE+4;         /* what a terriable hack! */
    }

    /* First see if there is enough space in the remainder of the transmit buffer */
    /* 7 for the status bytes and 1 for the control byte */
    if ((length + 7 + 1 + (uint16_t)s_ENC28J60_TRANSMIT_BUFFER_START) > ENC28J60_TRANSMIT_BUFFER_END)
    {
        s_ENC28J60_TRANSMIT_BUFFER_START = ENC28J60_TRANSMIT_BUFFER_START;
    }

    do
    {
        enc28j60_lwip_read_spi(spiHandle, ENC28J60_SPI_READ_CONTROL_REGISTER_OPCODE, ENC28J60_ECON1, &dataByte, 1, 0);
    } while (0 != (dataByte & (1 << ENC28J60_ECON1_TXRTS_BIT)) && retries-- > 0);

    /*  1. Appropriately program the ETXST pointer to
            point to an unused location in memory. It will
            point to the per packet control byte.
        2. Use the WBM SPI command to write the per
            packet control byte, the destination address, the
            source MAC address, the type/length and the
            data payload.
        3. Appropriately program the ETXND pointer. It
            should point to the last byte in the data payload.
        4. Clear EIR.TXIF, set EIE.TXIE and set EIE.INTIE
            to enable an interrupt when done (if desired).
        5. Start the transmission process by setting
            ECON1.TXRTS.
    */

    /* Making sure to select the right bank */
    enc28j60_lwip_select_bank(spiHandle, ENC28J60_CONTROL_REGISTER_BANK0);

    /* setup the EWRPTL AND EWRPTH to the beginning of the transmit buffer */
    dataByte = s_ENC28J60_TRANSMIT_BUFFER_START & 0xFF;
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_EWRPTL, dataByte);
    dataByte = (s_ENC28J60_TRANSMIT_BUFFER_START >> 8) & 0xFF;
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_EWRPTH, dataByte);

    /* 1. */
    dataByte = s_ENC28J60_TRANSMIT_BUFFER_START & 0xFF;
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_ETXSTL, dataByte);
    dataByte = (s_ENC28J60_TRANSMIT_BUFFER_START >> 8) & 0xFF;
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_ETXSTH, dataByte);

    /* 2. */
    /* Write per byte control byte and the first byte of data */
    perPacketControlByte = (1 << ENC28J60_XMIT_CONTROL_PPADEN_BIT) | (1 << ENC28J60_XMIT_CONTROL_PCRCEN_BIT);

    // FIXME
    // irq.Release();
    pTmp = pbuf_alloc(PBUF_RAW, length + 2, PBUF_RAM);
    // FIXME
    // irq.Acquire();

    if (!pTmp)
        return ERR_MEM;

    pTx = (uint8_t *)pTmp->payload;

    pTx[0] = ENC28J60_SPI_OPCODE_ARGUMENT(
        ENC28J60_SPI_WRITE_BUFFER_MEMORY_OPCODE,
        ENC28J60_SPI_WRITE_BUFFER_MEMORY_ARGUMENT);
    pTx[1] = perPacketControlByte;

    idx = 2;

    while (pPBuf)
    {
        memcpy(&pTx[idx], pPBuf->payload, pPBuf->len);

        idx += pPBuf->len;

        pPBuf = pPBuf->next;
    }

    // CPU_SPI_nWrite8_nRead8(spiHandle, false, pTx, length+2, 0, 0, 0 );
    SPI_WRITE_READ_SETTINGS wrc{false, 0, false, 0, ENC28J60_CS, ENC28J60_CS_ACTIVE};
    nanoSPI_Write_Read(spiHandle, wrc, pTx, length + 2, 0, 0);

    // FIXME
    // irq.Release();
    pbuf_free(pTmp);
    // FIXME
    // irq.Acquire();

    s_ENC28J60_TRANSMIT_BUFFER_START += length;

    /* 3. */
    /* Making sure to select the right bank */
    enc28j60_lwip_select_bank(spiHandle, ENC28J60_CONTROL_REGISTER_BANK0);
    dataByte = s_ENC28J60_TRANSMIT_BUFFER_START & 0xFF;
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_ETXNDL, dataByte);
    dataByte = (s_ENC28J60_TRANSMIT_BUFFER_START >> 8) & 0xFF;
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_ETXNDH, dataByte);

    /* TAKE CARE OF WRAP HERE */
    /* Leave 7 bytes for the status Vector + 1 for control byte */
    s_ENC28J60_TRANSMIT_BUFFER_START += 8;

    /* Make sure that it is on an even address */
    if ((s_ENC28J60_TRANSMIT_BUFFER_START % 2) != 0)
        s_ENC28J60_TRANSMIT_BUFFER_START++;

    /* 4. */
    dataByte = 1 << ENC28J60_EIR_TXIF_BIT;
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_BIT_FIELD_CLEAR_OPCODE, ENC28J60_EIR, dataByte);

    dataByte = ((1 << ENC28J60_EIE_INTIE_BIT) | (1 << ENC28J60_EIE_TXERIE_BIT));
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_BIT_FIELD_SET_OPCODE, ENC28J60_EIE, dataByte);

    /* 5. */
    dataByte = (1 << ENC28J60_ECON1_TXRTS_BIT);
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_BIT_FIELD_SET_OPCODE, ENC28J60_ECON1, dataByte);

    GLOBAL_UNLOCK();

    return ERR_OK;
}

void enc28j60_lwip_setup_recv_buffer(struct netif *pNetIF, uint32_t spiHandle)
{
    (void)pNetIF;

    uint8_t byteData;

    /* Making sure to select the right bank */
    enc28j60_lwip_select_bank(spiHandle, ENC28J60_CONTROL_REGISTER_BANK0);

    /* keep track of the receive pointer */
    s_ENC28J60_RECEIVE_BUFFER_START = ENC28J60_RECEIVE_BUFFER_START;

    /* Specifying the receive buffer size */
    /* First, write the low byte of the starting address*/
    byteData = ENC28J60_RECEIVE_BUFFER_START & 0xFF;
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_ERXSTL, byteData);

    /* Then, write the upper byte of the starting address*/
    byteData = (ENC28J60_RECEIVE_BUFFER_START >> 8) & 0xFF;
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_ERXSTH, byteData);

    /* First, write the low byte of the ending address*/
    byteData = ENC28J60_RECEIVE_BUFFER_END & 0xFF;
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_ERXNDL, byteData);
    /* Then, write the upper byte of the starting address*/
    byteData = ((ENC28J60_RECEIVE_BUFFER_END >> 8) & 0xFF);
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_ERXNDH, byteData);

    enc28j60_lwip_read_spi(spiHandle, ENC28J60_SPI_READ_CONTROL_REGISTER_OPCODE, ENC28J60_ERXWRPTL, &byteData, 1, 0);
    enc28j60_lwip_read_spi(spiHandle, ENC28J60_SPI_READ_CONTROL_REGISTER_OPCODE, ENC28J60_ERXWRPTH, &byteData, 1, 0);

    /* The ERXRDPT register should be program with the same value
        as ERXST in the beginning */
    /* LOOK AT ERRATA */
    // byteData = (ENC28J60_RECEIVE_BUFFER_START-1) & 0xFF;
    byteData = ENC28J60_RECEIVE_BUFFER_END & 0xFF;
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_ERXRDPTL, byteData);

    // byteData = (((ENC28J60_RECEIVE_BUFFER_START-1)>> 8) & 0xFF);
    byteData = ((ENC28J60_RECEIVE_BUFFER_END) >> 8) & 0xFF;
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_ERXRDPTH, byteData);
}

bool enc28j60_lwip_setup_device(struct netif *pNetIF)
{
    NATIVE_PROFILE_HAL_DRIVERS_ETHERNET();
    uint8_t byteData;
    uint16_t duplex;
    uint16_t phyID1;
    uint16_t phyID2;
    uint16_t shortData;
    int16_t nLoopCnt = 0;
    uint32_t spiHandle;

    spiHandle = g_ENC28J60_LWIP_Config.DeviceConfigs[0].spiHandle;

    // Save handle
    g_ENC28J60_LWIP_Config.DeviceConfigs[0].spiHandle = spiHandle;

    /* ---------------------------------------------------------------------------------------------------- */
    /*                                          CHECK IF THE PHY IS READY                                   */

    /* Making sure to select the right bank */
    enc28j60_lwip_select_bank(spiHandle, ENC28J60_CONTROL_REGISTER_BANK0);
    do
    {
        enc28j60_lwip_read_spi(spiHandle, ENC28J60_SPI_READ_CONTROL_REGISTER_OPCODE, ENC28J60_ESTAT, &byteData, 1, 0);

        // either the chip doesn't return a valid status bit or the chip is not connected
        if (50 < nLoopCnt++)
        {
            debug_printf("Ethernet PHY NOT IN READY STATE\n");
            break; // try any way
        }

        OS_DELAY(1);

    } while ((0x1 != (byteData & 0x1)) || (0xFF == byteData));

    /* Making sure to select the right bank */
    enc28j60_lwip_select_bank(spiHandle, ENC28J60_CONTROL_REGISTER_BANK3);
    enc28j60_lwip_read_spi(spiHandle, ENC28J60_SPI_READ_CONTROL_REGISTER_OPCODE, ENC28J60_EREVID, &byteData, 1, 0);

    /* ---------------------------------------------------------------------------------------------------- */
    /*                                                  SOFT RESET                                          */

    enc28j60_lwip_soft_reset(spiHandle);

    /* ---------------------------------------------------------------------------------------------------- */
    /*                                              VERIFY THE DEVICE ID                                    */

    phyID1 = enc28j60_lwip_read_phy_register(spiHandle, ENC28J60_PHID1);
    phyID2 = enc28j60_lwip_read_phy_register(spiHandle, ENC28J60_PHID2);

    if ((phyID1 != ENC28J60_PHYID1) || (phyID2 != ENC28J60_PHYID2))
    {
        hal_printf("enc28j60_lwip_open: Wrong Device");
    }

    /* ---------------------------------------------------------------------------------------------------- */
    /*                                          SETUP RECEIVE  BUFFER                                       */

    enc28j60_lwip_setup_recv_buffer(pNetIF, spiHandle);

    /* ---------------------------------------------------------------------------------------------------- */
    /*                                          SETUP RECEIVE FILTER                                       */

    /* Making sure to select the right bank */
    enc28j60_lwip_select_bank(spiHandle, ENC28J60_CONTROL_REGISTER_BANK1);
    /* Disable packet filtering (Promiscuous mode) */
    byteData = 0;
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_ERXFCON, byteData);

    /* ---------------------------------------------------------------------------------------------------- */
    /*                                          MAC INITIALIZATION SETTINGS                                 */

    /* Making sure to select the right bank */
    enc28j60_lwip_select_bank(spiHandle, ENC28J60_CONTROL_REGISTER_BANK2);

    /* Clear the MARST bit in MACON2 */
    // NEW MANUAL DOESN'T DO THIS
    // enc28j60_lwip_read_spi(SpiConf, ENC28J60_SPI_READ_CONTROL_REGISTER_OPCODE, ENC28J60_MACON2, &byteData, 1, 1);
    // byteData &= ~(1 << ENC28J60_MACON2_MARST_BIT);
    // enc28j60_lwip_write_spi(SpiConf,  ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE,ENC28J60_MACON2, byteData);

    /* Making sure to select the right bank */
    enc28j60_lwip_select_bank(spiHandle, ENC28J60_CONTROL_REGISTER_BANK2);

    /* Set the MARXEN bit in MACON1 */
    enc28j60_lwip_read_spi(spiHandle, ENC28J60_SPI_READ_CONTROL_REGISTER_OPCODE, ENC28J60_MACON1, &byteData, 1, 1);
    byteData = (1 << ENC28J60_MACON1_MARXEN_BIT);
#if (ENC28J60_FULL_DUPLEX)
    byteData |= (1 << ENC28J60_MACON1_TXPAUS_BIT);
    byteData |= (1 << ENC28J60_MACON1_RXPAUS_BIT);
#endif

    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_MACON1, byteData);

    /* Configure the PADCFT, TXCRECEN, AND FULLDPS bits
        of MACON3 */

    byteData = (5 << ENC28J60_MACON3_PADCFG0_BIT) | ENC28J60_MACON3_TXCRCEN_BIT;
    //(1 << ENC28J60_MACON3_TXCRCEN_BIT);
#if (ENC28J60_FULL_DUPLEX)
    byteData |= (1 << ENC28J60_MACON3_FULDPX_BIT);
#endif
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_MACON3, byteData);

    byteData = 0;
    /* Configure the bits in MACON4     */
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_MACON4, byteData);

    /* Configure the MAMXFL registers */
    byteData = ENC28J60_MAXIMUM_FRAME_SIZE & 0xFF;
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_MAMXFLL, byteData);
    byteData = (ENC28J60_MAXIMUM_FRAME_SIZE >> 8) & 0xFF;
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_MAMXFLH, byteData);

    /* Configure the Back-to-back inter-packet gap */
#if (ENC28J60_FULL_DUPLEX)
    byteData = 0x15;
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_MABBIPG, byteData);
#else
    byteData = 0x12;
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_MABBIPG, byteData);
#endif

    /* Configure the non-back-to-back inter-packet gap */
    byteData = 0x12;
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_MAIPGL, byteData);

#if (!ENC28J60_FULL_DUPLEX)
    byteData = 0xC;
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_MAIPGH, byteData);
#endif

    /* Making sure to select the right bank */
    enc28j60_lwip_select_bank(spiHandle, ENC28J60_CONTROL_REGISTER_BANK3);

    /* Setup the MAC address */
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_MAADR1, pNetIF->hwaddr[0]);
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_MAADR2, pNetIF->hwaddr[1]);
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_MAADR3, pNetIF->hwaddr[2]);
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_MAADR4, pNetIF->hwaddr[3]);
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_MAADR5, pNetIF->hwaddr[4]);
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_MAADR6, pNetIF->hwaddr[5]);

    /* ---------------------------------------------------------------------------------------------------- */
    /*                                  CHECK DUPLEX MODE                                                   */
    duplex = enc28j60_lwip_read_phy_register(spiHandle, ENC28J60_PHCON1);

    enc28j60_lwip_select_bank(spiHandle, ENC28J60_CONTROL_REGISTER_BANK2);
    enc28j60_lwip_read_spi(spiHandle, ENC28J60_SPI_READ_CONTROL_REGISTER_OPCODE, ENC28J60_MACON3, &byteData, 1, 1);

    if (((duplex >> ENC28J60_PHCON1_PDPXMD) & 1) != ((byteData >> ENC28J60_MACON3_FULDPX_BIT) & 1))
    {
        hal_printf("enc28j60_lwip_open: check duplex setting \r\n");
    }

    /* ---------------------------------------------------------------------------------------------------- */
    /*                                                                                                      */

    /* The first time the ethernet is initialized, let us
        point the ETXST pointer to the beginning of the
        buffer space. Subsequent increments of this register
        will be done in the transmit function */

    s_ENC28J60_TRANSMIT_BUFFER_START = ENC28J60_TRANSMIT_BUFFER_START;
    s_ENC28J60_RECEIVE_BUFFER_START = ENC28J60_RECEIVE_BUFFER_START;

    /* enabling reception */

    /* Making sure to select the right bank */
    enc28j60_lwip_select_bank(spiHandle, ENC28J60_CONTROL_REGISTER_BANK0);

    /* setup the ERDPTL AND ERDPTH to the beginning of the receive buffer */
    byteData = ENC28J60_RECEIVE_BUFFER_START & 0xFF;
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_ERDPTL, byteData);
    byteData = (ENC28J60_RECEIVE_BUFFER_START >> 8) & 0xFF;
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_ERDPTH, byteData);

    /* -----------------------------------DISABLE LOOPBACK FOR HALF DUPLEX--------------------------------- */
    /*                                                                                                      */
#if (!ENC28J60_FULL_DUPLEX)
    shortData = enc28j60_lwip_read_phy_register(spiHandle, ENC28J60_PHCON2);
    shortData = 1 << ENC28J60_PHCON2_HDLDIS;
    enc28j60_lwip_write_phy_register(spiHandle, ENC28J60_PHCON2, shortData);
    shortData = enc28j60_lwip_read_phy_register(spiHandle, ENC28J60_PHCON2);
#endif
    /* -----------------------------------DISABLE PHY INTERRUPTS ------------------------------------------ */
    /*                                                                                                      */
    shortData = 0x0;
    enc28j60_lwip_write_phy_register(spiHandle, ENC28J60_PHIE, shortData);

    /* -------------------------------------- START THE DEVICE -------------------------------------------- */
    /*                                                                                                      */
    /* enable interrupts when a packet is received and when transmit is done */
    enc28j60_lwip_write_spi(
        spiHandle,
        ENC28J60_SPI_BIT_FIELD_SET_OPCODE,
        ENC28J60_EIE,
        (uint8_t)((1 << ENC28J60_EIE_INTIE_BIT) | (1 << ENC28J60_EIE_PKTIE_BIT) | (1 << ENC28J60_EIE_TXERIE_BIT)));

    /* enable auto- increment */
    enc28j60_lwip_write_spi(
        spiHandle,
        ENC28J60_SPI_BIT_FIELD_SET_OPCODE,
        ENC28J60_ECON2,
        (uint8_t)(1 << ENC28J60_ECON2_AUTOINC_BIT));

    /* enable packet reception */
    enc28j60_lwip_write_spi(
        spiHandle,
        ENC28J60_SPI_BIT_FIELD_SET_OPCODE,
        ENC28J60_ECON1,
        (uint8_t)(1 << ENC28J60_ECON1_RXEN_BIT));

    return TRUE;
}

void enc28j60_lwip_destroy_device()
{
    uint32_t spiHandle = g_ENC28J60_LWIP_Config.DeviceConfigs[0].spiHandle;

    enc28j60_lwip_write_spi(
        spiHandle,
        ENC28J60_SPI_BIT_FIELD_CLEAR_OPCODE,
        ENC28J60_EIE,
        (uint8_t)((1 << ENC28J60_EIE_INTIE_BIT) | (1 << ENC28J60_EIE_PKTIE_BIT) | (1 << ENC28J60_EIE_TXIE_BIT) | (1 << ENC28J60_EIE_TXERIE_BIT)));
}

/*
    unsigned short enc28j60_lwip_read_phy_register (SPI_DEVICE_CONFIGURATION *spiConf,
                                                unsigned long registerAddress)

    This helper function reads from a PHY register given the address of
    the register to read from.

    RETURNS - The 16bit register value

    NOTES -
        To read from a PHY register:
        1. Write the address of the PHY register to read from into
            the MIREGADR register.
        2. Set the MICMD.MIIRD bit. The read operation begins and
            the MISTAT.BUSY bit is set.
        3. Wait 10.24 ?. Poll the MISTAT.BUSY bit to be certain
            that the operation is complete. While busy, the host
            controller should not start any MIISCAN operations or
            write to the MIWRH register. When the MAC has obtained
            the register contents, the BUSY bit will clear itself.
        4. Clear the MICMD.MIIRD bit.
        5. Read the desired data from the MIRDL and MIRDH registers.
            The order that these bytes are accessed is unimportant
*/
unsigned short enc28j60_lwip_read_phy_register(uint32_t spiHandle, uint8_t registerAddress)
{
    NATIVE_PROFILE_HAL_DRIVERS_ETHERNET();
    unsigned char byteData;
    unsigned short returnValue;
    uint32_t retries = 100;

    /* Select Bank 2 */
    enc28j60_lwip_select_bank(spiHandle, ENC28J60_CONTROL_REGISTER_BANK2);

    /* 1. */
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_MIREGADR, registerAddress);

    /* 2. */
    enc28j60_lwip_write_spi(
        spiHandle,
        ENC28J60_SPI_BIT_FIELD_SET_OPCODE,
        ENC28J60_MICMD,
        (uint8_t)(0x1 << ENC28J60_MICMD_MIIRD_BIT));

    /* 3. */
    /* Select Bank 3 */
    enc28j60_lwip_select_bank(spiHandle, ENC28J60_CONTROL_REGISTER_BANK3);

    do
    {
        enc28j60_lwip_read_spi(spiHandle, ENC28J60_SPI_READ_CONTROL_REGISTER_OPCODE, ENC28J60_MISTAT, &byteData, 1, 1);
        byteData &= (0x1 << ENC28J60_MISTAT_BUSY_BIT);
    } while (byteData && retries-- > 0);

    /* Select Bank 2 */
    enc28j60_lwip_select_bank(spiHandle, ENC28J60_CONTROL_REGISTER_BANK2);

    /* 4. */
    enc28j60_lwip_read_spi(spiHandle, ENC28J60_SPI_READ_CONTROL_REGISTER_OPCODE, ENC28J60_MICMD, &byteData, 1, 1);
    byteData = 0; //&= ~(0x1 << ENC28J60_MICMD_MIIRD_BIT);
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_MICMD, byteData);

    /* Select Bank 2 */
    enc28j60_lwip_select_bank(spiHandle, ENC28J60_CONTROL_REGISTER_BANK2);

    /* 5. */
    enc28j60_lwip_read_spi(spiHandle, ENC28J60_SPI_READ_CONTROL_REGISTER_OPCODE, ENC28J60_MIRDH, &byteData, 1, 1);
    returnValue = byteData << 8;

    enc28j60_lwip_read_spi(spiHandle, ENC28J60_SPI_READ_CONTROL_REGISTER_OPCODE, ENC28J60_MIRDL, &byteData, 1, 1);
    returnValue |= byteData;

    return (returnValue);
}

/*  void enc28j60_lwip_write_phy_register(unsigned long registerAddress,
                                     unsigned short data)

    This function write the parameter data to the register given
    by the registerAddress parameter.

    RETURNS - nothing

    NOTES -
    1. Write the address of the PHY register to write to
        into the MIREGADR register.
    2. Write the lower 8 bits of data to write into the
        MIWRL register.
    3. Write the upper 8 bits of data to write into the
        MIWRH register. Writing to this register automatically
        begins the MII transaction, so it must
        be written to after MIWRL. The MISTAT.BUSY
        bit becomes set.
*/
void enc28j60_lwip_write_phy_register(uint32_t spiHandle, uint8_t registerAddress, unsigned short data)
{
    NATIVE_PROFILE_HAL_DRIVERS_ETHERNET();
    unsigned char byteData;

    /* All the registers for this purpose are on bank 2 */
    enc28j60_lwip_select_bank(spiHandle, ENC28J60_CONTROL_REGISTER_BANK2);

    /* 1. */
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_MIREGADR, registerAddress);
    byteData = data & 0xFF;

    /* 2. */
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_MIWRL, byteData);
    byteData = (data >> 8) & 0xFF;

    /* 3. */
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_MIWRH, byteData);
}

/*
    void enc28j60_lwip_write_spi(PENC28J60_SOFTC sc,
                            RTP_uint8_t opcode,
                            RTP_uint8_t address,
                            RTP_Puint8_t &byteData,
                            RTP_uint32_t numBytes);

    This function writes the SPI interface using the configuration parameter
    within the sc structure.  The op code parameter is used to specify which
    control register to access.  This controller has the following seven control
    register commands
        1. read control register
        2. read buffer memory
        3. write control register
        4. write buffer memory
        5. bit field set
        6. bit field clear
        7. system command (soft reset)
    The op code field selects one of the above commands.  Within each control register
    command, there are a number of registers to be accessed.  The address of a register
    is specified using the address parameter.  The data pointed by byteData field
    is written to the register
 */
void enc28j60_lwip_write_spi(uint32_t spiHandle, uint8_t opcode, uint8_t address, uint8_t byteData)
{
    uint8_t opcodeArg;
    uint8_t commandWithData[2];

    opcodeArg = ENC28J60_SPI_OPCODE_ARGUMENT(opcode, address);

    /* Combine the command and the data */
    commandWithData[0] = opcodeArg;
    commandWithData[1] = byteData;

    // CPU_SPI_nWrite8_nRead8 (spiHandle, false, commandWithData, 2, 0, 0, 0);
    SPI_WRITE_READ_SETTINGS wrc{false, 0, false, 0, ENC28J60_CS, ENC28J60_CS_ACTIVE};
    nanoSPI_Write_Read(spiHandle, wrc, commandWithData, 2, 0, 0);
}

/* void enc28j60_lwip_soft_reset(SPI_DEVICE_CONFIGURATION *spiConf)

    A function to perform a soft reset on the Ethernet board
 */
void enc28j60_lwip_soft_reset(uint32_t spiHandle)
{
    NATIVE_PROFILE_HAL_DRIVERS_ETHERNET();
    uint8_t byteData;
    uint16_t shortData;
    int timeout = 1000; // 1 second

    /* First turn off RX/TX */
    enc28j60_lwip_select_bank(spiHandle, ENC28J60_CONTROL_REGISTER_BANK0);

    enc28j60_lwip_write_spi(
        spiHandle,
        ENC28J60_SPI_BIT_FIELD_CLEAR_OPCODE,
        ENC28J60_ECON1,
        (uint8_t)(1 << ENC28J60_ECON1_RXEN_BIT));

    /* Wait for RX to flush */
    do
    {
        OS_DELAY(1);

        enc28j60_lwip_read_spi(spiHandle, ENC28J60_SPI_READ_CONTROL_REGISTER_OPCODE, ENC28J60_ESTAT, &byteData, 1, 0);
    } while (timeout-- && (byteData & (1 << ENC28J60_ESTAT_RXBUSY)) != 0);

    timeout = 1000;
    /* Wait for TX to flush */
    do
    {
        OS_DELAY(1);

        enc28j60_lwip_read_spi(spiHandle, ENC28J60_SPI_READ_CONTROL_REGISTER_OPCODE, ENC28J60_ECON1, &byteData, 1, 0);
    } while (timeout-- && (byteData & (1 << ENC28J60_ECON1_TXRTS_BIT)) != 0);

    /* make sure voltage regulator is normal */
    enc28j60_lwip_write_spi(
        spiHandle,
        ENC28J60_SPI_BIT_FIELD_CLEAR_OPCODE,
        ENC28J60_ECON2,
        (uint8_t)(1 << ENC28J60_ECON2_VRPS_BIT));

    /* Reset PHY */
    enc28j60_lwip_write_phy_register(spiHandle, ENC28J60_PHCON1, (uint16_t)(1ul << ENC28J60_PHCON1_PRST));

    timeout = 1000;
    do
    {
        OS_DELAY(1);

        shortData = enc28j60_lwip_read_phy_register(spiHandle, ENC28J60_PHCON1);
    } while (timeout-- && (shortData & (1ul << ENC28J60_PHCON1_PRST)) != 0);

    enc28j60_lwip_write_spi(
        spiHandle,
        ENC28J60_SPI_BIT_FIELD_CLEAR_OPCODE,
        ENC28J60_EIE,
        (uint8_t)((1 << ENC28J60_EIE_INTIE_BIT) | (1 << ENC28J60_EIE_PKTIE_BIT) | (1 << ENC28J60_EIE_TXIE_BIT) | (1 << ENC28J60_EIE_TXERIE_BIT)));

    /* Combine the command and the data */
    byteData = (ENC28J60_SPI_SYSTEM_COMMAND_SOFT_RESET_OPCODE << 5) | ENC28J60_SPI_SYSTEM_COMMAND_SOFT_RESET_ARGUMENT;

    SPI_WRITE_READ_SETTINGS wrc{false, 0, false, 0, ENC28J60_CS, ENC28J60_CS_ACTIVE};
    nanoSPI_Write_Read(spiHandle, wrc, (uint8_t *)&byteData, 1, 0, 0);

    // Errata : After reset wait for 100 ms
    OS_DELAY(100);
}

/*
    void enc28j60_lwip_read_spi(PENC28J60_SOFTC sc,
                            RTP_uint8_t opcode,
                            RTP_uint8_t address,
                            RTP_Puint8_t &byteData,
                            RTP_uint32_t numBytes)

    This function reads from the SPI interface using the configuration parameter
    within the sc structure.  The opcode parameter is used to specify which
    control register to access.  This controller has the following seven control
    register commands
        1. read control register
        2. read buffer memory
        3. write control register
        4. write buffer memory
        5. bit field set
        6. bit field clear
        7. system command (soft reset)
    The opcode field selects one of the above commands.  Within each control register
    command, there are a number of registers to be accessed.  The address of a register
    is specified using the address parameter.  Contents of the register are written into
    the byteData
 */

void enc28j60_lwip_read_spi(
    uint32_t spiHandle,
    uint8_t opcode,
    uint8_t address,
    uint8_t *byteData,
    uint32_t numBytes,
    uint8_t offset)
{
    uint8_t opcodeArg;

    opcodeArg = ENC28J60_SPI_OPCODE_ARGUMENT(opcode, address);

    /* Write the command and read*/
    //    CPU_SPI_nWrite8_nRead8 (spiHandle, false, &opcodeArg, 1, byteData, numBytes+offset, offset+1);
    // Note uses offset on read data
    SPI_WRITE_READ_SETTINGS wrc{false, offset, false, 0, ENC28J60_CS, ENC28J60_CS_ACTIVE};
    nanoSPI_Write_Read(spiHandle, wrc, &opcodeArg, 1, byteData, numBytes);
}

/*
    void enc28j60_lwip_select_bank(sc, unsigned short bankNumber)

    Given the bank number this function selects one of the
    four banks.
*/

void enc28j60_lwip_select_bank(uint32_t spiHandle, uint8_t bankNumber)
{
    NATIVE_PROFILE_HAL_DRIVERS_ETHERNET();
    uint8_t byteData = 0;

    bankNumber &= 0x3;

    enc28j60_lwip_read_spi(spiHandle, ENC28J60_SPI_READ_CONTROL_REGISTER_OPCODE, ENC28J60_ECON1, &byteData, 1, 0);
    byteData &= 0xFC;
    byteData |= bankNumber;
    enc28j60_lwip_write_spi(spiHandle, ENC28J60_SPI_WRITE_CONTROL_REGISTER_OPCODE, ENC28J60_ECON1, byteData);
}

bool enc28j60_get_link_status(uint32_t spiHandle)
{
    uint16_t phyStat;

    GLOBAL_LOCK();

    phyStat = enc28j60_lwip_read_phy_register(spiHandle, ENC28J60_PHSTAT2);

    GLOBAL_UNLOCK();

    // linkstatus bit
    return ((phyStat & (1ul << ENC28J60_PHSTAT2_LSTAT_BIT)) != 0);
}
