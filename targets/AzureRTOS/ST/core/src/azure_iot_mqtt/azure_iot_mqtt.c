/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "azure_iot_mqtt.h"

#include <ctype.h>
#include <string.h>

#include "tx_api.h"

#include "nx_api.h"
#include "nxd_mqtt_client.h"

#include "azure_iot_cert.h"
#include "azure_iot_mqtt/azure_iot_dps_mqtt.h"
#include "azure_iot_mqtt/sas_token.h"

#define USERNAME                "%s/%s/?api-version=2020-09-30&model-id=%s"
#define PUBLISH_TELEMETRY_TOPIC "devices/%s/messages/events/"

#define DEVICE_MESSAGE_BASE  "messages/devicebound/"
#define DEVICE_MESSAGE_TOPIC "devices/%s/messages/devicebound/#"

#define DEVICE_TWIN_PUBLISH_TOPIC          "$iothub/twin/PATCH/properties/reported/?$rid=%d"
#define DEVICE_TWIN_REQUEST_TOPIC          "$iothub/twin/GET/?$rid=%d"
#define DEVICE_TWIN_RES_BASE               "$iothub/twin/res/"
#define DEVICE_TWIN_RES_TOPIC              "$iothub/twin/res/#"
#define DEVICE_TWIN_DESIRED_PROP_RES_BASE  "$iothub/twin/PATCH/properties/desired/"
#define DEVICE_TWIN_DESIRED_PROP_RES_TOPIC "$iothub/twin/PATCH/properties/desired/#"

#define DIRECT_METHOD_RECEIVE  "$iothub/methods/POST/"
#define DIRECT_METHOD_TOPIC    "$iothub/methods/POST/#"
#define DIRECT_METHOD_RESPONSE "$iothub/methods/res/%d/?$rid=%s"

#define MQTT_CLIENT_PRIORITY 2
#define MQTT_TIMEOUT         (10 * TX_TIMER_TICKS_PER_SECOND)
#define MQTT_KEEP_ALIVE      240

CHAR* azure_iot_x509_hostname;

static ULONG azure_iot_certificate_verify(NX_SECURE_TLS_SESSION* session, NX_SECURE_X509_CERT* certificate)
{
    UINT status;

    // Check certicate matches the correct address
    status = nx_secure_x509_common_name_dns_check(
        certificate, (UCHAR*)azure_iot_x509_hostname, strlen(azure_iot_x509_hostname));
    if (status)
    {
        printf("Error in certificate verification: DNS name did not match CN\r\n");
    }

    return status;
}

UINT azure_iot_mqtt_register_direct_method_callback(
    AZURE_IOT_MQTT* azure_iot_mqtt, func_ptr_direct_method mqtt_direct_method_callback)
{
    if (azure_iot_mqtt == NULL || azure_iot_mqtt->cb_ptr_mqtt_invoke_direct_method != NULL)
    {
        return NX_PTR_ERROR;
    }

    azure_iot_mqtt->cb_ptr_mqtt_invoke_direct_method = mqtt_direct_method_callback;
    return NX_SUCCESS;
}

UINT azure_iot_mqtt_register_c2d_message_callback(
    AZURE_IOT_MQTT* azure_iot_mqtt, func_ptr_c2d_message mqtt_c2d_message_callback)
{
    if (azure_iot_mqtt == NULL || azure_iot_mqtt->cb_ptr_mqtt_c2d_message != NULL)
    {
        return NX_PTR_ERROR;
    }

    azure_iot_mqtt->cb_ptr_mqtt_c2d_message = mqtt_c2d_message_callback;
    return NX_SUCCESS;
}

UINT azure_iot_mqtt_register_device_twin_desired_prop_callback(
    AZURE_IOT_MQTT* azure_iot_mqtt, func_ptr_device_twin_desired_prop mqtt_device_twin_desired_prop_callback)
{
    if (azure_iot_mqtt == NULL || azure_iot_mqtt->cb_ptr_mqtt_device_twin_desired_prop_callback != NULL)
    {
        return NX_PTR_ERROR;
    }

    azure_iot_mqtt->cb_ptr_mqtt_device_twin_desired_prop_callback = mqtt_device_twin_desired_prop_callback;
    return NX_SUCCESS;
}

UINT azure_iot_mqtt_register_device_twin_prop_callback(
    AZURE_IOT_MQTT* azure_iot_mqtt, func_ptr_device_twin_prop mqtt_device_twin_prop_callback)
{
    if (azure_iot_mqtt == NULL || azure_iot_mqtt->cb_ptr_mqtt_device_twin_prop_callback != NULL)
    {
        return NX_PTR_ERROR;
    }

    azure_iot_mqtt->cb_ptr_mqtt_device_twin_prop_callback = mqtt_device_twin_prop_callback;
    return NX_SUCCESS;
}

UINT tls_setup(NXD_MQTT_CLIENT* client,
    NX_SECURE_TLS_SESSION* tls_session,
    NX_SECURE_X509_CERT* cert,
    NX_SECURE_X509_CERT* trusted_cert)
{
    UINT status;

    AZURE_IOT_MQTT* azure_iot_mqtt = (AZURE_IOT_MQTT*)client->nxd_mqtt_packet_receive_context;

    // Create TLS session.
    status = _nx_secure_tls_session_create_ext(tls_session,
        _nx_azure_iot_tls_supported_crypto,
        _nx_azure_iot_tls_supported_crypto_size,
        _nx_azure_iot_tls_ciphersuite_map,
        _nx_azure_iot_tls_ciphersuite_map_size,
        azure_iot_mqtt->tls_metadata_buffer,
        sizeof(azure_iot_mqtt->tls_metadata_buffer));
    if (status != NX_SUCCESS)
    {
        printf("Failed to create TLS session status (0x%04x)\r\n", status);
        return status;
    }

    status = nx_secure_tls_remote_certificate_allocate(tls_session,	
        &azure_iot_mqtt->mqtt_remote_certificate,	
        azure_iot_mqtt->mqtt_remote_cert_buffer,	
        sizeof(azure_iot_mqtt->mqtt_remote_cert_buffer));	
    if (status != NX_SUCCESS)	
    {	
        printf("Failed to create remote certificate buffer (0x%04x)\r\n", status);	
        return status;	
    }

    // Add a CA Certificate to our trusted store for verifying incoming server certificates
    status = nx_secure_x509_certificate_initialize(trusted_cert,
        (UCHAR*)azure_iot_root_ca,
        azure_iot_root_ca_len,
        NX_NULL,
        0,
        NX_NULL,
        0,
        NX_SECURE_X509_KEY_TYPE_NONE);
    if (status != NX_SUCCESS)
    {
        printf("Unable to initialize CA certificate (0x%04x)\r\n", status);
        return status;
    }

    status = nx_secure_tls_trusted_certificate_add(tls_session, trusted_cert);
    if (status != NX_SUCCESS)
    {
        printf("Unable to add CA certificate to trusted store (0x%04x)\r\n", status);
        return status;
    }

    status = nx_secure_tls_session_packet_buffer_set(
        tls_session, azure_iot_mqtt->tls_packet_buffer, sizeof(azure_iot_mqtt->tls_packet_buffer));
    if (status != NX_SUCCESS)
    {
        printf("Could not set TLS session packet buffer (0x%02x)\r\n", status);
        return status;
    }

    // Setup the callback invoked when TLS has a certificate it wants to verify so we can
    // do additional checks not done automatically by TLS.
    status = nx_secure_tls_session_certificate_callback_set(tls_session, azure_iot_certificate_verify);
    if (status)
    {
        printf("Failed to set the session certificate callback: status: %d", status);
        return status;
    }

    // Add a timestamp function for time checking and timestamps in the TLS handshake
    nx_secure_tls_session_time_function_set(tls_session, azure_iot_mqtt->unix_time_get);

    return NX_SUCCESS;
}

UINT mqtt_publish(AZURE_IOT_MQTT* azure_iot_mqtt, CHAR* topic, CHAR* message)
{
    UINT status = nxd_mqtt_client_publish(&azure_iot_mqtt->nxd_mqtt_client,
        topic,
        strlen(topic),
        message,
        strlen(message),
        NX_FALSE,
        MQTT_QOS_1,
        NX_WAIT_FOREVER);
    if (status != NX_SUCCESS)
    {
        printf("Failed to publish %s (0x%02x)\r\n", message, status);
    }

    return status;
}

static UINT mqtt_publish_float(AZURE_IOT_MQTT* azure_iot_mqtt, CHAR* topic, CHAR* label, float value)
{
    CHAR mqtt_message[100];

    int decvalue  = value;
    int fracvalue = abs(100 * (value - (long)value));

    snprintf(mqtt_message, sizeof(mqtt_message), "{\"%s\":%d.%02d}", label, decvalue, fracvalue);
    printf("Sending message %s\r\n", mqtt_message);

    return mqtt_publish(azure_iot_mqtt, topic, mqtt_message);
}

static UINT mqtt_publish_bool(AZURE_IOT_MQTT* azure_iot_mqtt, CHAR* topic, CHAR* label, bool value)
{
    CHAR mqtt_message[200];

    snprintf(mqtt_message, sizeof(mqtt_message), "{\"%s\":%s}", label, (value ? "true" : "false"));
    printf("Sending message %s\r\n", mqtt_message);

    return mqtt_publish(azure_iot_mqtt, topic, mqtt_message);
}

static VOID process_direct_method(AZURE_IOT_MQTT* azure_iot_mqtt, CHAR* topic, CHAR* message)
{
    INT direct_method_receive_size = sizeof(DIRECT_METHOD_RECEIVE) - 1;
    CHAR direct_method_name[64]    = {0};

    CHAR* location = topic + direct_method_receive_size;
    CHAR* find;

    find = strchr(location, '/');
    if (find == 0)
    {
        return;
    }

    strncpy(direct_method_name, location, find - location);

    location = find;

    find = strstr(location, "$rid=");
    if (find == 0)
    {
        printf("Error: failed to parse direct method rid\r\n");
        return;
    }

    location = find + 5;
    strncpy(azure_iot_mqtt->direct_command_request_id, location, AZURE_IOT_MQTT_DIRECT_COMMAND_RID_SIZE);

    printf("Received direct method=%s, rid=%s, message=%s\r\n",
        direct_method_name,
        azure_iot_mqtt->direct_command_request_id,
        message);

    if (azure_iot_mqtt->cb_ptr_mqtt_invoke_direct_method == NULL)
    {
        printf("No callback is registered for MQTT direct method invoke\r\n");
        return;
    }

    azure_iot_mqtt->cb_ptr_mqtt_invoke_direct_method(azure_iot_mqtt, direct_method_name, message);
}

static VOID process_c2d_message(AZURE_IOT_MQTT* azure_iot_mqtt, CHAR* topic, CHAR* message)
{
    CHAR* properties;

    // Get to parameters list
    if ((properties = strstr(topic, ".to")) == 0)
    {
        printf("Received C2D message has no parameter list\r\n");
        return;
    }

    // Find the properties
    if ((properties = strstr(properties, "&")) == 0)
    {
        // No properties, point at the null terminator
        properties = topic + strlen(topic);
    }
    else
    {
        // Skip over the '&'
        properties++;
    }

    if (azure_iot_mqtt->cb_ptr_mqtt_c2d_message == NULL)
    {
        printf("No callback is registered for MQTT cloud to device message processing\r\n");
        return;
    }

    azure_iot_mqtt->cb_ptr_mqtt_c2d_message(azure_iot_mqtt, properties, message);
}

static VOID process_device_twin_response(AZURE_IOT_MQTT* azure_iot_mqtt, CHAR* topic, CHAR* message)
{
    INT response_status;

    // Parse the device twin response status
    CHAR* location = topic + sizeof(DEVICE_TWIN_RES_BASE) - 1;
    CHAR* find;

    find = strchr(location, '/');
    if (find == 0)
    {
        return;
    }

    response_status = atoi(location);

    printf("Processed device twin update response with status=%d\r\n", response_status);

    if (response_status == 200)
    {
        azure_iot_mqtt->cb_ptr_mqtt_device_twin_prop_callback(azure_iot_mqtt, message);
    }
}

static VOID process_device_twin_desired_prop_update(AZURE_IOT_MQTT* azure_iot_mqtt, CHAR* topic, CHAR* message)
{
    printf("Received device twin desired property\r\n");

    // Parse the device twin version
    CHAR* location = topic + sizeof(DEVICE_TWIN_DESIRED_PROP_RES_BASE) - 1;

    location = strstr(location, "$version=");
    if (location == 0)
    {
        printf("Error: Failed to parse version from desired property update\r\n");
        return;
    }

    azure_iot_mqtt->desired_property_version = atoi(location + 9);

    azure_iot_mqtt->cb_ptr_mqtt_device_twin_desired_prop_callback(azure_iot_mqtt, message);
}

static VOID mqtt_disconnect_cb(NXD_MQTT_CLIENT* client_ptr)
{
    printf("ERROR: MQTT disconnected, reconnecting...\r\n");

    AZURE_IOT_MQTT* azure_iot_mqtt = (AZURE_IOT_MQTT*)client_ptr;

    // Try and reconnect forever
    while (azure_iot_mqtt_connect(azure_iot_mqtt) != NX_SUCCESS)
    {
        tx_thread_sleep(10 * TX_TIMER_TICKS_PER_SECOND);
    }
}

static VOID mqtt_notify_cb(NXD_MQTT_CLIENT* client_ptr, UINT number_of_messages)
{
    UINT actual_topic_length;
    UINT actual_message_length;
    UINT status;

    AZURE_IOT_MQTT* azure_iot_mqtt = (AZURE_IOT_MQTT*)client_ptr->nxd_mqtt_packet_receive_context;

    for (UINT count = 0; count < number_of_messages; ++count)
    {
        // Get the mqtt client message
        status = nxd_mqtt_client_message_get(client_ptr,
            (UCHAR*)azure_iot_mqtt->mqtt_receive_topic_buffer,
            AZURE_IOT_MQTT_TOPIC_NAME_LENGTH,
            &actual_topic_length,
            (UCHAR*)azure_iot_mqtt->mqtt_receive_message_buffer,
            AZURE_IOT_MQTT_MESSAGE_LENGTH,
            &actual_message_length);
        if (status != NXD_MQTT_SUCCESS)
        {
            printf("ERROR: nxd_mqtt_client_message_get failed (0x%02x)\r\n", status);
            continue;
        }

        // Append null string terminators
        azure_iot_mqtt->mqtt_receive_topic_buffer[actual_topic_length]     = 0;
        azure_iot_mqtt->mqtt_receive_message_buffer[actual_message_length] = 0;

        if (strstr((CHAR*)azure_iot_mqtt->mqtt_receive_topic_buffer, DIRECT_METHOD_RECEIVE))
        {
            process_direct_method(
                azure_iot_mqtt, azure_iot_mqtt->mqtt_receive_topic_buffer, azure_iot_mqtt->mqtt_receive_message_buffer);
        }
        else if (strstr((CHAR*)azure_iot_mqtt->mqtt_receive_topic_buffer, DEVICE_MESSAGE_BASE))
        {
            process_c2d_message(
                azure_iot_mqtt, azure_iot_mqtt->mqtt_receive_topic_buffer, azure_iot_mqtt->mqtt_receive_message_buffer);
        }
        else if (strstr((CHAR*)azure_iot_mqtt->mqtt_receive_topic_buffer, DEVICE_TWIN_RES_BASE))
        {
            process_device_twin_response(
                azure_iot_mqtt, azure_iot_mqtt->mqtt_receive_topic_buffer, azure_iot_mqtt->mqtt_receive_message_buffer);
        }
        else if (strstr((CHAR*)azure_iot_mqtt->mqtt_receive_topic_buffer, DEVICE_TWIN_DESIRED_PROP_RES_BASE))
        {
            process_device_twin_desired_prop_update(
                azure_iot_mqtt, azure_iot_mqtt->mqtt_receive_topic_buffer, azure_iot_mqtt->mqtt_receive_message_buffer);
        }
        else
        {
            printf("Unknown topic received, no custom processing specified\r\n");
        }
    }
}

static UINT azure_iot_mqtt_create_common(AZURE_IOT_MQTT* azure_iot_mqtt, NX_IP* nx_ip, NX_PACKET_POOL* nx_pool)
{
    UINT status;

    printf("Initializing MQTT Hub client\r\n");

    status = nxd_mqtt_client_create(&azure_iot_mqtt->nxd_mqtt_client,
        "MQTT client",
        azure_iot_mqtt->mqtt_device_id,
        strlen(azure_iot_mqtt->mqtt_device_id),
        nx_ip,
        nx_pool,
        azure_iot_mqtt->mqtt_client_stack,
        AZURE_IOT_MQTT_CLIENT_STACK_SIZE,
        MQTT_CLIENT_PRIORITY,
        NX_NULL,
        0);
    if (status != NXD_MQTT_SUCCESS)
    {
        printf("Failed to create MQTT Client (0x%02x)\r\n", status);
        return status;
    }

    status = nxd_mqtt_client_receive_notify_set(&azure_iot_mqtt->nxd_mqtt_client, mqtt_notify_cb);
    if (status != NXD_MQTT_SUCCESS)
    {
        printf("Error in setting receive notify (0x%02x)\r\n", status);
        nxd_mqtt_client_delete(&azure_iot_mqtt->nxd_mqtt_client);
        return status;
    }

    status = nxd_mqtt_client_disconnect_notify_set(&azure_iot_mqtt->nxd_mqtt_client, mqtt_disconnect_cb);
    if (status != NXD_MQTT_SUCCESS)
    {
        printf("Error in seting disconnect notification (0x%02x)\r\n", status);
        nxd_mqtt_client_delete(&azure_iot_mqtt->nxd_mqtt_client);
        return status;
    }

    // Set the receive context (highjacking the packet_receive_context) for callbacks
    azure_iot_mqtt->nxd_mqtt_client.nxd_mqtt_packet_receive_context = azure_iot_mqtt;

    return NXD_MQTT_SUCCESS;
}

// Interact with Azure MQTT
UINT azure_iot_mqtt_publish_float_property(AZURE_IOT_MQTT* azure_iot_mqtt, CHAR* label, float value)
{
    CHAR mqtt_publish_topic[100];
    UINT status;

    printf("Sending device twin update with float value\r\n");

    snprintf(mqtt_publish_topic,
        sizeof(mqtt_publish_topic),
        DEVICE_TWIN_PUBLISH_TOPIC,
        azure_iot_mqtt->reported_property_version++);

    status = mqtt_publish_float(azure_iot_mqtt, mqtt_publish_topic, label, value);

    return status;
}

UINT azure_iot_mqtt_publish_bool_property(AZURE_IOT_MQTT* azure_iot_mqtt, CHAR* label, bool value)
{
    CHAR mqtt_publish_topic[100];

    printf("Sending device twin update with bool value\r\n");

    snprintf(mqtt_publish_topic,
        sizeof(mqtt_publish_topic),
        DEVICE_TWIN_PUBLISH_TOPIC,
        azure_iot_mqtt->reported_property_version++);

    return mqtt_publish_bool(azure_iot_mqtt, mqtt_publish_topic, label, value);
}

UINT azure_iot_mqtt_publish_float_telemetry(AZURE_IOT_MQTT* azure_iot_mqtt, CHAR* label, float value)
{
    CHAR mqtt_publish_topic[100];

    printf("Sending telemetry with float value\r\n");

    snprintf(mqtt_publish_topic,
        sizeof(mqtt_publish_topic),
        PUBLISH_TELEMETRY_TOPIC,
        azure_iot_mqtt->nxd_mqtt_client.nxd_mqtt_client_id);

    return mqtt_publish_float(azure_iot_mqtt, mqtt_publish_topic, label, value);
}

UINT azure_iot_mqtt_publish_int_writeable_property(AZURE_IOT_MQTT* azure_iot_mqtt, CHAR* label, int value)
{
    CHAR mqtt_publish_topic[100];
    CHAR mqtt_publish_message[100];

    printf("Reporting writeable property %s as %d\r\n", label, value);

    snprintf(mqtt_publish_topic,
        sizeof(mqtt_publish_topic),
        DEVICE_TWIN_PUBLISH_TOPIC,
        azure_iot_mqtt->reported_property_version++);

    snprintf(mqtt_publish_message,
        sizeof(mqtt_publish_message),
        "{\"%s\":{\"value\":%d,\"ac\":200,\"av\":1}}",
        label,
        value);

    return mqtt_publish(azure_iot_mqtt, mqtt_publish_topic, mqtt_publish_message);
}

UINT azure_iot_mqtt_respond_int_writeable_property(
    AZURE_IOT_MQTT* azure_iot_mqtt, CHAR* label, int value, int http_status)
{
    CHAR mqtt_publish_topic[100];
    CHAR mqtt_publish_message[100];

    printf("Responding to writeable property %s = %d\r\n", label, value);

    snprintf(mqtt_publish_topic,
        sizeof(mqtt_publish_topic),
        DEVICE_TWIN_PUBLISH_TOPIC,
        azure_iot_mqtt->reported_property_version++);

    snprintf(mqtt_publish_message,
        sizeof(mqtt_publish_message),
        "{\"%s\":{\"value\":%d,\"ac\":%d,\"av\":%d}}",
        label,
        value,
        http_status,
        azure_iot_mqtt->desired_property_version);

    return mqtt_publish(azure_iot_mqtt, mqtt_publish_topic, mqtt_publish_message);
}

UINT azure_iot_mqtt_respond_direct_method(AZURE_IOT_MQTT* azure_iot_mqtt, UINT response)
{
    CHAR mqtt_publish_topic[100];

    printf("Responding to direct command property with status:%d, rid:%s\r\n",
        response,
        azure_iot_mqtt->direct_command_request_id);

    snprintf(mqtt_publish_topic,
        sizeof(mqtt_publish_topic),
        DIRECT_METHOD_RESPONSE,
        response,
        azure_iot_mqtt->direct_command_request_id);

    return mqtt_publish(azure_iot_mqtt, mqtt_publish_topic, "{}");
}

UINT azure_iot_mqtt_device_twin_request(AZURE_IOT_MQTT* azure_iot_mqtt)
{
    CHAR mqtt_publish_topic[100];

    printf("Requesting device twin model\r\n");

    snprintf(mqtt_publish_topic, sizeof(mqtt_publish_topic), DEVICE_TWIN_REQUEST_TOPIC, 0);

    // Publish an empty message to request the device twin
    return mqtt_publish(azure_iot_mqtt, mqtt_publish_topic, "{}");
}

UINT azure_iot_mqtt_create(AZURE_IOT_MQTT* azure_iot_mqtt,
    NX_IP* nx_ip,
    NX_PACKET_POOL* nx_pool,
    NX_DNS* nx_dns,
    func_ptr_unix_time_get unix_time_get,
    CHAR* iot_hub_hostname,
    CHAR* iot_device_id,
    CHAR* iot_sas_key,
    CHAR* iot_model_id)
{
    if (azure_iot_mqtt == NULL)
    {
        printf("ERROR: azure_iot_mqtt is NULL\r\n");
        return NX_PTR_ERROR;
    }

    if (iot_hub_hostname[0] == 0 || iot_device_id[0] == 0 || iot_sas_key[0] == 0)
    {
        printf("ERROR: IoT Hub connection configuration is empty\r\n");
        return NX_PTR_ERROR;
    }

    memset(azure_iot_mqtt, 0, sizeof(*azure_iot_mqtt));

    // Stash the connection information
    azure_iot_mqtt->nx_dns        = nx_dns;
    azure_iot_mqtt->unix_time_get = unix_time_get;
    strncpy(azure_iot_mqtt->mqtt_hub_hostname, iot_hub_hostname, AZURE_IOT_MQTT_HOSTNAME_SIZE);
    strncpy(azure_iot_mqtt->mqtt_device_id, iot_device_id, AZURE_IOT_MQTT_DEVICE_ID_SIZE);
    azure_iot_mqtt->mqtt_sas_key  = iot_sas_key;
    azure_iot_mqtt->mqtt_model_id = iot_model_id;

    // call into common code
    return azure_iot_mqtt_create_common(azure_iot_mqtt, nx_ip, nx_pool);
}

UINT azure_iot_mqtt_create_with_dps(AZURE_IOT_MQTT* azure_iot_mqtt,
    NX_IP* nx_ip,
    NX_PACKET_POOL* nx_pool,
    NX_DNS* nx_dns,
    func_ptr_unix_time_get unix_time_get,
    CHAR* iot_dps_id_scope,
    CHAR* iot_registration_id,
    CHAR* iot_sas_key,
    CHAR* iot_model_id)
{
    UINT status;

    printf("Initializing MQTT DPS client\r\n");

    if (azure_iot_mqtt == NULL)
    {
        printf("ERROR: azure_iot_mqtt is NULL\r\n");
        return NX_PTR_ERROR;
    }

    if (iot_dps_id_scope[0] == 0 || iot_registration_id[0] == 0 || iot_sas_key[0] == 0)
    {
        printf("ERROR: IoT DPS connection configuration is empty\r\n");
        return NX_PTR_ERROR;
    }

    memset(azure_iot_mqtt, 0, sizeof(*azure_iot_mqtt));

    // Stash the connection information
    azure_iot_mqtt->nx_dns                   = nx_dns;
    azure_iot_mqtt->unix_time_get            = unix_time_get;
    azure_iot_mqtt->mqtt_dps_id_scope        = iot_dps_id_scope;
    azure_iot_mqtt->mqtt_dps_registration_id = iot_registration_id;
    azure_iot_mqtt->mqtt_sas_key             = iot_sas_key;
    azure_iot_mqtt->mqtt_model_id            = iot_model_id;

    // Setup DPS
    status = azure_iot_dps_create(azure_iot_mqtt, nx_ip, nx_pool);
    if (status != NX_SUCCESS)
    {
        printf("ERROR: Failed to create DPS client (0x%04x)\r\n", status);
        return status;
    }

    status = azure_iot_dps_register(azure_iot_mqtt, NX_WAIT_FOREVER);
    if (status != NX_SUCCESS)
    {
        printf("ERROR: Failed to register DPS device (0x%04x)\r\n", status);
        azure_iot_dps_delete(azure_iot_mqtt);
        return status;
    }

    status = azure_iot_dps_delete(azure_iot_mqtt);
    if (status != NX_SUCCESS)
    {
        printf("ERROR: Failed to delete DPS client (0x%04x)\r\n", status);
        return status;
    }

    printf("SUCCESS: MQTT DPS client initialized\r\n\r\n");

    // call into common code
    return azure_iot_mqtt_create_common(azure_iot_mqtt, nx_ip, nx_pool);
}

UINT azure_iot_mqtt_delete(AZURE_IOT_MQTT* azure_iot_mqtt)
{
    nxd_mqtt_client_disconnect(&azure_iot_mqtt->nxd_mqtt_client);
    nxd_mqtt_client_delete(&azure_iot_mqtt->nxd_mqtt_client);

    return NXD_MQTT_SUCCESS;
}

UINT azure_iot_mqtt_connect(AZURE_IOT_MQTT* azure_iot_mqtt)
{
    UINT status;
    CHAR mqtt_subscribe_topic[100];
    NXD_ADDRESS server_ip;

    printf("\tHub hostname: %s\r\n", azure_iot_mqtt->mqtt_hub_hostname);
    printf("\tDevice id: %s\r\n", azure_iot_mqtt->mqtt_device_id);
    printf("\tModel id: %s\r\n", azure_iot_mqtt->mqtt_model_id);

    // Create the username & password
    snprintf(azure_iot_mqtt->mqtt_username,
        AZURE_IOT_MQTT_USERNAME_SIZE,
        USERNAME,
        azure_iot_mqtt->mqtt_hub_hostname,
        azure_iot_mqtt->mqtt_device_id,
        azure_iot_mqtt->mqtt_model_id);

    if (!create_sas_token(azure_iot_mqtt->mqtt_sas_key,
            strlen(azure_iot_mqtt->mqtt_sas_key),
            azure_iot_mqtt->mqtt_hub_hostname,
            azure_iot_mqtt->mqtt_device_id,
            azure_iot_mqtt->unix_time_get(),
            azure_iot_mqtt->mqtt_password,
            AZURE_IOT_MQTT_PASSWORD_SIZE))
    {
        printf("ERROR: Unable to generate SAS token\r\n");
        return NX_PTR_ERROR;
    }

    status = nxd_mqtt_client_login_set(&azure_iot_mqtt->nxd_mqtt_client,
        azure_iot_mqtt->mqtt_username,
        strlen(azure_iot_mqtt->mqtt_username),
        azure_iot_mqtt->mqtt_password,
        strlen(azure_iot_mqtt->mqtt_password));
    if (status != NXD_MQTT_SUCCESS)
    {
        printf("Could not create Login Set (0x%02x)\r\n", status);
        nx_secure_tls_session_delete(&azure_iot_mqtt->nxd_mqtt_client.nxd_mqtt_tls_session);
        return status;
    }

    // Resolve the MQTT server IP address
    status = nxd_dns_host_by_name_get(azure_iot_mqtt->nx_dns,
        (UCHAR*)azure_iot_mqtt->mqtt_hub_hostname,
        &server_ip,
        NX_IP_PERIODIC_RATE,
        NX_IP_VERSION_V4);
    if (status != NX_SUCCESS)
    {
        printf("Unable to resolve DNS for MQTT Server %s (0x%02x)\r\n", azure_iot_mqtt->mqtt_hub_hostname, status);
        nx_secure_tls_session_delete(&azure_iot_mqtt->nxd_mqtt_client.nxd_mqtt_tls_session);
        return status;
    }

    // Stash the hostname in a global variable so we can verify the cert at connect
    azure_iot_x509_hostname = azure_iot_mqtt->mqtt_hub_hostname;

    status = nxd_mqtt_client_secure_connect(&azure_iot_mqtt->nxd_mqtt_client,
        &server_ip,
        NXD_MQTT_TLS_PORT,
        tls_setup,
        MQTT_KEEP_ALIVE,
        NX_TRUE,
        MQTT_TIMEOUT);
    if (status != NXD_MQTT_SUCCESS)
    {
        printf("Could not connect to MQTT server (0x%02x)\r\n", status);
        nx_secure_tls_session_delete(&azure_iot_mqtt->nxd_mqtt_client.nxd_mqtt_tls_session);
        return status;
    }

    snprintf(mqtt_subscribe_topic, sizeof(mqtt_subscribe_topic), DEVICE_MESSAGE_TOPIC, azure_iot_mqtt->mqtt_device_id);
    status = nxd_mqtt_client_subscribe(
        &azure_iot_mqtt->nxd_mqtt_client, mqtt_subscribe_topic, strlen(mqtt_subscribe_topic), MQTT_QOS_0);
    if (status != NXD_MQTT_SUCCESS)
    {
        printf("Error in subscribing to server (0x%02x)\r\n", status);
        nx_secure_tls_session_delete(&azure_iot_mqtt->nxd_mqtt_client.nxd_mqtt_tls_session);
        return status;
    }

    status = nxd_mqtt_client_subscribe(
        &azure_iot_mqtt->nxd_mqtt_client, DIRECT_METHOD_TOPIC, strlen(DIRECT_METHOD_TOPIC), MQTT_QOS_0);
    if (status != NXD_MQTT_SUCCESS)
    {
        printf("Error in direct method subscribing to server (0x%02x)\r\n", status);
        nx_secure_tls_session_delete(&azure_iot_mqtt->nxd_mqtt_client.nxd_mqtt_tls_session);
        return status;
    }

    status = nxd_mqtt_client_subscribe(
        &azure_iot_mqtt->nxd_mqtt_client, DEVICE_TWIN_RES_TOPIC, strlen(DEVICE_TWIN_RES_TOPIC), MQTT_QOS_0);
    if (status != NXD_MQTT_SUCCESS)
    {
        printf("Error in device twin response subscribing to server (0x%02x)\r\n", status);
        nx_secure_tls_session_delete(&azure_iot_mqtt->nxd_mqtt_client.nxd_mqtt_tls_session);
        return status;
    }

    status = nxd_mqtt_client_subscribe(&azure_iot_mqtt->nxd_mqtt_client,
        DEVICE_TWIN_DESIRED_PROP_RES_TOPIC,
        strlen(DEVICE_TWIN_DESIRED_PROP_RES_TOPIC),
        MQTT_QOS_0);
    if (status != NXD_MQTT_SUCCESS)
    {
        printf("Error in device twin desired properties response subscribing to server (0x%02x)\r\n", status);
        return status;
    }

    printf("SUCCESS: MQTT Hub client initialized\r\n\r\n");

    return NXD_MQTT_SUCCESS;
}

UINT azure_iot_mqtt_disconnect(AZURE_IOT_MQTT* azure_iot_mqtt)
{
    UINT status = nxd_mqtt_client_disconnect(&azure_iot_mqtt->nxd_mqtt_client);

    return status;
}
