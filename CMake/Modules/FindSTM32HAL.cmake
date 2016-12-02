if(STM32_SERIES STREQUAL "F0")
    set(HAL_COMPONENTS adc can cec comp cortex crc dac dma flash gpio i2c
                       i2s irda iwdg pcd pwr rcc rtc smartcard smbus
                       spi tim tsc uart usart wwdg)

    set(HAL_REQUIRED_COMPONENTS cortex pwr rcc)

    # Components that have _ex sources
    set(HAL_EX_COMPONENTS adc crc dac flash i2c pcd pwr rcc rtc smartcard spi tim uart)

    # Components that have ll_ in names instead of hal_
    set(HAL_LL_COMPONENTS "")

    set(HAL_PREFIX stm32f0xx_)

    set(HAL_HEADERS
        stm32f0xx_hal.h
        stm32f0xx_hal_def.h
    )
    set(HAL_SRCS
        stm32f0xx_hal.c
    )
elseif(STM32_SERIES STREQUAL "F1")
    set(HAL_COMPONENTS adc can cec cortex crc dac dma eth flash gpio hcd i2c
                       i2s irda iwdg nand nor pccard pcd pwr rcc rtc sd smartcard
                       spi sram tim uart usart wwdg fsmc sdmmc usb)

    set(HAL_REQUIRED_COMPONENTS cortex pwr rcc)

    # Components that have _ex sources
    set(HAL_EX_COMPONENTS adc dac flash gpio pcd rcc rtc tim)

    # Components that have ll_ in names instead of hal_
    set(HAL_LL_COMPONENTS fsmc sdmmc usb)

    set(HAL_PREFIX stm32f1xx_)

    set(HAL_HEADERS
        stm32f1xx_hal.h
        stm32f1xx_hal_def.h
    )
    set(HAL_SRCS
        stm32f1xx_hal.c
    )
elseif(STM32_SERIES STREQUAL "F2")
    set(HAL_COMPONENTS adc can cortex crc cryp dac dcmi dma eth flash
                       gpio hash hcd i2c i2s irda iwdg nand nor pccard
                       pcd pwr rcc rng rtc sd smartcard spi sram tim
                       uart usart wwdg fsmc sdmmc usb)

    set(HAL_REQUIRED_COMPONENTS cortex pwr rcc)

    # Components that have _ex sources
    set(HAL_EX_COMPONENTS adc dac dma flash pwr rcc rtc tim)

    # Components that have ll_ in names instead of hal_
    set(HAL_LL_COMPONENTS fsmc sdmmc usb)

    set(HAL_PREFIX stm32f2xx_)

    set(HAL_HEADERS
        stm32f2xx_hal.h
        stm32f2xx_hal_def.h
    )

    set(HAL_SRCS
        stm32f2xx_hal.c
    )
elseif(STM32_SERIES STREQUAL "F4")
    set(HAL_COMPONENTS adc can cec cortex crc cryp dac dcmi dma dma2d eth flash
                       flash_ramfunc fmpi2c gpio hash hcd i2c i2s irda iwdg ltdc
                       nand nor pccard pcd pwr qspi rcc rng rtc sai sd sdram
                       smartcard spdifrx spi sram tim uart usart wwdg fmc fsmc
                       sdmmc usb)

    set(HAL_REQUIRED_COMPONENTS cortex pwr rcc)

    # Components that have _ex sources
    set(HAL_EX_COMPONENTS adc cryp dac dcmi dma flash fmpi2c hash i2c i2s pcd
                          pwr rcc rtc sai tim)

    # Components that have ll_ in names instead of hal_
    set(HAL_LL_COMPONENTS fmc fsmc sdmmc usb)

    set(HAL_PREFIX stm32f4xx_)

    set(HAL_HEADERS
        stm32f4xx_hal.h
        stm32f4xx_hal_def.h
    )

    set(HAL_SRCS
        stm32f4xx_hal.c
    )
elseif(STM32_SERIES STREQUAL "F7")
    set(HAL_COMPONENTS adc can cec cortex crc cryp dac dcmi dma dma2d eth flash
                       gpio hash hcd i2c i2s irda iwdg lptim ltdc nand nor pcd
                       pwr qspi rcc rng rtc sai sd sdram smartcard spdifrx spi
                       sram tim uart usart wwdg fmc sdmmc usb)

    set(HAL_REQUIRED_COMPONENTS cortex pwr rcc)

    # Components that have _ex sources
    set(HAL_EX_COMPONENTS adc crc cryp dac dcmi dma flash hash i2c pcd
                          pwr rcc rtc sai tim)

    # Components that have ll_ in names instead of hal_
    set(HAL_LL_COMPONENTS fmc sdmmc usb)

    set(HAL_PREFIX stm32f7xx_)

    set(HAL_HEADERS
        stm32f7xx_hal.h
        stm32f7xx_hal_def.h
    )

    set(HAL_SRCS
        stm32f7xx_hal.c
    )
ENDif()

if(NOT STM32HAL_FIND_COMPONENTS)
    set(STM32HAL_FIND_COMPONENTS ${HAL_COMPONENTS})
    MESSAGE(STATUS "No STM32HAL components selected, using all: ${STM32HAL_FIND_COMPONENTS}")
ENDif()

FOREACH(cmp ${HAL_REQUIRED_COMPONENTS})
    LIST(FIND STM32HAL_FIND_COMPONENTS ${cmp} STM32HAL_FOUND_INDEX)
    if(${STM32HAL_FOUND_INDEX} LESS 0)
        LIST(APPEND STM32HAL_FIND_COMPONENTS ${cmp})
    ENDif()
ENDFOREACH()

FOREACH(cmp ${STM32HAL_FIND_COMPONENTS})
    LIST(FIND HAL_COMPONENTS ${cmp} STM32HAL_FOUND_INDEX)
    if(${STM32HAL_FOUND_INDEX} LESS 0)
        MESSAGE(FATAL_ERROR "Unknown STM32HAL component: ${cmp}. Available components: ${HAL_COMPONENTS}")
    ENDif()
    LIST(FIND HAL_LL_COMPONENTS ${cmp} STM32HAL_FOUND_INDEX)
    if(${STM32HAL_FOUND_INDEX} LESS 0)
        LIST(APPEND HAL_HEADERS ${HAL_PREFIX}hal_${cmp}.h)
        LIST(APPEND HAL_SRCS ${HAL_PREFIX}hal_${cmp}.c)
    ELSE()
        LIST(APPEND HAL_HEADERS ${HAL_PREFIX}ll_${cmp}.h)
        LIST(APPEND HAL_SRCS ${HAL_PREFIX}ll_${cmp}.c)
    ENDif()
    LIST(FIND HAL_EX_COMPONENTS ${cmp} STM32HAL_FOUND_INDEX)
    if(NOT (${STM32HAL_FOUND_INDEX} LESS 0))
        LIST(APPEND HAL_HEADERS ${HAL_PREFIX}hal_${cmp}_ex.h)
        LIST(APPEND HAL_SRCS ${HAL_PREFIX}hal_${cmp}_ex.c)
    ENDif()
ENDFOREACH()

LIST(REMOVE_DUPLICATES HAL_HEADERS)
LIST(REMOVE_DUPLICATES HAL_SRCS)

STRING(TOLOWER ${STM32_SERIES} STM32_SERIES_LOWER)

FIND_PATH(STM32HAL_INCLUDE_DIR ${HAL_HEADERS}
    PATH_SUFFIXES include stm32${STM32_SERIES_LOWER}
    HINTS ${STM32Cube_DIR}/Drivers/STM32${STM32_SERIES}xx_HAL_Driver/Inc
    CMAKE_FIND_ROOT_PATH_BOTH
)

FOREACH(HAL_SRC ${HAL_SRCS})
    set(HAL_${HAL_SRC}_FILE HAL_SRC_FILE-NOTFOUND)
    FIND_FILE(HAL_${HAL_SRC}_FILE ${HAL_SRC}
        PATH_SUFFIXES src stm32${STM32_SERIES_LOWER}
        HINTS ${STM32Cube_DIR}/Drivers/STM32${STM32_SERIES}xx_HAL_Driver/Src
        CMAKE_FIND_ROOT_PATH_BOTH
    )
    LIST(APPEND STM32HAL_SOURCES ${HAL_${HAL_SRC}_FILE})
ENDFOREACH()

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(STM32HAL DEFAULT_MSG STM32HAL_INCLUDE_DIR STM32HAL_SOURCES)
