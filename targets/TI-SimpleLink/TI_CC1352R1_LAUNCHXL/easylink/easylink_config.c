/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met
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

/***** Includes *****/
#include "EasyLink.h"
#include "Board.h"

/* TI Drivers */
#include <ti/drivers/rf/RF.h>
#include <Board.h>

#ifdef Board_SYSCONFIG_PREVIEW
#include <smartrf_settings/smartrf_settings.h>
#else
#include <smartrf_settings/smartrf_settings.h>
#include <smartrf_settings/smartrf_settings_predefined.h>
#endif

#if (defined Board_CC2650_LAUNCHXL) || (defined Board_CC26X2R1_LAUNCHXL)
#define RF_TX_POWER_TABLE NULL
#define RF_TX_POWER_TABLE_SIZE 0
#else
#define RF_TX_POWER_TABLE (RF_TxPowerTable_Entry *)RF_PROP_txPowerTable
#define RF_TX_POWER_TABLE_SIZE RF_PROP_TX_POWER_TABLE_SIZE
#endif//(defined Board_CC2650_LAUNCHXL) || (defined Board_CC26X2R1_LAUNCHXL)

EasyLink_RfSetting EasyLink_supportedPhys[] = {
#if defined(SUPPORT_PHY_CUSTOM)
    {.EasyLink_phyType = EasyLink_Phy_Custom,
     .RF_pProp = &RF_prop,
#if (defined(DeviceFamily_CC26X0R2) || defined(DeviceFamily_CC26X0))
     .RF_uCmdPropRadio.RF_pCmdPropRadioSetup = &RF_cmdPropRadioSetup,
#else
     .RF_uCmdPropRadio.RF_pCmdPropRadioDivSetup = &RF_cmdPropRadioDivSetup,
#endif // (defined(DeviceFamily_CC26X0R2) || defined(DeviceFamily_CC26X0))
     .RF_pCmdFs = &RF_cmdFs,
     .RF_pCmdPropTx = &RF_cmdPropTx,
     .RF_pCmdPropTxAdv = NULL,
     .RF_pCmdPropRxAdv = &RF_cmdPropRxAdv_preDef,
     .RF_pTxPowerTable = RF_TX_POWER_TABLE,
     .RF_txPowerTableSize = RF_TX_POWER_TABLE_SIZE
    },
#endif //defined(SUPPORT_PHY_CUSTOM)
#if defined(SUPPORT_PHY_50KBPS2GFSK)
    {.EasyLink_phyType = EasyLink_Phy_50kbps2gfsk,
     .RF_pProp = &RF_prop_fsk,
     .RF_uCmdPropRadio.RF_pCmdPropRadioDivSetup = &RF_cmdPropRadioDivSetup_fsk,
     .RF_pCmdFs = &RF_cmdFs_preDef,
     .RF_pCmdPropTx = &RF_cmdPropTx,
     .RF_pCmdPropTxAdv = NULL,
     .RF_pCmdPropRxAdv = &RF_cmdPropRxAdv_preDef,
     .RF_pTxPowerTable = RF_TX_POWER_TABLE,
     .RF_txPowerTableSize = RF_TX_POWER_TABLE_SIZE
    },
#endif //defined(SUPPORT_PHY_50KBPS2GFSK)
#if defined(SUPPORT_PHY_5KBPSSLLR)
    {.EasyLink_phyType = EasyLink_Phy_5kbpsSlLr,
     .RF_pProp = &RF_prop_sl_lr,
     .RF_uCmdPropRadio.RF_pCmdPropRadioDivSetup = &RF_cmdPropRadioDivSetup_sl_lr,
     .RF_pCmdFs = &RF_cmdFs_preDef,
     .RF_pCmdPropTx = &RF_cmdPropTx,
     .RF_pCmdPropTxAdv = NULL,
     .RF_pCmdPropRxAdv = &RF_cmdPropRxAdv_preDef,
     .RF_pTxPowerTable = RF_TX_POWER_TABLE,
     .RF_txPowerTableSize = RF_TX_POWER_TABLE_SIZE
    },
#endif //defined(SUPPORT_PHY_5KBPSSLLR)
#if defined(SUPPORT_PHY_625BPSLRM)
    {.EasyLink_phyType = EasyLink_Phy_625bpsLrm,
     .RF_pProp = &RF_prop_lrm,
     .RF_uCmdPropRadio.RF_pCmdPropRadioDivSetup = &RF_cmdPropRadioDivSetup_lrm,
     .RF_pCmdFs = &RF_cmdFs_preDef,
     .RF_pCmdPropTx = &RF_cmdPropTx,
     .RF_pCmdPropTxAdv = NULL,
     .RF_pCmdPropRxAdv = &RF_cmdPropRxAdv_preDef,
     .RF_pTxPowerTable = RF_TX_POWER_TABLE,
     .RF_txPowerTableSize = RF_TX_POWER_TABLE_SIZE
    },
#endif //defined(SUPPORT_PHY_625BPSLRM)
#if defined(SUPPORT_PHY_2_4_200KBPS2GFSK)
    {.EasyLink_phyType = EasyLink_Phy_2_4_200kbps2gfsk,
     .RF_pProp = &RF_prop_2_4G_fsk,
     .RF_uCmdPropRadio.RF_pCmdPropRadioSetup = &RF_cmdPropRadioSetup_2_4G_fsk,
     .RF_pCmdFs = &RF_cmdFs_preDef,
     .RF_pCmdPropTx = &RF_cmdPropTx,
     .RF_pCmdPropTxAdv = NULL,
     .RF_pCmdPropRxAdv = &RF_cmdPropRxAdv_preDef,
     .RF_pTxPowerTable = RF_TX_POWER_TABLE,
     .RF_txPowerTableSize = RF_TX_POWER_TABLE_SIZE
    },
#endif //defined(SUPPORT_PHY_2_4_200KBPS2GFSK)
#if defined(SUPPORT_PHY_2_4_100KBPS2GFSK)
    {.EasyLink_phyType = EasyLink_Phy_2_4_100kbps2gfsk,
     .RF_pProp = &RF_prop_2_4G_fsk,
     .RF_uCmdPropRadio.RF_pCmdPropRadioSetup = &RF_cmdPropRadioSetup_2_4G_fsk_100,
     .RF_pCmdFs = &RF_cmdFs_preDef,
     .RF_pCmdPropTx = &RF_cmdPropTx,
     .RF_pCmdPropTxAdv = NULL,
     .RF_pCmdPropRxAdv = &RF_cmdPropRxAdv_preDef,
     .RF_pTxPowerTable = RF_TX_POWER_TABLE,
     .RF_txPowerTableSize = RF_TX_POWER_TABLE_SIZE
    },
#endif //defined(SUPPORT_PHY_2_4_100KBPS2GFSK)
#if defined(SUPPORT_PHY_2_4_250KBPS2GFSK)
    {.EasyLink_phyType = EasyLink_Phy_2_4_250kbps2gfsk,
     .RF_pProp = &RF_prop_2_4G_fsk,
     .RF_uCmdPropRadio.RF_pCmdPropRadioSetup = &RF_cmdPropRadioSetup_2_4G_fsk,
     .RF_pCmdFs = &RF_cmdFs_preDef,
     .RF_pCmdPropTx = &RF_cmdPropTx,
     .RF_pCmdPropTxAdv = NULL,
     .RF_pCmdPropRxAdv = &RF_cmdPropRxAdv_preDef,
     .RF_pTxPowerTable = RF_TX_POWER_TABLE,
     .RF_txPowerTableSize = RF_TX_POWER_TABLE_SIZE
    },
#endif //defined(SUPPORT_PHY_2_4_250KBPS2GFSK)
#if defined(SUPPORT_PHY_200KBPS2GFSK)
    {.EasyLink_phyType = EasyLink_Phy_200kbps2gfsk,
     .RF_pProp = &RF_prop_fsk_200kbps,
     .RF_uCmdPropRadio.RF_pCmdPropRadioDivSetup = &RF_cmdPropRadioDivSetup_fsk_200kbps,
     .RF_pCmdFs = &RF_cmdFs_preDef,
     .RF_pCmdPropTx = NULL,
     .RF_pCmdPropTxAdv = &RF_cmdPropTxAdv_preDef,
     .RF_pCmdPropRxAdv = &RF_cmdPropRxAdv_preDef,
     .RF_pTxPowerTable = RF_TX_POWER_TABLE,
     .RF_txPowerTableSize = RF_TX_POWER_TABLE_SIZE
    },
#endif //defined(SUPPORT_PHY_50KBPS2GFSK)
};

const uint8_t EasyLink_numSupportedPhys = sizeof(EasyLink_supportedPhys)/sizeof(EasyLink_RfSetting);
