#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# NF_Kconfig.cmake — Bridge between Kconfig-generated .config and CMake cache variables.
#
# This module:
# 1. Runs kconfiglib defconfig (if NF_TARGET_DEFCONFIG is set and no .config exists)
# 2. Runs genconfig to produce .config and nf_config.h
# 3. Parses .config into CMake cache variables
# 4. Maps Kconfig symbol names (underscores) back to CMake variable names (dots for APIs)

# Mapping from Kconfig symbol names to CMake variable names.
set(_NF_KCONFIG_API_MAP
    # System APIs
    "API_SYSTEM_MATH|API_System.Math"
    "API_SYSTEM_NET|API_System.Net"
    "API_SYSTEM_DEVICE_GPIO|API_System.Device.Gpio"
    "API_SYSTEM_DEVICE_SPI|API_System.Device.Spi"
    "API_SYSTEM_DEVICE_I2C|API_System.Device.I2c"
    "API_SYSTEM_DEVICE_I2C_SLAVE|API_System.Device.I2c.Slave"
    "API_SYSTEM_DEVICE_PWM|API_System.Device.Pwm"
    "API_SYSTEM_DEVICE_ADC|API_System.Device.Adc"
    "API_SYSTEM_DEVICE_DAC|API_System.Device.Dac"
    "API_SYSTEM_DEVICE_I2S|API_System.Device.I2s"
    "API_SYSTEM_DEVICE_WIFI|API_System.Device.Wifi"
    "API_SYSTEM_DEVICE_USBSTREAM|API_System.Device.UsbStream"
    "API_SYSTEM_IO_PORTS|API_System.IO.Ports"
    "API_SYSTEM_IO_FILESYSTEM|API_System.IO.FileSystem"

    # nanoFramework APIs
    "API_NANOFRAMEWORK_DEVICE_ONEWIRE|API_nanoFramework.Device.OneWire"
    "API_NANOFRAMEWORK_DEVICE_CAN|API_nanoFramework.Device.Can"
    "API_NANOFRAMEWORK_DEVICE_BLUETOOTH|API_nanoFramework.Device.Bluetooth"
    "API_NANOFRAMEWORK_GRAPHICS|API_nanoFramework.Graphics"
    "API_NANOFRAMEWORK_RESOURCEMANAGER|API_nanoFramework.ResourceManager"
    "API_NANOFRAMEWORK_SYSTEM_COLLECTIONS|API_nanoFramework.System.Collections"
    "API_NANOFRAMEWORK_SYSTEM_TEXT|API_nanoFramework.System.Text"
    "API_NANOFRAMEWORK_SYSTEM_SECURITY_CRYPTOGRAPHY|API_nanoFramework.System.Security.Cryptography"
    "API_NANOFRAMEWORK_SYSTEM_IO_HASHING|API_nanoFramework.System.IO.Hashing"
    "API_NANOFRAMEWORK_NETWORKING_THREAD|API_nanoFramework.Networking.Thread"

    # Platform-specific APIs
    "API_HARDWARE_STM32|API_Hardware.Stm32"
    "API_HARDWARE_ESP32|API_Hardware.Esp32"
    "API_NANOFRAMEWORK_HARDWARE_ESP32_RMT|API_nanoFramework.Hardware.Esp32.Rmt"
    "API_NANOFRAMEWORK_HARDWARE_TI|API_nanoFramework.Hardware.TI"
    "API_NANOFRAMEWORK_TI_EASYLINK|API_nanoFramework.TI.EasyLink"
    "API_HARDWARE_GIANTGECKO|API_Hardware.GiantGecko"
    "API_NANOFRAMEWORK_GIANTGECKO_ADC|API_nanoFramework.GiantGecko.Adc"

    # Internal (auto-selected) APIs
    "API_NANOFRAMEWORK_RUNTIME_EVENTS|API_nanoFramework.Runtime.Events"
    "API_NANOFRAMEWORK_NETWORKING_SNTP|API_nanoFramework.Networking.Sntp"
    "API_SYSTEM_RUNTIME_SERIALIZATION|API_System.Runtime.Serialization"
)

# Build a lookup from Kconfig symbol -> CMake variable name
# For API symbols: Kconfig uses underscores, CMake uses dots
# For non-API symbols: names are identical (CONFIG_ prefix is stripped)
function(_nf_build_symbol_map)
    foreach(_entry IN LISTS _NF_KCONFIG_API_MAP)
        string(REPLACE "|" ";" _parts "${_entry}")
        list(GET _parts 0 _kconfig_name)
        list(GET _parts 1 _cmake_name)
        set("_NF_MAP_${_kconfig_name}" "${_cmake_name}" PARENT_SCOPE)
    endforeach()
endfunction()

# Translate a Kconfig symbol name to the corresponding CMake variable name
function(_nf_kconfig_to_cmake_name kconfig_symbol result_var)
    if(DEFINED "_NF_MAP_${kconfig_symbol}")
        set(${result_var} "${_NF_MAP_${kconfig_symbol}}" PARENT_SCOPE)
    else()
        # No mapping needed — use the symbol name as-is
        set(${result_var} "${kconfig_symbol}" PARENT_SCOPE)
    endif()
endfunction()

# Main entry point: load Kconfig configuration into CMake cache
function(nf_load_kconfig)

    # Require NF_TARGET_DEFCONFIG to be set
    if(NOT DEFINED NF_TARGET_DEFCONFIG)
        message(FATAL_ERROR "NF_TARGET_DEFCONFIG is not set. Please specify the path to the target defconfig file.")
    endif()

    # Resolve the defconfig path relative to the source tree
    if(NOT IS_ABSOLUTE "${NF_TARGET_DEFCONFIG}")
        set(_defconfig_path "${CMAKE_SOURCE_DIR}/${NF_TARGET_DEFCONFIG}")
    else()
        set(_defconfig_path "${NF_TARGET_DEFCONFIG}")
    endif()

    if(NOT EXISTS "${_defconfig_path}")
        message(FATAL_ERROR "Defconfig file not found: ${_defconfig_path}")
    endif()

    set(_kconfig_root "${CMAKE_SOURCE_DIR}/Kconfig")
    set(_dot_config "${CMAKE_BINARY_DIR}/.config")
    set(_header_file "${CMAKE_BINARY_DIR}/nf_config.h")
    set(_merge_script "${CMAKE_SOURCE_DIR}/scripts/nf_merge_config.py")

    # Resolve the user Kconfig overlay path.
    # Developers copy config/user-kconfig.conf.TEMPLATE to config/user-kconfig.conf
    # and customise it; that file is git-ignored and merged on top of the board
    # defconfig so per-developer preferences never touch shared files.
    # The path can be overridden via the NF_USER_KCONFIG_OVERLAY CMake variable.
    if(NOT DEFINED NF_USER_KCONFIG_OVERLAY)
        set(_user_overlay "${CMAKE_SOURCE_DIR}/config/user-kconfig.conf")
    else()
        set(_user_overlay "${NF_USER_KCONFIG_OVERLAY}")
    endif()

    # Find Python
    find_package(Python3 COMPONENTS Interpreter REQUIRED)

    # Step 1: Merge the board defconfig (plus optional user overlay) into .config.
    # Re-run whenever the defconfig or the user overlay is newer than .config.
    set(_needs_regen FALSE)
    if(NOT EXISTS "${_dot_config}")
        set(_needs_regen TRUE)
    elseif("${_defconfig_path}" IS_NEWER_THAN "${_dot_config}")
        set(_needs_regen TRUE)
    elseif(EXISTS "${_user_overlay}" AND "${_user_overlay}" IS_NEWER_THAN "${_dot_config}")
        set(_needs_regen TRUE)
    endif()

    if(_needs_regen)
        if(EXISTS "${_user_overlay}")
            message(STATUS "nF Kconfig: loading defconfig from ${NF_TARGET_DEFCONFIG} (+ user overlay)")
        else()
            message(STATUS "nF Kconfig: loading defconfig from ${NF_TARGET_DEFCONFIG}")
        endif()

        set(ENV{srctree} "${CMAKE_SOURCE_DIR}")
        execute_process(
            COMMAND ${Python3_EXECUTABLE} "${_merge_script}"
                "${_kconfig_root}"
                "${_defconfig_path}"
                "${_user_overlay}"
                "${_dot_config}"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            RESULT_VARIABLE _defconfig_result
            OUTPUT_VARIABLE _defconfig_output
            ERROR_VARIABLE _defconfig_error
        )

        if(NOT _defconfig_result EQUAL 0)
            message(FATAL_ERROR "nf_merge_config failed:\n${_defconfig_error}")
        endif()
    endif()

    # Step 2: Run genconfig to produce the C header
    # genconfig takes the Kconfig file as a positional arg and reads the
    # config from the KCONFIG_CONFIG environment variable.
    set(ENV{srctree} "${CMAKE_SOURCE_DIR}")
    set(ENV{KCONFIG_CONFIG} "${_dot_config}")
    execute_process(
        COMMAND ${Python3_EXECUTABLE} -m genconfig
            "${_kconfig_root}"
            "--header-path=${_header_file}"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        RESULT_VARIABLE _genconfig_result
        OUTPUT_VARIABLE _genconfig_output
        ERROR_VARIABLE _genconfig_error
    )

    if(NOT _genconfig_result EQUAL 0)
        message(FATAL_ERROR "kconfiglib genconfig failed:\n${_genconfig_error}")
    endif()

    # Build the symbol name mapping
    _nf_build_symbol_map()

    # Step 3: Parse .config and set CMake cache variables
    file(STRINGS "${_dot_config}" _config_lines)

    foreach(_line IN LISTS _config_lines)
        # Skip comments and empty lines
        if("${_line}" MATCHES "^#" OR "${_line}" STREQUAL "")
            continue()
        endif()

        # Parse CONFIG_<NAME>=<VALUE>
        if("${_line}" MATCHES "^CONFIG_([^=]+)=(.*)")
            set(_symbol "${CMAKE_MATCH_1}")
            set(_value "${CMAKE_MATCH_2}")

            # Translate Kconfig symbol name to CMake variable name
            _nf_kconfig_to_cmake_name("${_symbol}" _cmake_var)

            # Convert Kconfig values to CMake values
            if("${_value}" STREQUAL "y")
                set(${_cmake_var} ON CACHE INTERNAL "From Kconfig" FORCE)
            elseif("${_value}" STREQUAL "n")
                set(${_cmake_var} OFF CACHE INTERNAL "From Kconfig" FORCE)
            elseif("${_value}" MATCHES "^\"(.*)\"$")
                # Quoted string — strip quotes
                set(${_cmake_var} "${CMAKE_MATCH_1}" CACHE INTERNAL "From Kconfig" FORCE)
            else()
                # Integer or unquoted string
                set(${_cmake_var} "${_value}" CACHE INTERNAL "From Kconfig" FORCE)
            endif()
        endif()
    endforeach()

    # Also handle symbols that are commented out as "# CONFIG_XXX is not set"
    # These represent boolean options that are OFF
    foreach(_line IN LISTS _config_lines)
        if("${_line}" MATCHES "^# CONFIG_([^ ]+) is not set")
            set(_symbol "${CMAKE_MATCH_1}")
            _nf_kconfig_to_cmake_name("${_symbol}" _cmake_var)
            set(${_cmake_var} OFF CACHE INTERNAL "From Kconfig" FORCE)
        endif()
    endforeach()

    message(STATUS "nF Kconfig: configuration loaded from ${NF_TARGET_DEFCONFIG}")

endfunction()
