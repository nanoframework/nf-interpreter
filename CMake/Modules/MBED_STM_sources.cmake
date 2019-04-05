#####################################################################"######
# WHEN ADDING A NEW VENDOR add the source code from the vendor folder bellow
######################################################################"#####

# source files common for all STM targets
set(MBEDOS_VENDOR_SRCS
    hal_tick_16b.c
    hal_tick_32b.c
    i2c_api.c
    stm_spi_api.c
    trng_api.c
    us_ticker_16b.c
    us_ticker_32b.c
)
foreach(SRC_FILE ${MBEDOS_VENDOR_SRCS})
    set(MBEDOS_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(MBEDOS_SRC_FILE ${SRC_FILE}
        PATHS 
        ${PROJECT_BINARY_DIR}/mbedOS_Source/targets/TARGET_${MBEDOS_TARGET_VENDOR}
        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${MBEDOS_SRC_FILE}") # debug helper
    list(APPEND MBEDOS_SOURCES ${MBEDOS_SRC_FILE})
endforeach()

# HAL_CMx file
list(APPEND MBEDOS_SOURCES ${PROJECT_BINARY_DIR}/mbedOS_Source/rtos/rtx/TARGET_CORTEX_M/TARGET_${MBEDOS_TARGET_CORE_SHORT}/TOOLCHAIN_GCC/HAL_C${MBEDOS_TARGET_CORE_SHORT}.S)

# SVC_Table file
list(APPEND MBEDOS_SOURCES ${PROJECT_BINARY_DIR}/mbedOS_Source/rtos/rtx/TARGET_CORTEX_M/TARGET_${MBEDOS_TARGET_CORE_SHORT}/TOOLCHAIN_GCC/SVC_Table.S)
