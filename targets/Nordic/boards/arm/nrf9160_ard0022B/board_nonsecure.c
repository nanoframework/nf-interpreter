//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2019 Nordic Semiconductor ASA
// Portions Copyright (c) Ericsson AB 2020, all rights reserved
// See LICENSE file in the project root for full license information.
//

#include <init.h>

#include <net/socket.h>
#include <modem/nrf_modem_lib.h>
#include <nrf_modem_at.h>

#include <drivers/gpio.h>
#include <logging/log.h>

LOG_MODULE_REGISTER(board_nonsecure);

//
// NOTE: in v2.0.0, the MAGPIO and COEX0 initialization occurs in
// the modem_antenna library code. <root>/nrf/lib/modem_antenna.
// The strings for MAGPIO and COEX0 are set in the board directory
// ns_defconfig file.
//

#define GPIO_DRIVER_NAME          "GPIO_0"
#define BOARD_NS_GROVE_SELECT_PIN 10
#define BOARD_NS_SEL_GROVEDIGITAL 1

#define BOARD_NS_SIM_SELECT_PIN 0
#define BOARD_NS_USE_ESIM       0

// ard_ns_sim_ctrl_init - Configure the GPIO for the SIM select
static int ard_ns_sim_ctrl_init(const struct device *dev)
{
    int ret;

    ret = gpio_pin_configure(dev, BOARD_NS_SIM_SELECT_PIN, GPIO_OUTPUT | GPIO_ACTIVE_HIGH);

    if (ret)
    {
        LOG_ERR("Error configuring pin %d!", BOARD_NS_SIM_SELECT_PIN);
    }

    bool use_esim = BOARD_NS_USE_ESIM;
    ret = gpio_pin_set_raw(dev, BOARD_NS_SIM_SELECT_PIN, use_esim);

    if (ret)
    {
        LOG_ERR("Error setting pin %d!", BOARD_NS_SIM_SELECT_PIN);
    }

    return ret;
}

// ard_ns_grove_ctrl_init - Configure the GPIO for the Grove Conn A/D select
static int ard_ns_grove_ctrl_init(const struct device *dev)
{
    int ret;

    // While this file is typically board specific, keeping this file
    // consistent between the Combi and Combi Dev simplifies support.

#if defined CONFIG_BOARD_NRF9160_ARD0022BNS
    ret = gpio_pin_configure(dev, BOARD_NS_GROVE_SELECT_PIN, GPIO_OUTPUT | GPIO_ACTIVE_HIGH);

    if (ret)
    {
        LOG_ERR("Error configuring pin %d!", BOARD_NS_GROVE_SELECT_PIN);
    }

    bool sel_digital = BOARD_NS_SEL_GROVEDIGITAL;
    ret = gpio_pin_set_raw(dev, BOARD_NS_GROVE_SELECT_PIN, sel_digital);

    if (ret)
    {
        LOG_ERR("Error setting pin %d!", BOARD_NS_GROVE_SELECT_PIN);
    }

#else

    ret = gpio_pin_configure(dev, BOARD_NS_GROVE_SELECT_PIN, GPIO_DISCONNECTED);

    if (ret)
    {
        LOG_ERR("Error configuring pin %d!", BOARD_NS_GROVE_SELECT_PIN);
    }

#endif

    return ret;
}

// ard_ns_board_init - Called at app init to set up board
static int ard_ns_board_init(const struct device *dev)
{
    int err;
    const struct device *gpio_out_dev;

    gpio_out_dev = device_get_binding(GPIO_DRIVER_NAME);

    if (!gpio_out_dev)
    {
        LOG_ERR("Cannot find %s!", GPIO_DRIVER_NAME);
        return -ENODEV;
    }

    err = ard_ns_sim_ctrl_init(gpio_out_dev);

    if (err)
    {
        LOG_ERR("ard_ns_sim_ctrl_init failed with error: %d", err);
        // return err;
    }

    err = ard_ns_grove_ctrl_init(gpio_out_dev);

    if (err)
    {
        LOG_ERR("ard_ns_grove_ctrl_init failed with error: %d", err);
        // return err;
    }

    return 0;
}

#if defined(CONFIG_SPI)

#define SPI_CS_CNT      DT_PROP_LEN(DT_NODELABEL(spi3), cs_gpios)
#define SPI_CS_PIN(x)   DT_PHA_BY_IDX(DT_NODELABEL(spi3), cs_gpios, x, pin)
#define SPI_CS_FLAGS(x) DT_PHA_BY_IDX(DT_NODELABEL(spi3), cs_gpios, x, flags)

// ard_ns_spi_init - Called post kernel init to set SPI chip selects
static int ard_ns_spi_init(const struct device *dev)
{
    int ret, i;

    // Current SPI chip selects.
    struct cs_gpio
    {
        int pin;
        int flags;
    } cs_gpios[SPI_CS_CNT] = {
        {SPI_CS_PIN(0), SPI_CS_FLAGS(0)},
        {SPI_CS_PIN(1), SPI_CS_FLAGS(1)},
        {SPI_CS_PIN(2), SPI_CS_FLAGS(2)},

    };

    // Add this as a check in case someone changes the Chip Select count
    BUILD_ASSERT((SPI_CS_CNT == 3), "Too many SPI chip selects defined.");

    // Access the GPIO device
    const struct device *gpio_dev = device_get_binding(GPIO_DRIVER_NAME);

    if (!gpio_dev)
    {
        LOG_ERR("Cannot find %s!", GPIO_DRIVER_NAME);
        return -ENODEV;
    }

    // Configure the Chip Selects.
    for (i = 0; i < SPI_CS_CNT; i++)
    {
        ret = gpio_pin_configure(gpio_dev, cs_gpios[i].pin, GPIO_OUTPUT | cs_gpios[i].flags);

        if (ret)
        {
            LOG_ERR("Error configuring pin %d!", cs_gpios[i].pin);
        }

        ret = gpio_pin_set_raw(gpio_dev, cs_gpios[i].pin, 1);

        if (ret)
        {
            LOG_ERR("Error setting pin %d!", cs_gpios[i].pin);
        }
    }

    return ret;
}

SYS_INIT(ard_ns_spi_init, POST_KERNEL, CONFIG_SPI_INIT_PRIORITY);

#endif

SYS_INIT(ard_ns_board_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
