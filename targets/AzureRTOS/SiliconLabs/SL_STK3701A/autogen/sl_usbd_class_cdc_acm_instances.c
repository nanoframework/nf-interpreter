//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright 2018 Silicon Laboratories Inc. All rights reserved.
// See LICENSE file in the project root for full license information.
//

//****************************************************************************
// Includes.

#include <sl_status.h>
#include <sl_usbd_core.h>
#include <sl_usbd_class_cdc_acm.h>

/* template headers */
#include "sl_usbd_configuration_instances.h"
#include "sl_usbd_class_cdc_acm_instances.h"

/* include config file for the instances */

#include <sl_usbd_class_acm0_config.h>

//****************************************************************************
// Function declarations.

/* callback prototypes for acm0 instance */

void sli_usbd_cdc_acm_acm0_enable(uint8_t subclass_nbr);

void sli_usbd_cdc_acm_acm0_disable(uint8_t subclass_nbr);

void sli_usbd_cdc_acm_acm0_line_control_changed(uint8_t subclass_nbr, uint8_t event, uint8_t event_chngd);

bool sli_usbd_cdc_acm_acm0_line_coding_changed(uint8_t subclass_nbr, sl_usbd_cdc_acm_line_coding_t *p_line_coding);

//****************************************************************************
// Global variables.

/* variables for acm0 instance */

uint8_t sl_usbd_cdc_acm_acm0_number = 0;

sl_usbd_cdc_acm_callbacks_t sli_usbd_cdc_acm_acm0_callbacks = {
    sli_usbd_cdc_acm_acm0_enable,
    sli_usbd_cdc_acm_acm0_disable,
    sli_usbd_cdc_acm_acm0_line_control_changed,
    sli_usbd_cdc_acm_acm0_line_coding_changed,
};

//****************************************************************************
// Callback functions.

/* callback functions for acm0 instance */
void sli_usbd_cdc_acm_acm0_enable(uint8_t subclass_nbr)
{
    (void)&subclass_nbr;
    sl_usbd_cdc_acm_acm0_on_enable_event();
    return;
}

void sli_usbd_cdc_acm_acm0_disable(uint8_t subclass_nbr)
{
    (void)&subclass_nbr;
    sl_usbd_cdc_acm_acm0_on_disable_event();
    return;
}

void sli_usbd_cdc_acm_acm0_line_control_changed(uint8_t subclass_nbr, uint8_t event, uint8_t event_chngd)
{
    (void)&subclass_nbr;
    (void)&event;
    (void)&event_chngd;
    sl_usbd_cdc_acm_acm0_on_line_control_event();
    return;
}

bool sli_usbd_cdc_acm_acm0_line_coding_changed(uint8_t subclass_nbr, sl_usbd_cdc_acm_line_coding_t *p_line_coding)
{
    (void)&subclass_nbr;
    (void)&p_line_coding;
    sl_usbd_cdc_acm_acm0_on_line_coding_event();
    return true;
}

//****************************************************************************
// Global functions.

/* initialize acm0 instance */
void sli_usbd_cdc_acm_acm0_init()
{
    uint16_t interval = 0;
    uint16_t capabilities = 0;
    uint8_t class_number = 0;
    uint8_t config_number = 0;
    char *configs = NULL;
    char *token = NULL;

    /* configs to attach the class instance to */
    configs = SL_USBD_CDC_ACM_ACM0_CONFIGURATIONS;

    /* line state notification interval for that instance */
    interval = SL_USBD_CDC_ACM_ACM0_NOTIFY_INTERVAL;

    /* call management capabilities */
    if (SL_USBD_CDC_ACM_ACM0_CALL_MGMT_ENABLE == 1)
    {
        capabilities |= SL_USBD_CDC_ACM_CALL_MGMT_DEV;
    }

    /* call management DCI interface */
    if (SL_USBD_CDC_ACM_ACM0_CALL_MGMT_DCI == 1)
    {
        capabilities |= SL_USBD_CDC_ACM_CALL_MGMT_DATA_CCI_DCI;
    }

    /* create CDC ACM instance */
    sl_usbd_cdc_acm_create_instance(interval, capabilities, &sli_usbd_cdc_acm_acm0_callbacks, &class_number);

    /* store class number globally */
    sl_usbd_cdc_acm_acm0_number = class_number;

    /* tokenize configs by "," and spaces */
    token = strtok(configs, ", ");

    /* loop over tokens */
    while (token != NULL)
    {

        /* add to config0? */
        if (!strcmp(token, "config0") || !strcmp(token, "all"))
        {
            config_number = sl_usbd_configuration_config0_number;
            sl_usbd_cdc_acm_add_to_configuration(class_number, config_number);
        }

        /* next token */
        token = strtok(NULL, ", ");
    }
}

__WEAK void sl_usbd_cdc_acm_acm0_on_enable_event(void)
{
}

__WEAK void sl_usbd_cdc_acm_acm0_on_disable_event(void)
{
}

__WEAK void sl_usbd_cdc_acm_acm0_on_line_control_event(void)
{
}

__WEAK void sl_usbd_cdc_acm_acm0_on_line_coding_event(void)
{
}
