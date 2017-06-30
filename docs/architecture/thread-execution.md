# Thread execution

**About this document**

This document describes how the execution work on **nanoFramework** CLR.


## Introduction to Threads

Oversimplifying it a **nanoFramework** thread is (in what concerns the execution) basically a stream of IL instructions that are chewed by the interpreter making  things happen.
This execution occurs in a cooperative fashion, meaning that a thread is allowed to run for a certain amount of time, after that it stops and the execution is passed to the next thread that meets the required conditions to run.


## Thread execution

The **nanoFramework** CLR and interpreter run on a RTOS thread. When the RTOS works on a [cooperative](https://en.wikipedia.org/wiki/Computer_multitasking#Cooperative_multitasking) fashion (opposed to a [preemptive](https://en.wikipedia.org/wiki/Computer_multitasking#Preemptive_multitasking) fashion) the thread is expected to relinquish control to the RTOS so that the context switching can occur and the next RTOS thread is given the opportunity to run.
This context switching in **nanoFramework** is expected to occur after each time slot that a **nanoFramework** thread is allowed to run.
It's up to the target board developer to provide the correct way to perform that execution control relinquish according to the RTOS beneath.
This may not be required at all. For example when the RTOS works, by default, on a preemptive fashion. In this case the thread execution occurs in a round and robin among the various RTOS threads. 

The declaration of this _execution control relinquish_ is declared as `NANOCLR_RELINQUISHEXECUTIONCONTROL` in [nanoHAL_v2.h](../../src/HAL/Include/nanoHAL_v2.h). The actual implementation has to be provided at target level in the `targetHAL.h` header.
For the current version - that relies on ChibiOS (a CMSIS compliant RTOS) - this is a simple call to `osDelay(1)` which allows the kernel to run all the other threads with the same (or higher) priority.
