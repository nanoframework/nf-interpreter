//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

// these are stubs to keep the linker happy when these are pulled in 

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/_types.h>
#include <stddef.h>

/*
 * _gettimeofday primitive (Stub function)
 * */

/*
 * Structure used in select() call, taken from the BSD file sys/time.h.
 */
struct timeval {
        long    tv_sec;         /* seconds */
        long    tv_usec;        /* and microseconds */
};

struct timezone {
	int	tz_minuteswest;	/* minutes west of Greenwich */
	int	tz_dsttime;	/* type of dst correction */
};

int _gettimeofday (struct timeval * tp, struct timezone * tzp)
{
	(void)tp;
		
	/* Return fixed data for the timezone.  */
	if (tzp)
	{
		tzp->tz_minuteswest = 0;
		tzp->tz_dsttime = 0;
	}

  return 0;
}

extern _off_t _lseek_r( struct _reent *_r, int fd, _off_t pos, int whence )
{
   return 0;
}

extern _ssize_t _read_r( struct _reent *ptr, int fd, const void *buf, size_t cnt )
{
   return cnt;
}

extern void _kill(int pid, int sig)
{
	(void) pid;
	(void) sig;
	return;
}

extern int _getpid(void)
{
	return -1;
}

#ifdef __cplusplus
}
#endif
