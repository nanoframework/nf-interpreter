//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <stdint.h>
#include <nanoCLR_Application.h>

// POSIX Header files
#include <pthread.h>
#include <unistd.h>

// RTOS header files
#include <ti/sysbios/BIOS.h>

// board Header files
#include <Board.h>

//////////////////////////////

// Stack size in bytes
#define THREADSTACKSIZE   2048

CLR_SETTINGS clrSettings;

extern void * mainThread(void *arg); 

int main(void)
{
    pthread_t thread;
    pthread_attr_t threadAttributes;
    struct sched_param priorityParameters;
    
    int retc;

    // Call board init functions
    Board_initGeneral();

    // CLR settings to launch CLR thread   
    (void)memset(&clrSettings, 0, sizeof(CLR_SETTINGS));

    clrSettings.MaxContextSwitches         = 50;
    clrSettings.WaitForDebugger            = false;
    clrSettings.EnterDebuggerLoopAfterExit = true;

    // Set priority and stack size attributes
    pthread_attr_init(&threadAttributes);
    priorityParameters.sched_priority = 1;

    retc = pthread_attr_setdetachstate(&threadAttributes, PTHREAD_CREATE_DETACHED);

    pthread_attr_setschedparam(&threadAttributes, &priorityParameters);
    retc |= pthread_attr_setstacksize(&threadAttributes, THREADSTACKSIZE);
    retc |= pthread_create(&thread, &threadAttributes, mainThread, &clrSettings);
    if(retc != 0)
    {
        while(1)
        {
            ;
        }
    }

    BIOS_start();

    return (0);
}
