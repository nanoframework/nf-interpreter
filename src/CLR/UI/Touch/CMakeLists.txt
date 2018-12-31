
list(APPEND UI_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/TouchPanel/TouchPanelDriver.cpp")
list(APPEND UI_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}//TouchPanel/TouchPanelFunctions.cpp")
            
list(APPEND UI_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/Native/touch_native_Touch_TouchCollectorConfiguration.cpp")
list(APPEND UI_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/Native/touch_native_Touch_TouchEventProcessor.cpp")
list(APPEND UI_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/Native/touch_native_Touch_TouchPanel.cpp")
list(APPEND UI_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/Native/touch_native.cpp")


list(APPEND UI_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/Ink/Ink_driver.cpp")
list(APPEND UI_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/Ink/ink_functions.cpp")
list(APPEND UI_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/Ink/touch_native_Touch_Ink.cpp")

list(APPEND UI_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/")
list(APPEND UI_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/Includes")

# make var global
set(UI_SOURCE ${UI_SOURCE} CACHE INTERNAL "make global")
set(UI_INCLUDES ${UI_INCLUDES} CACHE INTERNAL "make global")
