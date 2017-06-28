/*
//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) 2006..2015 Giovanni Di Sirio.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
*/

/* Stack rules inclusion.*/
INCLUDE rules_stacks.ld

/* Code rules inclusion.*/
INCLUDE rules_code.ld


/* 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// rules_clr.ld have to be included **BEFORE** the rules_data.ld
// this is because the CRT heap (processed in rules_data.ld) takes up all the remaining free space of the memory region where it's assigned
// the size of the managed heap (processed in rules_clr.ld) is set in each target configuration
// in case it shares the same region as the CRT heap it won't have any room left if the CRT heap is assigned first
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* 

/* nanoCLR rules inclusion.*/
INCLUDE rules_clr.ld

/* Data rules inclusion.*/
INCLUDE rules_data.ld
