//*****************************************************************************
// osi_chibios.c
//
// Interface APIs for ChibiOS function calls
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
// 
// 
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions 
//  are met:
//
//    Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the 
//    documentation and/or other materials provided with the   
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ch.h>
#include <cmsis_os.h>
#include <osi.h>

#include "rom.h"
#include "rom_map.h"
#include "hw_types.h"
#include "interrupt.h"

//Local function definition
static void vSimpleLinkSpawnTask( void *pvParameters );

// Queue Handler
// Queue size 
#define slQUEUE_SIZE				3

osMessageQDef(simpleLink_mailbox, slQUEUE_SIZE, tSimpleLinkSpawnMsg);
osMessageQId(simpleLink_mailbox_id);

osThreadId SimpleLinkSpawnTaskHndl;


/*!
	\brief 	This function registers an interrupt in NVIC table

	The sync object is used for synchronization between different thread or ISR and
	a thread.

	\param	iIntrNum	-	Interrupt number to register
	\param	pEntry	    -	Pointer to the interrupt handler
	\param	ucPriority	-	priority of the interrupt

	\return upon successful creation the function should return 0
			Otherwise, a negative value indicating the error code shall be returned
	\note
	\warning
*/
OsiReturnVal_e osi_InterruptRegister(int iIntrNum,P_OSI_INTR_ENTRY pEntry,unsigned char ucPriority)
{
	MAP_IntRegister(iIntrNum,(void(*)(void))pEntry);
	MAP_IntPrioritySet(iIntrNum, ucPriority);
	MAP_IntEnable(iIntrNum);
	return OSI_OK;
}

/*!
	\brief 	This function De registers an interrupt in NVIC table


	\param	iIntrNum	-	Interrupt number to De register

	\return 	none
	\note
	\warning
*/

void osi_InterruptDeRegister(int iIntrNum)
{
	MAP_IntDisable(iIntrNum);
	MAP_IntUnregister(iIntrNum);
}

/*!
	\brief 	This function creates a sync object

	The sync object is used for synchronization between different thread or ISR and
	a thread.

	\param	pSyncObj	-	pointer to the sync object control block

	\return upon successful creation the function should return 0
			Otherwise, a negative value indicating the error code shall be returned
	\note
	\warning
*/
OsiReturnVal_e osi_SyncObjCreate(OsiSyncObj_t* pSyncObj)
{
    if(pSyncObj == NULL)
    {
        return OSI_INVALID_PARAMS;
    }

    osSemaphoreId semaphore_id = osSemaphoreCreate(NULL, 1);
	if (semaphore_id == NULL)
	{
		*pSyncObj = NULL;
		return OSI_FAILURE;
	}
    
	*pSyncObj = (OsiSyncObj_t)semaphore_id;

    return OSI_OK; 
}

/*!
	\brief 	This function deletes a sync object

	\param	pSyncObj	-	pointer to the sync object control block

	\return upon successful deletion the function should return 0
			Otherwise, a negative value indicating the error code shall be returned
	\note
	\warning
*/
OsiReturnVal_e osi_SyncObjDelete(OsiSyncObj_t* pSyncObj)
{
	if(pSyncObj == NULL)
	{
		return OSI_INVALID_PARAMS;
	}

    osSemaphoreDelete((osSemaphoreId*) pSyncObj);

    return OSI_OK;
}

/*!
	\brief 		This function generates a sync signal for the object.

	All suspended threads waiting on this sync object are resumed

	\param		pSyncObj	-	pointer to the sync object control block

	\return 	upon successful signaling the function should return 0
				Otherwise, a negative value indicating the error code shall be returned
	\note		the function could be called from ISR context
	\warning
*/
OsiReturnVal_e osi_SyncObjSignal(OsiSyncObj_t* pSyncObj)
{
	if(pSyncObj == NULL)
	{
		return OSI_INVALID_PARAMS;
	}

    osSemaphoreRelease((osSemaphoreId)*pSyncObj);

    return OSI_OK;
}
/*!
	\brief 		This function generates a sync signal for the object
				from ISR context.

	All suspended threads waiting on this sync object are resumed

	\param		pSyncObj	-	pointer to the sync object control block

	\return 	upon successful signalling the function should return 0
				Otherwise, a negative value indicating the error code shall be returned
	\note		the function is called from ISR context
	\warning
*/
OsiReturnVal_e osi_SyncObjSignalFromISR(OsiSyncObj_t* pSyncObj)
{
	if(pSyncObj == NULL)
	{
		return OSI_INVALID_PARAMS;
	}

    osSemaphoreRelease((osSemaphoreId)*pSyncObj);

    return OSI_OK;
}

/*!
	\brief 	This function waits for a sync signal of the specific sync object

	\param	pSyncObj	-	pointer to the sync object control block
	\param	Timeout		-	numeric value specifies the maximum number of mSec to
							stay suspended while waiting for the sync signal
							Currently, the simple link driver uses only two values:
								- OSI_WAIT_FOREVER
								- OSI_NO_WAIT

	\return upon successful reception of the signal within the timeout window return 0
			Otherwise, a negative value indicating the error code shall be returned
	\note
	\warning
*/
OsiReturnVal_e osi_SyncObjWait(OsiSyncObj_t* pSyncObj , OsiTime_t Timeout)
{
	if(pSyncObj == NULL)
	{
		return OSI_INVALID_PARAMS;
	}

    if(osSemaphoreWait((osSemaphoreId)*pSyncObj, (uint32_t) Timeout) != osOK)
    {
        return OSI_OPERATION_FAILED;
    }

    return OSI_OK;
}

/*!
	\brief 	This function clears a sync object

	\param	pSyncObj	-	pointer to the sync object control block

	\return upon successful clearing the function should return 0
			Otherwise, a negative value indicating the error code shall be returned
	\note
	\warning
*/
OsiReturnVal_e osi_SyncObjClear(OsiSyncObj_t* pSyncObj)
{
	if(pSyncObj == NULL)
	{
		return OSI_INVALID_PARAMS;
	}

    if (OSI_OK == osi_SyncObjWait(pSyncObj,0) )
    {
        return OSI_OK;
    }
    else
    {
        return OSI_OPERATION_FAILED;
    }
}

/*!
	\brief 	This function creates a locking object.

	The locking object is used for protecting a shared resources between different
	threads.

	\param	pLockObj	-	pointer to the locking object control block

	\return upon successful creation the function should return 0
			Otherwise, a negative value indicating the error code shall be returned
	\note
	\warning
*/
OsiReturnVal_e osi_LockObjCreate(OsiLockObj_t* pLockObj)
{
    if(pLockObj == NULL)
    {
        return OSI_INVALID_PARAMS;
    }


    osSemaphoreId semaphore_id = osSemaphoreCreate(NULL, 1);
	if (semaphore_id == NULL)
	{
		*pLockObj = NULL;
		return OSI_FAILURE;
	}
    
	*pLockObj = (OsiLockObj_t)semaphore_id;

    return OSI_OK;
}

/*!
	\brief 	This function creates a Task.

	Creates a new Task and add it to the last of tasks that are ready to run

	\param	pEntry	-	pointer to the Task Function
	\param	pcName	-	Task Name String
	\param	usStackDepth	-	Stack Size in bytes
	\param	pvParameters	-	pointer to structure to be passed to the Task Function
	\param	uxPriority	-	Task Priority

	\return upon successful creation the function should return 0
			Otherwise, a negative value indicating the error code shall be returned
	\note
	\warning
*/
OsiReturnVal_e osi_TaskCreate(P_OSI_TASK_ENTRY pEntry,const signed char * const pcName,
                              unsigned short usStackDepth, void *pvParameters,
                              unsigned long uxPriority,OsiTaskHandle* pTaskHandle)
{

    osThreadDef_t threadDefinition;
    threadDefinition.stacksize = usStackDepth;
    threadDefinition.tpriority = uxPriority;
    threadDefinition.name = pcName;
 
    osThreadId id = osThreadCreate(&threadDefinition, pvParameters); 
    
    if(id != NULL)
    {
        *pTaskHandle = (OsiTaskHandle)id;
    }

	if(id == NULL)
    {
		return OSI_FAILURE;
	}

	return OSI_OK;	
}

/*!
	\brief 	This function Deletes a Task.

	Deletes a  Task and remove it from list of running task

	\param	pTaskHandle	-	Task Handle

	\note
	\warning
*/
void osi_TaskDelete(OsiTaskHandle* pTaskHandle)
{
    osThreadTerminate((osThreadId)pTaskHandle); 
}

/*!
	\brief 	This function deletes a locking object.

	\param	pLockObj	-	pointer to the locking object control block

	\return upon successful deletion the function should return 0
			Otherwise, a negative value indicating the error code shall be returned
	\note
	\warning
*/
OsiReturnVal_e osi_LockObjDelete(OsiLockObj_t* pLockObj)
{
    osSemaphoreDelete((osSemaphoreId)*pLockObj );

    return OSI_OK;
}

/*!
	\brief 	This function locks a locking object.

	All other threads that call this function before this thread calls
	the osi_LockObjUnlock would be suspended

	\param	pLockObj	-	pointer to the locking object control block
	\param	Timeout		-	numeric value specifies the maximum number of mSec to
							stay suspended while waiting for the locking object
							Currently, the simple link driver uses only two values:
								- OSI_WAIT_FOREVER
								- OSI_NO_WAIT


	\return upon successful reception of the locking object the function should return 0
			Otherwise, a negative value indicating the error code shall be returned
	\note
	\warning
*/
OsiReturnVal_e osi_LockObjLock(OsiLockObj_t* pLockObj , OsiTime_t Timeout)
{
    if(pLockObj == NULL)
    {
            return OSI_INVALID_PARAMS;
    }

    //Take Semaphore
    if(osSemaphoreWait((osSemaphoreId)*pLockObj, (uint32_t) Timeout) != osOK)
    {
        return OSI_OPERATION_FAILED;
    }

    return OSI_OK;
}

/*!
	\brief 	This function unlock a locking object.

	\param	pLockObj	-	pointer to the locking object control block

	\return upon successful unlocking the function should return 0
			Otherwise, a negative value indicating the error code shall be returned
	\note
	\warning
*/
OsiReturnVal_e osi_LockObjUnlock(OsiLockObj_t* pLockObj)
{
	
	if(NULL == pLockObj)
	{
		return OSI_INVALID_PARAMS;
	}

	//Release Semaphore
    if(osSemaphoreRelease((osSemaphoreId)*pLockObj) == osOK)
    {
    	return OSI_OK;
    }
    else
    {
    	return OSI_OPERATION_FAILED;
    }
}

/*!
	\brief 	This function call the pEntry callback from a different context

	\param	pEntry		-	pointer to the entry callback function

	\param	pValue		- 	pointer to any type of memory structure that would be
							passed to pEntry callback from the execution thread.

	\param	flags		- 	execution flags - reserved for future usage

	\return upon successful registration of the spawn the function should return 0
			(the function is not blocked till the end of the execution of the function
			and could be returned before the execution is actually completed)
			Otherwise, a negative value indicating the error code shall be returned
	\note
	\warning
*/

OsiReturnVal_e osi_Spawn(P_OSI_SPAWN_ENTRY pEntry , void* pValue , unsigned long flags)
{
	tSimpleLinkSpawnMsg Msg;
	Msg.pEntry = pEntry;
	Msg.pValue = pValue;

	if(osMessagePut(simpleLink_mailbox_id, &Msg, osWaitForever) == osOK)
	{
		return OSI_OK;
	}

	return OSI_OPERATION_FAILED;
}

/*!
	\brief 	This is the simplelink spawn task to call SL callback from a different context 

	\param	pvParameters		-	pointer to the task parameter

	\return void
	\note
	\warning
*/
void vSimpleLinkSpawnTask(void *pvParameters)
{
	tSimpleLinkSpawnMsg* Msg;
    osEvent event;

	for(;;)
	{
        event = osMessageGet(simpleLink_mailbox_id, osWaitForever);
		if(event.status == osEventMessage)
		{
            Msg = (tSimpleLinkSpawnMsg*)event.value.p;
			Msg->pEntry(Msg->pValue);
		}
	}
}

/*!
	\brief 	This is the API to create SL spawn task and create the SL queue

	\param	uxPriority		-	task priority

	\return void
	\note
	\warning
*/
OsiReturnVal_e VStartSimpleLinkSpawnTask(unsigned long uxPriority)
{
    simpleLink_mailbox_id = osMessageCreate(osMessageQ(simpleLink_mailbox), NULL);

    osThreadDef(vSimpleLinkSpawnTask, uxPriority, 2048, "SimpleLinkTask");

    SimpleLinkSpawnTaskHndl = osThreadCreate(osThread(vSimpleLinkSpawnTask), NULL); 
	if (SimpleLinkSpawnTaskHndl == NULL)
	{
        return OSI_OPERATION_FAILED;
   	}

	return OSI_OK;
}

/*!
	\brief 	This is the API to delete SL spawn task and delete the SL queue

	\param	none

	\return void
	\note
	\warning
*/
void VDeleteSimpleLinkSpawnTask( void )
{
    osThreadTerminate(SimpleLinkSpawnTaskHndl);
}

/*!
	\brief 	This function to call the memory allocation function of the RTOS

	\param	pMem		-	pointer to the memory which needs to be freed

	\return - void *
	\note
	\warning
*/

void * mem_Malloc(unsigned long Size)
{
	return ( void * )chHeapAlloc(NULL, Size);
}

/*!
	\brief 	This function to call the memory de-allocation function of the RTOS

	\param	pMem		-	pointer to the memory which needs to be freed

	\return - void
	\note
	\warning
*/
void mem_Free(void *pMem)
{
	chHeapFree(pMem);
}

/*!
	\brief 	This function call the memset function
	\param	pBuf		-	pointer to the memory to be fill
    \param Val          -       Value to be fill
    \param Size         -      Size of the memory which needs to be fill

	\return - void
	\note
	\warning
*/

void mem_set(void *pBuf, int Val, size_t Size)
{
    memset(pBuf, Val, Size);
}

/*!
	\brief 	This function call the memset function
	\param	pDst		-	pointer to the destination
    \param pSrc         -   pointer to the source
    \param Size         -   Size of the memory which needs to be copy

	\return - void
	\note
	\warning
*/
void mem_copy(void *pDst, void *pSrc, size_t Size)
{
    memcpy(pDst, pSrc, Size);
}


/*!
	\brief 	This function use to entering into critical section
	\param	void
	\return - void
	\note
	\warning
*/

unsigned long osi_EnterCritical(void)
{
	chSysLock();
	return 0;
}

/*!
	\brief 	This function use to exit critical section
	\param	void
	\return - void
	\note
	\warning
*/

void osi_ExitCritical(unsigned long ulKey)
{
    chSysUnlock();
}

/*!
	\brief 	This function is used to create the MsgQ

	\param	pMsgQ	-	pointer to the message queue
	\param	pMsgQName	-	msg queue name
	\param	MsgSize	-	size of message on the queue
	\param	MaxMsgs	-	max. number of msgs that the queue can hold

	\return - OsiReturnVal_e
	\note
	\warning
*/
OsiReturnVal_e osi_MsgQCreate(OsiMsgQ_t* 		pMsgQ , 
			      char*			pMsgQName,
			      unsigned long 		MsgSize,
			      unsigned long		MaxMsgs)
{
	
	if(NULL == pMsgQ)
	{
		return OSI_INVALID_PARAMS;
	}

    *pMsgQ = (OsiMsgQ_t)osMessageCreate((osMessageQDef_t*)pMsgQ, NULL);

    if (*pMsgQ == NULL)
    {
        return OSI_OPERATION_FAILED;
    }

    return OSI_OK;
}
/*!
	\brief 	This function is used to delete the MsgQ

	\param	pMsgQ	-	pointer to the message queue

	\return - OsiReturnVal_e
	\note
	\warning
*/
OsiReturnVal_e osi_MsgQDelete(OsiMsgQ_t* pMsgQ)
{
    return OSI_OK;
}
/*!
	\brief 	This function is used to write data to the MsgQ

	\param	pMsgQ	-	pointer to the message queue
	\param	pMsg	-	pointer to the Msg strut to read into
	\param	Timeout	-	timeout to wait for the Msg to be available

	\return - OsiReturnVal_e
	\note
	\warning
*/

OsiReturnVal_e osi_MsgQWrite(OsiMsgQ_t* pMsgQ, void* pMsg , OsiTime_t Timeout)
{
	
	if(NULL == pMsgQ)
	{
		return OSI_INVALID_PARAMS;
	}

	if(osMessagePut((osMessageQDef_t*)pMsgQ, pMsg, osWaitForever) == osOK)
	{
		return OSI_OK;
	}

    return OSI_OPERATION_FAILED;
}
/*!
	\brief 	This function is used to read data from the MsgQ

	\param	pMsgQ	-	pointer to the message queue
	\param	pMsg	-	pointer to the Msg strut to read into
	\param	Timeout	-	timeout to wait for the Msg to be available

	\return - OsiReturnVal_e
	\note
	\warning
*/

OsiReturnVal_e osi_MsgQRead(OsiMsgQ_t* pMsgQ, void* pMsg , OsiTime_t Timeout)
{
    osEvent event;

	
	if(NULL == pMsgQ)
	{
		return OSI_INVALID_PARAMS;
	}

    event = osMessageGet((osMessageQDef_t*)pMsgQ, Timeout);
    if(event.status == osEventMessage)
    {
        pMsg = event.value.p;

        return OSI_OK;
    }

    return OSI_OPERATION_FAILED;
}

/*!
	\brief 	This function used to suspend the task for the specified number of milli secs
	\param	MilliSecs	-	Time in millisecs to suspend the task
	\return - void
	\note
	\warning
*/
void osi_Sleep(unsigned int MilliSecs)
{
    osDelay(MilliSecs);
}

/*!
	\brief 	This function used to disable the tasks
	\param	- void
	\return - Key with the suspended tasks
	\note
	\warning
*/
unsigned long osi_TaskDisable(void)
{
   return OSI_OK;
}

/*!
	\brief 	This function used to start the scheduler
	\param	void
	\return - void
	\note
	\warning
*/
void osi_start()
{
    osKernelStart();
}

/*!
	\brief 	This function used to resume all the tasks
	\param	key	-	returned from suspend tasks
	\return - void
	\note
	\warning
*/
void osi_TaskEnable(unsigned long key)
{
   
}

/*!
	\brief 	This function used to save the OS context before sleep
	\param	void
	\return - void
	\note
	\warning
*/
void osi_ContextSave()
{

}
/*!
	\brief 	This function used to restore the OS context after sleep
	\param	void
	\return - void
	\note
	\warning
*/
void osi_ContextRestore()
{

}
