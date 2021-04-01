//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// these are stubs to keep the linker happy when these are pulled in 

#ifdef __cplusplus
extern "C" {
#endif

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

extern void _exit(int status)
{
	(void)status;

	for (;;);
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
