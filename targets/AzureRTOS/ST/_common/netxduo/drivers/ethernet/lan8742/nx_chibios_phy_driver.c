//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <nx_chibios_phy_driver.h>

/**
  * @brief  Initialize the PHY interface
  * @param  none
  * @retval ETH_PHY_STATUS_OK on success, ETH_PHY_STATUS_ERROR otherwise
  */

int32_t nx_eth_phy_init(void)
{
    // handled by ChibiOS HAL
    return ETH_PHY_STATUS_OK;
}

/**
  * @brief  get the Phy link status.
  * @param  none
  * @retval the link status.
  */

int32_t nx_eth_phy_get_link_state(void)
{
  // TODO NETWORK
    // int32_t  linkstate = LAN8742_GetLinkState(&LAN8742);

    // return linkstate;
    return 0;
}

// /**
//   * @brief  get the driver object handle
//   * @param  none
//   * @retval pointer to the LAN8742 main object
//   */

// nx_eth_phy_handle_t nx_eth_phy_get_handle(void)
// {
//   // TODO NETWORK 
//     // return (nx_eth_phy_handle_t)&LAN8742;
// }

/**
  * @brief  Initialize the PHY MDIO interface
  * @param  None
  * @retval 0 if OK, -1 if ERROR
  */

int32_t lan8742_io_init(void)
{
    // handled by ChibiOS HAL
    return ETH_PHY_STATUS_OK;
}

/**
  * @brief  De-Initialize the MDIO interface
  * @param  None
  * @retval 0 if OK, -1 if ERROR
  */
int32_t lan8742_io_deinit (void)
{
    // always succesfull
    return ETH_PHY_STATUS_OK;
}

// /**
//   * @brief  Read a PHY register through the MDIO interface.
//   * @param  DevAddr: PHY port address
//   * @param  RegAddr: PHY register address
//   * @param  pRegVal: pointer to hold the register value
//   * @retval 0 if OK -1 if Error
//   */
// int32_t lan8742_io_read_reg(uint32_t DevAddr, uint32_t RegAddr, uint32_t *pRegVal)
// {
//   if(HAL_ETH_ReadPHYRegister(&eth_handle, DevAddr, RegAddr, pRegVal) != HAL_OK)
//   {
//     return ETH_PHY_STATUS_ERROR;
//   }

//   return ETH_PHY_STATUS_OK;
// }

// int32_t lan8742_io_write_reg(uint32_t DevAddr, uint32_t RegAddr, uint32_t RegVal)
// {
//   if(HAL_ETH_WritePHYRegister(&eth_handle, DevAddr, RegAddr, RegVal) != HAL_OK)
//   {
//     return ETH_PHY_STATUS_ERROR;
//   }

//   return ETH_PHY_STATUS_OK;
// }

// /**
//   * @brief  Get the time in millisecons used for internal PHY driver process.
//   * @retval Time value
//   */
// int32_t lan8742_io_get_tick(void)
// {
//   return HAL_GetTick();
// }

// clang-format on
