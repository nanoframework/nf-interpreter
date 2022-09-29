//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright 2018 Silicon Laboratories Inc. All rights reserved.
// See LICENSE file in the project root for full license information.
//

//****************************************************************************
// Includes.

#include "sl_status.h"

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wundef"
#endif

#include <sl_usbd_core.h>
#include "sl_usbd_class_hid.h"

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#include "sl_usbd_class_vendor.h"

/* template headers */
#include "sl_usbd_configuration_instances.h"
#include "sl_usbd_class_vendor_instances.h"

/* include config file for the instances */

#include "sl_usbd_class_winusb_config.h"

//****************************************************************************
// Function declarations.

/* callback prototypes for winusb instance */

void sli_usbd_vendor_winusb_enable(uint8_t class_nbr);

void sli_usbd_vendor_winusb_disable(uint8_t class_nbr);

void sli_usbd_vendor_winusb_setup_req(uint8_t class_nbr, const sl_usbd_setup_req_t *p_setup_req);

//****************************************************************************
// Global variables.

/* variables for winusb instance */

uint8_t sl_usbd_vendor_winusb_number = 0;

sl_usbd_vendor_callbacks_t sli_usbd_vendor_winusb_callbacks = {
    sli_usbd_vendor_winusb_enable,
    sli_usbd_vendor_winusb_disable,
    sli_usbd_vendor_winusb_setup_req,
};

//****************************************************************************
// Callback functions.

void sli_usbd_vendor_winusb_enable(uint8_t class_nbr)
{
    (void)&class_nbr;

    sl_usbd_vendor_winusb_on_enable_event();

    return;
}

void sli_usbd_vendor_winusb_disable(uint8_t class_nbr)
{
    (void)&class_nbr;

    sl_usbd_vendor_winusb_on_disable_event();

    return;
}

void sli_usbd_vendor_winusb_setup_req(uint8_t class_nbr, const sl_usbd_setup_req_t *p_setup_req)
{
    (void)&class_nbr;

    sl_usbd_vendor_winusb_on_setup_request_event(p_setup_req);

    return;
}

//****************************************************************************
// Global functions.

/* initialize winusb instance */
void sli_usbd_vendor_winusb_init()
{
    bool intr_en = true;
    uint16_t interval = 0;

    uint8_t class_number = 0;
    uint8_t config_number = 0;

    char *configs = NULL;
    char *token = NULL;

    /* configs to attach the class instance to */
    configs = SL_USBD_VENDOR_WINUSB_CONFIGURATIONS;

    /* read interrupt enable flag */
    intr_en = SL_USBD_VENDOR_WINUSB_INTERRUPT_ENDPOINTS;

    /* read interval */
    interval = SL_USBD_VENDOR_WINUSB_INTERVAL;

    /* create vendor instance */
    sl_usbd_vendor_create_instance(intr_en, interval, &sli_usbd_vendor_winusb_callbacks, &class_number);

    /* store class number globally */
    sl_usbd_vendor_winusb_number = class_number;

    /* tokenize configs by "," and spaces */
    token = strtok(configs, ", ");

    /* loop over tokens */
    while (token != NULL)
    {

        /* add to config0? */
        if (!strcmp(token, "config0") || !strcmp(token, "all"))
        {
            config_number = sl_usbd_configuration_config0_number;
            sl_usbd_vendor_add_to_configuration(class_number, config_number);
        }

        /* next token */
        token = strtok(NULL, ", ");
    }
}

void sl_usbd_vendor_winusb_on_enable_event(void)
{
}

void sl_usbd_vendor_winusb_on_disable_event(void)
{
}

void sl_usbd_vendor_winusb_on_setup_request_event(const sl_usbd_setup_req_t *p_setup_req)
{
    (void)p_setup_req;
}
