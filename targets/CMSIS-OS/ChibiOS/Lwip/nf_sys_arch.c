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

#include "hal.h"

#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/sys.h"
#include "lwip/stats.h"

#include "arch/cc.h"
#include "arch/sys_arch.h"

void sys_init(void) {

}

err_t sys_sem_new(sys_sem_t *sem, u8_t count) {

  *sem = chHeapAlloc(NULL, sizeof(semaphore_t));
  if (*sem == 0) {
    SYS_STATS_INC(sem.err);
    return ERR_MEM;
  }
  else {
    chSemObjectInit(*sem, (cnt_t)count);
    SYS_STATS_INC_USED(sem);
    return ERR_OK;
  }
}

void sys_sem_free(sys_sem_t *sem) {

  chHeapFree(*sem);
  *sem = SYS_SEM_NULL;
  SYS_STATS_DEC(sem.used);
}

void sys_sem_signal(sys_sem_t *sem) {

  chSemSignal(*sem);
}

/* CHIBIOS FIX: specific variant of this call to be called from within
   a lock.*/
void sys_sem_signal_S(sys_sem_t *sem) {

  chSemSignalI(*sem);
  chSchRescheduleS();
}

u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout) {
  systime_t start;
  sysinterval_t tmo, remaining;

  osalSysLock();
  tmo = timeout > 0 ? TIME_MS2I((time_msecs_t)timeout) : TIME_INFINITE;
  start = osalOsGetSystemTimeX();
  if (chSemWaitTimeoutS(*sem, tmo) != MSG_OK) {
    osalSysUnlock();
    return SYS_ARCH_TIMEOUT;
  }
  remaining = chTimeDiffX(start, osalOsGetSystemTimeX());
  osalSysUnlock();
  return (u32_t)TIME_I2MS(remaining);
}

int sys_sem_valid(sys_sem_t *sem) {
  return *sem != SYS_SEM_NULL;
}

// typically called within lwIP after freeing a semaphore
// to make sure the pointer is not left pointing to invalid data
void sys_sem_set_invalid(sys_sem_t *sem) {
  *sem = SYS_SEM_NULL;
}

err_t sys_mbox_new(sys_mbox_t *mbox, int size) {
  
  *mbox = chHeapAlloc(NULL, sizeof(mailbox_t) + sizeof(msg_t) * size);
  if (*mbox == 0) {
    SYS_STATS_INC(mbox.err);
    return ERR_MEM;
  }
  else {
    chMBObjectInit(*mbox, (void *)(((uint8_t *)*mbox) + sizeof(mailbox_t)), size);
    SYS_STATS_INC(mbox.used);
    return ERR_OK;
  }
}

void sys_mbox_free(sys_mbox_t *mbox) {
  cnt_t tmpcnt;

  osalSysLock();
  tmpcnt = chMBGetUsedCountI(*mbox);
  osalSysUnlock();

  if (tmpcnt != 0) {
    // If there are messages still present in the mailbox when the mailbox
    // is deallocated, it is an indication of a programming error in lwIP
    // and the developer should be notified.
    SYS_STATS_INC(mbox.err);
    chMBReset(*mbox);
  }
  chHeapFree(*mbox);
  *mbox = SYS_MBOX_NULL;
  SYS_STATS_DEC(mbox.used);
}

void sys_mbox_post(sys_mbox_t *mbox, void *msg) {

  chMBPostTimeout(*mbox, (msg_t)msg, TIME_INFINITE);
}

err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg) {

  if (chMBPostTimeout(*mbox, (msg_t)msg, TIME_IMMEDIATE) == MSG_TIMEOUT) {
    SYS_STATS_INC(mbox.err);
    return ERR_MEM;
  }
  return ERR_OK;
}

u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout) {
  systime_t start;
  sysinterval_t tmo, remaining;

  osalSysLock();
  tmo = timeout > 0 ? TIME_MS2I((time_msecs_t)timeout) : TIME_INFINITE;
  start = osalOsGetSystemTimeX();
  if (chMBFetchTimeoutS(*mbox, (msg_t *)msg, tmo) != MSG_OK) {
    osalSysUnlock();
    return SYS_ARCH_TIMEOUT;
  }
  remaining = chTimeDiffX(start, osalOsGetSystemTimeX());
  osalSysUnlock();
  return (u32_t)TIME_I2MS(remaining);
}

u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg) {

  if (chMBFetchTimeout(*mbox, (msg_t *)msg, TIME_IMMEDIATE) == MSG_TIMEOUT)
    return SYS_MBOX_EMPTY;
  return 0;
}

int sys_mbox_valid(sys_mbox_t *mbox) {
  return *mbox != SYS_MBOX_NULL;
}

// typically called within lwIP after freeing an mbox
// to make sure the pointer is not left pointing to invalid data
void sys_mbox_set_invalid(sys_mbox_t *mbox) {
  *mbox = SYS_MBOX_NULL;
}

sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread,
                            void *arg, int stacksize, int prio) {
  thread_t *tp;

  tp = chThdCreateFromHeap(NULL, THD_WORKING_AREA_SIZE(stacksize),
                           name, prio, (tfunc_t)thread, arg);
  return (sys_thread_t)tp;
}

sys_prot_t sys_arch_protect(void) {

  return chSysGetStatusAndLockX();
}

void sys_arch_unprotect(sys_prot_t pval) {

  osalSysRestoreStatusX((syssts_t)pval);
}

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

u32_t sys_now(void) {

#if OSAL_ST_FREQUENCY == 1000
  return (u32_t)osalOsGetSystemTimeX();
#elif (OSAL_ST_FREQUENCY / 1000) >= 1 && (OSAL_ST_FREQUENCY % 1000) == 0
  return ((u32_t)osalOsGetSystemTimeX() - 1) / (OSAL_ST_FREQUENCY / 1000) + 1;
#elif (1000 / OSAL_ST_FREQUENCY) >= 1 && (1000 % OSAL_ST_FREQUENCY) == 0
  return ((u32_t)osalOsGetSystemTimeX() - 1) * (1000 / OSAL_ST_FREQUENCY) + 1;
#else
  return (u32_t)(((u64_t)(osalOsGetSystemTimeX() - 1) * 1000) / OSAL_ST_FREQUENCY) + 1;
#endif
}
