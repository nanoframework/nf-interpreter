//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2001-2004 Swedish Institute of Computer Science.  All rights reserved.
// Portions Copyright (c) 2006..2015 Giovanni Di Sirio. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <cmsis_os.h>
#include <nanoHAL_v2.h>
#include <targetHAL.h>

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
    GLOBAL_LOCK();

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
    GLOBAL_UNLOCK();

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
