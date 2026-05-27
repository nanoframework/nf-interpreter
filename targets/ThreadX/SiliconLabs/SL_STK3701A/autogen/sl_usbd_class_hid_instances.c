//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright 2018 Silicon Laboratories Inc. All rights reserved.
// See LICENSE file in the project root for full license information.
//

//****************************************************************************
// Includes.

#include <sl_status.h>
#include <sl_usbd_core.h>
#include <sl_usbd_class_hid.h>

/* template headers */
#include <sl_usbd_configuration_instances.h>
#include <sl_usbd_class_hid_instances.h>

/* include config file for the instances */

#include <sl_usbd_class_hid0_config.h>

//****************************************************************************
// Function declarations.

/* callback prototypes for hid0 instance */
void sli_usbd_hid_hid0_enable(uint8_t class_nbr);

void sli_usbd_hid_hid0_disable(uint8_t class_nbr);

void sli_usbd_hid_hid0_get_report_desc(uint8_t class_nbr, const uint8_t **p_report_ptr, uint16_t *p_report_len);

void sli_usbd_hid_hid0_get_phy_desc(uint8_t class_nbr, const uint8_t **p_report_ptr, uint16_t *p_report_len);

void sli_usbd_hid_hid0_set_output_report(
    uint8_t class_nbr,
    uint8_t report_id,
    uint8_t *p_report_buf,
    uint16_t report_len);

void sli_usbd_hid_hid0_get_feature_report(
    uint8_t class_nbr,
    uint8_t report_id,
    uint8_t *p_report_buf,
    uint16_t report_len);

void sli_usbd_hid_hid0_set_feature_report(
    uint8_t class_nbr,
    uint8_t report_id,
    uint8_t *p_report_buf,
    uint16_t report_len);

void sli_usbd_hid_hid0_get_protocol(uint8_t class_nbr, uint8_t *p_protocol);

void sli_usbd_hid_hid0_set_protocol(uint8_t class_nbr, uint8_t protocol);

//****************************************************************************
// Global variables.

/* variables for mouse0 instance */

uint8_t sl_usbd_hid_hid0_number = 0;

uint8_t sl_usbd_hid_hid0_default_protocol = 0;

static const uint8_t sli_usbd_hid_hid0_default_desc[] = {
    SL_USBD_HID_GLOBAL_USAGE_PAGE + 1,   SL_USBD_HID_USAGE_PAGE_GENERIC_DEVICE_CONTROLS,
    SL_USBD_HID_LOCAL_USAGE + 1,         0x00,
    SL_USBD_HID_MAIN_COLLECTION + 1,     0xFF,
    SL_USBD_HID_LOCAL_USAGE + 1,         0x09,
    SL_USBD_HID_MAIN_COLLECTION + 1,     0x01,
    SL_USBD_HID_GLOBAL_USAGE_PAGE + 1,   0xA1,
    SL_USBD_HID_LOCAL_USAGE_MIN + 1,     0x01,
    SL_USBD_HID_LOCAL_USAGE_MAX + 1,     0x03,
    SL_USBD_HID_GLOBAL_LOG_MIN + 1,      0x00,
    SL_USBD_HID_GLOBAL_LOG_MAX + 1,      0x01,
    SL_USBD_HID_GLOBAL_REPORT_COUNT + 1, 0x03,
    SL_USBD_HID_GLOBAL_REPORT_SIZE + 1,  0x01,
    SL_USBD_HID_MAIN_INPUT + 1,          SL_USBD_HID_MAIN_DATA | SL_USBD_HID_MAIN_VARIABLE | SL_USBD_HID_MAIN_ABSOLUTE,
    SL_USBD_HID_GLOBAL_REPORT_COUNT + 1, 0x01,
    SL_USBD_HID_GLOBAL_REPORT_SIZE + 1,  0x0D,
    SL_USBD_HID_MAIN_INPUT + 1,          SL_USBD_HID_MAIN_CONSTANT,
    SL_USBD_HID_GLOBAL_USAGE_PAGE + 1,   SL_USBD_HID_USAGE_PAGE_GENERIC_DESKTOP_CONTROLS,
    SL_USBD_HID_LOCAL_USAGE + 1,         SL_USBD_HID_DV_X,
    SL_USBD_HID_LOCAL_USAGE + 1,         SL_USBD_HID_DV_Y,
    SL_USBD_HID_GLOBAL_LOG_MIN + 1,      0x81,
    SL_USBD_HID_GLOBAL_LOG_MAX + 1,      0x7F,
    SL_USBD_HID_GLOBAL_REPORT_SIZE + 1,  0x08,
    SL_USBD_HID_GLOBAL_REPORT_COUNT + 1, 0x02,
    SL_USBD_HID_MAIN_INPUT + 1,          SL_USBD_HID_MAIN_DATA | SL_USBD_HID_MAIN_VARIABLE | SL_USBD_HID_MAIN_RELATIVE,
    SL_USBD_HID_MAIN_ENDCOLLECTION,      SL_USBD_HID_MAIN_ENDCOLLECTION};

sl_usbd_hid_callbacks_t sli_usbd_hid_hid0_callbacks = {
    sli_usbd_hid_hid0_enable,
    sli_usbd_hid_hid0_disable,
    sli_usbd_hid_hid0_get_report_desc,
    sli_usbd_hid_hid0_get_phy_desc,
    sli_usbd_hid_hid0_set_output_report,
    sli_usbd_hid_hid0_get_feature_report,
    sli_usbd_hid_hid0_set_feature_report,
    sli_usbd_hid_hid0_get_protocol,
    sli_usbd_hid_hid0_set_protocol,
};

//****************************************************************************
// Callback functions.

/* callback functions for mouse0 instance */
void sli_usbd_hid_hid0_enable(uint8_t class_nbr)
{
    (void)&class_nbr;

    sl_usbd_hid_hid0_on_enable_event();

    return;
}

void sli_usbd_hid_hid0_disable(uint8_t class_nbr)
{
    (void)&class_nbr;

    sl_usbd_hid_hid0_on_disable_event();

    return;
}

void sli_usbd_hid_hid0_get_report_desc(uint8_t class_nbr, const uint8_t **p_report_ptr, uint16_t *p_report_len)
{
    (void)&class_nbr;

    *p_report_ptr = sli_usbd_hid_hid0_default_desc;
    *p_report_len = sizeof(sli_usbd_hid_hid0_default_desc);

    sl_usbd_hid_hid0_on_get_report_desc_event(p_report_ptr, p_report_len);

    return;
}

void sli_usbd_hid_hid0_get_phy_desc(uint8_t class_nbr, const uint8_t **p_report_ptr, uint16_t *p_report_len)
{
    (void)&class_nbr;

    *p_report_ptr = NULL;
    *p_report_len = 0;

    sl_usbd_hid_hid0_on_get_phy_desc_event(p_report_ptr, p_report_len);

    return;
}

void sli_usbd_hid_hid0_set_output_report(
    uint8_t class_nbr,
    uint8_t report_id,
    uint8_t *p_report_buf,
    uint16_t report_len)
{
    (void)&class_nbr;

    sl_usbd_hid_hid0_on_set_output_report_event(report_id, p_report_buf, report_len);

    return;
}

void sli_usbd_hid_hid0_get_feature_report(
    uint8_t class_nbr,
    uint8_t report_id,
    uint8_t *p_report_buf,
    uint16_t report_len)
{
    (void)&class_nbr;

    memset(p_report_buf, 0, report_len);

    sl_usbd_hid_hid0_on_get_feature_report_event(report_id, p_report_buf, report_len);

    return;
}

void sli_usbd_hid_hid0_set_feature_report(
    uint8_t class_nbr,
    uint8_t report_id,
    uint8_t *p_report_buf,
    uint16_t report_len)
{
    (void)&class_nbr;

    sl_usbd_hid_hid0_on_set_feature_report_event(report_id, p_report_buf, report_len);

    return;
}

void sli_usbd_hid_hid0_get_protocol(uint8_t class_nbr, uint8_t *p_protocol)
{
    (void)&class_nbr;

    *p_protocol = sl_usbd_hid_hid0_default_protocol;

    sl_usbd_hid_hid0_on_get_protocol_event(p_protocol);

    return;
}

void sli_usbd_hid_hid0_set_protocol(uint8_t class_nbr, uint8_t protocol)
{
    (void)&class_nbr;

    sl_usbd_hid_hid0_default_protocol = protocol;

    sl_usbd_hid_hid0_on_set_protocol_event(protocol);

    return;
}

//****************************************************************************
// Global functions.

/* initialize hid0 instance */
void sli_usbd_hid_hid0_init()
{
    sl_usbd_hid_country_code_t country = SL_USBD_HID_COUNTRY_CODE_NOT_SUPPORTED;

    uint8_t subclass = 0;
    uint8_t protocol = 0;

    uint16_t interval_in = 0;
    uint16_t interval_out = 0;
    bool ctrl_rd_en = true;

    uint8_t class_number = 0;
    uint8_t config_number = 0;
    char *configs = NULL;
    char *token = NULL;

    /* configs to attach the class instance to */
    configs = SL_USBD_HID_HID0_CONFIGURATIONS;

    /* read subclass, protocol, and country codes */
    subclass = SL_USBD_HID_HID0_SUBCLASS;
    protocol = SL_USBD_HID_HID0_PROTOCOL;
    country = SL_USBD_HID_HID0_COUNTRY_CODE;

    /* read endpoint parameters */
    interval_in = SL_USBD_HID_HID0_INTERVAL_IN;
    interval_out = SL_USBD_HID_HID0_INTERVAL_OUT;
    ctrl_rd_en = SL_USBD_HID_HID0_ENABLE_CTRL_RD;

    /* create HID instance */
    sl_usbd_hid_create_instance(
        subclass,
        protocol,
        country,
        interval_in,
        interval_out,
        ctrl_rd_en,
        &sli_usbd_hid_hid0_callbacks,
        &class_number);

    /* store class number globally */
    sl_usbd_hid_hid0_number = class_number;

    /* tokenize configs by "," and spaces */
    token = strtok(configs, ", ");

    /* loop over tokens */
    while (token != NULL)
    {

        /* add to config0? */
        if (!strcmp(token, "config0") || !strcmp(token, "all"))
        {
            config_number = sl_usbd_configuration_config0_number;
            sl_usbd_hid_add_to_configuration(class_number, config_number);
        }

        /* next token */
        token = strtok(NULL, ", ");
    }
}
