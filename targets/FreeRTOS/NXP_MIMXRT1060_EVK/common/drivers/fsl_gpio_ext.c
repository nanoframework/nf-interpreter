#include "fsl_gpio_ext.h"
#include "fsl_iomuxc.h"

static const GPIO_Ext gpioExt1245[] = GPIO_EXT_1245;
static const GPIO_Ext gpioExt3[] = GPIO_EXT_3;

static uint32_t GPIO_GetRegister1245(uint32_t base, uint32_t pin, uint8_t type)
{
    if (pin < gpioExt1245[base - 1].NUM_OF_GPIO)
    {
        switch (type)
        {
            case CNF_REGISTER:
                return gpioExt1245[base - 1].CNF + pin * 4u;
            case MUX_REGISTER:
                return gpioExt1245[base - 1].MUX + pin * 4u;
        }
    }

    return 0;
}

static uint32_t GPIO_GetRegister3(uint32_t pin, uint8_t type)
{
    for (uint32_t i = 0; i < ARRAY_SIZE(gpioExt3); i++)
    {
        if (pin < gpioExt3[i].NUM_OF_GPIO)
        {
            switch (type)
            {
                case CNF_REGISTER:
                    return gpioExt3[i].CNF + pin * 4u;
                case MUX_REGISTER:
                    return gpioExt3[i].MUX + pin * 4u;
            }
        }

        pin -= gpioExt3[i].NUM_OF_GPIO;
    }

    return 0;
}

static uint32_t GPIO_GetRegister(uint32_t base, uint32_t pin, uint8_t type)
{
    switch (base)
    {
        case 1:
        case 2:
        case 4:
        case 5:
            return GPIO_GetRegister1245(base, pin, type);
            break;
        case 3:
            return GPIO_GetRegister3(pin, type);
            break;
    }

    return 0;
}

void GPIO_PinMux(uint32_t base, uint32_t pin, uint32_t muxMode)
{
    uint32_t muxRegister = GPIO_GetRegister(base, pin, MUX_REGISTER);

    if (muxRegister)
    {
        IOMUXC_SetPinMux(muxRegister, muxMode, 0, 0, 0, 0);
    }
}

void GPIO_PinConfig(uint32_t base, uint32_t pin, uint32_t configValue)
{
    uint32_t configRegister = GPIO_GetRegister(base, pin, CNF_REGISTER);

    if (configRegister)
    {
        IOMUXC_SetPinConfig(0, 0, 0, 0, configRegister, configValue);
    }
}
