//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _ST_CMSIS_DEFINES_H_
#define _ST_CMSIS_DEFINES_H_ 1

// these are taken from STM32L0 CubePackage @ Middlewares\Third_Party\FreeRTOS\Source\CMSIS_RTOS\cmsis_os.c
// defines being set at a source code file cause issues because they become available only after that file is compiled

#define __ASM            __asm                                      /*!< asm keyword for GNU Compiler          */
#define __INLINE         inline                                     /*!< inline keyword for GNU Compiler       */
#define __STATIC_INLINE  static inline

#endif //_ST_CMSIS_DEFINES_H_
