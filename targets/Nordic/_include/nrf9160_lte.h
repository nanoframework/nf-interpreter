/*
 * Copyright (c) Ericsson AB 2020, all rights reserved
 */
#ifndef NRF9610_LTE_H__
#define NRF9610_LTE_H__

// Include basic operating system dependencies

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CONFIG_NET_SOCKETS_SOCKOPT_TLS)
#include <modem/modem_key_mgmt.h>
#define TLS_SEC_TAG 42
#endif

// Default connection timeout
#define NETWORK_TIMEOUT 120
#define IMxI_LEN        16
#define ICCID_LEN       22
#define IP_ADDR_LEN     17
#define FW_VER_LEN      32
#define APN_LEN         64

#define DEFAULT_TIMEOUT -1

// Connect State values
#define CONN_STATE_DISCONNECTED   0
#define CONN_STATE_CONN_HOME      1
#define CONN_STATE_CONN_ROAM      2

// Initialize the LTE library
int nrf9160_lte_init(uint16_t timeout);

// Connect to the network.
int nrf9160_lte_connect(uint16_t timeout);

// Disconnect to the network.
int nrf9160_lte_offline(void);

// Turn off the modem.
int nrf9160_lte_power_off(void);


#ifdef __cplusplus
}
#endif

#endif /* NRF9610_LTE_H__ */

