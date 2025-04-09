//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Friendly declaration for interrupt vectors of Cortex-Mx devices.

#ifndef VECTORS_H
#define VECTORS_H

typedef void  (*irq_vector_t)(void);

//  Type of a structure representing the 16 system vectors of Cortex-M.
// The remaining interrupt vectors are not included.
typedef struct {
  uint32_t      *InitStack;
  irq_vector_t  ResetHandler;
  irq_vector_t  NmiHandler;
  irq_vector_t  HardfaultHandler;
  irq_vector_t  MemmanageHandler;
  irq_vector_t  BusfaultHandler;
  irq_vector_t  UsagefaultHandler;
  irq_vector_t  Vector1c;
  irq_vector_t  Vector20;
  irq_vector_t  Vector24;
  irq_vector_t  Vector28;
  irq_vector_t  SvcHandler;
  irq_vector_t  DebugmonitorHandler;
  irq_vector_t  Vector34;
  irq_vector_t  PendsvHandler;
  irq_vector_t  SystickHandler;
  // the remaining vectors would be here
} vectors_t;

#endif // VECTORS_H
