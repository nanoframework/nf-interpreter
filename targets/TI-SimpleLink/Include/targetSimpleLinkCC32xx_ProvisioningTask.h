//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Texas Instruments Incorporated.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _SIMPLELINK_CC32XX_PROVISIONING_TASK_H_
#define _SIMPLELINK_CC32XX_PROVISIONING_TASK_H_

#include <ti/drivers/net/wifi/simplelink.h>

/* POSIX Header files */
#include <semaphore.h>
#include <pthread.h>
#include <time.h>

#include <stdlib.h>
// #include <string.h>
// #include <stdint.h>

/*!
 *  \brief  Provisioning events
 */
typedef enum
{
    PrvnEvent_Triggered,
    PrvnEvent_Started,
    PrvnEvent_StartFailed,
    PrvnEvent_ConfirmationSuccess,
    PrvnEvent_ConfirmationFailed,
    PrvnEvent_Stopped,
    PrvnEvent_WaitForConn,
    PrvnEvent_Timeout,
    PrvnEvent_Error,
    PrvnEvent_Max,
}PrvnEvent;

/*!
 *  \brief  Provisioning states
 */
typedef enum
{
    PrvnState_Init,
    PrvnState_Idle,
    PrvnState_WaitForConfirmation,
    PrvnState_Completed,
    PrvnState_Error,
    PrvnState_Max
}PrvnState;

typedef struct Provisioning_ControlBlock_t
{
    sem_t connectionAsyncEvent;
    sem_t provisioningDoneSignal;
    sem_t provisioningConnDoneToOtaServerSignal;
}Provisioning_CB;


/****************************************************************************
                      GLOBAL VARIABLES
****************************************************************************/
extern Provisioning_CB Provisioning_ControlBlock;

//****************************************************************************
//                      FUNCTION PROTOTYPES
//****************************************************************************

//*****************************************************************************
//
//! \brief This function signals the application events
//!
//! \param[in]  None
//!
//! \return 0 on success, negative value otherwise
//!
//****************************************************************************
_i16 SignalProvisioningEvent(PrvnEvent event);

//*****************************************************************************
//
//! \brief This function gets the current provisioning state
//!
//! \param[in]  None
//!
//! \return provisioning state
//!
//****************************************************************************
PrvnState GetProvisioningState();

//*****************************************************************************
//
//! \brief The interrupt handler for the LED timer
//!
//! \param[in]  None
//!
//! \return None
//!
//****************************************************************************
void LedTimerIntHandler(sigval val);

//*****************************************************************************
//
//! \brief This function starts the led toggling timer
//!
//! \param[in]  None
//!
//! \return 0 on success, negative value otherwise
//!
//****************************************************************************
int32_t StartLedEvtTimer(uint32_t timeout);

//*****************************************************************************
//
//! \brief This function stops the led toggling timer
//!
//! \param[in]  None
//!
//! \return 0 on success, negative value otherwise
//!
//****************************************************************************
int32_t StopLedEvtTimer(void);

//*****************************************************************************
//
//! \brief This function stops provisioning process
//!
//! \param[in]  None
//!
//! \return SL_RET_CODE_PROVISIONING_IN_PROGRESS if provisioning was running, otherwise 0
//!
//****************************************************************************
int32_t provisioningStop(void);

//*****************************************************************************
//
//! \brief This is the main provisioning task
//!
//! \param[in]  None
//!
//! \return None
//!
//****************************************************************************
void * provisioningTask(void *pvParameters);

#endif //_SIMPLELINK_CC32XX_PROVISIONING_TASK_H_
