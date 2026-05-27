//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2001-2004 Swedish Institute of Computer Science.  All rights reserved.
// Portions Copyright (c) 2006..2015 Giovanni Di Sirio. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <hal.h>

#ifndef SYS_ARCH_H
#define SYS_ARCH_H

typedef semaphore_t *sys_sem_t;
typedef mailbox_t *sys_mbox_t;
typedef thread_t *sys_thread_t;
typedef syssts_t sys_prot_t;

#define SYS_MBOX_NULL   (mailbox_t *)0
#define SYS_THREAD_NULL (thread_t *)0
#define SYS_SEM_NULL    (semaphore_t *)0

/* let sys.h use binary semaphores for mutexes */
#define LWIP_COMPAT_MUTEX 1

#endif // SYS_ARCH_H
