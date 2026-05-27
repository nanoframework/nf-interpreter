//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

// clang-format off

#ifndef NX_CHIBIOS_PHY_DRIVER_H
#define NX_CHIBIOS_PHY_DRIVER_H

#ifdef   __cplusplus
extern   "C" {
#endif
#include <stdint.h>


#define  ETH_PHY_STATUS_ERROR                 ((int32_t)-1)
#define  ETH_PHY_STATUS_OK                    ((int32_t) 0)

#define  ETH_PHY_STATUS_LINK_ERROR            ((int32_t) 0)
#define  ETH_PHY_STATUS_LINK_DOWN             ((int32_t) 1)
#define  ETH_PHY_STATUS_100MBITS_FULLDUPLEX   ((int32_t) 2)
#define  ETH_PHY_STATUS_100MBITS_HALFDUPLEX   ((int32_t) 3)
#define  ETH_PHY_STATUS_10MBITS_FULLDUPLEX    ((int32_t) 4)
#define  ETH_PHY_STATUS_10MBITS_HALFDUPLEX    ((int32_t) 5)
#define  ETH_PHY_STATUS_AUTONEGO_NOT_DONE     ((int32_t) 6)

typedef void * 	nx_eth_phy_handle_t;

int32_t nx_eth_phy_init(void);

int32_t nx_eth_phy_get_link_state(void);

int32_t nx_eth_phy_set_link_state(int32_t linkstate);

nx_eth_phy_handle_t nx_eth_phy_get_handle(void);

#ifdef   __cplusplus
}
#endif
#endif

// clang-format on
