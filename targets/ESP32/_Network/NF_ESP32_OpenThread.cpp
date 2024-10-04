//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#if (HAL_USE_THREAD == TRUE)

#include <list>
#include <string>

#include <NF_ESP32_Network.h>
#include <esp32_idf.h>
#include <nanoprintf.h>

#include "esp_openthread.h"
#include "esp_openthread_cli.h"
#include "esp_openthread_lock.h"
#include "esp_openthread_netif_glue.h"
#include "esp_openthread_types.h"
#include "esp_vfs_eventfd.h"
#include "openthread/logging.h"
#include "openthread/cli.h"
#include "openthread/coap.h"
#include "Esp32_DeviceMapping.h"
#include "../_nanoCLR/nanoFramework.Networking.Thread/net_thread_native.h"

static const char *TAG = "espOt";

extern void ThreadSetInterfaceNumber(int networkInterfaceNumber);

#define OPENTHREAD_NETWORK_POLLPERIOD_TIME 3000

#define OPENTHREAD_RCP_UART_RX_PIN UART_PIN_NO_CHANGE
#define OPENTHREAD_RCP_UART_TX_PIN UART_PIN_NO_CHANGE

#define ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG()                                                                          \
    {                                                                                                                  \
        .radio_mode = RADIO_MODE_NATIVE,                                                                               \
    }

// Default config for RADIO connected via UART
#define ESP_OPENTHREAD_DEFAULT_UART_HOST_CONFIG()                                                                      \
    {                                                                                                                  \
        .host_connection_mode = HOST_CONNECTION_MODE_RCP_UART,                                                         \
        .host_uart_config =                                                                                            \
            {                                                                                                          \
                .port = (uart_port_t)0,                                                                                \
                .uart_config =                                                                                         \
                    {                                                                                                  \
                        .baud_rate = 460800,                                                                           \
                        .data_bits = UART_DATA_8_BITS,                                                                 \
                        .parity = UART_PARITY_DISABLE,                                                                 \
                        .stop_bits = UART_STOP_BITS_1,                                                                 \
                        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,                                                         \
                        .rx_flow_ctrl_thresh = 0,                                                                      \
                        .source_clk = UART_SCLK_DEFAULT,                                                               \
                    },                                                                                                 \
                .rx_pin = (gpio_num_t)OPENTHREAD_RCP_UART_RX_PIN,                                                      \
                .tx_pin = (gpio_num_t)OPENTHREAD_RCP_UART_TX_PIN,                                                      \
            },                                                                                                         \
    }

#define ESP_OPENTHREAD_DEFAULT_SPI_HOST_CONFIG()                                                                       \
    {                                                                                                                  \
        .host_connection_mode = HOST_CONNECTION_MODE_RCP_SPI,                                                          \
        .spi_slave_config =                                                                                            \
            {                                                                                                          \
                .host_device = SPI2_HOST,                                                                              \
                .bus_config =                                                                                          \
                    {                                                                                                  \
                        .mosi_io_num = 3,                                                                              \
                        .miso_io_num = 1,                                                                              \
                        .sclk_io_num = 0,                                                                              \
                        .quadwp_io_num = -1,                                                                           \
                        .quadhd_io_num = -1,                                                                           \
                        .isr_cpu_id = ESP_INTR_CPU_AFFINITY_0,                                                         \
                    },                                                                                                 \
                .slave_config =                                                                                        \
                    {                                                                                                  \
                        .spics_io_num = 2,                                                                             \
                        .flags = 0,                                                                                    \
                        .queue_size = 3,                                                                               \
                        .mode = 0,                                                                                     \
                    },                                                                                                 \
                .intr_pin = (gpio_num_t)9,                                                                             \
                                                                                                                       \
            },                                                                                                         \
    }

// Default config for native RADIO
#define ESP_OPENTHREAD_NO_HOST_CONFIG() {.host_connection_mode = HOST_CONNECTION_MODE_NONE}

#define ESP_OPENTHREAD_DEFAULT_PORT_CONFIG()                                                                           \
    {                                                                                                                  \
        .storage_partition_name = "nvs",                                                                               \
        .netif_queue_size = 10,                                                                                        \
        .task_queue_size = 10,                                                                                         \
    }

static esp_netif_t *openthread_netif = NULL;

// Maximum number of entries in cliOutputsList
#define cliListMaxSize 50
// Maximum line size
#define cliBufferSize 120

// line buffer used to accumulate cli output until we have a whole line
static char cliLineBuffer[cliBufferSize];
static short cliLineBufferCount = 0;
static short cliIgnoreLine = 0;

static std::list<std::string> cliOutputsList;
bool cliHandleResponse = false;
bool IsThreadInitialised = false;
TaskHandle_t threadTask;

static esp_netif_t *init_openthread_netif(const esp_openthread_platform_config_t *config)
{
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_OPENTHREAD();
    esp_netif_t *netif = esp_netif_new(&cfg);
    assert(netif != NULL);

    ESP_ERROR_CHECK(esp_netif_attach(netif, esp_openthread_netif_glue_init(config)));

    return netif;
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

static void setDeviceType(ThreadDeviceType devType, uint32_t pollPeriod)
{
    otInstance *instance = esp_openthread_get_instance();

    otLinkModeConfig linkMode = {linkMode.mRxOnWhenIdle = true};

    switch (devType)
    {
        case ThreadDeviceType_Router:
            linkMode.mRxOnWhenIdle = true;
            linkMode.mDeviceType = true;
            linkMode.mNetworkData = true;
            break;

        case ThreadDeviceType_EndDevice:
            linkMode.mRxOnWhenIdle = true;
            // Not FTD ( end device )
            linkMode.mDeviceType = false;
            linkMode.mNetworkData = false;
            break;

        case ThreadDeviceType_SleepyEndDevice:
            // Not receiving all the time
            // sleepy end device will poll router for info
            linkMode.mRxOnWhenIdle = false;
            // Not FTD ( end device )
            linkMode.mDeviceType = false;
            linkMode.mNetworkData = false;

            // Set poll period for polling
            if (otLinkSetPollPeriod(instance, pollPeriod) != OT_ERROR_NONE)
            {
                ESP_LOGE(TAG, "Failed to set OpenThread poll period.");
            }

            powerSaveInit();
            break;
    }

    if (otThreadSetLinkMode(instance, linkMode) != OT_ERROR_NONE)
    {
        ESP_LOGE(TAG, "Failed to set OpenThread linkmode.");
    }
}

uint8_t charTobyte(char i)
{
    if (i >= '0' && i <= '9')
    {
        return (uint8_t)(i - '0');
    }

    if (i >= 'A' && i <= 'F')
    {
        return (uint8_t)(i - 'A' + 10);
    }

    if (i >= 'a' && i <= 'f')
    {
        return (uint8_t)(i - 'a' + 10);
    }

    return 0;
}

int hexTobin(const char *src, uint8_t *target)
{
    int len = 0;

    while (*src && src[1])
    {
        *(target++) = charTobyte(*src) * 16 + charTobyte(src[1]);
        src += 2;
        len++;
    }

    return len;
}

void ResetLineBuffer()
{
    cliLineBuffer[0] = 0;
    cliLineBufferCount = 0;
}

//
//  We need to accumulate output into line buffer until we see a CRLF
//
int cliOutputCallback(void *aContext, const char *aFormat, va_list aArguments)
{
    // Find length of string with args
    int length = vsnprintf(nullptr, 0, aFormat, aArguments);

    if (((cliLineBufferCount + length) > cliBufferSize) || cliIgnoreLine > 0)
    {
        cliIgnoreLine = 0;

        // ignore as too big for buffer
        return length;
    }

    // format into buffer
    vsnprintf(&cliLineBuffer[cliLineBufferCount], cliBufferSize - length, aFormat, aArguments);

    // Update count
    cliLineBufferCount += length;

    // zero Terminate
    cliLineBuffer[cliLineBufferCount] = 0;

    // End of line ?
    if ((cliLineBufferCount >= 2) && (cliLineBuffer[cliLineBufferCount - 1] == 10) &&
        (cliLineBuffer[cliLineBufferCount - 2] == 13))
    {
        // remove CRLF and re-terminate
        cliLineBufferCount -= 2;
        cliLineBuffer[cliLineBufferCount] = 0;

        if (cliLineBufferCount > 0)
        {
            std::string line(cliLineBuffer);

            // Restrict size of list or not insert last line of command
            if (cliOutputsList.size() < cliListMaxSize)
            {
                // Save line to list
                cliOutputsList.push_back(line);
            }

            // Reset line buffer
            ResetLineBuffer();

            // Command response done ?
            if (cliHandleResponse)
            {
                // Command output complete 'Done' ?
                if (hal_strncmp_s(line.c_str(), "Done", 4) == 0)
                {
                    // Signal managed code of CLI output
                    PostManagedEvent(EVENT_OPENTHREAD, OpenThreadEventType_CommandOutputAvailable, 0, 0);

                    cliHandleResponse = false;

                    // Ignore next output (prompt)
                    cliIgnoreLine = 1;
                }
            }
            else
            {
                // Unsolicited message then signal managed code of output
                PostManagedEvent(EVENT_OPENTHREAD, OpenThreadEventType_CommandOutputAvailable, 0, 0);
            }
        }
    }

    return length;
}

HRESULT OpenThreadCliOutput(CLR_RT_HeapBlock &arrayBlock)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock_Array *array;

        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(
            arrayBlock,
            cliOutputsList.size(),
            g_CLR_RT_WellKnownTypes.m_String));

        array = arrayBlock.Array();

        int index = 0;
        for (std::list<std::string>::iterator it = cliOutputsList.begin(); it != cliOutputsList.end(); it++, index++)
        {
            CLR_RT_HeapBlock *blk = (CLR_RT_HeapBlock *)array->GetElement(index);

            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(*blk, (*it).c_str()));
        }

        cliOutputsList.clear();
    }
    NANOCLR_NOCLEANUP();
}

//
// Send command line to openthread CLI
//
void OpenThreadCliInput(const char *inputLine, bool response)
{
    if (response)
    {
        cliOutputsList.clear();
        cliHandleResponse = true;
    }

    // Need to take a copy of inputLine as otCliInputLine modifies the line when parsing
    int length = hal_strlen_s(inputLine);
    char *cliLine = new char[length + 1];
    hal_strcpy_s(cliLine, length + 1, inputLine);

    otCliInputLine((char *)cliLine);

    delete cliLine;
}

//
//  Initialize openThread for the required radio interface
//  This will depend on SOC this is running on.  ESP32_C6 & ESP32_H2 have a openTHread radio
//  Other SOC will need to interface to another SOC with radio like ESP32_H2.
//  Return 0 if openThread successfully initialized.
//
esp_err_t initOpenThread(ThreadDeviceType deviceType, esp_openthread_radio_mode_t radioMode, int port, int baud_rate)
{
    esp_openthread_platform_config_t config;
    esp_err_t err;

    esp_vfs_eventfd_config_t eventfd_config = {
        .max_fds = 3,
    };

    err = esp_vfs_eventfd_register(&eventfd_config);
    if (err != ERR_OK)
    {
        ESP_LOGI(TAG, "esp_vfs_eventfd_register failed with error %d", err);
        return err;
    }

    switch (radioMode)
    {
        case RADIO_MODE_NATIVE:
            config.radio_config = ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG();
            config.host_config = ESP_OPENTHREAD_NO_HOST_CONFIG();
            break;

        case RADIO_MODE_UART_RCP:
            config.radio_config.radio_mode = RADIO_MODE_UART_RCP;

            // Use default uart config 460800 baud, 8 bit, no parity
            config.host_config = ESP_OPENTHREAD_DEFAULT_UART_HOST_CONFIG();

            // Set COM port using ESP32 configured pins
            config.host_config.host_uart_config.port = (uart_port_t)port;

            config.host_config.host_uart_config.uart_config.baud_rate = baud_rate;

            // Pick up configured pin numbers for port
            config.host_config.host_uart_config.tx_pin =
                (gpio_num_t)Esp32_GetMappedDevicePins(DEV_TYPE_SERIAL, port, Esp32SerialPin_Tx);
            config.host_config.host_uart_config.rx_pin =
                (gpio_num_t)Esp32_GetMappedDevicePins(DEV_TYPE_SERIAL, port, Esp32SerialPin_Rx);
            break;

        case RADIO_MODE_SPI_RCP:
            config.radio_config.radio_mode = RADIO_MODE_SPI_RCP;
            config.host_config = ESP_OPENTHREAD_DEFAULT_SPI_HOST_CONFIG();
            config.host_config.spi_slave_config.host_device = (spi_host_device_t)port;
            config.host_config.spi_slave_config.bus_config.miso_io_num =
                Esp32_GetMappedDevicePins(DEV_TYPE_SPI, port, Esp32SpiPin_Mosi);
            config.host_config.spi_slave_config.bus_config.mosi_io_num =
                Esp32_GetMappedDevicePins(DEV_TYPE_SPI, port, Esp32SpiPin_Miso);
            config.host_config.spi_slave_config.bus_config.sclk_io_num =
                Esp32_GetMappedDevicePins(DEV_TYPE_SPI, port, Esp32SpiPin_Clk);
            break;

        default:
            return false;
    }

    config.port_config = ESP_OPENTHREAD_DEFAULT_PORT_CONFIG();

    // Initialize the OpenThread stack
    err = esp_openthread_init(&config);
    if (err != ERR_OK)
    {
        ESP_LOGI(TAG, "esp_openthread_init failed with error %d", err);
        return err;
    }

    IsThreadInitialised = true;

    // The OpenThread log level directly matches ESP log level
    (void)otLoggingSetLevel(CONFIG_LOG_DEFAULT_LEVEL);

    otCliInit(esp_openthread_get_instance(), cliOutputCallback, NULL);

    // Clear '>' from output on init
    ResetLineBuffer();

    // Initialize the esp_netif bindings
    openthread_netif = init_openthread_netif(&config);
    err = esp_netif_set_default_netif(openthread_netif);
    if (err != ERR_OK)
    {
        ESP_LOGI(TAG, "esp_netif_set_default_netif failed with error %d", err);
    }

    setDeviceType(deviceType, OPENTHREAD_NETWORK_POLLPERIOD_TIME);

    ThreadSetInterfaceNumber(openthread_netif->lwip_netif->num);

    return err;
}

static void openThreadMainTask(void *aContext)
{
    // Run the main loop
    esp_openthread_launch_mainloop();

    // Clean up
    esp_netif_destroy(openthread_netif);
    esp_openthread_netif_glue_deinit();

    esp_vfs_eventfd_unregister();
    vTaskDelete(NULL);
}

bool startStopOpenThread(bool start)
{
    // Start Thread operation
    if (otThreadSetEnabled(esp_openthread_get_instance(), start) != OT_ERROR_NONE)
    {
        return false;
    }
    return true;
}

//
// Start Thread and main thread task
void startOpenThreadTask()
{
    int stack = 10000;
    xTaskCreate(openThreadMainTask, "otmain", stack, xTaskGetCurrentTaskHandle(), 5, &threadTask);
}

int NF_ESP32_OpenThread_Open(HAL_Configuration_NetworkInterface *config)
{
    (void)config;

    // OpenThread is always started from managed code (nanoFramework.Networking.Thread)

    return SOCK_SOCKET_ERROR;
}

bool NF_ESP32_OpenThread_Close()
{
    if (IsThreadInitialised)
    {
        // Make Thread close down
        esp_openthread_deinit();

        // Give it time to stop thread & maybe exit
        vTaskDelay(500 / portTICK_PERIOD_MS);

        // Seems to be bug in IDF, the main loop task never shutdowns so do it manually for now until fixed in IDF
        vTaskDelete(threadTask);

        // Clean up
        esp_netif_destroy(openthread_netif);
        esp_openthread_netif_glue_deinit();

        esp_vfs_eventfd_unregister();

        IsThreadInitialised = false;

        // Remove interface number from nanoclr
        ThreadSetInterfaceNumber(-1);
    }

    return true;
}

//
// Return the local mesh address
void GetThreadLocalMeshAddress(unsigned char *addr)
{
    const otIp6Address *ipaddress = otThreadGetMeshLocalEid(esp_openthread_get_instance());
    memcpy(addr, ipaddress->mFields.m8, sizeof(ipaddress->mFields.m8));
}

void JoinerCallback(otError aError, void *aContext)
{
    // Signal managed code of Joiner start completed
    PostManagedEvent(EVENT_OPENTHREAD, OpenThreadEventType_JoinerComplete, 0, aError);
}

void JoinerStart(const char *pskc, const char *url)
{
    otError oterr;

    oterr = otJoinerStart(esp_openthread_get_instance(), pskc, url, NULL, NULL, NULL, NULL, JoinerCallback, NULL);
    if (oterr != OT_ERROR_NONE)
    {
        JoinerCallback(oterr, NULL);
    }
}

#endif
