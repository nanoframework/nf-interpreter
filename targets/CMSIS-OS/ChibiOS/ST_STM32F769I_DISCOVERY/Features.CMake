#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

function(GraphicsWpf)
	    # Common graphics code  
		set ( GRAPHICS_DISPLAY "Otm8009a_MIPI_DSI.cpp")
		set ( TOUCHPANEL_DEVICE "ft6x06.cpp")

		# Platform/board specific code
		Set (PlatformSpecificGraphics   "${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/Chibios/ST_STM32F769I_DISCOVERY/nanoCLR/nanoFramework.Graphics.Wpf")
		set ( GRAPHICS_DISPLAY_INTERFACE "${PlatformSpecificGraphics}/MIPI_DSI_To_Display.cpp")
		set ( GRAPHICS_MEMORY	         "${PlatformSpecificGraphics}/Graphics_Memory.cpp")
		set ( TOUCHPANEL_INTERFACE       "${PlatformSpecificGraphics}/I2C_To_TouchPanel.cpp")
		
		# Common source code
		find_package(nanoFramework.Graphics.Wpf REQUIRED)

endfunction()

function(GraphicsWpfCompileDefinitions)
    target_compile_definitions(${NANOCLR_PROJECT_NAME}.elf PUBLIC -DGRAPHICS=1)
endfunction()


