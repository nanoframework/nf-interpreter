#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# set include directories
list(APPEND System.Net_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/HAL/Include")
list(APPEND System.Net_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/DeviceInterfaces/System.Net")

# source files
set(System.Net_SRCS

    # System.Net
    sys_net_native.cpp  
    sys_net_native_System_Net_Sockets_NativeSocket.cpp

    # System.Net.NetworkInformation
    sys_net_native_System_Net_NetworkInformation_NetworkInterface.cpp
    sys_net_native_System_Net_NetworkInformation_Wireless80211Configuration.cpp
    sys_net_native_System_Net_NetworkInformation_WirelessAPConfiguration.cpp

    # System.Security.Cryptography.X509Certificates 
    sys_net_native_System_Security_Cryptography_X509Certificates_X509Certificate.cpp
    sys_net_native_System_Security_Cryptography_X509Certificates_X509Certificate2.cpp

    # System.Net.Security
    sys_net_native_System_Net_Security_SslNative.cpp
    sys_net_native_System_Net_Security_CertificateManager.cpp

)

foreach(SRC_FILE ${System.Net_SRCS})
    set(System.Net_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(System.Net_SRC_FILE ${SRC_FILE}
        PATHS

            "${PROJECT_SOURCE_DIR}/src/DeviceInterfaces/System.Net"

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${System.Net_SRC_FILE}") # debug helper
    list(APPEND System.Net_SOURCES ${System.Net_SRC_FILE})
endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(System.Net DEFAULT_MSG System.Net_INCLUDE_DIRS System.Net_SOURCES)
