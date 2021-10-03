/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _AZURE_IOT_DPS_MQTT_H
#define _AZURE_IOT_DPS_MQTT_H

#include "tx_api.h"
#include "nx_api.h"

#include "azure_iot_mqtt.h"

UINT azure_iot_dps_create(AZURE_IOT_MQTT* azure_iot_mqtt, NX_IP* nx_ip, NX_PACKET_POOL* nx_pool);
UINT azure_iot_dps_delete(AZURE_IOT_MQTT* azure_iot_mqtt);
UINT azure_iot_dps_register(AZURE_IOT_MQTT* azure_iot_mqtt, UINT wait);

#endif