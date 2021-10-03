/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/

#include "nx_azure_iot_pnp_helpers.h"

#include <stdio.h>

#include "azure/core/az_json.h"
#include "nx_api.h"

/* Telemetry message property used to indicate the message's component.  */
static const CHAR sample_pnp_telemetry_component_property[] = "$.sub";

/* Reported property response property keys.  */
static const CHAR sample_pnp_component_type_property_name[] = "__t";
static const CHAR reported_component_type_value[]           = "c";
static const CHAR reported_value_property_name[]            = "value";
static const CHAR reported_status_property_name[]           = "ac";
static const CHAR reported_version_property_name[]          = "av";
static const CHAR reported_description_property_name[]      = "ad";

/* PnP command seperator.  */
static const az_span command_separator = AZ_SPAN_LITERAL_FROM_STR("*");

/* Device twin keys */
static const CHAR sample_iot_hub_twin_desired_version[] = "$version";
static const CHAR sample_iot_hub_twin_desired[]         = "desired";

/* Move reader to the value of property name.  */
static UINT sample_json_child_token_move(
    NX_AZURE_IOT_JSON_READER* json_reader_ptr, UCHAR* property_name_ptr, UINT property_name_len)
{
    while (nx_azure_iot_json_reader_next_token(json_reader_ptr) == NX_AZURE_IOT_SUCCESS)
    {
        if ((nx_azure_iot_json_reader_token_type(json_reader_ptr) == NX_AZURE_IOT_READER_TOKEN_PROPERTY_NAME) &&
            nx_azure_iot_json_reader_token_is_text_equal(json_reader_ptr, property_name_ptr, property_name_len))
        {
            if (nx_azure_iot_json_reader_next_token(json_reader_ptr))
            {
                printf("Failed to read next token\r\n");
                return (NX_NOT_SUCCESSFUL);
            }

            return (NX_AZURE_IOT_SUCCESS);
        }
        else if (nx_azure_iot_json_reader_token_type(json_reader_ptr) == NX_AZURE_IOT_READER_TOKEN_BEGIN_OBJECT)
        {
            if (nx_azure_iot_json_reader_skip_children(json_reader_ptr))
            {
                printf("Failed to skip child of complex object\r\n");
                return (NX_NOT_SUCCESSFUL);
            }
        }
        else if (nx_azure_iot_json_reader_token_type(json_reader_ptr) == NX_AZURE_IOT_READER_TOKEN_END_OBJECT)
        {
            return (NX_AZURE_IOT_NOT_FOUND);
        }
    }

    return (NX_AZURE_IOT_NOT_FOUND);
}

/* Visit component property Object and call callback on each property of that component.  */
static UINT visit_component_properties(UCHAR* component_name_ptr,
    UINT component_name_len,
    NX_AZURE_IOT_JSON_READER* json_reader_ptr,
    UINT version,
    UCHAR* scratch_buf,
    UINT scratch_buf_len,
    VOID (*sample_desired_property_callback)(UCHAR* component_name_ptr,
        UINT component_name_len,
        UCHAR* property_name_ptr,
        UINT property_name_len,
        NX_AZURE_IOT_JSON_READER property_value_reader,
        UINT version,
        VOID* userContextCallback),
    VOID* context_ptr)
{
    UINT len;

    while (nx_azure_iot_json_reader_next_token(json_reader_ptr) == NX_AZURE_IOT_SUCCESS)
    {
        if (nx_azure_iot_json_reader_token_type(json_reader_ptr) == NX_AZURE_IOT_READER_TOKEN_PROPERTY_NAME)
        {
            if (nx_azure_iot_json_reader_token_string_get(json_reader_ptr, scratch_buf, scratch_buf_len, &len))
            {
                printf("Failed to get string property value\r\n");
                return (NX_NOT_SUCCESSFUL);
            }

            if (nx_azure_iot_json_reader_next_token(json_reader_ptr))
            {
                printf("Failed to get next token\r\n");
                return (NX_NOT_SUCCESSFUL);
            }

            if ((len == sizeof(sample_pnp_component_type_property_name) - 1) &&
                (memcmp((VOID*)scratch_buf, (VOID*)sample_pnp_component_type_property_name, len) == 0))
            {
                continue;
            }

            if ((len == sizeof(sample_iot_hub_twin_desired_version) - 1) &&
                (memcmp((VOID*)scratch_buf, (VOID*)sample_iot_hub_twin_desired_version, len) == 0))
            {
                continue;
            }

            sample_desired_property_callback(
                component_name_ptr, component_name_len, scratch_buf, len, *json_reader_ptr, version, context_ptr);
        }

        if (nx_azure_iot_json_reader_token_type(json_reader_ptr) == NX_AZURE_IOT_READER_TOKEN_BEGIN_OBJECT)
        {
            if (nx_azure_iot_json_reader_skip_children(json_reader_ptr))
            {
                printf("Failed to skip children of object\r\n");
                return (NX_NOT_SUCCESSFUL);
            }
        }
        else if (nx_azure_iot_json_reader_token_type(json_reader_ptr) == NX_AZURE_IOT_READER_TOKEN_END_OBJECT)
        {
            break;
        }
    }

    return (NX_AZURE_IOT_SUCCESS);
}

/* Check if component is part of component list.  */
static UINT is_component_in_model(UCHAR* component_name_ptr,
    UINT component_name_len,
    CHAR** sample_components_ptr,
    UINT sample_components_num,
    UINT* out_index)
{
    UINT index = 0;

    if (component_name_ptr == NX_NULL || component_name_len == 0)
    {
        return (NX_NOT_SUCCESSFUL);
    }

    while (index < sample_components_num)
    {
        if ((component_name_len == strlen(sample_components_ptr[index])) &&
            (memcmp((VOID*)component_name_ptr, (VOID*)sample_components_ptr[index], component_name_len) == 0))
        {
            *out_index = index;
            return (NX_AZURE_IOT_SUCCESS);
        }

        index++;
    }

    return (NX_AZURE_IOT_NOT_FOUND);
}

/* Parse PnP command names.  */
UINT nx_azure_iot_pnp_helper_command_name_parse(const UCHAR* method_name_ptr,
    UINT method_name_length,
    const UCHAR** component_name_pptr,
    UINT* component_name_length_ptr,
    const UCHAR** pnp_command_name_pptr,
    UINT* pnp_command_name_length_ptr)
{
    INT index;
    az_span method_name = az_span_create((UCHAR*)method_name_ptr, (INT)method_name_length);

    if ((index = az_span_find(method_name, command_separator)) != -1)
    {
        /* If a separator character is present in the device method name, then a command on a subcomponent of
           the model is being targeted (e.g. thermostat1*getMaxMinReport).  */
        *component_name_pptr         = method_name_ptr;
        *component_name_length_ptr   = (UINT)index;
        *pnp_command_name_pptr       = method_name_ptr + index + 1;
        *pnp_command_name_length_ptr = method_name_length - (UINT)index - 1;
    }
    else
    {
        /* The separator character is optional.  If it is not present, it indicates a command of the root
           component and not a subcomponent (e.g. "reboot").  */
        *component_name_pptr         = NULL;
        *component_name_length_ptr   = 0;
        *pnp_command_name_pptr       = method_name_ptr;
        *pnp_command_name_length_ptr = method_name_length;
    }

    return (NX_AZURE_IOT_SUCCESS);
}

/* Parse twin data and call callback on each desired property.  */
UINT nx_azure_iot_pnp_helper_twin_data_parse(NX_AZURE_IOT_JSON_READER* json_reader_ptr,
    UINT is_partial,
    CHAR** sample_components_ptr,
    UINT sample_components_num,
    UCHAR* scratch_buf,
    UINT scratch_buf_len,
    VOID (*sample_desired_property_callback)(UCHAR* component_name_ptr,
        UINT component_name_len,
        UCHAR* property_name_ptr,
        UINT property_name_len,
        NX_AZURE_IOT_JSON_READER property_value_reader,
        UINT version,
        VOID* userContextCallback),
    VOID* context_ptr)
{
    NX_AZURE_IOT_JSON_READER copy_json_reader;
    UINT version;
    UINT len;
    UINT index;
    UINT status;

    if ((status = nx_azure_iot_json_reader_next_token(json_reader_ptr)))
    {
        printf("Failed to initialize json reader: error %d\r\n", status);
        return (status);
    }

    if (!is_partial &&
        sample_json_child_token_move(
            json_reader_ptr, (UCHAR*)sample_iot_hub_twin_desired, sizeof(sample_iot_hub_twin_desired) - 1))
    {
        printf("Failed to get desired property\r\n");
        return (NX_NOT_SUCCESSFUL);
    }

    copy_json_reader = *json_reader_ptr;
    if (sample_json_child_token_move(&copy_json_reader,
            (UCHAR*)sample_iot_hub_twin_desired_version,
            sizeof(sample_iot_hub_twin_desired_version) - 1) ||
        nx_azure_iot_json_reader_token_int32_get(&copy_json_reader, (int32_t*)&version))
    {
        printf("Failed to get version\r\n");
        return (NX_NOT_SUCCESSFUL);
    }

    while (nx_azure_iot_json_reader_next_token(json_reader_ptr) == NX_AZURE_IOT_SUCCESS)
    {
        if (nx_azure_iot_json_reader_token_type(json_reader_ptr) == NX_AZURE_IOT_READER_TOKEN_PROPERTY_NAME)
        {
            if (nx_azure_iot_json_reader_token_string_get(json_reader_ptr, scratch_buf, scratch_buf_len, &len))
            {
                printf("Failed to string value for property name\r\n");
                return (NX_NOT_SUCCESSFUL);
            }

            if (nx_azure_iot_json_reader_next_token(json_reader_ptr))
            {
                printf("Failed to next token\r\n");
                return (NX_NOT_SUCCESSFUL);
            }

            if ((len == sizeof(sample_iot_hub_twin_desired_version) - 1) &&
                (memcmp((VOID*)sample_iot_hub_twin_desired_version, (VOID*)scratch_buf, len) == 0))
            {
                continue;
            }

            if (nx_azure_iot_json_reader_token_type(json_reader_ptr) == NX_AZURE_IOT_READER_TOKEN_BEGIN_OBJECT &&
                sample_components_ptr != NX_NULL &&
                (is_component_in_model(scratch_buf, len, sample_components_ptr, sample_components_num, &index) ==
                    NX_AZURE_IOT_SUCCESS))
            {
                if (visit_component_properties((UCHAR*)sample_components_ptr[index],
                        strlen(sample_components_ptr[index]),
                        json_reader_ptr,
                        version,
                        scratch_buf,
                        scratch_buf_len,
                        sample_desired_property_callback,
                        context_ptr))
                {
                    printf("Failed to visit component properties\r\n");
                    return (NX_NOT_SUCCESSFUL);
                }
            }
            else
            {
                sample_desired_property_callback(NX_NULL, 0, scratch_buf, len, *json_reader_ptr, version, context_ptr);

                if (nx_azure_iot_json_reader_token_type(json_reader_ptr) == NX_AZURE_IOT_READER_TOKEN_BEGIN_OBJECT)
                {
                    if (nx_azure_iot_json_reader_skip_children(json_reader_ptr))
                    {
                        printf("Failed to skip children of object\r\n");
                        return (NX_NOT_SUCCESSFUL);
                    }
                }
            }
        }
        else if (nx_azure_iot_json_reader_token_type(json_reader_ptr) == NX_AZURE_IOT_READER_TOKEN_BEGIN_OBJECT)
        {
            if (nx_azure_iot_json_reader_skip_children(json_reader_ptr))
            {
                printf("Failed to skip children of object\r\n");
                return (NX_NOT_SUCCESSFUL);
            }
        }
        else if (nx_azure_iot_json_reader_token_type(json_reader_ptr) == NX_AZURE_IOT_READER_TOKEN_END_OBJECT)
        {
            break;
        }
    }

    return (NX_AZURE_IOT_SUCCESS);
}

/* Create PnP telemetry message.  */
UINT nx_azure_iot_pnp_helper_telemetry_message_create(NX_AZURE_IOT_HUB_CLIENT* iothub_client_ptr,
    UCHAR* component_name,
    UINT component_name_len,
    NX_PACKET** packet_pptr,
    UINT wait_option)
{
    UINT status;

    /* Create a telemetry message packet.  */
    if ((status = nx_azure_iot_hub_client_telemetry_message_create(iothub_client_ptr, packet_pptr, wait_option)))
    {
        printf("Telemetry message create failed!: error code = 0x%08x\r\n", status);
    }

    /* If the component will be used, then specify this as a property of the message.  */
    else if ((component_name != NULL) && (status = nx_azure_iot_hub_client_telemetry_property_add(*packet_pptr,
                                              (UCHAR*)sample_pnp_telemetry_component_property,
                                              (USHORT)sizeof(sample_pnp_telemetry_component_property) - 1,
                                              component_name,
                                              (USHORT)component_name_len,
                                              NX_WAIT_FOREVER)) != NX_AZURE_IOT_SUCCESS)
    {
        printf("nx_azure_iot_hub_client_telemetry_property_add=%s failed, error=%d",
            sample_pnp_telemetry_component_property,
            status);
        nx_azure_iot_hub_client_telemetry_message_delete(*packet_pptr);
    }
    else
    {
        status = NX_AZURE_IOT_SUCCESS;
    }

    return (status);
}

/* Build PnP reported property into user provided buffer.  */
UINT nx_azure_iot_pnp_helper_build_reported_property(UCHAR* component_name_ptr,
    UINT component_name_len,
    UINT (*append_reported_property)(NX_AZURE_IOT_JSON_WRITER* json_builder_ptr, VOID* context),
    VOID* context,
    NX_AZURE_IOT_JSON_WRITER* json_builder_ptr)
{
    UINT status;

    if (nx_azure_iot_json_writer_append_begin_object(json_builder_ptr) ||
        (component_name_ptr != NX_NULL &&
            (nx_azure_iot_json_writer_append_property_name(json_builder_ptr, component_name_ptr, component_name_len) ||
                nx_azure_iot_json_writer_append_begin_object(json_builder_ptr) ||
                nx_azure_iot_json_writer_append_property_with_string_value(json_builder_ptr,
                    (UCHAR*)sample_pnp_component_type_property_name,
                    sizeof(sample_pnp_component_type_property_name) - 1,
                    (UCHAR*)reported_component_type_value,
                    sizeof(reported_component_type_value) - 1))) ||
        (append_reported_property(json_builder_ptr, context) != NX_AZURE_IOT_SUCCESS) ||
        (component_name_ptr != NX_NULL && nx_azure_iot_json_writer_append_end_object(json_builder_ptr)) ||
        nx_azure_iot_json_writer_append_end_object(json_builder_ptr))
    {
        printf("Failed to build reported property\r\n");
        status = NX_NOT_SUCCESSFUL;
    }
    else
    {
        status = NX_AZURE_IOT_SUCCESS;
    }

    return (status);
}

/* Build reported property with status.  */
UINT nx_azure_iot_pnp_helper_build_reported_property_with_status(UCHAR* component_name_ptr,
    UINT component_name_len,
    UCHAR* property_name_ptr,
    UINT property_name_len,
    UINT (*append_value)(NX_AZURE_IOT_JSON_WRITER* builder, VOID* context),
    VOID* context,
    INT result,
    UCHAR* description_ptr,
    UINT description_len,
    UINT ack_version,
    NX_AZURE_IOT_JSON_WRITER* json_builder_ptr)
{
    if (nx_azure_iot_json_writer_append_begin_object(json_builder_ptr))
    {
        printf("Failed initializing json writer \r\n");
        return (NX_NOT_SUCCESSFUL);
    }

    if (component_name_ptr != NX_NULL &&
        (nx_azure_iot_json_writer_append_property_name(json_builder_ptr, component_name_ptr, component_name_len) ||
            nx_azure_iot_json_writer_append_begin_object(json_builder_ptr) ||
            nx_azure_iot_json_writer_append_property_with_string_value(json_builder_ptr,
                (UCHAR*)sample_pnp_component_type_property_name,
                sizeof(sample_pnp_component_type_property_name) - 1,
                (UCHAR*)reported_component_type_value,
                sizeof(reported_component_type_value) - 1)))
    {
        printf("Failed build reported property with status message \r\n");
        return (NX_NOT_SUCCESSFUL);
    }

    if (nx_azure_iot_json_writer_append_property_name(json_builder_ptr, property_name_ptr, property_name_len) ||
        nx_azure_iot_json_writer_append_begin_object(json_builder_ptr) ||
        nx_azure_iot_json_writer_append_property_name(
            json_builder_ptr, (UCHAR*)reported_value_property_name, sizeof(reported_value_property_name) - 1) ||
        (append_value(json_builder_ptr, context) != NX_AZURE_IOT_SUCCESS) ||
        nx_azure_iot_json_writer_append_property_with_int32_value(json_builder_ptr,
            (UCHAR*)reported_status_property_name,
            sizeof(reported_status_property_name) - 1,
            result) ||
        nx_azure_iot_json_writer_append_property_with_string_value(json_builder_ptr,
            (UCHAR*)reported_description_property_name,
            sizeof(reported_description_property_name) - 1,
            description_ptr,
            description_len) ||
        nx_azure_iot_json_writer_append_property_with_int32_value(json_builder_ptr,
            (UCHAR*)reported_version_property_name,
            sizeof(reported_version_property_name) - 1,
            (INT)ack_version) ||
        nx_azure_iot_json_writer_append_end_object(json_builder_ptr) ||
        nx_azure_iot_json_writer_append_end_object(json_builder_ptr))
    {
        printf("Failed build reported property with status message\r\n");
        return (NX_NOT_SUCCESSFUL);
    }

    if (component_name_ptr != NX_NULL && nx_azure_iot_json_writer_append_end_object(json_builder_ptr))
    {
        printf("Failed build reported property with status message\r\n");
        return (NX_NOT_SUCCESSFUL);
    }

    return (NX_AZURE_IOT_SUCCESS);
}
