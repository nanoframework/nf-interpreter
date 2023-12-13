//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (C) 2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
// See LICENSE file in the project root for full license information.
//

/*
 * Utility functions required to gather run time statistics.  See:
 * https://www.FreeRTOS.org/rtos-run-time-stats.html
 *
 * Note that this is a simulated port, where simulated time is a lot slower than
 * real time, therefore the run time counter values have no real meaningful
 * units.
 *
 * Also note that it is assumed this demo is going to be used for short periods
 * of time only, and therefore timer overflows are not handled.
*/

/* FreeRTOS includes. */
#include <FreeRTOS.h>

/* Variables used in the creation of the run time stats time base.  Run time
stats record how much time each task spends in the Running state. */
static long long llInitialRunTimeCounterValue = 0LL, llTicksPerHundedthMillisecond = 0LL;

/*-----------------------------------------------------------*/

void vConfigureTimerForRunTimeStats( void )
{
LARGE_INTEGER liPerformanceCounterFrequency, liInitialRunTimeValue;

	/* Initialise the variables used to create the run time stats time base.
	Run time stats record how much time each task spends in the Running
	state. */

	if( QueryPerformanceFrequency( &liPerformanceCounterFrequency ) == 0 )
	{
		llTicksPerHundedthMillisecond = 1;
	}
	else
	{
		/* How many times does the performance counter increment in 1/100th
		millisecond. */
		llTicksPerHundedthMillisecond = liPerformanceCounterFrequency.QuadPart / 100000LL;

		/* What is the performance counter value now, this will be subtracted
		from readings taken at run time. */
		QueryPerformanceCounter( &liInitialRunTimeValue );
		llInitialRunTimeCounterValue = liInitialRunTimeValue.QuadPart;
	}
}
/*-----------------------------------------------------------*/

configRUN_TIME_COUNTER_TYPE ulGetRunTimeCounterValue( void )
{
LARGE_INTEGER liCurrentCount;
configRUN_TIME_COUNTER_TYPE ulReturn;

	/* What is the performance counter value now? */
	QueryPerformanceCounter( &liCurrentCount );

	/* Subtract the performance counter value reading taken when the
	application started to get a count from that reference point, then
	scale to (simulated) 1/100ths of a millisecond. */
	if( llTicksPerHundedthMillisecond == 0 )
	{
		/* The trace macros are probably calling this function before the
		scheduler has been started. */
		ulReturn = 0;
	}
	else
	{
		ulReturn = ( configRUN_TIME_COUNTER_TYPE ) ( ( liCurrentCount.QuadPart - llInitialRunTimeCounterValue ) / llTicksPerHundedthMillisecond );
	}

	return ulReturn;
}
/*-----------------------------------------------------------*/
