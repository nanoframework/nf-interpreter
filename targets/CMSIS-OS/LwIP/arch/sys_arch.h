//
// Copyright (c) .NET Foundation and Contributors
// Copyright (c) 2001-2004 Swedish Institute of Computer Science. All rights reserved.
// Portions Copyright (c) 2006..2018 Giovanni Di Sirio. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <cmsis_os.h>

#ifndef __SYS_ARCH_H__
#define __SYS_ARCH_H__

typedef osSemaphoreId sys_sem_t;
typedef osSemaphoreId sys_mutex_t;
typedef osMessageQId  sys_mbox_t;
typedef osThreadId    sys_thread_t;

#define SYS_MBOX_NULL (osMessageQId)0
#define SYS_SEM_NULL  (osSemaphoreId)0
#define SYS_THREAD_NULL (thread_t *)0

/* let sys.h use binary semaphores for mutexes */
#define LWIP_COMPAT_MUTEX 1

#endif // __SYS_ARCH_H__
