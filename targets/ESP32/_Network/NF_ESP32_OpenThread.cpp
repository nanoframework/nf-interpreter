//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// This file includes the board specific OPenThread Intialisation

#if (HAL_USE_THREAD == TRUE)

#include <NF_ESP32_Network.h>
#include <esp32_idf.h>

#include "esp_openthread.h"
#include "esp_openthread_cli.h"
#include "esp_openthread_lock.h"
#include "esp_openthread_netif_glue.h"
#include "esp_openthread_types.h"
#include "esp_vfs_eventfd.h"
#include "openthread/logging.h"

//#define THREAD_DEVICE_MODE

static const char *TAG = "espOt";

#define OPENTHREAD_NETWORK_POLLPERIOD_TIME 3000

#define OPENTHREAD_RCP_UART_RX_PIN UART_PIN_NO_CHANGE
#define OPENTHREAD_RCP_UART_TX_PIN UART_PIN_NO_CHANGE


#define ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG()                   \
    {                                                           \
        .radio_mode = RADIO_MODE_NATIVE,                        \
    }

// Default config for RADIO connected via UART
#define ESP_OPENTHREAD_DEFAULT_UART_HOST_CONFIG()               \
    {                                                           \
        .host_connection_mode = HOST_CONNECTION_MODE_RCP_UART,  \
        .host_uart_config = {                                   \
            .port = 0,                                          \
            .uart_config =                                      \
                {                                               \
                    .baud_rate =  460800,                       \
                    .data_bits = UART_DATA_8_BITS,              \
                    .parity = UART_PARITY_DISABLE,              \
                    .stop_bits = UART_STOP_BITS_1,              \
                    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,      \
                    .rx_flow_ctrl_thresh = 0,                   \
                    .source_clk = UART_SCLK_DEFAULT,            \
                },                                              \
            .rx_pin = (gpio_num_t)OPENTHREAD_RCP_UART_RX_PIN,   \
            .tx_pin = (gpio_num_t)OPENTHREAD_RCP_UART_TX_PIN,   \
        },                                                      \
    }

#define ESP_OPENTHREAD_DEFAULT_SPI_HOST_CONFIG()                \
    {                                                           \
        .host_connection_mode = HOST_CONNECTION_MODE_RCP_SPI,   \
        .spi_slave_config = {                                   \
            .host_device = SPI2_HOST,                           \
            .bus_config = {                                     \
                .mosi_io_num = 3,                               \
                .miso_io_num = 1,                               \
                .sclk_io_num = 0,                               \
                .quadhd_io_num = -1,                            \
                .quadwp_io_num = -1,                            \
                .isr_cpu_id = INTR_CPU_ID_0,                    \
            },                                                  \
            .slave_config = {                                   \
                .mode = 0,                                      \
                .spics_io_num = 2,                              \
                .queue_size = 3,                                \
                .flags = 0,                                     \
            },                                                  \
            .intr_pin = 9,                                      \
        },                                                      \
    }


// Default config for native RADIO
#define ESP_OPENTHREAD_NO_HOST_CONFIG()                         \
    {                                                           \
        .host_connection_mode = HOST_CONNECTION_MODE_NONE       \
    }

#define ESP_OPENTHREAD_DEFAULT_PORT_CONFIG()    \
    {                                           \
        .storage_partition_name = "nvs",        \
        .netif_queue_size = 10,                 \
        .task_queue_size = 10,                  \
    }

static esp_netif_t *init_openthread_netif(const esp_openthread_platform_config_t *config)
{
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_OPENTHREAD();
    esp_netif_t *netif = esp_netif_new(&cfg);
    assert(netif != NULL);
 
    ESP_ERROR_CHECK(esp_netif_attach(netif, esp_openthread_netif_glue_init(config)));

    return netif;
}

#if (THREAD_NODE_ROLE == SED)
static void enableSleepyEndDevice(uint32_t pollPeriod)
{
    otInstance *instance = esp_openthread_get_instance();

    otLinkModeConfig linkMode = { 0 };
    linkMode.mRxOnWhenIdle = false;
    linkMode.mDeviceType = false;
    linkMode.mNetworkData = false;

    if (otLinkSetPollPeriod(instance, pollPeriod) != OT_ERROR_NONE) {
        ESP_LOGE(TAG, "Failed to set OpenThread pollperiod.");
    }

    if (otThreadSetLinkMode(instance, linkMode) != OT_ERROR_NONE) {
        ESP_LOGE(TAG, "Failed to set OpenThread linkmode.");
    }
}

static esp_err_t powerSaveInit()
{
    esp_err_t rc = ESP_OK;
#ifdef CONFIG_PM_ENABLE
    int cur_cpu_freq_mhz = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ;

    esp_pm_config_t pm_config = {
        .max_freq_mhz = cur_cpu_freq_mhz,
        .min_freq_mhz = cur_cpu_freq_mhz,
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
        .light_sleep_enable = true
#endif
    };

    rc = esp_pm_configure(&pm_config);
#endif
    return rc;
}
#endif

uint8_t charTobyte(char i)
{
  if(i >= '0' && i <= '9')
    return (uint8_t)(i - '0');
  if(i >= 'A' && i <= 'F')
    return (uint8_t)(i - 'A' + 10);
  if(i >= 'a' && i <= 'f')
    return (uint8_t)(i - 'a' + 10);
  return 0;
}

int hexTobin(const char* src, uint8_t* target)
{
  int len = 0;
  while(*src && src[1])
  {
    *(target++) = charTobyte(*src)*16 + charTobyte(src[1]);
    src += 2;
    len++;
  }
  return len;
}

static void threadMainTask(void *aContext)
{
    esp_openthread_platform_config_t config;

#if defined(CONFIG_OPENTHREAD_RADIO_NATIVE)
    config.radio_config = ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG();
    config.host_config = ESP_OPENTHREAD_NO_HOST_CONFIG();
#endif
#if defined(CONFIG_OPENTHREAD_RADIO_SPINEL_UART)
    config.radio_mode = RADIO_MODE_UART_RCP;
    config.host_config = ESP_OPENTHREAD_DEFAULT_UART_HOST_CONFIG();
#endif
#if defined(CONFIG_OPENTHREAD_RADIO_SPINEL_SPI)
    config.radio_mode = RADIO_MODE_SPI_RCP;
    config.host_config = ESP_OPENTHREAD_DEFAULT_SPI_HOST_CONFIG();
#endif

    config.port_config =  ESP_OPENTHREAD_DEFAULT_PORT_CONFIG();

    // Initialize the OpenThread stack
    ESP_ERROR_CHECK(esp_openthread_init(&config));

    // The OpenThread log level directly matches ESP log level
    (void)otLoggingSetLevel(CONFIG_LOG_DEFAULT_LEVEL);

    esp_netif_t *openthread_netif;
    // Initialize the esp_netif bindings
    openthread_netif = init_openthread_netif(&config);
    esp_netif_set_default_netif(openthread_netif);
    
#if (THREAD_NODE_ROLE == SED)
    // For enabling sleepy end device
    enableSleepyEndDevice(OPENTHREAD_NETWORK_POLLPERIOD_TIME);
    powerSaveInit();
#endif

    if (hal_strlen_s(THREAD_DATASETTLVS) > 0 )
    {
        otOperationalDatasetTlvs datasetTlvs;
    
//    const char * datasetString = "0e080000000000010000000300001435060004001fffe00208473b6d471ac9b62d0708fd881c8e81b2f28c0510b933f687d1537c252ee7e680d41e1483030f4f70656e5468726561642d35363139010256190410fd62447741af00be7f8139d6489127ef0c0402a0f7f8";
        const char * datasetString = THREAD_DATASETTLVS;
        datasetTlvs.mLength = hexTobin(datasetString, datasetTlvs.mTlvs);
        esp_openthread_auto_start(&datasetTlvs);
    }
    
    // Run the main loop
    esp_openthread_launch_mainloop();

    // Clean up
    esp_netif_destroy(openthread_netif);
    esp_openthread_netif_glue_deinit();
    
    esp_vfs_eventfd_unregister();
    vTaskDelete(NULL);
}

esp_err_t NF_ESP32_InitialiseOpenThread()
{   
    esp_vfs_eventfd_config_t eventfd_config = {
        .max_fds = 3,
    };

    ESP_ERROR_CHECK(esp_vfs_eventfd_register(&eventfd_config));

    int stack = 10000;

    xTaskCreate(threadMainTask, "otmain", stack, xTaskGetCurrentTaskHandle(), 5, NULL);

    return ESP_OK;
}

int NF_ESP32_OpenThread_Open(HAL_Configuration_NetworkInterface *config)
{
    (void)config;

    if (NF_ESP32_InitialiseOpenThread() == ESP_OK)
    {
         return NF_ESP32_Wait_NetNumber(IDF_OT_DEF);
    }

    return 0;
}

bool NF_ESP32_OpenThread_Close()
{
    esp_openthread_deinit();

    return true;
}
#endif