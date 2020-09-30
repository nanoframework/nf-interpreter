//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "stdafx.h"

#include <nanoPAL.h>

HAL_DblLinkedList<HAL_CONTINUATION> g_HAL_Completion_List;

/***************************************************************************/

void HAL_COMPLETION::Execute()
{
	NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();

#if defined(_DEBUG)
	this->Start_RTC_Ticks = 0;
#endif

	if (this->ExecuteInISR)
	{
		HAL_CONTINUATION* cont = this;

		cont->Execute();
	}
	else
	{
		this->Enqueue();
	}
}

//--//

void HAL_COMPLETION::InitializeList()
{
	NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();
	g_HAL_Completion_List.Initialize();
}

//--//

void HAL_COMPLETION::DequeueAndExec()
{
	NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();

	GLOBAL_LOCK();

	HAL_COMPLETION* ptr = (HAL_COMPLETION*)g_HAL_Completion_List.FirstNode();

	// waitforevents does not have an associated completion, therefore we need to verify
	// than their is a next completion and that the current one has expired.
	if (ptr->Next())
	{
		// Current one expired ?
		if (HAL_Time_CurrentTime() >= ptr->EventTimeTicks)
		{
			Events_Set(SYSTEM_EVENT_FLAG_SYSTEM_TIMER);

			ptr->Unlink();

#if defined(_DEBUG)
			ptr->EventTimeTicks = 0;
#endif  // defined(_DEBUG)

			//// let the ISR turn on interrupts, if it needs to
			ptr->Execute();
		}

		//
		// Set the next timer to run otherwise set the next interrupt to be 356 years since last powerup (@25kHz).
		Time_SetCompare(ptr->Next() ? ptr->EventTimeTicks : HAL_COMPLETION_IDLE_VALUE);
	}

	GLOBAL_UNLOCK();
}

//--//

void HAL_COMPLETION::EnqueueTicks(uint64_t eventTimeTicks)
{
	NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();
	ASSERT(eventTimeTicks != 0);

	this->EventTimeTicks = eventTimeTicks;

#if defined(_DEBUG)
	this->Start_RTC_Ticks = HAL_Time_CurrentSysTicks();
#endif

	GLOBAL_LOCK();

	HAL_COMPLETION* ptr = (HAL_COMPLETION*)g_HAL_Completion_List.FirstNode();
	HAL_COMPLETION* ptrNext;

	// If not empty list the find position in Completion list based on time
	// If empty just add at head.
	if (!g_HAL_Completion_List.IsEmpty())
	{
		do
		{
			ptrNext = (HAL_COMPLETION *)ptr->Next();

			if (eventTimeTicks < ptr->EventTimeTicks)
			{
				break;
			}

			ptr = ptrNext;
		} while (ptr);
	}

	g_HAL_Completion_List.InsertBeforeNode(ptr, this);

	if (this == g_HAL_Completion_List.FirstNode())
	{
		Time_SetCompare(eventTimeTicks);
	}

	GLOBAL_UNLOCK();
}

//--//

void HAL_COMPLETION::Abort()
{
	NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();

	GLOBAL_LOCK();

	HAL_COMPLETION* firstNode = (HAL_COMPLETION*)g_HAL_Completion_List.FirstNode();

	this->Unlink();

#if defined(_DEBUG)
	this->Start_RTC_Ticks = 0;
#endif

	if (firstNode == this)
	{
		uint64_t nextTicks;

		if (g_HAL_Completion_List.IsEmpty())
		{
			//
			// In case there's no other request to serve, set the next interrupt to be 356 years since last powerup (@25kHz).
			//
			nextTicks = HAL_COMPLETION_IDLE_VALUE;
		}
		else
		{
			firstNode = (HAL_COMPLETION*)g_HAL_Completion_List.FirstNode();

			nextTicks = firstNode->EventTimeTicks;
		}

		Time_SetCompare(nextTicks);
	}

	GLOBAL_UNLOCK();
}

//--//


//
//	WaitForInterrupts for expireTimeInTicks time
//
//  Interrupts should be disabled
//
void HAL_COMPLETION::WaitForInterrupts(uint64_t expireTimeInTicks, uint32_t sleepLevel, uint64_t wakeEvents)
{
	NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();

	const int setCompare = 1;
	const int resetCompare = 2;
	const int nilCompare = 4;

	HAL_COMPLETION* ptr = (HAL_COMPLETION*)g_HAL_Completion_List.FirstNode();
	int             state;

	// Any Completion events been Queued ?
	if (ptr->Next() == NULL)
	{
		// No
		state = setCompare | nilCompare;
	}
	// Is Queued event later than passed expire timeout
	else if (ptr->EventTimeTicks > expireTimeInTicks)
	{
		// Yes
		state = setCompare | resetCompare;
	}
	else
	{
		state = 0;
	}

	if (state & setCompare)
	{
		// Set timer for expireTimeInTicks time
		Time_SetCompare(expireTimeInTicks);
	}

	// Wait for next interrupt ( timer etc)
	CPU_Sleep((SLEEP_LEVEL_type)sleepLevel, wakeEvents);

	if (state & (resetCompare | nilCompare))
	{
		// let's get the first node again
		// it could have changed since CPU_Sleep re-enabled interrupts
		ptr = (HAL_COMPLETION*)g_HAL_Completion_List.FirstNode();
		Time_SetCompare((state & resetCompare) ? ptr->EventTimeTicks : HAL_COMPLETION_IDLE_VALUE);
	}
}

void HAL_COMPLETION::Uninitialize()
{
	NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();

	HAL_COMPLETION* ptr;

	GLOBAL_LOCK();

	while (TRUE)
	{
		ptr = (HAL_COMPLETION*)g_HAL_Completion_List.ExtractFirstNode();

		if (!ptr)
		{
			break;
		}

	}

	GLOBAL_UNLOCK();
}
