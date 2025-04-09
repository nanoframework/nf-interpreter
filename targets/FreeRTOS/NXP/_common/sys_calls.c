//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// dummy implementations for the newlib reentrant functions
// required to link with NXP because adding --specs=nosys.specs doesn't seem to work

#include <sys/stat.h>

int _close(int file)
{
    (void)file;
    return 0;
}

void _exit(int status)
{
    (void)status;
    while (1)
    {
    } /* Hang here forever... */
}

int _fstat(int file, struct stat *st)
{
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}

int _getpid(void)
{
    return 1;
}

int _isatty(int file)
{
    (void)file;
    return 1;
}

int _kill(int pid, int sig)
{
    (void)pid;
    (void)sig;
    return -1;
}

int _lseek(int file, int ptr, int dir)
{
    (void)file;
    (void)ptr;
    (void)dir;
    return 0;
}

int _read(int file, char *ptr, int len)
{
    (void)file;
    (void)ptr;
    (void)len;

    return -1;
}

int _write(int file, const char *ptr, int len)
{
    int txCount;

    (void)file;

    for (txCount = 0; txCount < len; txCount++)
    {
        ptr++;
    }

    return len;
}
