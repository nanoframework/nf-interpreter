/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "nx_client.h"
#include <hal.h>

#include <stdio.h>

//#include "stm32l475e_iot01.h"
//#include "stm32l475e_iot01_tsensor.h"

#include "nx_api.h"
//#include "nx_azure_iot_hub_client.h"
//#include "nx_azure_iot_json_reader.h"
//#include "nx_azure_iot_provisioning_client.h"

//#include "azure_iot_nx_client.h"
//#include "nx_azure_iot_pnp_helpers.h"

#include "azure_config.h"
#include "azure_device_x509_cert_config.h"
//#include "azure_pnp_info.h"

#define IOT_MODEL_ID "dtmi:azurertos:devkit:gsg;2"

#define TELEMETRY_TEMPERATURE       "temperature"
#define TELEMETRY_INTERVAL_PROPERTY "telemetryInterval"
#define LED_STATE_PROPERTY          "ledState"
#define SET_LED_STATE_COMMAND       "setLedState"

#define TELEMETRY_INTERVAL_EVENT 1

/*******************
static AZURE_IOT_NX_CONTEXT azure_iot_nx_client;
static TX_EVENT_FLAGS_GROUP azure_iot_flags;

static int32_t telemetry_interval = 10;

static UINT append_device_info_properties(NX_AZURE_IOT_JSON_WRITER *json_writer, VOID *context)
{
    (void)context;
    if (nx_azure_iot_json_writer_append_property_with_string_value(
            json_writer,
            (UCHAR *)DEVICE_INFO_MANUFACTURER_PROPERTY_NAME,
            sizeof(DEVICE_INFO_MANUFACTURER_PROPERTY_NAME) - 1,
            (UCHAR *)DEVICE_INFO_MANUFACTURER_PROPERTY_VALUE,
            sizeof(DEVICE_INFO_MANUFACTURER_PROPERTY_VALUE) - 1) ||
        nx_azure_iot_json_writer_append_property_with_string_value(
            json_writer,
            (UCHAR *)DEVICE_INFO_MODEL_PROPERTY_NAME,
            sizeof(DEVICE_INFO_MODEL_PROPERTY_NAME) - 1,
            (UCHAR *)DEVICE_INFO_MODEL_PROPERTY_VALUE,
            sizeof(DEVICE_INFO_MODEL_PROPERTY_VALUE) - 1) ||
        nx_azure_iot_json_writer_append_property_with_string_value(
            json_writer,
            (UCHAR *)DEVICE_INFO_SW_VERSION_PROPERTY_NAME,
            sizeof(DEVICE_INFO_SW_VERSION_PROPERTY_NAME) - 1,
            (UCHAR *)DEVICE_INFO_SW_VERSION_PROPERTY_VALUE,
            sizeof(DEVICE_INFO_SW_VERSION_PROPERTY_VALUE) - 1) ||
        nx_azure_iot_json_writer_append_property_with_string_value(
            json_writer,
            (UCHAR *)DEVICE_INFO_OS_NAME_PROPERTY_NAME,
            sizeof(DEVICE_INFO_OS_NAME_PROPERTY_NAME) - 1,
            (UCHAR *)DEVICE_INFO_OS_NAME_PROPERTY_VALUE,
            sizeof(DEVICE_INFO_OS_NAME_PROPERTY_VALUE) - 1) ||
        nx_azure_iot_json_writer_append_property_with_string_value(
            json_writer,
            (UCHAR *)DEVICE_INFO_PROCESSOR_ARCHITECTURE_PROPERTY_NAME,
            sizeof(DEVICE_INFO_PROCESSOR_ARCHITECTURE_PROPERTY_NAME) - 1,
            (UCHAR *)DEVICE_INFO_PROCESSOR_ARCHITECTURE_PROPERTY_VALUE,
            sizeof(DEVICE_INFO_PROCESSOR_ARCHITECTURE_PROPERTY_VALUE) - 1) ||
        nx_azure_iot_json_writer_append_property_with_string_value(
            json_writer,
            (UCHAR *)DEVICE_INFO_PROCESSOR_MANUFACTURER_PROPERTY_NAME,
            sizeof(DEVICE_INFO_PROCESSOR_MANUFACTURER_PROPERTY_NAME) - 1,
            (UCHAR *)DEVICE_INFO_PROCESSOR_MANUFACTURER_PROPERTY_VALUE,
            sizeof(DEVICE_INFO_PROCESSOR_MANUFACTURER_PROPERTY_VALUE) - 1) ||
        nx_azure_iot_json_writer_append_property_with_double_value(
            json_writer,
            (UCHAR *)DEVICE_INFO_TOTAL_STORAGE_PROPERTY_NAME,
            sizeof(DEVICE_INFO_TOTAL_STORAGE_PROPERTY_NAME) - 1,
            DEVICE_INFO_TOTAL_STORAGE_PROPERTY_VALUE,
            2) ||
        nx_azure_iot_json_writer_append_property_with_double_value(
            json_writer,
            (UCHAR *)DEVICE_INFO_TOTAL_MEMORY_PROPERTY_NAME,
            sizeof(DEVICE_INFO_TOTAL_MEMORY_PROPERTY_NAME) - 1,
            DEVICE_INFO_TOTAL_MEMORY_PROPERTY_VALUE,
            2))
    {
        return NX_NOT_SUCCESSFUL;
    }

    return NX_AZURE_IOT_SUCCESS;
}

static UINT append_device_telemetry(NX_AZURE_IOT_JSON_WRITER *json_writer, VOID *context)
{
    (void)context;
    float temperature = 42; // RS BSP_TSENSOR_ReadTemp();

    if (nx_azure_iot_json_writer_append_property_with_double_value(
            json_writer,
            (UCHAR *)TELEMETRY_TEMPERATURE,
            sizeof(TELEMETRY_TEMPERATURE) - 1,
            temperature,
            2))
    {
        return NX_NOT_SUCCESSFUL;
    }

    return NX_AZURE_IOT_SUCCESS;
}

static void set_led_state(bool level)
{
    if (level)
    {
        printf("LED is turned ON\r\n");
        // BSP_LED_On(LED_GREEN);
        palSetPad(GPIOB, GPIOB_LD2);
    }
    else
    {
        printf("LED is turned OFF\r\n");
        // BSP_LED_Off(LED_GREEN);
        palClearPad(GPIOB, GPIOB_LD2);
    }
}

static void direct_method_cb(
    AZURE_IOT_NX_CONTEXT *nx_context,
    const UCHAR *method,
    USHORT method_length,
    UCHAR *payload,
    USHORT payload_length,
    VOID *context,
    USHORT context_length)
{
    UINT status;
    UINT http_status = 501;
    CHAR *http_response = "{}";

    if (strncmp((CHAR *)method, SET_LED_STATE_COMMAND, method_length) == 0)
    {
        bool arg = (strncmp((CHAR *)payload, "true", payload_length) == 0);
        set_led_state(arg);

        azure_iot_nx_client_publish_bool_property(&azure_iot_nx_client, LED_STATE_PROPERTY, arg);

        http_status = 200;
    }

    if ((status = nx_azure_iot_hub_client_direct_method_message_response(
             &nx_context->iothub_client,
             http_status,
             context,
             context_length,
             (UCHAR *)http_response,
             strlen(http_response),
             NX_WAIT_FOREVER)))
    {
        printf("Direct method response failed! (0x%08x)\r\n", status);
        return;
    }
}

static void device_twin_desired_property_cb(
    UCHAR *component_name,
    UINT component_name_len,
    UCHAR *property_name,
    UINT property_name_len,
    NX_AZURE_IOT_JSON_READER property_value_reader,
    UINT version,
    VOID *userContextCallback)
{
    (void)component_name;
    UINT status;
    AZURE_IOT_NX_CONTEXT *nx_context = (AZURE_IOT_NX_CONTEXT *)userContextCallback;
    (void)component_name_len;
    if (strncmp((CHAR *)property_name, TELEMETRY_INTERVAL_PROPERTY, property_name_len) == 0)
    {
        status = nx_azure_iot_json_reader_token_int32_get(&property_value_reader, &telemetry_interval);
        if (status == NX_AZURE_IOT_SUCCESS)
        {
            // Confirm reception back to hub
            azure_nx_client_respond_int_writeable_property(
                nx_context,
                TELEMETRY_INTERVAL_PROPERTY,
                telemetry_interval,
                200,
                version);

            // Set a telemetry event so we pick up the change immediately
            tx_event_flags_set(&azure_iot_flags, TELEMETRY_INTERVAL_EVENT, TX_OR);
        }
    }
}

static void device_twin_property_cb(
    UCHAR *component_name,
    UINT component_name_len,
    UCHAR *property_name,
    UINT property_name_len,
    NX_AZURE_IOT_JSON_READER property_value_reader,
    UINT version,
    VOID *userContextCallback)
{
    (void)component_name;
    (void)component_name_len;
    (void)version;
    (void)userContextCallback;

    if (strncmp((CHAR *)property_name, TELEMETRY_INTERVAL_PROPERTY, property_name_len) == 0)
    {
        nx_azure_iot_json_reader_token_int32_get(&property_value_reader, &telemetry_interval);
    }
}

UINT azure_iot_nx_client_entry(
    NX_IP *ip_ptr,
    NX_PACKET_POOL *pool_ptr,
    NX_DNS *dns_ptr,
    UINT (*unix_time_callback)(ULONG *unix_time))
{
    UINT status;
    ULONG events = 0;

    if ((status = tx_event_flags_create(&azure_iot_flags, "Azure IoT flags")))
    {
        printf("FAIL: Unable to create nx_client event flags (0x%08x)\r\n", status);
        return status;
    }

    status =
        azure_iot_nx_client_create(&azure_iot_nx_client, ip_ptr, pool_ptr, dns_ptr, unix_time_callback, IOT_MODEL_ID);
    if (status != NX_SUCCESS)
    {
        printf("ERROR: azure_iot_nx_client_create failed (0x%08x)\r\n", status);
        return status;
    }

#ifdef ENABLE_X509
    status = azure_iot_nx_client_cert_set(
        &azure_iot_nx_client,
        (UCHAR *)iot_x509_device_cert,
        iot_x509_device_cert_len,
        (UCHAR *)iot_x509_private_key,
        iot_x509_private_key_len);
#else
    status = azure_iot_nx_client_sas_set(&azure_iot_nx_client, IOT_DEVICE_SAS_KEY);
#endif
    if (status != NX_SUCCESS)
    {
        printf("ERROR: azure_iot_nx_client_[sas|cert]_set failed (0x%08x)\r\n", status);
        return status;
    }

#ifdef ENABLE_DPS
    status = azure_iot_nx_client_dps_create(&azure_iot_nx_client, IOT_DPS_ID_SCOPE, IOT_DPS_REGISTRATION_ID);
#else
    status = azure_iot_nx_client_hub_create(&azure_iot_nx_client, IOT_HUB_HOSTNAME, IOT_HUB_DEVICE_ID);
#endif
    if (status != NX_SUCCESS)
    {
        printf("ERROR: azure_iot_nx_client_[hub|dps]_create failed (0x%08x)\r\n", status);
        return status;
    }

    // Register the callbacks
    azure_iot_nx_client_register_direct_method(&azure_iot_nx_client, direct_method_cb);
    azure_iot_nx_client_register_device_twin_desired_prop(&azure_iot_nx_client, device_twin_desired_property_cb);
    azure_iot_nx_client_register_device_twin_prop(&azure_iot_nx_client, device_twin_property_cb);

    if ((status = azure_iot_nx_client_connect(&azure_iot_nx_client)))
    {
        printf("ERROR: failed to connect nx client (0x%08x)\r\n", status);
        return status;
    }

    // Request the device twin for writeable property update
    if ((status = azure_iot_nx_client_device_twin_request_and_wait(&azure_iot_nx_client)))
    {
        printf("ERROR: azure_iot_nx_client_device_twin_request_and_wait failed (0x%08x)\r\n", status);
        return status;
    }

    // Send out property updates
    azure_iot_nx_client_publish_int_writeable_property(
        &azure_iot_nx_client,
        TELEMETRY_INTERVAL_PROPERTY,
        telemetry_interval);
    azure_iot_nx_client_publish_bool_property(&azure_iot_nx_client, LED_STATE_PROPERTY, false);
    azure_iot_nx_client_publish_properties(
        &azure_iot_nx_client,
        DEVICE_INFO_COMPONENT_NAME,
        append_device_info_properties);

    printf("\r\nStarting Main loop\r\n");

    while (true)
    {
        tx_event_flags_get(
            &azure_iot_flags,
            TELEMETRY_INTERVAL_EVENT,
            TX_OR_CLEAR,
            &events,
            telemetry_interval * NX_IP_PERIODIC_RATE);

        azure_iot_nx_client_publish_telemetry(&azure_iot_nx_client, append_device_telemetry);
    }

    return NX_SUCCESS;
}
***************/