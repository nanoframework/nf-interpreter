//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_CONFIGURATIONMANAGER_CC32XX_H
#define TARGET_CONFIGURATIONMANAGER_CC32XX_H

// we'll store up to 4 network configuration blocks
#define NETWORK_CONFIG_MAX_COUNT            (4)
// files with network configuration have this namming patter "network-config-N.bin"
#define NETWORK_CONFIG_FILE_NAME            "nf/network-config0.bin"
// position of the index in the file name above (that's the 0)
#define NETWORK_CONFIG_FILE_INDEX_POSITION  (17)

// CA (Certificate Authority) root file name
#define CA_ROOT_CERT_FILE_NAME              "ca-cert.pem"

#endif // TARGET_CONFIGURATIONMANAGER_CC32XX_H
