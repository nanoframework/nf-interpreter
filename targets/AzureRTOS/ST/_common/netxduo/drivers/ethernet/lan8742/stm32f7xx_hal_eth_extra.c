//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2017 STMicroelectronics. All rights reserved.
// See LICENSE file in the project root for full license information.
//

// This was taken from the Azure RTOS Cube MX package.
// Seems that this code hasn't been included in the SMT32F7 HAL package. Waiting for version >=1.3.0

#include "stm32f7xx_hal.h"

/**
  * @brief  Configures the Clock range of ETH MDIO interface.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval None
  */
void HAL_ETH_SetMDIOClockRange(ETH_HandleTypeDef *heth)
{
  uint32_t tmpreg, hclk;

  /* Get the ETHERNET MACMIIAR value */
  tmpreg = (heth->Instance)->MACMIIAR;
  /* Clear CSR Clock Range CR[2:0] bits */
  tmpreg &= ETH_MACMIIAR_CR_MASK;

  /* Get hclk frequency value */
  hclk = HAL_RCC_GetHCLKFreq();

  /* Set CR bits depending on hclk value */
  if((hclk >= 20000000U)&&(hclk < 35000000U))
  {
    /* CSR Clock Range between 20-35 MHz */
    tmpreg |= (uint32_t)ETH_MACMIIAR_CR_Div16;
  }
  else if((hclk >= 35000000U)&&(hclk < 60000000U))
  {
    /* CSR Clock Range between 35-60 MHz */
    tmpreg |= (uint32_t)ETH_MACMIIAR_CR_Div26;
  }
  else if((hclk >= 60000000U)&&(hclk < 100000000U))
  {
    /* CSR Clock Range between 60-100 MHz */
    tmpreg |= (uint32_t)ETH_MACMIIAR_CR_Div42;
  }
  else if((hclk >= 100000000U)&&(hclk < 150000000U))
  {
    /* CSR Clock Range between 100-150 MHz */
    tmpreg |= (uint32_t)ETH_MACMIIAR_CR_Div62;
  }
  else /* ((hclk >= 150000000)&&(hclk <= 183000000)) */
  {
    /* CSR Clock Range between 150-183 MHz */
    tmpreg |= (uint32_t)ETH_MACMIIAR_CR_Div102;
  }

  /* Write to ETHERNET MAC MIIAR: Configure the ETHERNET CSR Clock Range */
  (heth->Instance)->MACMIIAR = (uint32_t)tmpreg;
}

/**
  * @brief  Read a PHY register
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  PHYAddr: PHY port address, must be a value from 0 to 31
  * @param  PHYReg: PHY register address, must be a value from 0 to 31
  * @param pRegValue: parameter to hold read value
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETH_ReadPHYRegister(ETH_HandleTypeDef *heth, uint32_t PHYAddr, uint32_t PHYReg, uint32_t *pRegValue)
{
  uint32_t tmpreg1 = 0U;
  uint32_t tickstart = 0U;
  
  /* Check parameters */
  assert_param(IS_ETH_PHY_ADDRESS(PHYAddr));
  
  /* Check the ETH peripheral state */
  if(heth->State == HAL_ETH_STATE_BUSY_RD)
  {
    return HAL_BUSY;
  }
  /* Set ETH HAL State to BUSY_RD */
  heth->State = HAL_ETH_STATE_BUSY_RD;
  
  /* Get the ETHERNET MACMIIAR value */
  tmpreg1 = heth->Instance->MACMIIAR;
  
  /* Keep only the CSR Clock Range CR[2:0] bits value */
  tmpreg1 &= ~ETH_MACMIIAR_CR_MASK;
  
  /* Prepare the MII address register value */
  tmpreg1 |=((PHYAddr << 11U) & ETH_MACMIIAR_PA);                         /* Set the PHY device address   */
  tmpreg1 |=(((uint32_t)PHYReg<<6U) & ETH_MACMIIAR_MR);                   /* Set the PHY register address */
  tmpreg1 &= ~ETH_MACMIIAR_MW;                                            /* Set the read mode            */
  tmpreg1 |= ETH_MACMIIAR_MB;                                             /* Set the MII Busy bit         */
  
  /* Write the result value into the MII Address register */
  heth->Instance->MACMIIAR = tmpreg1;
  
  /* Get tick */
  tickstart = HAL_GetTick();
  
  /* Check for the Busy flag */
  while((tmpreg1 & ETH_MACMIIAR_MB) == ETH_MACMIIAR_MB)
  {
    /* Check for the Timeout */
    if((HAL_GetTick() - tickstart ) > PHY_READ_TO)
    {
      heth->State= HAL_ETH_STATE_READY;
  
      /* Process Unlocked */
      __HAL_UNLOCK(heth);
    
      return HAL_TIMEOUT;
    }
    
    tmpreg1 = heth->Instance->MACMIIAR;
  }
  
  /* Get MACMIIDR value */
  *pRegValue = (uint16_t)(heth->Instance->MACMIIDR);
  
  /* Set ETH HAL State to READY */
  heth->State = HAL_ETH_STATE_READY;
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Writes to a PHY register.
  * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  PHYAddr: PHY port address, must be a value from 0 to 31
  * @param  PHYReg: PHY register address, must be a value from 0 to 31
  * @param  RegValue: the value to write
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETH_WritePHYRegister(ETH_HandleTypeDef *heth, uint32_t PHYAddr, uint32_t PHYReg, uint32_t RegValue)
{
  uint32_t tmpreg1 = 0U;
  uint32_t tickstart = 0U;
  
  /* Check parameters */
  assert_param(IS_ETH_PHY_ADDRESS(PHYAddr));
  
  /* Check the ETH peripheral state */
  if(heth->State == HAL_ETH_STATE_BUSY_WR)
  {
    return HAL_BUSY;
  }
  /* Set ETH HAL State to BUSY_WR */
  heth->State = HAL_ETH_STATE_BUSY_WR;
  
  /* Get the ETHERNET MACMIIAR value */
  tmpreg1 = heth->Instance->MACMIIAR;
  
  /* Keep only the CSR Clock Range CR[2:0] bits value */
  tmpreg1 &= ~ETH_MACMIIAR_CR_MASK;
  
  /* Prepare the MII register address value */
  tmpreg1 |=((PHYAddr << 11U) & ETH_MACMIIAR_PA);                       /* Set the PHY device address */
  tmpreg1 |=(((uint32_t)PHYReg<<6U) & ETH_MACMIIAR_MR);                 /* Set the PHY register address */
  tmpreg1 |= ETH_MACMIIAR_MW;                                           /* Set the write mode */
  tmpreg1 |= ETH_MACMIIAR_MB;                                           /* Set the MII Busy bit */
  
  /* Give the value to the MII data register */
  heth->Instance->MACMIIDR = (uint16_t)RegValue;
  
  /* Write the result value into the MII Address register */
  heth->Instance->MACMIIAR = tmpreg1;
  
  /* Get tick */
  tickstart = HAL_GetTick();
  
  /* Check for the Busy flag */
  while((tmpreg1 & ETH_MACMIIAR_MB) == ETH_MACMIIAR_MB)
  {
    /* Check for the Timeout */
    if((HAL_GetTick() - tickstart ) > PHY_WRITE_TO)
    {
      heth->State= HAL_ETH_STATE_READY;
  
      /* Process Unlocked */
      __HAL_UNLOCK(heth);
    
      return HAL_TIMEOUT;
    }
    
    tmpreg1 = heth->Instance->MACMIIAR;
  }
  
  /* Set ETH HAL State to READY */
  heth->State = HAL_ETH_STATE_READY;
  
  /* Return function status */
  return HAL_OK; 
}
