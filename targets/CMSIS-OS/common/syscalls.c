//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

// these are stubs to keep the linker happy when these are pulled in 

#ifdef __cplusplus
extern "C" {
#endif

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
