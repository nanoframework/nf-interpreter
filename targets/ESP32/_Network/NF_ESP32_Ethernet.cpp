//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// This file includes the board specific Ethernet Intialisation

#include <NF_ESP32_Network.h>
#include <esp32_ethernet_options.h>

static const char *TAG = "ETH";

esp_eth_handle_t eth_handle = NULL;

// OLIMEX ESP32-EVB Rev B, OLIMEX ESP32-Gateway, Generic Lan8270
// ETH_PHY_ADDR  0
// ETH_MDC_GPIO  23
// ETH_MDIO_GPIO 18

// OLIMEX POE and LILYGO use ETH_RMII_CLK_OUT_GPIO 17
// OLIMEX POE use ETH_PHY_RST_GPIO 12
// OLIMEX gateway revs newer than C use ETH_PHY_RST_GPIO 5

// default values for ESP32 boards
// values taken from IDF CONFIG files

#ifndef ETH_PHY_ADDR
// PHY address
#define ETH_PHY_ADDR 0
#endif

#if CONFIG_IDF_TARGET_ESP32
#define NANO_ETH_ESP32_EMAC_DEFAULT_CONFIG()               \
    {                                                 \
        .smi_gpio =                                   \
        {                                             \
            .mdc_num = 23,                            \
            .mdio_num = 18                            \
        },                                            \
        .interface = EMAC_DATA_INTERFACE_RMII,        \
        .clock_config =                               \
        {                                             \
            .rmii =                                   \
            {                                         \
                .clock_mode = DEFAULT_RMII_CLK_MODE,  \
                .clock_gpio = (emac_rmii_clock_gpio_t)DEFAULT_RMII_CLK_GPIO   \
            }                                         \
        },                                            \
        .dma_burst_len = ETH_DMA_BURST_LEN_32,        \
        .intr_priority = 0,                           \
    }
#elif CONFIG_IDF_TARGET_ESP32P4
#define NANO_ETH_ESP32_EMAC_DEFAULT_CONFIG()          \
    {                                                 \
        .smi_gpio =                                   \
        {                                             \
            .mdc_num = 31,                            \
            .mdio_num = 27                            \
        },                                            \
        .interface = EMAC_DATA_INTERFACE_RMII,        \
        .clock_config =                               \
        {                                             \
            .rmii =                                   \
            {                                         \
                .clock_mode = EMAC_CLK_EXT_IN,        \
                .clock_gpio = 50                      \
            }                                         \
        },                                            \
        .dma_burst_len = ETH_DMA_BURST_LEN_32,        \
        .intr_priority = 0,                           \
        .emac_dataif_gpio =                           \
        {                                             \
            .rmii =                                   \
            {                                         \
                .tx_en_num = 49,                      \
                .txd0_num = 34,                       \
                .txd1_num = 35,                       \
                .crs_dv_num = 28,                     \
                .rxd0_num = 29,                       \
                .rxd1_num = 30                        \
            }                                         \
        },                                            \
        .clock_config_out_in =                        \
        {                                             \
            .rmii =                                   \
            {                                         \
                .clock_mode = EMAC_CLK_EXT_IN,        \
                .clock_gpio = -1                      \
            }                                         \
        },                                            \
    }
#endif // CONFIG_IDF_TARGET_ESP32P4


#if CONFIG_IDF_TARGET_ESP32
#define NANO_ETH_ESP32_EMAC_DEFAULT_CONFIG()               \
    {                                                 \
        .smi_gpio =                                   \
        {                                             \
            .mdc_num = 23,                            \
            .mdio_num = 18                            \
        },                                            \
        .interface = EMAC_DATA_INTERFACE_RMII,        \
        .clock_config =                               \
        {                                             \
            .rmii =                                   \
            {                                         \
                .clock_mode = DEFAULT_RMII_CLK_MODE,  \
                .clock_gpio = (emac_rmii_clock_gpio_t)DEFAULT_RMII_CLK_GPIO   \
            }                                         \
        },                                            \
        .dma_burst_len = ETH_DMA_BURST_LEN_32,        \
        .intr_priority = 0,                           \
    }
#elif CONFIG_IDF_TARGET_ESP32P4
#define NANO_ETH_ESP32_EMAC_DEFAULT_CONFIG()          \
    {                                                 \
        .smi_gpio =                                   \
        {                                             \
            .mdc_num = 31,                            \
            .mdio_num = 27                            \
        },                                            \
        .interface = EMAC_DATA_INTERFACE_RMII,        \
        .clock_config =                               \
        {                                             \
            .rmii =                                   \
            {                                         \
                .clock_mode = EMAC_CLK_EXT_IN,        \
                .clock_gpio = 50                      \
            }                                         \
        },                                            \
        .dma_burst_len = ETH_DMA_BURST_LEN_32,        \
        .intr_priority = 0,                           \
        .emac_dataif_gpio =                           \
        {                                             \
            .rmii =                                   \
            {                                         \
                .tx_en_num = 49,                      \
                .txd0_num = 34,                       \
                .txd1_num = 35,                       \
                .crs_dv_num = 28,                     \
                .rxd0_num = 29,                       \
                .rxd1_num = 30                        \
            }                                         \
        },                                            \
        .clock_config_out_in =                        \
        {                                             \
            .rmii =                                   \
            {                                         \
                .clock_mode = EMAC_CLK_EXT_IN,        \
                .clock_gpio = -1                      \
            }                                         \
        },                                            \
    }
#endif // CONFIG_IDF_TARGET_ESP32P4


esp_err_t NF_ESP32_InitialiseEthernet(uint8_t *pMacAdr)
{
    (void)pMacAdr;

#ifdef ESP32_ETHERNET_SUPPORT

    if (eth_handle != NULL)
    {
        return esp_eth_start(eth_handle);
    }

    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
    esp_netif_t *eth_netif = esp_netif_new(&cfg);

    // now MAC and PHY configs
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_esp32_emac_config_t esp32_emac_config = NANO_ETH_ESP32_EMAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    phy_config.phy_addr = ETH_PHY_ADDR;

#ifdef ETH_PHY_RST_GPIO
    phy_config.reset_gpio_num = ETH_PHY_RST_GPIO;
    CPU_GPIO_ReservePin(ETH_PHY_RST_GPIO, true); // Reset_N
#else
    phy_config.reset_gpio_num = -1;
#endif

    //    phy_config.reset_timeout_ms = 200;
    ESP_LOGI(
        TAG,
        "Ethernet phy config reset %d timeout %d addr %d\n",
        phy_config.reset_gpio_num,
        phy_config.reset_timeout_ms,
        phy_config.phy_addr);

#if ESP32_ETHERNET_INTERNAL == TRUE

    // Internal Ethernet

    // Set Clock modes to override whats in sdkconfig
#ifdef ETH_RMII_CLK_OUT_GPIO
    esp32_emac_config.clock_config.rmii.clock_mode = EMAC_CLK_OUT;
    esp32_emac_config.clock_config.rmii.clock_gpio = (emac_rmii_clock_gpio_t)ETH_RMII_CLK_OUT_GPIO; // always 16 or 17
    ESP_LOGI(TAG, "Ethernet clock_config OUT gpio %d\n", ETH_RMII_CLK_OUT_GPIO);

    CPU_GPIO_ReservePin(EMAC_CLK_OUT, true);          // REF_CLK OUT
    CPU_GPIO_ReservePin(ETH_RMII_CLK_OUT_GPIO, true); // REF_CLK OUT
#else
    ESP_LOGI(TAG, "Ethernet clock_config IN gpio %d\n", esp32_emac_config.clock_config.rmii.clock_gpio);

    CPU_GPIO_ReservePin(EMAC_CLK_EXT_IN, true);  // REF_CLK EXT
    CPU_GPIO_ReservePin(esp32_emac_config.clock_config.rmii.clock_gpio, true); // REF_CLK IN
#endif

// If ETH_MDC_GPIO or ETH_MDIO_GPIO defined then use new values
#ifdef ETH_MDC_GPIO
    esp32_emac_config.smi_gpio.mdc_num = ETH_MDC_GPIO;
#endif

#ifdef ETH_MDIO_GPIO
    esp32_emac_config.smi_gpio.mdio_num = ETH_MDIO_GPIO;
#endif

    ESP_LOGI(TAG, "Ethernet pins for MDC %d MDIO %d\n", esp32_emac_config.smi_gpio.mdc_num, esp32_emac_config.smi_gpio.mdio_num);

    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config);

    // Reserve all pins used by ethernet interface
    CPU_GPIO_ReservePin(esp32_emac_config.smi_gpio.mdio_num, true); // MDIO
    CPU_GPIO_ReservePin(esp32_emac_config.smi_gpio.mdc_num, true);  // MDC

#if SOC_EMAC_USE_MULTI_IO_MUX || SOC_EMAC_MII_USE_GPIO_MATRIX
    // ESP32_P4 with Ethernet 
    const eth_mac_rmii_gpio_config_t &rmii = esp32_emac_config.emac_dataif_gpio.rmii;
    CPU_GPIO_ReservePin(rmii.txd0_num,  true);
    CPU_GPIO_ReservePin(rmii.tx_en_num, true);
    CPU_GPIO_ReservePin(rmii.txd1_num,  true);
    CPU_GPIO_ReservePin(rmii.rxd0_num,  true);
    CPU_GPIO_ReservePin(rmii.rxd1_num,  true);
    CPU_GPIO_ReservePin(rmii.crs_dv_num,true);
#else
    // ESP32 with Ethernet 
    CPU_GPIO_ReservePin(19, true);            // TXD0
    CPU_GPIO_ReservePin(21, true);            // TX_EN
    CPU_GPIO_ReservePin(22, true);            // TXD1
    CPU_GPIO_ReservePin(25, true);            // RXD0
    CPU_GPIO_ReservePin(26, true);            // RXD1
    CPU_GPIO_ReservePin(27, true);            // CRS_DV
#endif

    // Define PHY to use with internal Ethernet
#if defined(ESP32_ETHERNET_PHY_IP101)
    ESP_LOGI(TAG, "Ethernet IP101 phy\n");
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);
#elif defined(ESP32_ETHERNET_PHY_RTL8201)
    ESP_LOGI(TAG, "Ethernet RTL8201 phy\n");
    esp_eth_phy_t *phy = esp_eth_phy_new_rtl8201(&phy_config);
#elif defined(ESP32_ETHERNET_PHY_LAN8720)
    ESP_LOGI(TAG, "Ethernet Lan8720 phy\n");
    esp_eth_phy_t *phy = esp_eth_phy_new_lan87xx(&phy_config);
#elif defined(ESP32_ETHERNET_PHY_DP83848)
    ESP_LOGI(TAG, "Ethernet DP83848 phy\n");
    esp_eth_phy_t *phy = esp_eth_phy_new_dp83848(&phy_config);
#elif defined(ESP32_ETHERNET_PHY_KSZ8041)
    ESP_LOGI(TAG, "Ethernet KSZ8041 phy\n");
    esp_eth_phy_t *phy = esp_eth_phy_new_ksz80xx(&phy_config);
#endif

#elif ESP32_ETHERNET_SPI == TRUE
    // Or Use SPI ethernet module

    // Install gpio isr service for spi interupts
    gpio_install_isr_service(0);

    // Initialise SPI bus
    spi_bus_config_t buscfg = {0};
    buscfg.miso_io_num = ESP32_ETHERNET_SPI_MISO_GPIO;
    buscfg.mosi_io_num = ESP32_ETHERNET_SPI_MOSI_GPIO;
    buscfg.sclk_io_num = ESP32_ETHERNET_SPI_SCLK_GPIO;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;

    ESP_ERROR_CHECK_WITHOUT_ABORT(spi_bus_initialize(ESP32_ETHERNET_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));

#pragma
    // Define SPI interface to use
#ifdef ESP32_ETHERNET_SPI_MODULE_DM9051
    spi_device_interface_config_t devcfg = {
        .command_bits = 1,
        .address_bits = 7,
        .mode = 0,
        .clock_speed_hz = ESP32_ETHERNET_SPI_CLOCK_MHZ * 1000 * 1000,
        .spics_io_num = ESP32_ETHERNET_SPI_CS,
        .queue_size = 20};

    /* dm9051 ethernet driver is based on spi driver */
    eth_dm9051_config_t dm9051_config = ETH_DM9051_DEFAULT_CONFIG(ESP32_ETHERNET_SPI_HOST, &devcfg);
    dm9051_config.int_gpio_num = ESP32_ETHERNET_SPI_INT_GPIO;
    esp_eth_mac_t *mac = esp_eth_mac_new_dm9051(&dm9051_config, &mac_config);
    esp_eth_phy_t *phy = esp_eth_phy_new_dm9051(&phy_config);

    ESP_LOGI(TAG, "Ethernet DM9051 spi\n");

#elif defined(ESP32_ETHERNET_SPI_MODULE_W5500)
    // Use W5500 in mac mode
    spi_device_interface_config_t devcfg = {
        .command_bits = 16, // Actually it's the address phase in W5500 SPI frame
        .address_bits = 8,  // Actually it's the control phase in W5500 SPI frame
        .mode = 0,
        .clock_speed_hz = ESP32_ETHERNET_SPI_CLOCK_MHZ * 1000 * 1000,
        .spics_io_num = ESP32_ETHERNET_SPI_CS,
        .queue_size = 20};

    // w5500 ethernet driver is based on spi driver
    eth_w5500_config_t w5500_config = ETH_W5500_DEFAULT_CONFIG(ESP32_ETHERNET_SPI_HOST, &devcfg);
    w5500_config.int_gpio_num = ESP32_ETHERNET_SPI_INT_GPIO;
    esp_eth_mac_t *mac = esp_eth_mac_new_w5500(&w5500_config, &mac_config);
    esp_eth_phy_t *phy = esp_eth_phy_new_w5500(&phy_config);

    ESP_LOGI(TAG, "Ethernet W5500 spi\n");

#elif ESP32_ETHERNET_INTERFACE == ENJ28J60
    spi_device_interface_config_t devcfg = {
        .command_bits = 1,
        .address_bits = 7,
        .mode = 0,
        .clock_speed_hz = ESP32_ETHERNET_SPI_CLOCK_MHZ * 1000 * 1000,
        .spics_io_num = ESP32_ETHERNET_SPI_CS,
        .queue_size = 20};

    //  enj28j60 ethernet driver is based on spi driver
    eth_dm9051_config_t enj28j60_config = ETH_ENJ28J60_DEFAULT_CONFIG(ESP32_ETHERNET_SPI_HOST, &devcfg);
    enj28j60_config.int_gpio_num = ESP32_ETHERNET_SPI_INT_GPIO;
    esp_eth_mac_t *mac = esp_eth_mac_new_enj28j60(&enj28j60_config, &mac_config);

    ESP_LOGI(TAG, "Ethernet ENJ28J60 spi\n");

#else
#error "Ethernet option not defined (has to be either SPI or INTERNAL)"
#endif

#endif // ESP32_ETHERNET_SPI

    // now the do the Ethernet config
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_err_t err = esp_eth_driver_install(&config, &eth_handle);
    if (err != ESP_OK)
    {
        ESP_LOGI(TAG, "Unable to install ethernet driver");
        return err;
    }

#ifdef ESP32_ETHERNET_SPI
    // The SPI Ethernet module doesn't have a burned factory MAC address, we have to set it manually.
    // Supplied in the config
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_eth_ioctl(eth_handle, ETH_CMD_S_MAC_ADDR, pMacAdr));
#endif

    // attach Ethernet driver to TCP/IP stack
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle)));

    // start Ethernet driver state machine
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_eth_start(eth_handle));

#endif // ESP32_ETHERNET_SUPPORT

    return ESP_OK;
}

//
//  Open Ethernet Network driver
//
int NF_ESP32_Ethernet_Open(HAL_Configuration_NetworkInterface *config)
{
    if (NF_ESP32_InitialiseEthernet(config->MacAddress) == ESP_OK)
    {
        // Return NetIf number for Esp32 Ethernet
        return NF_ESP32_Wait_NetNumber(IDF_ETH_DEF);
    }

    return SOCK_SOCKET_ERROR;
}

bool NF_ESP32_Ethernet_Close()
{
    return esp_eth_stop(eth_handle) == ESP_OK;
}
