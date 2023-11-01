#ifndef SL_MEMORY_CONFIG_H
#define SL_MEMORY_CONFIG_H

// These parameters are meant to be set in the target CMakeLists.txt file
#ifndef SL_STACK_SIZE
#error "Missing compiler define for SL_STACK_SIZE in target CMakeLists.txt"
#endif

#ifndef SL_HEAP_SIZE
#error "Missing compiler define for SL_HEAP_SIZE in target CMakeLists.txt"
#endif

#endif
