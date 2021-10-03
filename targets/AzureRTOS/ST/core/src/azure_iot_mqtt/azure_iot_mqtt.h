/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _AZURE_IOT_MQTT_H
#define _AZURE_IOT_MQTT_H

#include <stdbool.h>

#include "tx_api.h"

#include "nx_api.h"
#include "nxd_dns.h"
#include "nxd_mqtt_client.h"

#include "azure_iot_ciphersuites.h"

#define AZURE_IOT_MQTT_HOSTNAME_SIZE           100
#define AZURE_IOT_MQTT_DEVICE_ID_SIZE          64
#define AZURE_IOT_MQTT_USERNAME_SIZE           256
#define AZURE_IOT_MQTT_PASSWORD_SIZE           256
#define AZURE_IOT_MQTT_TOPIC_NAME_LENGTH       256
#define AZURE_IOT_MQTT_MESSAGE_LENGTH          1024
#define AZURE_IOT_MQTT_DIRECT_COMMAND_RID_SIZE 6

#define AZURE_IOT_MQTT_CLIENT_STACK_SIZE 4096
#define AZURE_IOT_MQTT_CERT_BUFFER_SIZE 4096

#define TLS_PACKET_BUFFER 4096

#define MQTT_QOS_0 0 // QoS 0 - Deliver at most once
#define MQTT_QOS_1 1 // QoS 1 - Deliver at least once
#define MQTT_QOS_2 2 // QoS 2 - Deliver exactly once

typedef struct AZURE_IOT_MQTT_STRUCT AZURE_IOT_MQTT;

typedef void (*func_ptr_direct_method)(AZURE_IOT_MQTT*, CHAR*, CHAR*);
typedef void (*func_ptr_c2d_message)(AZURE_IOT_MQTT*, CHAR*, CHAR*);
typedef void (*func_ptr_device_twin_desired_prop)(AZURE_IOT_MQTT*, CHAR*);
typedef void (*func_ptr_device_twin_prop)(AZURE_IOT_MQTT*, CHAR*);
typedef ULONG (*func_ptr_unix_time_get)(VOID);

struct AZURE_IOT_MQTT_STRUCT
{
    NXD_MQTT_CLIENT nxd_mqtt_client;
    NX_DNS* nx_dns;

    // TX_MUTEX mqtt_mutex;
    TX_EVENT_FLAGS_GROUP mqtt_event_flags;
    UINT mqtt_dps_status;

    // Hub config
    CHAR mqtt_hub_hostname[AZURE_IOT_MQTT_HOSTNAME_SIZE];

    // DPS config
    CHAR* mqtt_dps_id_scope;
    CHAR* mqtt_dps_registration_id;

    // Device config
    CHAR mqtt_device_id[AZURE_IOT_MQTT_DEVICE_ID_SIZE];
    CHAR* mqtt_sas_key;
    CHAR* mqtt_model_id;

    UINT dps_retry_interval;

    UINT reported_property_version;
    UINT desired_property_version;
    CHAR direct_command_request_id[AZURE_IOT_MQTT_DIRECT_COMMAND_RID_SIZE];

    CHAR mqtt_username[AZURE_IOT_MQTT_USERNAME_SIZE];
    CHAR mqtt_password[AZURE_IOT_MQTT_PASSWORD_SIZE];

    CHAR mqtt_receive_topic_buffer[AZURE_IOT_MQTT_TOPIC_NAME_LENGTH];
    CHAR mqtt_receive_message_buffer[AZURE_IOT_MQTT_MESSAGE_LENGTH];

    ULONG mqtt_client_stack[AZURE_IOT_MQTT_CLIENT_STACK_SIZE / sizeof(ULONG)];

    ULONG tls_metadata_buffer[NX_AZURE_IOT_TLS_METADATA_BUFFER_SIZE / sizeof(ULONG)];
    UCHAR tls_packet_buffer[TLS_PACKET_BUFFER];

    NX_SECURE_X509_CERT mqtt_remote_certificate;	
    UCHAR mqtt_remote_cert_buffer[AZURE_IOT_MQTT_CERT_BUFFER_SIZE];

    func_ptr_direct_method cb_ptr_mqtt_invoke_direct_method;
    func_ptr_c2d_message cb_ptr_mqtt_c2d_message;
    func_ptr_device_twin_desired_prop cb_ptr_mqtt_device_twin_desired_prop_callback;
    func_ptr_device_twin_prop cb_ptr_mqtt_device_twin_prop_callback;

    func_ptr_unix_time_get unix_time_get;
};

UINT azure_iot_mqtt_register_direct_method_callback(
    AZURE_IOT_MQTT* azure_iot_mqtt, func_ptr_direct_method mqtt_direct_method_callback);
UINT azure_iot_mqtt_register_c2d_message_callback(
    AZURE_IOT_MQTT* azure_iot_mqtt, func_ptr_c2d_message mqtt_c2d_message_callback);
UINT azure_iot_mqtt_register_device_twin_desired_prop_callback(
    AZURE_IOT_MQTT* azure_iot_mqtt, func_ptr_device_twin_desired_prop mqtt_device_twin_desired_prop_update_callback);
UINT azure_iot_mqtt_register_device_twin_prop_callback(
    AZURE_IOT_MQTT* azure_iot_mqtt, func_ptr_device_twin_prop mqtt_device_twin_prop_callback);

UINT tls_setup(NXD_MQTT_CLIENT* client,
    NX_SECURE_TLS_SESSION* tls_session,
    NX_SECURE_X509_CERT* cert,
    NX_SECURE_X509_CERT* trusted_cert);

UINT mqtt_publish(AZURE_IOT_MQTT* azure_iot_mqtt, CHAR* topic, CHAR* message);

UINT azure_iot_mqtt_publish_float_property(AZURE_IOT_MQTT* azure_iot_mqtt, CHAR* label, float value);
UINT azure_iot_mqtt_publish_bool_property(AZURE_IOT_MQTT* azure_iot_mqtt, CHAR* label, bool value);
UINT azure_iot_mqtt_publish_float_telemetry(AZURE_IOT_MQTT* azure_iot_mqtt, CHAR* label, float value);
UINT azure_iot_mqtt_publish_int_writeable_property(AZURE_IOT_MQTT* azure_iot_mqtt, CHAR* label, int value);
UINT azure_iot_mqtt_respond_int_writeable_property(
    AZURE_IOT_MQTT* azure_iot_mqtt, CHAR* label, int value, int http_status);
UINT azure_iot_mqtt_respond_direct_method(AZURE_IOT_MQTT* azure_iot_mqtt, UINT response);
UINT azure_iot_mqtt_device_twin_request(AZURE_IOT_MQTT* azure_iot_mqtt);

UINT azure_iot_mqtt_create(AZURE_IOT_MQTT* azure_iot_mqtt,
    NX_IP* nx_ip,
    NX_PACKET_POOL* nx_pool,
    NX_DNS* nx_dns,
    func_ptr_unix_time_get unix_time_get,
    CHAR* iot_hub_hostname,
    CHAR* iot_device_id,
    CHAR* iot_sas_key,
    CHAR* iot_model_id);
UINT azure_iot_mqtt_create_with_dps(AZURE_IOT_MQTT* azure_iot_mqtt,
    NX_IP* nx_ip,
    NX_PACKET_POOL* nx_pool,
    NX_DNS* nx_dns,
    func_ptr_unix_time_get unix_time_get,
    CHAR* iot_dps_id_scope,
    CHAR* iot_device_id,
    CHAR* iot_sas_key,
    CHAR* iot_model_id);
UINT azure_iot_mqtt_delete(AZURE_IOT_MQTT* azure_iot_mqtt);

UINT azure_iot_mqtt_connect(AZURE_IOT_MQTT* azure_iot_mqtt);
UINT azure_iot_mqtt_disconnect(AZURE_IOT_MQTT* azure_iot_mqtt);

#endif // _AZURE_IOT_MQTT_H