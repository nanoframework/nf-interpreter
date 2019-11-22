#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

function(GraphicsWpf)
	    # Common graphics code  
		set ( GRAPHICS_DISPLAY "ILI9341_240x320_SPI.cpp")
		set ( TOUCHPANEL_DEVICE "XPT2046.cpp")

		# Platform/board specific code
		set ( GRAPHICS_DISPLAY_INTERFACE "${PROJECT_SOURCE_DIR}/targets/FreeRTOS_ESP32/ESP32_WROOM_32/nanoCLR/nanoFramework.Graphics.Wpf/Spi_To_Display.cpp")
		set ( GRAPHICS_MEMORY	         "${PROJECT_SOURCE_DIR}/targets/FreeRTOS_ESP32/ESP32_WROOM_32/nanoCLR/nanoFramework.Graphics.Wpf/Graphics_Memory.cpp")
		set ( TOUCHPANEL_INTERFACE       "${PROJECT_SOURCE_DIR}/targets/FreeRTOS_ESP32/ESP32_WROOM_32/nanoCLR/nanoFramework.Graphics.Wpf/Spi_To_TouchPanel.cpp")
		
		# Common source code
		find_package(nanoFramework.Graphics.Wpf REQUIRED)



		# Add unrelated includes to the graphics library build to enable clean compilation due to hierachical inclusion of header files
		list(APPEND Graphics_Includes "${PROJECT_SOURCE_DIR}/src/HAL/Include")
		list(APPEND Graphics_Includes "${PROJECT_SOURCE_DIR}/src/CLR/Include")
		list(APPEND Graphics_Includes "${PROJECT_SOURCE_DIR}/src/CLR/Core")
		list(APPEND Graphics_Includes "${PROJECT_SOURCE_DIR}/src/DeviceInterfaces/Networking.Sntp")
		list(APPEND Graphics_Includes "${PROJECT_SOURCE_DIR}/targets/FreeRTOS_ESP32/ESP32_WROOM_32/Include")
		list(APPEND Graphics_Includes "${PROJECT_SOURCE_DIR}/targets/FreeRTOS_ESP32/ESP32_WROOM_32/Lwip")
		list(APPEND Graphics_Includes "${PROJECT_SOURCE_DIR}/targets/FreeRTOS_ESP32/ESP32_WROOM_32/WROVER_KIT_V41")
		list(APPEND Graphics_Includes "${PROJECT_SOURCE_DIR}/targets/FreeRTOS_ESP32/ESP32_WROOM_32")
		list(APPEND Graphics_Includes "${PROJECT_SOURCE_DIR}/src/DeviceInterfaces/System.Net")
		list(APPEND Graphics_Includes "${PROJECT_SOURCE_DIR}/src/CLR/CorLib")

		list(APPEND Graphics_Includes "${CMAKE_BINARY_DIR}/targets/FreeRTOS_ESP32/ESP32_WROOM_32")
		list(APPEND Graphics_Includes "${CMAKE_BINARY_DIR}/targets/FreeRTOS_ESP32/ESP32_WROOM_32")
		list(APPEND Graphics_Includes "${CMAKE_BINARY_DIR}/targets/FreeRTOS_ESP32/ESP32_WROOM_32/nanoCLR")

		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/app_trace/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/app_update/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/bt/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/driver/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/driver/include/driver")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/esp_event/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/esp_ringbuf/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/esp32/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/esp32/include/rom")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/Ethernet/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/fatfs/src")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/freertos/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/freertos/include/freertos")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/heap/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/Log/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/lwip/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/lwip/include/apps")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/lwip/include/lwip")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/lwip/include/lwip/lwip")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/lwip/include/lwip/port")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/lwip/lwip/src")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/lwip/lwip/src/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/lwip/lwip/src/include/lwip")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/lwip/lwip/src/include/posix")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/lwip/port/esp32/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/micro-ecc/micro-ecc")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/newlib/platform_include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/nghttp/nghttp2/lib/includes")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/nghttp/port/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/nvs_flash/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/soc/esp32/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/soc/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/spi_flash/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/spiffs/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/tcpip_adapter/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/tcpip_adapter/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/ulp/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/ulp/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/vfs/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/wear_levelling/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/wpa_supplicant/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/wpa_supplicant/port/include")
		list(APPEND Graphics_Includes "${ESP32_IDF_PATH}/components/xtensa-debug-module/include")

		set(Graphics_Includes ${Graphics_Includes} CACHE INTERNAL "make global")

		# Build UI components as a separate library
		# Keep getting "Createprocess: file not found" errors in linker.
		# Appears to be related to the size of the @CMake/nanoclr.rsp file becoming greater than ~31-32K.
		# This problem goes away for GCC 8.2
	
		add_library(GraphicsWpfLib STATIC "${Graphics_Sources}")
		target_include_directories(GraphicsWpfLib PUBLIC "${Graphics_Includes}")
		add_dependencies(nanoCLR.elf GraphicsWpfLib)
        set_property(TARGET nanoCLR.elf  APPEND_STRING PROPERTY LINK_FLAGS " -L${CMAKE_CURRENT_BINARY_DIR} -lGraphicsWpfLib " )
endfunction()

function(GraphicsWpfCompileDefinitions)
	target_compile_definitions(GraphicsWpfLib PUBLIC "-DPLATFORM_ESP32 -Werror=unused-parameter" )
    target_compile_definitions(${NANOCLR_PROJECT_NAME}.elf PUBLIC -DGRAPHICS=1)
endfunction()


