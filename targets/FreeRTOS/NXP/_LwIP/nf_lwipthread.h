//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef LWIPTHREAD_H
#define LWIPTHREAD_H

#include <stdint.h>
#include "lwip/opt.h"

/**
 *  @brief   Startup network assigning modes.
 */
typedef enum {
#if LWIP_DHCP || defined(__DOXYGEN__)
    /** 
     * @brief   Assign a DHCP given address.
     */
    NET_ADDRESS_DHCP = 1,
#endif
    /**
     * @brief   Assign a statically IPv4 address. 
     */
    NET_ADDRESS_STATIC = 2,
#if LWIP_AUTOIP  || defined(__DOXYGEN__)
    /**
     * @brief   Assign an IPv4 link-Local address.
     */
    NET_ADDRESS_AUTO = 3
#endif
} net_addr_mode_t;

/**
 * @brief   Runtime TCP/IP settings.
 */
typedef struct lwipthread_opts {
  /**
   * @brief   Pointer to MAC address as an array of 6 unsigned bytes.
   */
  uint8_t         *macaddress;
  /**
   * @brief   Network address as 32-bit unsigned integer.
   */
  uint32_t        address;
  /**
   * @brief   Network subnet mask as 32-bit unsigned integer.
   */
  uint32_t        netmask;
  /**
   * @brief   Network gateway as 32-bit unsigned integer.
   */
  uint32_t        gateway;
  /**
   * @brief   Startup network addressing mode - static, DHCP, auto.
   */
  net_addr_mode_t addrMode;
  /**
   * @brief   Host name. If NULL, a default string is used.
   * @note    Not checked for validity. In particular, spaces not allowed.
   */
#if LWIP_NETIF_HOSTNAME || defined(__DOXYGEN__)
  const char              *ourHostName;
#endif
} lwipthread_opts_t;

#ifdef __cplusplus
extern "C" {
#endif
  void lwipInit(const lwipthread_opts_t *opts);
#ifdef __cplusplus
}
#endif

#endif /* LWIPTHREAD_H */