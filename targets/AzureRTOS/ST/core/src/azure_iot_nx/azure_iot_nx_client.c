/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "azure_iot_nx_client.h"

#include <stdio.h>

#include "azure_iot_cert.h"
#include "azure_iot_ciphersuites.h"
#include "nx_azure_iot_pnp_helpers.h"

#define NX_AZURE_IOT_THREAD_PRIORITY 4
#define THREAD_PRIORITY              16

// Incoming events from the middleware
#define ALL_EVENTS                         0xFF
#define DIRECT_METHOD_EVENT                0x01
#define DEVICE_TWIN_GET_EVENT              0x02
#define DEVICE_TWIN_DESIRED_PROPERTY_EVENT 0x04
#define DEVICE_TWIN_COMPLETE_EVENT         0x08

#define AZURE_IOT_DPS_ENDPOINT "global.azure-devices-provisioning.net"

#define MODULE_ID   ""
#define DPS_PAYLOAD "{\"modelId\":\"%s\"}"

#define DPS_PAYLOAD_SIZE    200
#define PUBLISH_BUFFER_SIZE 512

#define MAX_EXPONENTIAL_BACKOFF_JITTER_PERCENT 60
#define MAX_EXPONENTIAL_BACKOFF_IN_SEC         (10 * 60)
#define INITIAL_EXPONENTIAL_BACKOFF_IN_SEC     3

// Connection timeouts in threadx ticks
#define HUB_CONNECT_TIMEOUT_TICKS  (10 * TX_TIMER_TICKS_PER_SECOND)
#define DPS_REGISTER_TIMEOUT_TICKS (3 * TX_TIMER_TICKS_PER_SECOND)

static UINT exponential_backoff_with_jitter(UINT* exponential_retry_count)
{
    float jitter_percent = (MAX_EXPONENTIAL_BACKOFF_JITTER_PERCENT / 100.0f) * (rand() / ((float)RAND_MAX));
    UINT base_delay      = MAX_EXPONENTIAL_BACKOFF_IN_SEC;

    base_delay = (1 << *exponential_retry_count) * INITIAL_EXPONENTIAL_BACKOFF_IN_SEC;

    if (base_delay > MAX_EXPONENTIAL_BACKOFF_IN_SEC)
    {
        base_delay = MAX_EXPONENTIAL_BACKOFF_IN_SEC;
    }
    else
    {
        (*exponential_retry_count)++;
    }

    return (base_delay * (1 + jitter_percent)) * TX_TIMER_TICKS_PER_SECOND;
}

static VOID connection_status_callback(NX_AZURE_IOT_HUB_CLIENT* hub_client_ptr, UINT status)
{
    if (status == NX_SUCCESS)
    {
        printf("Connected to IoT Hub\r\n");
    }
    else
    {
        printf("Connection failure from IoT Hub (0x%08x)\r\n", status);

        UINT connect_status = NX_AZURE_IOT_FAILURE;
        UINT retry_count    = 0;
        while (connect_status)
        {
            printf("Reconnecting to IoT Hub...\r\n");

            if ((connect_status = nx_azure_iot_hub_client_connect(hub_client_ptr, NX_TRUE, HUB_CONNECT_TIMEOUT_TICKS)))
            {
                printf("Failed reconnect on nx_azure_iot_hub_client_connect (0x%08x)\r\n", connect_status);
                tx_thread_sleep(exponential_backoff_with_jitter(&retry_count));
            }
        }
    }
}

static VOID message_receive_direct_method(NX_AZURE_IOT_HUB_CLIENT* hub_client_ptr, VOID* context)
{
    AZURE_IOT_NX_CONTEXT* nx_context = (AZURE_IOT_NX_CONTEXT*)context;
    tx_event_flags_set(&nx_context->events, DIRECT_METHOD_EVENT, TX_OR);
}

static VOID message_receive_callback_twin(NX_AZURE_IOT_HUB_CLIENT* hub_client_ptr, VOID* context)
{
    AZURE_IOT_NX_CONTEXT* nx_context = (AZURE_IOT_NX_CONTEXT*)context;
    tx_event_flags_set(&nx_context->events, DEVICE_TWIN_GET_EVENT, TX_OR);
}

static VOID message_receive_callback_desire_property(NX_AZURE_IOT_HUB_CLIENT* hub_client_ptr, VOID* context)
{
    AZURE_IOT_NX_CONTEXT* nx_context = (AZURE_IOT_NX_CONTEXT*)context;
    tx_event_flags_set(&nx_context->events, DEVICE_TWIN_DESIRED_PROPERTY_EVENT, TX_OR);
}

static VOID process_direct_method(AZURE_IOT_NX_CONTEXT* nx_context)
{
    UINT status;
    NX_PACKET* packet;
    const UCHAR* method_name;
    USHORT method_name_length;
    VOID* context;
    USHORT context_length;
    UCHAR* payload;
    USHORT payload_length;

    while ((status = nx_azure_iot_hub_client_direct_method_message_receive(&nx_context->iothub_client,
                &method_name,
                &method_name_length,
                &context,
                &context_length,
                &packet,
                NX_NO_WAIT)) == NX_AZURE_IOT_SUCCESS)
    {
        printf("Receive direct method: %.*s\r\n", (INT)method_name_length, (CHAR*)method_name);
        printf_packet(packet, "\tPayload: ");

        payload        = packet->nx_packet_prepend_ptr;
        payload_length = packet->nx_packet_append_ptr - packet->nx_packet_prepend_ptr;

        if (nx_context->direct_method_cb)
        {
            nx_context->direct_method_cb(
                nx_context, method_name, method_name_length, payload, payload_length, context, context_length);
        }

        // Release the received packet, as ownership was passed to the application from the middleware
        nx_packet_release(packet);
    }

    // If we failed for anything other than no packet, then report error
    if (status != NX_AZURE_IOT_NO_PACKET)
    {
        printf("ERROR: direct method receive failed (0x%08x)\r\n", status);
        return;
    }
}

static VOID process_device_twin_get(AZURE_IOT_NX_CONTEXT* nx_context)
{
    UINT status;
    NX_PACKET* packet_ptr;
    NX_AZURE_IOT_JSON_READER json_reader;
    UCHAR buffer[PUBLISH_BUFFER_SIZE];

    if ((status = nx_azure_iot_hub_client_device_twin_properties_receive(
             &nx_context->iothub_client, &packet_ptr, NX_WAIT_FOREVER)))
    {
        printf("Error: receive device twin property failed (0x%08x)\r\n", status);
        return;
    }

    printf_packet(packet_ptr, "Receive twin properties: ");

    if (packet_ptr->nx_packet_length > (ULONG)(packet_ptr->nx_packet_append_ptr - packet_ptr->nx_packet_prepend_ptr))
    {
        printf("ERROR: json is large than nx_packet\r\n");
        nx_packet_release(packet_ptr);
        return;
    }

    if ((status = nx_azure_iot_json_reader_init(&json_reader, packet_ptr)))
    {
        printf("ERROR: failed to initialize json reader (0x%08x)\r\n", status);
        nx_packet_release(packet_ptr);
        return;
    }

    if (nx_context->device_twin_get_cb)
    {
        if ((status = nx_azure_iot_pnp_helper_twin_data_parse(&json_reader,
                 NX_FALSE,
                 NX_NULL,
                 0,
                 buffer,
                 PUBLISH_BUFFER_SIZE,
                 nx_context->device_twin_get_cb,
                 nx_context)))
        {
            printf("ERROR: failed to parse twin data (0x%08x)\r\n", status);
        }
    }

    // Deinit the reader, the reader owns the NX_PACKET at this point, so will release it
    nx_azure_iot_json_reader_deinit(&json_reader);

    // Send event to notify device twin received
    tx_event_flags_set(&nx_context->events, DEVICE_TWIN_COMPLETE_EVENT, TX_OR);
}

static VOID process_device_twin_desired_property(AZURE_IOT_NX_CONTEXT* nx_context)
{
    UINT status;
    NX_PACKET* packet_ptr;
    NX_AZURE_IOT_JSON_READER json_reader;
    UCHAR buffer[128];

    while ((status = nx_azure_iot_hub_client_device_twin_desired_properties_receive(
                &nx_context->iothub_client, &packet_ptr, NX_NO_WAIT)) == NX_AZURE_IOT_SUCCESS)
    {
        printf_packet(packet_ptr, "Receive twin writeable property: ");

        if (packet_ptr->nx_packet_length >
            (ULONG)(packet_ptr->nx_packet_append_ptr - packet_ptr->nx_packet_prepend_ptr))
        {
            printf("ERROR: json is large than nx_packet\r\n");
            nx_packet_release(packet_ptr);
            continue;
        }

        if ((status = nx_azure_iot_json_reader_init(&json_reader, packet_ptr)))
        {
            printf("ERROR: failed to initialize json reader (0x%08x)\r\n", status);
            nx_packet_release(packet_ptr);
            continue;
        }

        if (nx_context->device_twin_desired_prop_cb)
        {
            if ((status = nx_azure_iot_pnp_helper_twin_data_parse(&json_reader,
                     NX_TRUE,
                     NX_NULL,
                     0,
                     buffer,
                     sizeof(buffer),
                     nx_context->device_twin_desired_prop_cb,
                     nx_context)))
            {
                printf("ERROR: failed to parse twin data (0x%08x)\r\n", status);
            }
        }

        // Deinit the reader, the reader owns the NX_PACKET at this point, so will release it
        nx_azure_iot_json_reader_deinit(&json_reader);
    }

    // If we failed for anything other than no packet, then report error
    if (status != NX_AZURE_IOT_NO_PACKET)
    {
        printf("ERROR: device twin writeable property receive failed (0x%08x)\r\n", status);
        return;
    }
}

static VOID event_thread(ULONG parameter)
{
    ULONG app_events;

    AZURE_IOT_NX_CONTEXT* context = (AZURE_IOT_NX_CONTEXT*)parameter;

    while (true)
    {
        tx_event_flags_get(&context->events, ALL_EVENTS, TX_OR_CLEAR, &app_events, NX_IP_PERIODIC_RATE);

        if (app_events & DIRECT_METHOD_EVENT)
        {
            process_direct_method(context);
        }

        if (app_events & DEVICE_TWIN_GET_EVENT)
        {
            process_device_twin_get(context);
        }

        if (app_events & DEVICE_TWIN_DESIRED_PROPERTY_EVENT)
        {
            process_device_twin_desired_property(context);
        }

        app_events = 0;
    }
}

static UINT azure_iot_nx_client_hub_create_internal(AZURE_IOT_NX_CONTEXT* context)
{
    UINT status;

    printf("Initializing Azure IoT Hub client\r\n");
    printf("\tHub hostname: %s\r\n", context->azure_iot_hub_hostname);
    printf("\tDevice id: %s\r\n", context->azure_iot_device_id);
    printf("\tModel id: %s\r\n", context->azure_iot_model_id);

    // Initialize IoT Hub client.
    if ((status = nx_azure_iot_hub_client_initialize(&context->iothub_client,
             &context->nx_azure_iot,
             (UCHAR*)context->azure_iot_hub_hostname,
             strlen(context->azure_iot_hub_hostname),
             (UCHAR*)context->azure_iot_device_id,
             strlen(context->azure_iot_device_id),
             (UCHAR*)MODULE_ID,
             strlen(MODULE_ID),
             _nx_azure_iot_tls_supported_crypto,
             _nx_azure_iot_tls_supported_crypto_size,
             _nx_azure_iot_tls_ciphersuite_map,
             _nx_azure_iot_tls_ciphersuite_map_size,
             (UCHAR*)context->nx_azure_iot_tls_metadata_buffer,
             sizeof(context->nx_azure_iot_tls_metadata_buffer),
             &context->root_ca_cert)))
    {
        printf("ERROR: on nx_azure_iot_hub_client_initialize (0x%08x)\r\n", status);
        return status;
    }

    // Set credentials
    if (context->azure_iot_auth_mode == AZURE_IOT_AUTH_MODE_SAS)
    {
        // Symmetric (SAS) Key
        if ((status = nx_azure_iot_hub_client_symmetric_key_set(&context->iothub_client,
                 (UCHAR*)context->azure_iot_device_sas_key,
                 context->azure_iot_device_sas_key_len)))
        {
            printf("ERROR: failed on nx_azure_iot_hub_client_symmetric_key_set (0x%08x)\r\n", status);
        }
    }
    else if (context->azure_iot_auth_mode == AZURE_IOT_AUTH_MODE_CERT)
    {
        // X509 Certificate
        if ((status = nx_azure_iot_hub_client_device_cert_set(&context->iothub_client, &context->device_certificate)))
        {
            printf("ERROR: failed on nx_azure_iot_hub_client_device_cert_set!: error code = 0x%08x\r\n", status);
        }
    }

    // Set Model id
    if ((status = nx_azure_iot_hub_client_model_id_set(
             &context->iothub_client, (UCHAR*)context->azure_iot_model_id, strlen(context->azure_iot_model_id))))
    {
        printf("ERROR: nx_azure_iot_hub_client_model_id_set (0x%08x)\r\n", status);
    }

    // Set connection status callback
    else if ((status = nx_azure_iot_hub_client_connection_status_callback_set(
                  &context->iothub_client, connection_status_callback)))
    {
        printf("ERROR: failed on connection_status_callback (0x%08x)\r\n", status);
    }

    // Enable direct methods
    else if ((status = nx_azure_iot_hub_client_direct_method_enable(&context->iothub_client)))
    {
        printf("ERROR: direct method receive enable failed (0x%08x)\r\n", status);
    }

    // Enable device twin
    else if ((status = nx_azure_iot_hub_client_device_twin_enable(&context->iothub_client)))
    {
        printf("ERROR: device twin enabled failed (0x%08x)\r\n", status);
    }

    // Set device twin callback
    else if ((status = nx_azure_iot_hub_client_receive_callback_set(&context->iothub_client,
                  NX_AZURE_IOT_HUB_DEVICE_TWIN_PROPERTIES,
                  message_receive_callback_twin,
                  (VOID*)context)))
    {
        printf("ERROR: device twin callback set (0x%08x)\r\n", status);
    }

    // Set direct method callback
    else if ((status = nx_azure_iot_hub_client_receive_callback_set(&context->iothub_client,
                  NX_AZURE_IOT_HUB_DIRECT_METHOD,
                  message_receive_direct_method,
                  (VOID*)context)))
    {
        printf("ERROR: device method callback set (0x%08x)\r\n", status);
    }

    // Set the writeable property callback
    else if ((status = nx_azure_iot_hub_client_receive_callback_set(&context->iothub_client,
                  NX_AZURE_IOT_HUB_DEVICE_TWIN_DESIRED_PROPERTIES,
                  message_receive_callback_desire_property,
                  (VOID*)context)))
    {
        printf("ERROR: device twin desired property callback set (0x%08x)\r\n", status);
    }

    if (status != NX_AZURE_IOT_SUCCESS)
    {
        nx_azure_iot_hub_client_deinitialize(&context->iothub_client);
    }

    return status;
}

UINT azure_iot_nx_client_register_direct_method(AZURE_IOT_NX_CONTEXT* context, func_ptr_direct_method callback)
{
    if (context == NULL || context->direct_method_cb != NULL)
    {
        return NX_PTR_ERROR;
    }

    context->direct_method_cb = callback;
    return NX_SUCCESS;
}

UINT azure_iot_nx_client_register_device_twin_desired_prop(
    AZURE_IOT_NX_CONTEXT* context, func_ptr_device_twin_desired_prop callback)
{
    if (context == NULL || context->device_twin_desired_prop_cb != NULL)
    {
        return NX_PTR_ERROR;
    }

    context->device_twin_desired_prop_cb = callback;
    return NX_SUCCESS;
}

UINT azure_iot_nx_client_register_device_twin_prop(AZURE_IOT_NX_CONTEXT* context, func_ptr_device_twin_prop callback)
{
    if (context == NULL || context->device_twin_get_cb != NULL)
    {
        return NX_PTR_ERROR;
    }

    context->device_twin_get_cb = callback;
    return NX_SUCCESS;
}

UINT azure_iot_nx_client_sas_set(AZURE_IOT_NX_CONTEXT* context, CHAR* device_sas_key)
{
    if (device_sas_key[0] == 0)
    {
        printf("ERROR: azure_iot_nx_client_sas_set device_sas_key is null\r\n");
        return NX_PTR_ERROR;
    }

    context->azure_iot_auth_mode          = AZURE_IOT_AUTH_MODE_SAS;
    context->azure_iot_device_sas_key     = device_sas_key;
    context->azure_iot_device_sas_key_len = strlen(device_sas_key);

    return NX_SUCCESS;
}

UINT azure_iot_nx_client_cert_set(AZURE_IOT_NX_CONTEXT* context,
    UCHAR* device_x509_cert,
    UINT device_x509_cert_len,
    UCHAR* device_x509_key,
    UINT device_x509_key_len)
{
    UINT status;

    if (device_x509_cert[0] == 0 || device_x509_cert_len == 0 || device_x509_key[0] == 0 || device_x509_key_len == 0)
    {
        printf("ERROR: azure_iot_nx_client_cert_set cert/key is null\r\n");
        return NX_PTR_ERROR;
    }

    context->azure_iot_auth_mode = AZURE_IOT_AUTH_MODE_CERT;

    // Create the device certificate
    if ((status = nx_secure_x509_certificate_initialize(&context->device_certificate,
             (UCHAR*)device_x509_cert,
             (USHORT)device_x509_cert_len,
             NX_NULL,
             0,
             (UCHAR*)device_x509_key,
             (USHORT)device_x509_key_len,
             NX_SECURE_X509_KEY_TYPE_RSA_PKCS1_DER)))
    {
        printf("Error: Failed on device nx_secure_x509_certificate_initialize!: error code = 0x%08x\r\n", status);
    }

    return NX_SUCCESS;
}

UINT azure_iot_nx_client_create(AZURE_IOT_NX_CONTEXT* context,
    NX_IP* nx_ip,
    NX_PACKET_POOL* nx_pool,
    NX_DNS* nx_dns,
    UINT (*unix_time_callback)(ULONG* unix_time),
    CHAR* iot_model_id)
{
    UINT status;

    if (iot_model_id[0] == 0)
    {
        printf("ERROR: UINT azure_iot_nx_client_create_new empty device_id or model_id\r\n");
        return NX_PTR_ERROR;
    }

    // Initialise the context
    memset(context, 0, sizeof(AZURE_IOT_NX_CONTEXT));

    // Stash parameters
    context->azure_iot_model_id = iot_model_id;

    if ((status = tx_event_flags_create(&context->events, "nx_client")))
    {
        printf("ERROR: failed on create nx_client event flags (0x%08x)\r\n", status);
        return status;
    }

    // Create Azure IoT handler
    if ((status = nx_azure_iot_create(&context->nx_azure_iot,
             (UCHAR*)"Azure IoT",
             nx_ip,
             nx_pool,
             nx_dns,
             context->nx_azure_iot_thread_stack,
             sizeof(context->nx_azure_iot_thread_stack),
             NX_AZURE_IOT_THREAD_PRIORITY,
             unix_time_callback)))
    {
        printf("ERROR: failed on nx_azure_iot_create (0x%08x)\r\n", status);
        return status;
    }

    // Initialize CA root certificate
    if ((status = nx_secure_x509_certificate_initialize(&context->root_ca_cert,
             (UCHAR*)azure_iot_root_ca,
             (USHORT)azure_iot_root_ca_len,
             NX_NULL,
             0,
             NULL,
             0,
             NX_SECURE_X509_KEY_TYPE_NONE)))
    {
        printf("Failed to initialize ROOT CA certificate!: error code = 0x%08x\r\n", status);
        nx_azure_iot_delete(&context->nx_azure_iot);
        return status;
    }

    return NX_AZURE_IOT_SUCCESS;
}

UINT azure_iot_nx_client_hub_create(AZURE_IOT_NX_CONTEXT* context, CHAR* iot_hub_hostname, CHAR* iot_device_id)
{
    if (context == NULL)
    {
        printf("ERROR: context is NULL\r\n");
        return NX_PTR_ERROR;
    }

    // Return error if empty hostname or device id
    if (iot_hub_hostname[0] == 0 || iot_device_id[0] == 0)
    {
        printf("ERROR: azure_iot_nx_client_hub_create iot_hub_hostname is null\r\n");
        return NX_PTR_ERROR;
    }

    // Stash parameters
    memcpy(context->azure_iot_hub_hostname, iot_hub_hostname, AZURE_IOT_DEVICE_ID_SIZE);
    memcpy(context->azure_iot_device_id, iot_device_id, AZURE_IOT_DEVICE_ID_SIZE);

    return azure_iot_nx_client_hub_create_internal(context);
}

UINT azure_iot_nx_client_dps_create(AZURE_IOT_NX_CONTEXT* context, CHAR* dps_id_scope, CHAR* dps_registration_id)
{
    UINT status;
    CHAR payload[DPS_PAYLOAD_SIZE];
    UINT iot_hub_hostname_len = AZURE_IOT_HOST_NAME_SIZE;
    UINT iot_device_id_len    = AZURE_IOT_DEVICE_ID_SIZE;

    printf("Initializing Azure IoT DPS client\r\n");
    printf("\tDPS endpoint: %s\r\n", AZURE_IOT_DPS_ENDPOINT);
    printf("\tDPS ID scope: %s\r\n", dps_id_scope);
    printf("\tRegistration ID: %s\r\n", dps_registration_id);

    if (context == NULL)
    {
        printf("ERROR: context is NULL\r\n");
        return NX_PTR_ERROR;
    }

    // Return error if empty credentials
    if (dps_id_scope[0] == 0 || dps_registration_id[0] == 0)
    {
        printf("ERROR: azure_iot_nx_client_dps_create incorrect parameters\r\n");
        return NX_PTR_ERROR;
    }

    if (snprintf(payload, sizeof(payload), DPS_PAYLOAD, context->azure_iot_model_id) > DPS_PAYLOAD_SIZE - 1)
    {
        printf("ERROR: insufficient buffer size to create DPS payload\r\n");
        return NX_SIZE_ERROR;
    }

    // Initialize IoT provisioning client
    if ((status = nx_azure_iot_provisioning_client_initialize(&context->dps_client,
             &context->nx_azure_iot,
             (UCHAR*)AZURE_IOT_DPS_ENDPOINT,
             strlen(AZURE_IOT_DPS_ENDPOINT),
             (UCHAR*)dps_id_scope,
             strlen(dps_id_scope),
             (UCHAR*)dps_registration_id,
             strlen(dps_registration_id),
             _nx_azure_iot_tls_supported_crypto,
             _nx_azure_iot_tls_supported_crypto_size,
             _nx_azure_iot_tls_ciphersuite_map,
             _nx_azure_iot_tls_ciphersuite_map_size,
             (UCHAR*)context->nx_azure_iot_tls_metadata_buffer,
             sizeof(context->nx_azure_iot_tls_metadata_buffer),
             &context->root_ca_cert)))
    {
        printf("Failed on nx_azure_iot_provisioning_client_initialize (0x%08x)\r\n", status);
        return status;
    }

    // Set credentials
    if (context->azure_iot_auth_mode == AZURE_IOT_AUTH_MODE_SAS)
    {
        // Symmetric (SAS) Key
        if ((status = nx_azure_iot_provisioning_client_symmetric_key_set(&context->dps_client,
                 (UCHAR*)context->azure_iot_device_sas_key,
                 context->azure_iot_device_sas_key_len)))
        {
            printf("Failed on nx_azure_iot_hub_client_symmetric_key_set (0x%08x)\r\n", status);
        }
    }
    else if (context->azure_iot_auth_mode == AZURE_IOT_AUTH_MODE_CERT)
    {
        // X509 Certificate
        if ((status = nx_azure_iot_provisioning_client_device_cert_set(
                 &context->dps_client, &context->device_certificate)))
        {
            printf("Failed on nx_azure_iot_hub_client_device_cert_set! (0x%08x)\r\n", status);
        }
    }

    // Set the payload containing the model Id
    if ((status = nx_azure_iot_provisioning_client_registration_payload_set(
             &context->dps_client, (UCHAR*)payload, strlen(payload))))
    {
        printf("ERROR: nx_azure_iot_provisioning_client_registration_payload_set (0x%08x\r\n", status);
    }

    // Register device
    else
    {
        while (true)
        {
            status = nx_azure_iot_provisioning_client_register(&context->dps_client, DPS_REGISTER_TIMEOUT_TICKS);
            if (status == NX_AZURE_IOT_PENDING)
            {
                printf("\tPending DPS connection, retrying\r\n");
                continue;
            }

            // Registration complete
            break;
        }
    }

    if (status != NX_AZURE_IOT_SUCCESS)
    {
        printf("\tERROR: nx_azure_iot_provisioning_client_register (0x%08x)\r\n", status);
    }

    // Get Device info
    else if ((status = nx_azure_iot_provisioning_client_iothub_device_info_get(&context->dps_client,
                  (UCHAR*)context->azure_iot_hub_hostname,
                  &iot_hub_hostname_len,
                  (UCHAR*)context->azure_iot_device_id,
                  &iot_device_id_len)))
    {
        printf("ERROR: nx_azure_iot_provisioning_client_iothub_device_info_get (0x%08x)\r\n", status);
    }

    // Destroy Provisioning Client
    nx_azure_iot_provisioning_client_deinitialize(&context->dps_client);

    if (status != NX_SUCCESS)
    {
        return status;
    }

    // Null terminate returned values
    context->azure_iot_hub_hostname[iot_hub_hostname_len] = 0;
    context->azure_iot_device_id[iot_device_id_len]       = 0;

    printf("SUCCESS: Azure IoT DPS client initialized\r\n\r\n");

    return azure_iot_nx_client_hub_create_internal(context);
}

UINT azure_iot_nx_client_delete(AZURE_IOT_NX_CONTEXT* context)
{
    // Destroy IoTHub Client
    nx_azure_iot_hub_client_disconnect(&context->iothub_client);
    nx_azure_iot_hub_client_deinitialize(&context->iothub_client);

    // Destroy the common object
    nx_azure_iot_delete(&context->nx_azure_iot);

    return NX_SUCCESS;
}

UINT azure_iot_nx_client_connect(AZURE_IOT_NX_CONTEXT* context)
{
    UINT status;

    // Connect to IoTHub client
    if ((status = nx_azure_iot_hub_client_connect(&context->iothub_client, NX_TRUE, HUB_CONNECT_TIMEOUT_TICKS)))
    {
        printf("Failed on nx_azure_iot_hub_client_connect (0x%08x)\r\n", status);
        return status;
    }

    if ((status = tx_thread_create(&context->azure_iot_thread,
             "Nx Thread",
             event_thread,
             (ULONG)context,
             (UCHAR*)context->azure_iot_thread_stack,
             AZURE_IOT_STACK_SIZE,
             THREAD_PRIORITY,
             THREAD_PRIORITY,
             1,
             TX_AUTO_START)))
    {
        printf("Failed to create telemetry thread (0x%08x)\r\n", status);
        return status;
    }

    printf("SUCCESS: Azure IoT Hub client initialized\r\n\r\n");

    return NX_SUCCESS;
}

UINT azure_iot_nx_client_disconnect(AZURE_IOT_NX_CONTEXT* context)
{
    nx_azure_iot_hub_client_disconnect(&context->iothub_client);

    return NX_SUCCESS;
}

UINT azure_iot_nx_client_device_twin_request_and_wait(AZURE_IOT_NX_CONTEXT* context)
{
    UINT status;
    ULONG app_events;

    // Request the device twin for writeable property update
    if ((status = nx_azure_iot_hub_client_device_twin_properties_request(&context->iothub_client, NX_WAIT_FOREVER)))
    {
        printf("ERROR: failed to request device twin (0x%08x)\r\n", status);
        return status;
    }

    // Wait for the device twin to be processed
    if ((status = tx_event_flags_get(
             &context->events, DEVICE_TWIN_COMPLETE_EVENT, TX_OR_CLEAR, &app_events, 10 * NX_IP_PERIODIC_RATE)))
    {
        printf("ERROR: failed to execute tx_event_flags_get (0x%08x)\r\n", status);
        return status;
    }

    return NX_SUCCESS;
}

UINT azure_iot_nx_client_publish_telemetry(
    AZURE_IOT_NX_CONTEXT* context, UINT (*append_properties)(NX_AZURE_IOT_JSON_WRITER* json_builder_ptr, VOID* context))
{
    UINT status;
    NX_PACKET* packet_ptr;
    NX_AZURE_IOT_JSON_WRITER json_builder;
    UINT telemetry_length;
    UCHAR buffer[PUBLISH_BUFFER_SIZE];

    if ((status = nx_azure_iot_pnp_helper_telemetry_message_create(
             &context->iothub_client, NX_NULL, 0, &packet_ptr, NX_WAIT_FOREVER)))
    {
        printf("Telemetry message create failed!: error code = 0x%08x\r\n", status);
        return (status);
    }

    if ((status = nx_azure_iot_json_writer_with_buffer_init(&json_builder, buffer, PUBLISH_BUFFER_SIZE)))
    {
        printf("Failed to initialize json writer\r\n");
        nx_azure_iot_hub_client_telemetry_message_delete(packet_ptr);
        return NX_NOT_SUCCESSFUL;
    }

    if ((status = nx_azure_iot_pnp_helper_build_reported_property(NULL, 0, append_properties, NX_NULL, &json_builder)))
    {
        printf("Failed to build telemetry!: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_builder);
        nx_azure_iot_hub_client_telemetry_message_delete(packet_ptr);
        return status;
    }

    telemetry_length = nx_azure_iot_json_writer_get_bytes_used(&json_builder);
    if ((status = nx_azure_iot_hub_client_telemetry_send(
             &context->iothub_client, packet_ptr, buffer, telemetry_length, NX_WAIT_FOREVER)))
    {
        printf("Telemetry message send failed (0x%08x)\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_builder);
        nx_azure_iot_hub_client_telemetry_message_delete(packet_ptr);
        return status;
    }

    printf("Telemetry message sent: %.*s.\r\n", telemetry_length, buffer);

    nx_azure_iot_json_writer_deinit(&json_builder);

    return status;
}

UINT azure_iot_nx_client_publish_properties(AZURE_IOT_NX_CONTEXT* context,
    CHAR* component,
    UINT (*append_properties)(NX_AZURE_IOT_JSON_WRITER* json_builder_ptr, VOID* context))
{
    UINT reported_properties_length;
    UINT status;
    UINT response_status;
    UINT request_id;
    NX_AZURE_IOT_JSON_WRITER json_builder;
    ULONG reported_property_version;
    UCHAR buffer[PUBLISH_BUFFER_SIZE];

    if ((status = nx_azure_iot_json_writer_with_buffer_init(&json_builder, buffer, PUBLISH_BUFFER_SIZE)))
    {
        printf("Failed to initialize json writer\r\n");
        return NX_NOT_SUCCESSFUL;
    }

    if ((status = nx_azure_iot_pnp_helper_build_reported_property(
             (UCHAR*)component, strlen(component), append_properties, NX_NULL, &json_builder)))
    {
        printf("Failed to build reported property!: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_builder);
        return status;
    }

    reported_properties_length = nx_azure_iot_json_writer_get_bytes_used(&json_builder);
    if ((status = nx_azure_iot_hub_client_device_twin_reported_properties_send(&context->iothub_client,
             buffer,
             reported_properties_length,
             &request_id,
             &response_status,
             &reported_property_version,
             (5 * NX_IP_PERIODIC_RATE))))
    {
        printf("Device twin reported properties failed!: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_builder);
        return status;
    }

    nx_azure_iot_json_writer_deinit(&json_builder);

    if ((response_status < 200) || (response_status >= 300))
    {
        printf("device twin report properties failed with code : %d\r\n", response_status);
        return NX_NOT_SUCCESSFUL;
    }

    printf("Device twin property sent: %.*s.\r\n", reported_properties_length, buffer);

    return status;
}

UINT azure_iot_nx_client_publish_float_property(AZURE_IOT_NX_CONTEXT* context, CHAR* key, float value)
{
    UINT status;
    UINT response_status;
    UINT request_id;
    ULONG version;
    CHAR buffer[PUBLISH_BUFFER_SIZE];

    int intvalue  = value;
    int fracvalue = abs(100 * (value - (long)value));

    if (snprintf(buffer, PUBLISH_BUFFER_SIZE, "{\"%s\":%d.%2d}", key, intvalue, fracvalue) > PUBLISH_BUFFER_SIZE - 1)
    {
        printf("ERROR: insufficient buffer size to publish float property\r\n");
        return NX_SIZE_ERROR;
    }

    if ((status = nx_azure_iot_hub_client_device_twin_reported_properties_send(&context->iothub_client,
             (UCHAR*)buffer,
             strlen(buffer),
             &request_id,
             &response_status,
             &version,
             NX_WAIT_FOREVER)))
    {
        printf("Device twin reported properties failed (0x%08x)\r\n", status);
        return status;
    }

    if ((response_status < 200) || (response_status >= 300))
    {
        printf("device twin report properties failed (%d)\r\n", response_status);
        return status;
    }

    printf("Device twin property sent: %s\r\n", buffer);

    return NX_SUCCESS;
}

UINT azure_iot_nx_client_publish_bool_property(AZURE_IOT_NX_CONTEXT* context, CHAR* key, bool value)
{
    UINT status;
    UINT response_status;
    UINT request_id;
    ULONG version;
    CHAR buffer[PUBLISH_BUFFER_SIZE];

    if (snprintf(buffer, PUBLISH_BUFFER_SIZE, "{\"%s\":%s}", key, (value ? "true" : "false")) > PUBLISH_BUFFER_SIZE - 1)
    {
        printf("ERROR: Unsufficient buffer size to publish bool property\r\n");
        return NX_SIZE_ERROR;
    }

    if ((status = nx_azure_iot_hub_client_device_twin_reported_properties_send(&context->iothub_client,
             (UCHAR*)buffer,
             strlen(buffer),
             &request_id,
             &response_status,
             &version,
             NX_WAIT_FOREVER)))
    {
        printf("Error: device twin reported properties failed (0x%08x)\r\n", status);
        return status;
    }

    if ((response_status < 200) || (response_status >= 300))
    {
        printf("Error: device twin report properties failed (%d)\r\n", response_status);
        return status;
    }

    printf("Device twin property sent: %s\r\n", buffer);

    return NX_SUCCESS;
}

UINT azure_iot_nx_client_publish_int_writeable_property(AZURE_IOT_NX_CONTEXT* context, CHAR* key, UINT value)
{
    UINT status;
    UINT response_status;
    UINT request_id;
    ULONG version;
    CHAR buffer[PUBLISH_BUFFER_SIZE];

    if (snprintf(buffer, PUBLISH_BUFFER_SIZE, "{\"%s\":{\"value\":%d,\"ac\":200,\"av\":1}}", key, value) >
        PUBLISH_BUFFER_SIZE - 1)
    {
        printf("ERROR: Unsufficient buffer size to publish int property\r\n");
        return NX_SIZE_ERROR;
    }

    if ((status = nx_azure_iot_hub_client_device_twin_reported_properties_send(&context->iothub_client,
             (UCHAR*)buffer,
             strlen(buffer),
             &request_id,
             &response_status,
             &version,
             NX_WAIT_FOREVER)))
    {
        printf("Error: device twin reported properties failed (0x%08x)\r\n", status);
        return status;
    }

    if ((response_status < 200) || (response_status >= 300))
    {
        printf("Error: device twin report properties failed (%d)\r\n", response_status);
        return status;
    }

    printf("Device twin writeable property sent: %s\r\n", buffer);

    return NX_SUCCESS;
}

UINT azure_nx_client_respond_int_writeable_property(
    AZURE_IOT_NX_CONTEXT* context, CHAR* property, int value, int http_status, int version)
{
    UINT status;
    UINT response_status;
    UINT request_id;
    ULONG version_dt;
    CHAR buffer[PUBLISH_BUFFER_SIZE];

    if (snprintf(buffer,
            PUBLISH_BUFFER_SIZE,
            "{\"%s\":{\"value\":%d,\"ac\":%d,\"av\":%d}}",
            property,
            value,
            http_status,
            version) > PUBLISH_BUFFER_SIZE - 1)
    {
        printf("ERROR: insufficient buffer size to respond to writeable property\r\n");
        return NX_SIZE_ERROR;
    }

    if ((status = nx_azure_iot_hub_client_device_twin_reported_properties_send(&context->iothub_client,
             (UCHAR*)buffer,
             strlen(buffer),
             &request_id,
             &response_status,
             &version_dt,
             NX_WAIT_FOREVER)))
    {
        printf("ERROR: device twin reported properties failed (0x%08x)\r\n", status);
    }

    if ((response_status < 200) || (response_status >= 300))
    {
        printf("ERROR: device twin report properties failed (%d)\r\n", response_status);
        return status;
    }

    printf("Device twin writeable property response: %s\r\n", buffer);

    return status;
}

VOID printf_packet(NX_PACKET* packet_ptr, CHAR* prepend)
{
    printf("%s", prepend);

    while (packet_ptr != NX_NULL)
    {
        printf("%.*s", (INT)(packet_ptr->nx_packet_length), (CHAR*)packet_ptr->nx_packet_prepend_ptr);
        packet_ptr = packet_ptr->nx_packet_next;
    }

    printf("\r\n");
}
