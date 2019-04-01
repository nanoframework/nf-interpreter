//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) 2001-2004 Swedish Institute of Computer Science.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

// from the original source code file
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote products
//    derived from this software without specific prior written permission. 
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
// SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
// OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
// IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
// OF SUCH DAMAGE.
//
// This file is part of the lwIP TCP/IP stack.
// 
// Author: Adam Dunkels <adam@sics.se>

// see http://lwip.wikia.com/wiki/Porting_for_an_OS for instructions

#include <cmsis_os.h>
#include <nanoHAL_v2.h>

#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/sys.h"
#include "lwip/stats.h"
#include "lwip/api.h"

#include "arch/cc.h"
#include "arch/sys_arch.h"

osMutexId lwip_sys_mutex;
osMutexDef(lwip_sys_mutex);

void sys_init(void)
{
    lwip_sys_mutex = osMutexCreate(osMutex(lwip_sys_mutex));
}

//  Creates a new semaphore. The "count" argument specifies
//  the initial state of the semaphore.
err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
    osSemaphoreDef(SEM);

    *sem = osSemaphoreCreate (osSemaphore(SEM), 1);

    if(*sem == NULL)
    {
      #if SYS_STATS
        ++lwip_stats.sys.sem.err;
      #endif /* SYS_STATS */	
        return ERR_MEM;
    }

    if(count == 0)	// Means it can't be taken
    {
        osSemaphoreWait(*sem,0);
    }

  #if SYS_STATS
    ++lwip_stats.sys.sem.used;

    if (lwip_stats.sys.sem.max < lwip_stats.sys.sem.used)
    {
        lwip_stats.sys.sem.max = lwip_stats.sys.sem.used;
    }
  #endif /* SYS_STATS */
        
	return ERR_OK;
}

// Deallocates a semaphore
void sys_sem_free(sys_sem_t *sem)
{
  #if SYS_STATS
    --lwip_stats.sys.sem.used;
  #endif /* SYS_STATS */
  
    osSemaphoreDelete(*sem);
}

// Signals a semaphore
void sys_sem_signal(sys_sem_t *sem)
{
    osSemaphoreRelease(*sem);
}

/*
  Blocks the thread while waiting for the semaphore to be
  signaled. If the "timeout" argument is non-zero, the thread should
  only be blocked for the specified time (measured in
  milliseconds).

  If the timeout argument is non-zero, the return value is the number of
  milliseconds spent waiting for the semaphore to be signaled. If the
  semaphore wasn't signaled within the specified time, the return value is
  SYS_ARCH_TIMEOUT. If the thread didn't have to wait for the semaphore
  (i.e., it was already signaled), the function may return zero.

  Notice that lwIP implements a function with a similar name,
  sys_sem_wait(), that uses the sys_arch_sem_wait() function.
*/
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
    uint32_t starttime = osKernelSysTick();
    
    if(timeout != 0)
    {    
        if(osSemaphoreWait (*sem, timeout) == osOK)
        {
        return (osKernelSysTick() - starttime);
        }
        else
        {
        return SYS_ARCH_TIMEOUT;
        } 
    }
    else
    {
        while(osSemaphoreWait (*sem, osWaitForever) != osOK);
        return (osKernelSysTick() - starttime);
    }
}

int sys_sem_valid(sys_sem_t *sem)                                               
{
    if (*sem == SYS_SEM_NULL)
        return 0;
    else
        return 1;                                       
}

void sys_sem_set_invalid(sys_sem_t *sem)                                        
{                                                                               
    *sem = SYS_SEM_NULL;                                                          
}

//  Creates an empty mailbox.
/////////////////////////////////////////////////////////
// NEEDS strong implementation at platform level
/////////////////////////////////////////////////////////
__nfweak err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
    (void)mbox;
    (void)size;

    // breaks execution to ensure developer is aware that this function needs a strong implementation at platform level
    __asm volatile("BKPT #0\n");

    return ERR_OK;
}

/*
  Deallocates a mailbox. If there are messages still present in the
  mailbox when the mailbox is deallocated, it is an indication of a
  programming error in lwIP and the developer should be notified.
*/
/////////////////////////////////////////////////////////
// NEEDS strong implementation at platform level
/////////////////////////////////////////////////////////
__nfweak void sys_mbox_free(sys_mbox_t *mbox)
{
    (void)mbox;

    // breaks execution to ensure developer is aware that this function needs a strong implementation at platform level
    __asm volatile("BKPT #0\n");
}

//   Posts the "msg" to the mailbox.
void sys_mbox_post(sys_mbox_t *mbox, void *data)
{
    while(osMessagePut(*mbox, (uint32_t)data, osWaitForever) != osOK);
}

//   Try to post the "msg" to the mailbox.
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
    err_t result;

    if ( osMessagePut(*mbox, (uint32_t)msg, 0) == osOK)
    {
        result = ERR_OK;
    }
    else 
    {
        // could not post, queue must be full
        result = ERR_MEM;
        
      #if SYS_STATS
        lwip_stats.sys.mbox.err++;
      #endif /* SYS_STATS */
    
    }

    return result;
}

/*
  Blocks the thread until a message arrives in the mailbox, but does
  not block the thread longer than "timeout" milliseconds (similar to
  the sys_arch_sem_wait() function). The "msg" argument is a result
  parameter that is set by the function (i.e., by doing "*msg =
  ptr"). The "msg" parameter maybe NULL to indicate that the message
  should be dropped.

  The return values are the same as for the sys_arch_sem_wait() function:
  Number of milliseconds spent waiting or SYS_ARCH_TIMEOUT if there was a
  timeout.

  Note that a function with a similar name, sys_mbox_fetch(), is
  implemented by lwIP.
*/
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
    osEvent event;
    uint32_t starttime = osKernelSysTick();;
    
    if(timeout != 0)
    { 
        event = osMessageGet (*mbox, timeout);
        
        if(event.status == osEventMessage)
        {
            *msg = (void *)event.value.v;
            return (osKernelSysTick() - starttime);
        }
        else
        {
            return SYS_ARCH_TIMEOUT;
        } 
    }
    else
    {
        event = osMessageGet (*mbox, osWaitForever);
        *msg = (void *)event.value.v;
        return (osKernelSysTick() - starttime);
    }
}

/*
  Similar to sys_arch_mbox_fetch, but if message is not ready immediately, we'll
  return with SYS_MBOX_EMPTY.  On success, 0 is returned.
*/
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
    osEvent event;
    
    event = osMessageGet (*mbox, 0);
    
    if(event.status == osEventMessage)
    {
        *msg = (void *)event.value.v;
        return ERR_OK;
    }
    else
    {
        return SYS_MBOX_EMPTY;
    }
}

int sys_mbox_valid(sys_mbox_t *mbox)          
{      
    if (*mbox == SYS_MBOX_NULL)
    { 
        return 0;
    }
    else
    {
        return 1;
    }
}  

void sys_mbox_set_invalid(sys_mbox_t *mbox)   
{                                             
    *mbox = SYS_MBOX_NULL;                      
}

sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
    const osThreadDef_t os_thread_def = { (os_pthread)thread, (osPriority)prio, stacksize, (char *)name };
    sys_thread_t newThread = osThreadCreate(&os_thread_def, arg);

    netconn_thread_init();

    return newThread;
}

sys_prot_t sys_arch_protect(void) 
{
    osMutexWait(lwip_sys_mutex, osWaitForever);
    return (sys_prot_t)1;
}

void sys_arch_unprotect(sys_prot_t pval) 
{
    (void) pval;
    osMutexRelease(lwip_sys_mutex);
}

#if LWIP_NETCONN_SEM_PER_THREAD

/////////////////////////////////////////////////////////
// NEEDS to be implementated at platform level
// CMSIS RTOS API doesn't have a decent API to tackle
/////////////////////////////////////////////////////////

// sys_sem_t* sys_arch_netconn_sem_get(void)
// {
//     return 0;
// }

// void sys_arch_netconn_sem_alloc(void)
// {
// }

// void sys_arch_netconn_sem_free(void)
// {
// }

#endif // LWIP_NETCONN_SEM_PER_THREAD


////////////////////////////////////////////////////
// nanoFramework "hack" extending LwIP original code
// with this callback here we don't need any reference to CLR nor need to include any nanoFramework headers here
void (*signal_sock_functionPtr)() = 0;

void set_signal_sock_function( void (*funcPtr)() )
{
   signal_sock_functionPtr = funcPtr;
}

void sys_signal_sock_event()
{
  if ( signal_sock_functionPtr != 0 )
     signal_sock_functionPtr();    
}

////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
// NEEDS strong implementation at platform level
/////////////////////////////////////////////////////////
__nfweak u32_t sys_now(void)
{

    // breaks execution to ensure developer is aware that this function needs a strong implementation at platform level
    __asm volatile("BKPT #0\n");

    return 0;
}
