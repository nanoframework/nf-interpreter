//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

// Need to have calls to these two functions in C code.
// Because they are called only on asm code, GCC linker with LTO option thinks they are not used and just removes them.
// Having them called from a dummy function that is never called it a workaround for this.
// The clean alternative would be to add the GCC attribute used in those functions, but that's not our code to touch.

void dummyFunction(void) __attribute__((used));

// Never called.
void dummyFunction(void) {
    vTaskSwitchContext();
    localProgramStart();
}
