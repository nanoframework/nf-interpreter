//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) 2001-2004 Swedish Institute of Computer Science.  All rights reserved.
// Portions Copyright (c) 2006..2015 Giovanni Di Sirio. All rights reserved.
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

#include <hal.h>
#include <cmsis_os.h>
#include <nanoHAL_v2.h>

#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/sys.h"
#include "lwip/stats.h"

#include "arch/cc.h"
#include "arch/sys_arch.h"

// Creates an empty mailbox.
// Developer notes
// Because ChibiOS CMSIS wrapper requires static allocation of message queues we can't use the 
// standard CMSIS define like osMessageQDef(QUEUE, size, void *);
// the workaround is to use a ChibiOS mailbox and allocate memory for the mailbox and the buffer
err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
    *mbox = platform_malloc(sizeof(mailbox_t) + sizeof(msg_t) * size);
    if (*mbox == NULL)
    {
        return ERR_MEM;
    }

    // init the mailbox
    chMBObjectInit((mailbox_t*)*mbox, (void *)(((uint8_t *)*mbox) + sizeof(mailbox_t)), size);

#if SYS_STATS
      ++lwip_stats.sys.mbox.used;
      if (lwip_stats.sys.mbox.max < lwip_stats.sys.mbox.used) {
         lwip_stats.sys.mbox.max = lwip_stats.sys.mbox.used;
	  }
#endif /* SYS_STATS */
    if (*mbox == NULL)
    {
        return ERR_MEM;
    }
 
    return ERR_OK;
}

// Deallocates a mailbox.
// Because this mailbox was created from heap (see sys_mbox_new above) need to free it here
void sys_mbox_free(sys_mbox_t *mbox)
{
    msg_t msgp;

    // check for messages waiting 
    // If there are messages still present in the
    // mailbox when the mailbox is deallocated, it is an indication of a
    // programming error in lwIP and the developer should be notified.
    if(chMBFetchI((mailbox_t*)*mbox, &msgp) != MSG_TIMEOUT)
	{
      #if SYS_STATS
	    lwip_stats.sys.mbox.err++;
      #endif /* SYS_STATS */

        // breaks execution to ensure developer is aware that this function needs a strong implementation at platform level
        __asm volatile("BKPT #0\n");	
	}

    // ...free memory from mailbox
    platform_free(*mbox);
}

u32_t sys_now(void) 
{
  #if OSAL_ST_FREQUENCY == 1000
    return (u32_t)chVTGetSystemTimeX();
  #elif (OSAL_ST_FREQUENCY / 1000) >= 1 && (OSAL_ST_FREQUENCY % 1000) == 0
    return ((u32_t)chVTGetSystemTimeX() - 1) / (OSAL_ST_FREQUENCY / 1000) + 1;
  #elif (1000 / OSAL_ST_FREQUENCY) >= 1 && (1000 % OSAL_ST_FREQUENCY) == 0
    return ((u32_t)chVTGetSystemTimeX() - 1) * (1000 / OSAL_ST_FREQUENCY) + 1;
  #else
    return (u32_t)(((u64_t)(chVTGetSystemTimeX() - 1) * 1000) / OSAL_ST_FREQUENCY) + 1;
  #endif
}

// need to implement this calling the CMSIS implementation because ChibiOS declares this
// TODO can be removed if we ever stop using ChibiOS lwIP source
void sys_sem_signal_S(sys_sem_t* sem)
{
    sys_sem_signal(sem);
}

#if LWIP_NETCONN_SEM_PER_THREAD

//////////////////////////////////////////////////////////////////////////////////////////////////////
// required for LWIP_NETCONN_SEM_PER_THREAD
// need to implement this with ChibiOS API because CMSIS RTOS doesn't have an API to deal with this
//
// For this to work ChibiOS thread struct has to be extended at target level.
// This is accomplished by adding the following line at the target 'chconf.h' file
//   void* localStorage;

sys_sem_t* sys_arch_netconn_sem_get(void)
{
    thread_t* currentThread = chThdGetSelfX();
    LWIP_ASSERT("thread != NULL", currentThread != NULL);
volatile char* tName = (char*)currentThread->name;
(void)tName;
    return currentThread->localStorage;
}

void sys_arch_netconn_sem_alloc(void)
{
    void* storage;

    thread_t* currentThread = chThdGetSelfX();
    LWIP_ASSERT("thread != NULL", currentThread != NULL);

    storage = currentThread->localStorage;
    if(storage == NULL)
    {
        // semaphore doesn't exist
        sys_sem_t* semaphore = NULL;
        err_t err;

        err = sys_sem_new(semaphore, 0);
        LWIP_ASSERT("err == ERR_OK", err == ERR_OK);
        LWIP_ASSERT("semaphore invalid", sys_sem_valid(semaphore));

        // store it in thread struc
        currentThread->localStorage = semaphore;
    }
}

void sys_arch_netconn_sem_free(void)
{
    void* storage;

    thread_t* currentThread = chThdGetSelfX();
    LWIP_ASSERT("thread != NULL", currentThread != NULL);

    storage = currentThread->localStorage;
    if(storage == NULL)
    {
        // there is a semaphore
        sys_sem_t* semaphore = storage;

        // free memory allocation
        sys_sem_free(semaphore);
        
        // clear pointer in thread struct
        currentThread->localStorage = NULL;
    }
}

#endif // LWIP_NETCONN_SEM_PER_THREAD
