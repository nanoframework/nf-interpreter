# CMakeList.txt : CMake project for nf, include source and define
# project specific logic here.
# 
cmake_minimum_required (VERSION 3.8)

    set(CMAKE_BUILD_TYPE "Debug")
    string(TIMESTAMP BUILD_TIMESTAMP UTC)

    set(CMAKE_MODULE_PATH "${NANOFRAMEWORK_ROOT}/nf-interpreter/CMake/Modules")
    set(CHIBIOS_SOURCE "${OPERATING_SYSTEM}/ChibiOS")


    set(BUILD_VERSION 0.0.0.0)
    project(nanoFramework VERSION ${BUILD_VERSION})  

   add_subdirectory("nanobooter")
   add_subdirectory("nanoclr")

   include(${NANOFRAMEWORK_ROOT}/CMakeUserFunctions.cmake)


########################################################################################
## The parameters passed in the function calls below are setup as variables defined and 
## edited in the CMakeSettings.json file for each target board configuration
########################################################################################

    ######################
    ## Hardware Features 
    ######################
    Watch_Dog(${WatchDog})                                                
    Realtime_Clock(${RealtimeClock})                                           
    CRC32(${CRC32})                                                  
    Hardware_Random_Number_Generation(${HardwareRandomNumberGeneration})                       
    Config_Block( ${ConfigBlock})                                           
                                                                   
    ######################                                         
    ## Extended Debugging                                          
    ######################                                        
    Debugger(${Debugger})                                                
    CLR_Trace(${CLRTrace})                                               
    CLR_IL_Inline(${CLRILInline})                                           
    Trace_Errors(${TraceErrors})                                           
    Trace_Headers(${TraceHeaders})                                           
    Trace_State(${TraceState})                                             
    Trace_NoData(${TraceNoData})                                             
    Trace_ALL( ${TraceALL})                                               
                                                                  
    #########################                                      
    ## Extended data features                                      
    #########################                                      
    Any_Base_Conversion(${AnyBaseConversion})                                     
    App_Domains( ${AppDomains})                                            
                                                                   
    #########################                                      
    #     Mathematics                                             
    #########################                                     
    Double_Floating_Point(${DoubleFloatingPoint})                                    
                                                                   
    #########################                                      
    #     Network                                                  
    #########################                                      
    Network(${Network} ${NetworkSecurity} ${NetworkSNTP} ${NetworkWiFI}  ${NetworkMac})
    
    #############################
    ## File System Support
    #############################  
    File_System(${FileSystem})

    #############################
    ## Sensors and Communications
    #############################
    Adc(${Adc})
    GPIO(${GPIO})
    I2C(${I2C})
    PWM(${PWM})
    SPI(${SPI})
    CAN(${CAN})
    One_Wire(${OneWire})
    Serial_Communication(${SerialCommunication})

    ############################
    ## Platform specific section
    ############################
    Arm_Cortex_SWO(${ArmCortexSWO})

    ############################
    ## Interop Assemblies
    ############################
    Interop_Assemblies(${InteropAssemblies})

    ##########################
    # Release Build
    ##########################
    Build_RTM(${BuildRTM})
         