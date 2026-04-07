// clang-format off

/**
  ******************************************************************************
  * @file    es_wifi_conf.h
  * @author  MCD Application Team
  * @brief   ES-WIFI configuration.
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

#ifndef ES_WIFI_CONF_H
#define ES_WIFI_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif  

#include <tx_api.h>
extern TX_MUTEX WiFiMutex;

#define LOCK_WIFI()             tx_mutex_get(&WiFiMutex, TX_WAIT_FOREVER)
#define UNLOCK_WIFI()           tx_mutex_put(&WiFiMutex)
#define LOCK_SPI()
#define UNLOCK_SPI()
#define SEM_SIGNAL(a)
#define SPI_INTERFACE_PRIO              0

#define ES_WIFI_MAX_SSID_NAME_SIZE                  32
#define ES_WIFI_MAX_PSWD_NAME_SIZE                  32
#define ES_WIFI_PRODUCT_ID_SIZE                     32
#define ES_WIFI_PRODUCT_NAME_SIZE                   32
#define ES_WIFI_FW_REV_SIZE                         24
#define ES_WIFI_API_REV_SIZE                        16
#define ES_WIFI_STACK_REV_SIZE                      16
#define ES_WIFI_RTOS_REV_SIZE                       16

#define ES_WIFI_DATA_SIZE                           2000  /*Increased from 1400 to fit scan result.*/
#define ES_WIFI_MAX_DETECTED_AP                     10

#ifndef ES_WIFI_TIMEOUT   
#define ES_WIFI_TIMEOUT                             5000
#endif /* ES_WIFI_TIMEOUT  */
#ifndef ES_WIFI_TRANSPORT_TIMEOUT
#define ES_WIFI_TRANSPORT_TIMEOUT                   30000
#endif /* ES_WIFI_TRANSPORT_TIMEOUT  */
                                                    
#define ES_WIFI_USE_PING                            1
#define ES_WIFI_USE_AWS                             0
#define ES_WIFI_USE_FIRMWAREUPDATE                  0
#define ES_WIFI_USE_WPS                             0
                                                    
#define ES_WIFI_USE_SPI                             1  
#define ES_WIFI_USE_UART                            (!ES_WIFI_USE_SPI)
   


#ifdef __cplusplus
}
#endif
#endif /* ES_WIFI_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

// clang-format on