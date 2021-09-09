cmake_minimum_required(VERSION 3.5)

project(idf_as_lib LANGUAGES C ASM)

    # Include for ESP-IDF build system functions
    include($ENV{IDF_PATH}/tools/cmake/idf.cmake)
    # Create idf::esp32 and idf::freertos static libraries
    idf_build_process(${TARGET_SERIES_SHORT}
                    # try and trim the build; additional components
                    # will be included as needed based on dependency tree
                    #
                    # although esptool_py does not generate static library,
                    # processing the component is needed for flashing related
                    # targets and file generation
                    COMPONENTS esp32 freertos esptool_py
                    BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})

# set(elf_file ${CMAKE_PROJECT_NAME}.elf)
# add_executable(${elf_file} main.c)

# target_link_libraries(${elf_file} idf::esp32s2 idf::freertos idf::spi_flash)
# Attach additional targets to the executable file for flashing,
# linker script generation, partition_table generation, etc.
# idf_build_executable(${elf_file})

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
