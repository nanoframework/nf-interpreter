#ifndef SL_MEMORY_CONFIG_H
#define SL_MEMORY_CONFIG_H

// <o SL_STACK_SIZE> Stack size for the application.
// This is set to a value that will blow up memory assignment to force this to be set at target level
#ifndef SL_STACK_SIZE
  #define SL_STACK_SIZE  0xDEADBEEF
#endif

// <o SL_HEAP_SIZE> Minimum heap size for the application.
// This is set to a value that will blow up memory assignment to force this to be set at target level
#ifndef SL_HEAP_SIZE
  #define SL_HEAP_SIZE   0xDEADBEEF
#endif

#endif
