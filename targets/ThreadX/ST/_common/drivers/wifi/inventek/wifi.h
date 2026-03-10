// clang-format off

/**
  ******************************************************************************
  * @file    wifi.h
  * @author  MCD Application Team
  * @brief   This file contains the different WiFi core resources definitions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
#ifndef WIFI_H
#define WIFI_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "es_wifi.h"
#include "es_wifi_io.h"

/* Exported constants --------------------------------------------------------*/
#define WIFI_MAX_SSID_NAME            100
#define WIFI_MAX_PSWD_NAME            100
#define WIFI_MAX_APS                  20
#define WIFI_MAX_CONNECTIONS          4
#define WIFI_MAX_MODULE_NAME          100
#define WIFI_MAX_CONNECTED_STATIONS   2
#define  WIFI_MSG_JOINED      1
#define  WIFI_MSG_ASSIGNED    2


/* Exported types ------------------------------------------------------------*/
typedef enum {
  WIFI_ECN_OPEN = 0x00,
  WIFI_ECN_WEP = 0x01,
  WIFI_ECN_WPA_PSK = 0x02,
  WIFI_ECN_WPA2_PSK = 0x03,
  WIFI_ECN_WPA_WPA2_PSK = 0x04,
}WIFI_Ecn_t;

typedef enum {
  WIFI_TCP_PROTOCOL = 0,
  WIFI_UDP_PROTOCOL = 1,
}WIFI_Protocol_t;

typedef enum {
  WIFI_SERVER = 0,
  WIFI_CLIENT = 1,
}WIFI_Type_t;

typedef enum {
  WIFI_STATUS_OK             = 0,
  WIFI_STATUS_ERROR          = 1,
  WIFI_STATUS_NOT_SUPPORTED  = 2,
  WIFI_STATUS_JOINED         = 3,
  WIFI_STATUS_ASSIGNED       = 4,
  WIFI_STATUS_TIMEOUT        = 5,
}WIFI_Status_t;

typedef struct {
  WIFI_Ecn_t Ecn;                                           /*!< Security of Wi-Fi spot. This parameter has a value of \ref WIFI_Ecn_t enumeration */
  char SSID[WIFI_MAX_SSID_NAME + 1];                        /*!< Service Set Identifier value. Wi-Fi spot name */
  int16_t RSSI;                                             /*!< Signal strength of Wi-Fi spot */
  uint8_t MAC[6];                                           /*!< MAC address of spot */
  uint8_t Channel;                                          /*!< Wi-Fi channel */
  uint8_t Offset;                                           /*!< Frequency offset from base 2.4GHz in kHz */
  uint8_t Calibration;                                      /*!< Frequency offset calibration */
}WIFI_AP_t;

typedef struct {
  WIFI_AP_t    ap[WIFI_MAX_APS];
  uint8_t      count;
} WIFI_APs_t;


typedef struct {
  uint8_t Number;                                           /*!< Connection number */
  uint16_t RemotePort;                                      /*!< Remote PORT number */
  uint16_t LocalPort;
  uint8_t RemoteIP[4];                                      /*!< IP address of device */
  WIFI_Protocol_t Protocol;                                 /*!< Connection type. Parameter is valid only if connection is made as client */
  uint32_t TotalBytesReceived;                              /*!< Number of bytes received in entire connection lifecycle */
  uint32_t TotalBytesSent;                                  /*!< Number of bytes sent in entire connection lifecycle */
  uint8_t Active;                                           /*!< Status if connection is active */
  uint8_t Client;                                           /*!< Set to 1 if connection was made as client */
} WIFI_Socket_t;


typedef struct {
  uint8_t          SSID[WIFI_MAX_SSID_NAME + 1];
  uint8_t          PSWD[WIFI_MAX_PSWD_NAME + 1];
  uint8_t          channel;
  WIFI_Ecn_t       Ecn;
} WIFI_APConfig_t;

typedef struct {
  uint8_t SSID[WIFI_MAX_SSID_NAME + 1];                         /*!< Network public name for ESP AP mode */
  uint8_t IP_Addr[4];                                           /*!< IP Address */
  uint8_t MAC_Addr[6];                                          /*!< MAC address */
} WIFI_APSettings_t;

typedef struct {
  uint8_t          IsConnected;
  uint8_t          IP_Addr[4];
  uint8_t          IP_Mask[4];
  uint8_t          Gateway_Addr[4];
} WIFI_Conn_t;

/* Exported macro ------------------------------------------------------------*/

// Define the macro for building IPv4 addresses. 
//#define IP_ADDRESS(a, b, c, d)                     ((((uint32_t)a) << 24) | (((uint32_t)b) << 16) | (((uint32_t)c) << 8) | ((uint32_t)d))

/* Exported functions ------------------------------------------------------- */
WIFI_Status_t       WIFI_Init(void);
WIFI_Status_t       WIFI_ListAccessPoints(WIFI_APs_t *APs, uint8_t AP_MaxNbr);
WIFI_Status_t       WIFI_Connect(
                             const char* SSID,
                             const char* Password,
                             WIFI_Ecn_t ecn);
WIFI_Status_t       WIFI_IsConnected();
WIFI_Status_t       WIFI_GetIP_Address(uint8_t  *ipaddr);
WIFI_Status_t       WIFI_GetIP_Mask(uint8_t  *mask);
WIFI_Status_t       WIFI_GetDNS_Address (uint8_t  *DNS1addr,uint8_t  *DNS2addr);
WIFI_Status_t       WIFI_GetGateway_Address (uint8_t  *Gateway_addr);
WIFI_Status_t       WIFI_GetMAC_Address(uint8_t  *mac);

WIFI_Status_t       WIFI_Disconnect(void);
WIFI_Status_t       WIFI_ConfigureAP(
                                        uint8_t *ssid,
                                        uint8_t *pass,
                                        WIFI_Ecn_t ecn,
                                        uint8_t channel,
                                        uint8_t max_conn);

WIFI_Status_t       WIFI_HandleAPEvents(WIFI_APSettings_t *setting);
WIFI_Status_t       WIFI_Ping(uint8_t *ipaddr, uint16_t count, uint16_t interval_ms,int32_t result[]);
WIFI_Status_t       WIFI_GetHostAddress(const char *location, uint8_t *ipaddr);
WIFI_Status_t       WIFI_OpenClientConnection(uint32_t socket, WIFI_Protocol_t protocol, const char *name, uint8_t *ipaddr, uint16_t port, uint16_t local_port);
WIFI_Status_t       WIFI_CloseClientConnection(uint32_t socket);

WIFI_Status_t       WIFI_StartServer(uint32_t socket, WIFI_Protocol_t type, uint16_t backlog, const char *name, uint16_t port);
WIFI_Status_t       WIFI_WaitServerConnection(int socket,uint32_t Timeout,uint8_t *remoteipaddr, uint16_t *remoteport);
WIFI_Status_t       WIFI_CloseServerConnection(int socket);
WIFI_Status_t       WIFI_StopServer(uint32_t socket);

WIFI_Status_t       WIFI_SendData(uint8_t socket, uint8_t *pdata, uint16_t Reqlen, uint16_t *SentDatalen, uint32_t Timeout);
WIFI_Status_t       WIFI_SendDataTo(uint8_t socket, uint8_t *pdata, uint16_t Reqlen, uint16_t *SentDatalen, uint32_t Timeout, uint8_t *ipaddr, uint16_t port);
WIFI_Status_t       WIFI_ReceiveData(uint8_t socket, uint8_t *pdata, uint16_t Reqlen, uint16_t *RcvDatalen, uint32_t Timeout);
WIFI_Status_t       WIFI_ReceiveDataFrom(uint8_t socket, uint8_t *pdata, uint16_t Reqlen, uint16_t *RcvDatalen, uint32_t Timeout, uint8_t *ipaddr, uint16_t *port);
WIFI_Status_t       WIFI_StartClient(void);
WIFI_Status_t       WIFI_StopClient(void);

WIFI_Status_t       WIFI_SetOEMProperties(const char *name, uint8_t *Mac);
WIFI_Status_t       WIFI_ResetModule(void);
WIFI_Status_t       WIFI_SetModuleDefault(void);
WIFI_Status_t       WIFI_ModuleFirmwareUpdate(const char *url);
WIFI_Status_t       WIFI_GetModuleID(char *Id);
WIFI_Status_t       WIFI_GetModuleFwRevision(char *rev);
WIFI_Status_t       WIFI_GetModuleName(char *ModuleName);

WIFI_Status_t       WIFI_StoreCA(uint8_t credSet, uint8_t* ca, uint16_t caLength);
#ifdef __cplusplus
}
#endif

#endif /* WIFI_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

// clang-format on
