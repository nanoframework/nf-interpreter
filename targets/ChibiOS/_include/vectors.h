/*
    ChibiOS - Copyright (C) 2006..2016 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

// Interrupt vectors for Cortex-Mx devices.

#ifndef VECTORS_H
#define VECTORS_H

#include "cmparams.h"

#if !defined(_FROM_ASM_)
/**
 * @brief   Type of an IRQ vector.
 */
typedef void  (*irq_vector_t)(void);

//  Type of a structure representing the 16 system vectors of Cortex-M.
// The remaining interrupt vectors are not included.
typedef struct {
  uint32_t      *init_stack;
  irq_vector_t  reset_handler;
  irq_vector_t  nmi_handler;
  irq_vector_t  hardfault_handler;
  irq_vector_t  memmanage_handler;
  irq_vector_t  busfault_handler;
  irq_vector_t  usagefault_handler;
  irq_vector_t  vector1c;
  irq_vector_t  vector20;
  irq_vector_t  vector24;
  irq_vector_t  vector28;
  irq_vector_t  svc_handler;
  irq_vector_t  debugmonitor_handler;
  irq_vector_t  vector34;
  irq_vector_t  pendsv_handler;
  irq_vector_t  systick_handler;
  irq_vector_t  vectors[CORTEX_NUM_VECTORS];
} vectors_t;
#endif /* !defined(_FROM_ASM_) */

#if !defined(_FROM_ASM_)
extern vectors_t _vectors;
#endif

#endif // VECTORS_H
