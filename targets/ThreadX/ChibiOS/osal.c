//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2006..2015 Giovanni Di Sirio. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "osal.h"

/*===========================================================================*/
/* Module local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Module exported variables.                                                */
/*===========================================================================*/

/**
 * @brief   Pointer to a halt error message.
 * @note    The message is meant to be retrieved by the debugger after the
 *          system halt caused by an unexpected error.
 */
const char *osal_halt_msg;
unsigned int interrupt_save;

/*===========================================================================*/
/* Module local types.                                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Module local variables.                                                   */
/*===========================================================================*/
TX_MUTEX criticalSection;

/*===========================================================================*/
/* Module local functions.                                                   */
/*===========================================================================*/

static void SuspendTimeoutCallback(uint32_t p)
{
    osalSysLockFromISR();
    osalThreadResumeI((thread_reference_t *)p, MSG_TIMEOUT);
    osalSysUnlockFromISR();
}

static void ElementDequeue(queue_elem_t *e)
{
    e->queue.prev->queue.next = e->queue.next;
    e->queue.next->queue.prev = e->queue.prev;
}

static inline bool ThreadsQueueIsEmpty(threads_queue_t *tqp)
{
    return (tqp->next == (const queue_elem_t *)tqp);
}

static void ThreadsQueuePush(threads_queue_t *tqp, queue_elem_t *e)
{
    e->queue.next = (queue_elem_t *)tqp;
    e->queue.prev = tqp->prev;
    e->queue.prev->queue.next = e;
    tqp->prev = e;
}

static queue_elem_t *ThreadsQueuePop(threads_queue_t *tqp)
{
    queue_elem_t *e;

    e = tqp->next;
    tqp->next = e->queue.next;
    tqp->next->queue.prev = (queue_elem_t *)tqp;

    return e;
}

/*===========================================================================*/
/* Module exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   OSAL module initialization.
 *
 * @api
 */
void osalInit(void)
{
    tx_mutex_create(&criticalSection, NULL, TX_NO_INHERIT);
}

/**
 * @brief   System halt with error message.
 *
 * @param[in] reason    the halt message pointer
 *
 * @api
 */
__attribute__((weak, noreturn)) void osalSysHalt(const char *reason)
{
    osalSysDisable();
    osal_halt_msg = reason;
    while (true)
    {
    }
}

/**
 * @brief   Polled delay.
 * @note    The real delay is always few cycles in excess of the specified
 *          value.
 *
 * @param[in] cycles    number of cycles
 *
 * @xclass
 */
void osalSysPolledDelayX(rtcnt_t cycles)
{
    (void)cycles;
}

/**
 * @brief   System timer handler.
 * @details The handler is used for scheduling and Virtual Timers management.
 *
 * @iclass
 */
void osalOsTimerHandlerI(void)
{
    osalDbgCheckClassI();
}

/**
 * @brief   Checks if a reschedule is required and performs it.
 * @note    I-Class functions invoked from thread context must not reschedule
 *          by themselves, an explicit reschedule using this function is
 *          required in this scenario.
 * @note    Not implemented in this simplified OSAL.
 *
 * @sclass
 */
void osalOsRescheduleS(void)
{
}

/**
 * @brief   Current system time.
 * @details Returns the number of system ticks since the @p osalInit()
 *          invocation.
 * @note    The counter can reach its maximum and then restart from zero.
 * @note    This function can be called from any context but its atomicity
 *          is not guaranteed on architectures whose word size is less than
 *          @p systime_t size.
 *
 * @return              The system time in ticks.
 *
 * @xclass
 */
systime_t osalOsGetSystemTimeX(void)
{
    return (systime_t)tx_time_get();
}

/**
 * @brief   Suspends the invoking thread for the specified time.
 *
 * @param[in] time      the delay in system ticks, the special values are
 *                      handled as follow:
 *                      - @a TIME_INFINITE is allowed but interpreted as a
 *                        normal time specification.
 *                      - @a TIME_IMMEDIATE this value is not allowed.
 *                      .
 *
 * @sclass
 */
void osalThreadSleepS(sysinterval_t time)
{
    tx_thread_sleep(time);
}

/**
 * @brief   Suspends the invoking thread for the specified time.
 *
 * @param[in] time      the delay in system ticks, the special values are
 *                      handled as follow:
 *                      - @a TIME_INFINITE is allowed but interpreted as a
 *                        normal time specification.
 *                      - @a TIME_IMMEDIATE this value is not allowed.
 *                      .
 *
 * @api
 */
void osalThreadSleep(sysinterval_t time)
{
    osalSysLock();
    tx_thread_sleep(time);
    osalSysUnlock();
}

/**
 * @brief   Sends the current thread sleeping and sets a reference variable.
 * @note    This function must reschedule, it can only be called from thread
 *          context.
 *
 * @param[in] trp       a pointer to a thread reference object
 * @return              The wake up message.
 *
 * @sclass
 */
msg_t osalThreadSuspendS(thread_reference_t *trp)
{
    thread_t thd;

    osalDbgCheck(trp != NULL);

    *trp = &thd;
    thd.handle = tx_thread_identify();
    thd.message = MSG_WAIT;

    while (thd.message == MSG_WAIT)
    {
        tx_thread_suspend(thd.handle);
        tx_thread_relinquish();
    }

    return thd.message;
}

/**
 * @brief   Sends the current thread sleeping and sets a reference variable.
 * @note    This function must reschedule, it can only be called from thread
 *          context.
 *
 * @param[in] trp       a pointer to a thread reference object
 * @param[in] timeout   the timeout in system ticks, the special values are
 *                      handled as follow:
 *                      - @a TIME_INFINITE the thread enters an infinite sleep
 *                        state.
 *                      - @a TIME_IMMEDIATE the thread is not enqueued and
 *                        the function returns @p MSG_TIMEOUT as if a timeout
 *                        occurred.
 *                      .
 * @return              The wake up message.
 * @retval MSG_TIMEOUT  if the operation timed out.
 *
 * @sclass
 */
msg_t osalThreadSuspendTimeoutS(thread_reference_t *trp, sysinterval_t timeout)
{
    TX_TIMER timer;
    msg_t msg;

    osalDbgCheck(trp != NULL);

    if (TIME_IMMEDIATE == timeout)
    {
        return MSG_TIMEOUT;
    }

    if (TIME_INFINITE == timeout)
    {
        return osalThreadSuspendS(trp);
    }

    tx_timer_create(&timer, NULL, SuspendTimeoutCallback, (uint32_t)trp, timeout, 0, TX_AUTO_ACTIVATE);

    msg = osalThreadSuspendS(trp);

    tx_timer_delete(&timer);

    return msg;
}

/**
 * @brief   Wakes up a thread waiting on a thread reference object.
 * @note    This function must not reschedule because it can be called from
 *          ISR context.
 *
 * @param[in] trp       a pointer to a thread reference object
 * @param[in] msg       the message code
 *
 * @iclass
 */
void osalThreadResumeI(thread_reference_t *trp, msg_t msg)
{
    osalDbgCheck(trp != NULL);

    if (*trp != NULL)
    {
        thread_t *tp = *trp;

        tp->message = msg;

        tx_thread_resume(tp->handle);
    }
}

/**
 * @brief   Wakes up a thread waiting on a thread reference object.
 * @note    This function must reschedule, it can only be called from thread
 *          context.
 *
 * @param[in] trp       a pointer to a thread reference object
 * @param[in] msg       the message code
 *
 * @iclass
 */
void osalThreadResumeS(thread_reference_t *trp, msg_t msg)
{
    osalDbgCheck(trp != NULL);

    osalThreadResumeI(trp, msg);
}

/**
 * @brief   Enqueues the caller thread.
 * @details The caller thread is enqueued and put to sleep until it is
 *          dequeued or the specified timeouts expires.
 *
 * @param[in] tqp       pointer to the threads queue object
 * @param[in] timeout   the timeout in system ticks, the special values are
 *                      handled as follow:
 *                      - @a TIME_INFINITE the thread enters an infinite sleep
 *                        state.
 *                      - @a TIME_IMMEDIATE the thread is not enqueued and
 *                        the function returns @p MSG_TIMEOUT as if a timeout
 *                        occurred.
 *                      .
 * @return              The message from @p osalQueueWakeupOneI() or
 *                      @p osalQueueWakeupAllI() functions.
 * @retval MSG_TIMEOUT  if the thread has not been dequeued within the
 *                      specified timeout or if the function has been
 *                      invoked with @p TIME_IMMEDIATE as timeout
 *                      specification.
 *
 * @sclass
 */
msg_t osalThreadEnqueueTimeoutS(threads_queue_t *tqp, sysinterval_t timeout)
{
    queue_elem_t e;
    msg_t msg;

    osalDbgCheck(tqp != NULL);

    ThreadsQueuePush(tqp, &e);

    msg = osalThreadSuspendTimeoutS(&e.tr, timeout);

    ElementDequeue(&e);

    return msg;
}

/**
 * @brief   Dequeues and wakes up one thread from the queue, if any.
 *
 * @param[in] tqp       pointer to the threads queue object
 * @param[in] msg       the message code
 *
 * @iclass
 */
void osalThreadDequeueNextI(threads_queue_t *tqp, msg_t msg)
{
    queue_elem_t *e;

    osalDbgCheck(tqp != NULL);

    if (!ThreadsQueueIsEmpty(tqp))
    {
        e = ThreadsQueuePop(tqp);
        osalThreadResumeI(&e->tr, msg);
    }
}

/**
 * @brief   Dequeues and wakes up all threads from the queue.
 *
 * @param[in] tqp       pointer to the threads queue object
 * @param[in] msg       the message code
 *
 * @iclass
 */
void osalThreadDequeueAllI(threads_queue_t *tqp, msg_t msg)
{
    osalDbgCheck(tqp != NULL);

    while (!ThreadsQueueIsEmpty(tqp))
    {
        osalThreadDequeueNextI(tqp, msg);
    }
}

/**
 * @brief   Add flags to an event source object.
 *
 * @param[in] esp       pointer to the event flags object
 * @param[in] flags     flags to be ORed to the flags mask
 *
 * @iclass
 */
void osalEventBroadcastFlagsI(event_source_t *esp, eventflags_t flags)
{
    osalDbgCheck(esp != NULL);

    esp->flags |= flags;

    if (esp->cb != NULL)
    {
        esp->cb(esp);
    }
}

/**
 * @brief   Add flags to an event source object.
 *
 * @param[in] esp       pointer to the event flags object
 * @param[in] flags     flags to be ORed to the flags mask
 *
 * @iclass
 */
void osalEventBroadcastFlags(event_source_t *esp, eventflags_t flags)
{
    osalDbgCheck(esp != NULL);

    osalSysLock();

    osalEventBroadcastFlagsI(esp, flags);

    osalSysUnlock();
}

/**
 * @brief   Event callback setup.
 * @note    The callback is invoked from ISR context and can
 *          only invoke I-Class functions. The callback is meant
 *          to wakeup the task that will handle the event by
 *          calling @p osalEventGetAndClearFlagsI().
 * @note    This function is not part of the OSAL API and is provided
 *          exclusively as an example and for convenience.
 *
 * @param[in] esp       pointer to the event flags object
 * @param[in] cb        pointer to the callback function
 * @param[in] param     parameter to be passed to the callback function
 *
 * @api
 */
void osalEventSetCallback(event_source_t *esp, eventcallback_t cb, void *param)
{
    osalDbgCheck(esp != NULL);

    esp->cb = cb;
    esp->param = param;
}

/**
 * @brief   Locks the specified mutex.
 * @post    The mutex is locked and inserted in the per-thread stack of owned
 *          mutexes.
 *
 * @param[in,out] mp    pointer to the @p mutex_t object
 *
 * @api
 */
void osalMutexLock(mutex_t *mp)
{
    osalDbgCheck(mp != NULL);

    tx_mutex_get(mp, TX_WAIT_FOREVER);
}

/**
 * @brief   Unlocks the specified mutex.
 * @note    The HAL guarantees to release mutex in reverse lock order. The
 *          mutex being unlocked is guaranteed to be the last locked mutex
 *          by the invoking thread.
 *          The implementation can rely on this behavior and eventually
 *          ignore the @p mp parameter which is supplied in order to support
 *          those OSes not supporting a stack of the owned mutexes.
 *
 * @param[in,out] mp    pointer to the @p mutex_t object
 *
 * @api
 */
void osalMutexUnlock(mutex_t *mp)
{
    osalDbgCheck(mp != NULL);

    tx_mutex_put(mp);
}
