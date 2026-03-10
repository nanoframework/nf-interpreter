#ifndef SL_IOSTREAM_INIT_USART_INSTANCES_H
#define SL_IOSTREAM_INIT_USART_INSTANCES_H
#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#include "sl_power_manager.h"
#endif
#include "sl_iostream.h"
#include "sl_iostream_uart.h"
#include <target_platform.h>

#ifdef __cplusplus
extern "C"
{
#endif

#if HAL_WP_USE_SERIAL == TRUE
    extern sl_iostream_t *sl_iostream_vcom_handle;
    extern sl_iostream_uart_t *sl_iostream_uart_vcom_handle;
    extern sl_iostream_instance_info_t sl_iostream_instance_vcom_info;
#endif
#if HAL_USE_ONEWIRE == TRUE
    extern sl_iostream_t *sl_iostream_onewire_handle;
    extern sl_iostream_uart_t *sl_iostream_uart_onewire_handle;
    extern sl_iostream_instance_info_t sl_iostream_instance_onewire_info;
#endif

    // Initialize only iostream usart instance(s)
    void sl_iostream_usart_init_instances(void);

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)

#if HAL_WP_USE_SERIAL == TRUE
    sl_power_manager_on_isr_exit_t sl_iostream_usart_vcom_sleep_on_isr_exit(void);
    bool sl_iostream_usart_vcom_is_ok_to_sleep(void);
#endif
#if HAL_USE_ONEWIRE == TRUE
    sl_power_manager_on_isr_exit_t sl_iostream_usart_onewire_sleep_on_isr_exit(void);
    bool sl_iostream_usart_onewire_is_ok_to_sleep(void);
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif // SL_IOSTREAM_INIT_USART_INSTANCES_H
