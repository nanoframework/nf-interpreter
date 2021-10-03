/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifdef __GNUC__

#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>

#include <sys/stat.h>

extern int errno;
extern int _end;

void* _sbrk(int incr)
{
    static unsigned char* heap = NULL;
    unsigned char* prev_heap;

    if (heap == NULL)
    {
        heap = (unsigned char*)&_end;
    }
    prev_heap = heap;

    heap += incr;

    return prev_heap;
}

int _close(int file)
{
    return -1;
}

int _fstat(int file, struct stat* st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file)
{
    return 1;
}

int _lseek(int file, int ptr, int dir)
{
    return 0;
}

void _exit(int status)
{
    printf("Exiting with status %d.\n", status);
    while (1)
        ;
}

void _kill(int pid, int sig)
{
    return;
}

int _getpid(void)
{
    return -1;
}

// function aliases to support different runtimes
int lseek(int file, int ptr, int dir) __attribute__((weak, alias("_lseek")));
int fstat(int file, struct stat* st) __attribute__((weak, alias("_fstat")));
int close(int file) __attribute__((weak, alias("_close")));
int isatty(int file) __attribute__((weak, alias("_isatty")));

#endif
