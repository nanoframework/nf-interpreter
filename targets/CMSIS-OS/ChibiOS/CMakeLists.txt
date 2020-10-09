    # Define base path for the class libraries
    set(BASE_PATH_FOR_CLASS_LIBRARIES_MODULES "${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/ST/nanoCLR")

    add_subdirectory(targets/CMSIS-OS/ChibiOS)
    add_subdirectory(targets/CMSIS-OS/common)
    add_subdirectory(targets/CMSIS-OS/common/Include)
    add_subdirectory(targets/CMSIS-OS/nanoBooter)
    add_subdirectory(targets/CMSIS-OS/nanoCLR)

    # now add the subdirectory for the board
    # try to find board in the targets folder
    if(EXISTS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/ST/${CHIBIOS_BOARD})

        # board found
        message(STATUS "Support for target board '${CHIBIOS_BOARD}' found")
        message(STATUS "${CHIBIOS_BOARD_DEFINITIONS_LOCATION}")

        # add TARGET board directory
        add_subdirectory("${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/ST/${CHIBIOS_BOARD}")

    else()

        # try to find board in the Community targets folder
        if(EXISTS ${PROJECT_SOURCE_DIR}/targets-community/CMSIS-OS/ChibiOS/ST/${CHIBIOS_BOARD})
            # board found
            message(STATUS "Support for target board '${CHIBIOS_BOARD}' found in Community targets")
            message(STATUS "${CHIBIOS_BOARD_DEFINITIONS_LOCATION}")

            # add TARGET board directory from Community
            add_subdirectory("targets-community/CMSIS-OS/ChibiOS/ST/${CHIBIOS_BOARD}")
        else()
        # board NOT found in targets folder
            # board NOT found in targets folder
            message(FATAL_ERROR "\n\nSorry but support for ${CHIBIOS_BOARD} target is not available...\n\You can wait for that to be added or you might want to contribute and start working on a PR for that.\n\n")
        endif()

    endif()