
add_subdirectory(ChibiOS)
add_subdirectory(common)
add_subdirectory(common/Include)
add_subdirectory(nanoBooter)
add_subdirectory(nanoCLR)

find_package(CMSIS REQUIRED)

# include common directories
include_directories(
    ${TARGET_CMSIS_COMMON_INCLUDE_DIRS}
)