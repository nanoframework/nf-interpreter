#include <FreeRTOS.h>
#include <task.h>

void vAssertCalled(unsigned long ulLine, const char *const pcFileName)
{
    static BaseType_t xPrinted = pdFALSE;
    volatile uint32_t ulSetToNonZeroInDebuggerToContinue = 0;

    /* Called if an assertion passed to configASSERT() fails.  See
     * http://www.freertos.org/a00110.html#configASSERT for more information. */

    /* Parameters are not used. */
    (void)ulLine;
    (void)pcFileName;

    taskENTER_CRITICAL();
    {
        printf("ASSERT! Line %ld, file %s, GetLastError() %ld\r\n", ulLine, pcFileName, GetLastError());

        /* Stop the trace recording and save the trace. */
        //(void)xTraceDisable();
        // prvSaveTraceFile();

        /* Cause debugger break point if being debugged. */
        __debugbreak();

        /* You can step out of this function to debug the assertion by using
         * the debugger to set ulSetToNonZeroInDebuggerToContinue to a non-zero
         * value. */
        while (ulSetToNonZeroInDebuggerToContinue == 0)
        {
            //__asm {
            //    NOP
            //};
            //__asm {
            //    NOP
            //};
        }

        /* Re-enable the trace recording. */
        //(void)xTraceEnable(TRC_START);
    }
    taskEXIT_CRITICAL();
}

void vApplicationIdleHook(void)
{

#if (mainCREATE_SIMPLE_BLINKY_DEMO_ONLY != 1)
    {
        const unsigned long ulMSToSleep = 15;

        /* Sleep to reduce CPU load, but don't sleep indefinitely in case there are
        tasks waiting to be terminated by the idle task. */
        Sleep(ulMSToSleep);
    }
#endif
}

void vApplicationTickHook(void)
{
    /* This function will be called by each tick interrupt if
     * configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
     * added here, but the tick hook is called from an interrupt context, so
     * code must not attempt to block, and only the interrupt safe FreeRTOS API
     * functions can be used (those that end in FromISR()). */
}
///* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
// * implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
// * used by the Idle task. */
// void vApplicationGetIdleTaskMemory(
//    StaticTask_t **ppxIdleTaskTCBBuffer,
//    StackType_t **ppxIdleTaskStackBuffer,
//    uint32_t *pulIdleTaskStackSize)
//{
//    /* If the buffers to be provided to the Idle task are declared inside this
//     * function then they must be declared static - otherwise they will be allocated on
//     * the stack and so not exists after this function exits. */
//    static StaticTask_t xIdleTaskTCB;
//    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];
//
//    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
//     * state will be stored. */
//    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
//
//    /* Pass out the array that will be used as the Idle task's stack. */
//    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
//
//    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
//     * Note that, as the array is necessarily of type StackType_t,
//     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
//    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
//}

void vApplicationMallocFailedHook(void)
{
    /* vApplicationMallocFailedHook() will only be called if
     * configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
     * function that will get called if a call to pvPortMalloc() fails.
     * pvPortMalloc() is called internally by the kernel whenever a task, queue,
     * timer or semaphore is created.  It is also called by various parts of the
     * demo application.  If heap_1.c, heap_2.c or heap_4.c is being used, then the
     * size of the	heap available to pvPortMalloc() is defined by
     * configTOTAL_HEAP_SIZE in FreeRTOSConfig.h, and the xPortGetFreeHeapSize()
     * API function can be used to query the size of free heap space that remains
     * (although it does not provide information on how the remaining heap might be
     * fragmented).  See http://www.freertos.org/a00111.html for more
     * information. */
    vAssertCalled(__LINE__, __FILE__);
}
