// clang-format off

// ChibiOs version can be found at:
// https://github.com/ArduPilot/ChibiOS.svn/blob/master/os/various/fatfs_bindings/fatfs_syscall.c
// but currently locked to R0.14b.
// This file aligns with compatibility with R0.15a+
// And also allows use of nF specific capabilities.

/*------------------------------------------------------------------------*/
/* A Sample Code of User Provided OS Dependent Functions for FatFs        */
/*------------------------------------------------------------------------*/

#include "ff.h"

#if FF_USE_LFN == 3	/* Use dynamic memory allocation */

#include "nanoHAL_v2.h"

/*------------------------------------------------------------------------*/
/* Allocate/Free a Memory Block                                           */
/*------------------------------------------------------------------------*/

void* ff_memalloc (	/* Returns pointer to the allocated memory block (null if not enough core) */
    UINT msize		/* Number of bytes to allocate */
)
{
    // NOTE: we no longer choose to use the ChibiOs memory allocation functions.
    //return chHeapAlloc(NULL, msize);
    return platform_malloc(msize); /* Allocate a new memory block */
}


void ff_memfree (
    void* mblock	/* Pointer to the memory block to free (no effect if null) */
)
{
    // NOTE: we no longer choose to use the ChibiOs memory allocation functions.
    //chHeapFree(mblock);
    platform_free(mblock); /* Free the memory block */
}

#endif



#if FF_FS_REENTRANT	/* Mutal exclusion */
/*------------------------------------------------------------------------*/
/* Definitions of Mutex                                                   */
/*------------------------------------------------------------------------*/

#include "hal.h"

static semaphore_t Mutex[FF_VOLUMES + 1];	/* Table of mutex handle */


/*------------------------------------------------------------------------*/
/* Create a Mutex                                                         */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount function to create a new mutex
/  or semaphore for the volume. When a 0 is returned, the f_mount function
/  fails with FR_INT_ERR.
*/

int ff_mutex_create (	/* Returns 1:Function succeeded or 0:Could not create the mutex */
    int vol				/* Mutex ID: Volume mutex (0 to FF_VOLUMES - 1) or system mutex (FF_VOLUMES) */
)
{
    chSemObjectInit(&Mutex[vol], 1);
    return TRUE;
}


/*------------------------------------------------------------------------*/
/* Delete a Mutex                                                         */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount function to delete a mutex or
/  semaphore of the volume created with ff_mutex_create function.
*/

void ff_mutex_delete (	/* Returns 1:Function succeeded or 0:Could not delete due to an error */
    int vol				/* Mutex ID: Volume mutex (0 to FF_VOLUMES - 1) or system mutex (FF_VOLUMES) */
)
{
    chSemReset(&Mutex[vol], 0);
}


/*------------------------------------------------------------------------*/
/* Request a Grant to Access the Volume                                   */
/*------------------------------------------------------------------------*/
/* This function is called on enter file functions to lock the volume.
/  When a 0 is returned, the file function fails with FR_TIMEOUT.
*/

int ff_mutex_take (	/* Returns 1:Succeeded or 0:Timeout */
    int vol			/* Mutex ID: Volume mutex (0 to FF_VOLUMES - 1) or system mutex (FF_VOLUMES) */
)
{
    msg_t msg = chSemWaitTimeout(&Mutex[vol], (systime_t)FF_FS_TIMEOUT);
    return msg == MSG_OK;
}



/*------------------------------------------------------------------------*/
/* Release a Grant to Access the Volume                                   */
/*------------------------------------------------------------------------*/
/* This function is called on leave file functions to unlock the volume.
*/

void ff_mutex_give (
    int vol			/* Mutex ID: Volume mutex (0 to FF_VOLUMES - 1) or system mutex (FF_VOLUMES) */
)
{
    chSemSignal(&Mutex[vol]);
}

#endif	/* FF_FS_REENTRANT */

// clang-format on
