//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

//
// wiznet5500.h - device driver interface
//

#ifndef WIZNET5500_H
#define WIZNET5500_H

#include <nanohal.h>
#include <wiznet5500_lwip_driver.h>

bool wiznet5500_lwip_open(struct netif *pNetIF);
void wiznet5500_lwip_close(struct netif *pNetIF);
err_t wiznet5500_lwip_xmit(struct netif *pNetIF, struct pbuf *pPBuf);
int wiznet5500_lwip_recv(struct netif *pNetIF);
void wiznet5500_lwip_interrupt(struct netif *pNetIF);
void wiznet5500_lwip_pre_interrupt(GPIO_PIN Pin, bool PinState, void *pArg);
void wiznet5500_lwip_setup_recv_buffer(struct netif *pNetIF, uint32_t spiHandle);

/*          FUNCTION PROTOTYPE                                          */

/*          STRUCTURES                                                  */

#define ETHERSIZE                                                                                                      \
    ((WIZNET5500_TRANSMIT_BUFFER_END - WIZNET5500_TRANSMIT_BUFFER_START) >>                                                \
     1) /* maximum number of bytes in ETHERNET packet */
        /* (used by ethernet drivers)   */

#define ETHER_MIN_LEN 64 /* minimum number of bytes in an ETHERNET */
                         /* packet   */

/*              WIZNET5500 CONFIGURATION                                  */
#define CFG_NUM_WIZNET5500 NETWORK_INTERFACE_COUNT
#define CFG_MAX_PACKETS_PROCESSED                                                                                      \
    10 /* The maximum number of packets to process in one                                                              \
           shot */

#define WIZNET5500_FULL_DUPLEX        0    /* DON'T CHANGE adds support for full duplex */
#define WIZNET5500_MAXIMUM_FRAME_SIZE 1530 /* maximum frame sizes to be transmitted */

#define WIZNET5500_MAC_ADDRESS0 0xBA /* Configures the MAC address to */
#define WIZNET5500_MAC_ADDRESS1 0xAD /* these values */
#define WIZNET5500_MAC_ADDRESS2 0xCA
#define WIZNET5500_MAC_ADDRESS3 0xAB
#define WIZNET5500_MAC_ADDRESS4 0x04
#define WIZNET5500_MAC_ADDRESS5 0x05

#define WIZNET5500_PHYID1 0x0083 /* Unique device id */
#define WIZNET5500_PHYID2 0x1400 /* Unique device id */

/*              BUFFER ALLOCATION SPACES                                */

#define WIZNET5500_RECEIVE_BUFFER_START  0x0000
#define WIZNET5500_RECEIVE_BUFFER_END    0x0FFF
#define WIZNET5500_TRANSMIT_BUFFER_START 0x1000
#define WIZNET5500_TRANSMIT_BUFFER_END   0x1FFF

/*              WIZNET5500 control register banks                         */
#define WIZNET5500_CONTROL_REGISTER_BANK0 0
#define WIZNET5500_CONTROL_REGISTER_BANK1 1
#define WIZNET5500_CONTROL_REGISTER_BANK2 2
#define WIZNET5500_CONTROL_REGISTER_BANK3 3

/*              WIZNET5500 control register map                           */

// #define WIZNET5500_ERDPTL 0x00

// #define WIZNET5500_EHT0 0x00

// #define WIZNET5500_MACON1             0x00
// #define WIZNET5500_MACON1_LOOPBK_BIT  4
// #define WIZNET5500_MACON1_TXPAUS_BIT  3
// #define WIZNET5500_MACON1_RXPAUS_BIT  2
// #define WIZNET5500_MACON1_PASSALL_BIT 1
// #define WIZNET5500_MACON1_MARXEN_BIT  0

// #define WIZNET5500_MAADR2 0x05

// #define WIZNET5500_ERDPTH 0x01

// #define WIZNET5500_EHT1 0x01

// #define WIZNET5500_MACON2             0x01
// #define WIZNET5500_MACON2_MARST_BIT   7
// #define WIZNET5500_MACON2_RNDRST_BIT  6
// #define WIZNET5500_MACON2_MARXRST_BIT 3
// #define WIZNET5500_MACON2_RFUNRST_BIT 2
// #define WIZNET5500_MACON2_MATXRST_BIT 1
// #define WIZNET5500_MACON2_TFUNRST_BIT 0

// #define WIZNET5500_MAADR1 0x04

// #define WIZNET5500_EWRPTL 0x02

// #define WIZNET5500_EHT2 0x02

// #define WIZNET5500_MACON3             0x02
// #define WIZNET5500_MACON3_PADCFG2_BIT 7
// #define WIZNET5500_MACON3_PADCFG1_BIT 6
// #define WIZNET5500_MACON3_PADCFG0_BIT 5
// #define WIZNET5500_MACON3_TXCRCEN_BIT 4
// #define WIZNET5500_MACON3_PHDRLEN_BIT 3
// #define WIZNET5500_MACON3_HFRMEN_BIT  2
// #define WIZNET5500_MACON3_FRMLNEN_BIT 1
// #define WIZNET5500_MACON3_FULDPX_BIT  0

// #define WIZNET5500_MAADR4 0x03

// #define WIZNET5500_EWRPTH 0x03

// #define WIZNET5500_EHT3 0x03

// #define WIZNET5500_MACON4             0x03
// #define WIZNET5500_MACON4_DEFER_BIT   6
// #define WIZNET5500_MACON4_BPEN_BIT    5
// #define WIZNET5500_MACON4_NOBKOFF_BIT 4
// #define WIZNET5500_MACON4_LONGPRE_BIT 1
// #define WIZNET5500_MACON4_PUREPRE_BIT 0

// #define WIZNET5500_MAADR3 0x02

// #define WIZNET5500_ETXSTL 0x04

// #define WIZNET5500_EHT4 0x04

// #define WIZNET5500_MABBIPG 0x04

// #define WIZNET5500_MAADR6 0x01

// #define WIZNET5500_ETXSTH 0x05

// #define WIZNET5500_EHT5 0x05

// #define WIZNET5500_MAADR5 0x00

// #define WIZNET5500_ETXNDL 0x06

// #define WIZNET5500_EHT6 0x06

// #define WIZNET5500_MAIPGL 0x06

// #define WIZNET5500_EBSTSD 0x06

// #define WIZNET5500_ETXNDH 0x07

// #define WIZNET5500_EHT7 0x07

// #define WIZNET5500_MAIPGH 0x07

// #define WIZNET5500_EBSTCON 0x07

// #define WIZNET5500_ERXSTL 0x08

// #define WIZNET5500_EPMM0 0x08

// #define WIZNET5500_MACLCON1 0x08

// #define WIZNET5500_EBSTCSL 0x08

// #define WIZNET5500_ERXSTH 0x09

// #define WIZNET5500_EPMM1 0x09

// #define WIZNET5500_MACLCON2 0x09

// #define WIZNET5500_EBSTCSH 0x09

// #define WIZNET5500_ERXNDL 0x0A

// #define WIZNET5500_EPMM2 0x0A

// #define WIZNET5500_MAMXFLL 0x0A

// #define WIZNET5500_MISTAT            0x0A
// #define WIZNET5500_MISTAT_NVALID_BIT 2
// #define WIZNET5500_MISTAT_SCAN_BIT   1
// #define WIZNET5500_MISTAT_BUSY_BIT   0

// #define WIZNET5500_ERXNDH 0x0B

// #define WIZNET5500_EPMM3 0x0B

// #define WIZNET5500_MAMXFLH 0x0B

// #define WIZNET5500_ERXRDPTL 0x0C

// #define WIZNET5500_EPMM4 0x0C

// #define WIZNET5500_ERXRDPTH 0x0D

// #define WIZNET5500_EPMM5 0x0D

// #define WIZNET5500_MAPHSUP 0x0D

// #define WIZNET5500_ERXWRPTL 0x0E

// #define WIZNET5500_EPMM6 0x0E

// #define WIZNET5500_ERXWRPTH 0x0F

// #define WIZNET5500_EPMM7 0x0F

// #define WIZNET5500_EDMASTL 0x10

// #define WIZNET5500_EPMCSL 0x10

// #define WIZNET5500_EDMASTH 0x11

// #define WIZNET5500_EPMCSH 0x11

// #define WIZNET5500_MICON 0x11

// #define WIZNET5500_EDMANDL 0x12

// #define WIZNET5500_MICMD            0x12
// #define WIZNET5500_MICMD_MISCAN_BIT 1
// #define WIZNET5500_MICMD_MIIRD_BIT  0

// #define WIZNET5500_EREVID 0x12

// #define WIZNET5500_EDMANDH 0x13

// #define WIZNET5500_EDMADSTL 0x14

// #define WIZNET5500_EPMOL 0x14

// #define WIZNET5500_MIREGADR 0x14

// #define WIZNET5500_EDMADSTH 0x15

// #define WIZNET5500_EPMOH 0x15

// #define WIZNET5500_ECOCON 0x15

// #define WIZNET5500_EDMACSL 0x16

// #define WIZNET5500_EWOLIE 0x16

// #define WIZNET5500_MIWRL 0x16

// #define WIZNET5500_EDMACSH 0x17

// #define WIZNET5500_EWOLIR 0x17

// #define WIZNET5500_MIWRH 0x17

// #define WIZNET5500_EFLOCON 0x17

// #define WIZNET5500_ERXFCON           0x18
// #define WIZNET5500_ERXFCON_UCEN_BIT  7
// #define WIZNET5500_ERXFCON_ANDOR_BIT 6
// #define WIZNET5500_ERXFCON_CRCEN_BIT 5
// #define WIZNET5500_ERXFCON_PMEN_BIT  4
// #define WIZNET5500_ERXFCON_MPEN_BIT  3
// #define WIZNET5500_ERXFCON_HTEN_BIT  2
// #define WIZNET5500_ERXFCON_MCEN_BIT  1
// #define WIZNET5500_ERXFCON_BCEN_BIT  0

// #define WIZNET5500_MIRDL 0x18

// #define WIZNET5500_EPAUSL 0x18

// #define WIZNET5500_EPKTCNT 0x19

// #define WIZNET5500_MIRDH 0x19

// #define WIZNET5500_EPAUSH 0x19

// #define WIZNET5500_EIE            0x1B
// #define WIZNET5500_EIE_INTIE_BIT  7
// #define WIZNET5500_EIE_PKTIE_BIT  6
// #define WIZNET5500_EIE_DMAIE_BIT  5
// #define WIZNET5500_EIE_LINKIE_BIT 4
// #define WIZNET5500_EIE_TXIE_BIT   3
// #define WIZNET5500_EIE_WOLIE_BIT  2
// #define WIZNET5500_EIE_TXERIE_BIT 1
// #define WIZNET5500_EIE_RXERIE_BIT 0

// #define WIZNET5500_EIR            0x1C
// #define WIZNET5500_EIR_PKTIF_BIT  6
// #define WIZNET5500_EIR_DMAIF_BIT  5
// #define WIZNET5500_EIR_LINKIF_BIT 4
// #define WIZNET5500_EIR_TXIF_BIT   3
// #define WIZNET5500_EIR_WOLIF_BIT  2
// #define WIZNET5500_EIR_TXERIF_BIT 1
// #define WIZNET5500_EIR_RXERIF_BIT 0

// #define WIZNET5500_ESTAT         0x1D
// #define WIZNET5500_ESTAT_INT     7
// #define WIZNET5500_ESTAT_BUFER   6
// #define WIZNET5500_ESTAT_LATECOL 4
// #define WIZNET5500_ESTAT_RXBUSY  2
// #define WIZNET5500_ESTAT_TXABRT  1
// #define WIZNET5500_ESTAT_CLKRDY  0

// #define WIZNET5500_ECON2             0x1E
// #define WIZNET5500_ECON2_AUTOINC_BIT 7
// #define WIZNET5500_ECON2_PKTDEC_BIT  6
// #define WIZNET5500_ECON2_PWRSV_BIT   5
// #define WIZNET5500_ECON2_VRPS_BIT    4

// #define WIZNET5500_ECON1            0x1F
// #define WIZNET5500_ECON1_TXRST_BIT  7
// #define WIZNET5500_ECON1_RXRST_BIT  6
// #define WIZNET5500_ECON1_DMAST_BIT  5
// #define WIZNET5500_ECON1_CSUMEN_BIT 4
// #define WIZNET5500_ECON1_TXRTS_BIT  3
// #define WIZNET5500_ECON1_RXEN_BIT   2
// #define WIZNET5500_ECON1_BSEL1_BIT  1
// #define WIZNET5500_ECON1_BSEL0_BIT  0

// #define WIZNET5500_PHCON1         0x0
// #define WIZNET5500_PHCON1_PRST    15
// #define WIZNET5500_PHCON1_PLOOPBK 14
// #define WIZNET5500_PHCON1_PPWRSV  11
// #define WIZNET5500_PHCON1_PDPXMD  8

// #define WIZNET5500_PHSTAT1 0x1

// #define WIZNET5500_PHID1 0x2

// #define WIZNET5500_PHID2 0x3

// #define WIZNET5500_PHCON2        0x10
// #define WIZNET5500_PHCON2_FRCLNK 14
// #define WIZNET5500_PHCON2_TXDIS  13
// #define WIZNET5500_PHCON2_JABBER 10
// #define WIZNET5500_PHCON2_HDLDIS 8

// #define WIZNET5500_PHSTAT2           0x11
// #define WIZNET5500_PHSTAT2_LSTAT_BIT 10

// #define WIZNET5500_PHIE   0x12
// #define WIZNET5500_PHIR   0x13
// #define WIZNET5500_PHLCON 0x14
// /*                  SPI Instruction set                                 */
// #define WIZNET5500_SPI_READ_CONTROL_REGISTER_OPCODE 0x0

// #define WIZNET5500_SPI_READ_BUFFER_MEMORY_OPCODE   0x1
// #define WIZNET5500_SPI_READ_BUFFER_MEMORY_ARGUMENT 0x1A

// #define WIZNET5500_SPI_WRITE_CONTROL_REGISTER_OPCODE 0x2

// #define WIZNET5500_SPI_WRITE_BUFFER_MEMORY_OPCODE   0x3
// #define WIZNET5500_SPI_WRITE_BUFFER_MEMORY_ARGUMENT 0x1A

// #define WIZNET5500_SPI_BIT_FIELD_SET_OPCODE 0x4

// #define WIZNET5500_SPI_BIT_FIELD_CLEAR_OPCODE 0x5

// #define WIZNET5500_SPI_SYSTEM_COMMAND_SOFT_RESET_OPCODE   0x7
// #define WIZNET5500_SPI_SYSTEM_COMMAND_SOFT_RESET_ARGUMENT 0x1F

// #define WIZNET5500_SPI_OPCODE_ARGUMENT(OP, ARG) (OP << 5) | (ARG & 0x1F)

// /*                  XMIT format for per packet control                  */

// #define WIZNET5500_XMIT_CONTROL_PHUGEEN_BIT   3
// #define WIZNET5500_XMIT_CONTROL_PPADEN_BIT    2
// #define WIZNET5500_XMIT_CONTROL_PCRCEN_BIT    1
// #define WIZNET5500_XMIT_CONTROL_POVERRIDE_BIT 0

#endif // WIZNET5500_H
