#ifndef COM_SKYWORKSINC_NANOFRAMEWORK_DEVICES_C1_NATIVE_TARGET_H
#define COM_SKYWORKSINC_NANOFRAMEWORK_DEVICES_C1_NATIVE_TARGET_H
#include <em_device.h>
#include "em_timer.h"
#include "target_com_sky_nf_dev_c1_config.h"

// Forward declaration of bus configuration structure
typedef struct {
    GPIO_Port_TypeDef port;
    unsigned int pin;
    bool initialized;
} C1BusConfig_t;

// External reference to bus configuration array (defined in main .cpp file)
extern C1BusConfig_t c1BusConfigs[];

// Default configuration for backwards compatibility
#ifndef GECKO_USE_C1
#define GECKO_USE_C1    FALSE
#define C1_GPIO_PORT    gpioPortB
#define C1_GPIO_PIN     10
#endif

#if defined(_SILICON_LABS_32B_SERIES_1) && (GECKO_USE_C1 == TRUE)

    // Bus-aware macros that use the configuration array
    #define REGISTER_BIT(busIndex)        (1 << c1BusConfigs[busIndex].pin)
    #define GPIO_READ(busIndex)           ((GPIO->P[c1BusConfigs[busIndex].port].DIN & REGISTER_BIT(busIndex)) ? 1 : 0)
    
    #define GPIO_SIGNAL(busIndex, state) \
    do { \
        GPIO_Port_TypeDef port = c1BusConfigs[busIndex].port; \
        unsigned int pin = c1BusConfigs[busIndex].pin; \
        \
        if ((state) == 0) { \
            /* Set pin LOW */ \
            GPIO->P[port].DOUT &= ~(1 << pin); \
            /* Configure as push-pull output */ \
            if (pin >= 8) { \
                GPIO->P[port].MODEH = (GPIO->P[port].MODEH & ~(_GPIO_P_MODEH_MODE8_MASK << ((pin % 8) * 4))) | (GPIO_P_MODEH_MODE8_PUSHPULL << ((pin % 8) * 4)); \
            } else { \
                GPIO->P[port].MODEL = (GPIO->P[port].MODEL & ~(_GPIO_P_MODEL_MODE0_MASK << (pin * 4))) | (GPIO_P_MODEL_MODE0_PUSHPULL << (pin * 4)); \
            } \
        } else if ((state) == 1) { \
            /* Set pin HIGH */ \
            GPIO->P[port].DOUT |= (1 << pin); \
            /* Configure as push-pull output */ \
            if (pin >= 8) { \
                GPIO->P[port].MODEH = (GPIO->P[port].MODEH & ~(_GPIO_P_MODEH_MODE8_MASK << ((pin % 8) * 4))) | (GPIO_P_MODEH_MODE8_PUSHPULL << ((pin % 8) * 4)); \
            } else { \
                GPIO->P[port].MODEL = (GPIO->P[port].MODEL & ~(_GPIO_P_MODEL_MODE0_MASK << (pin * 4))) | (GPIO_P_MODEL_MODE0_PUSHPULL << (pin * 4)); \
            } \
        } else if ((state) == 2) { \
            /* Configure as input */ \
            if (pin >= 8) { \
                GPIO->P[port].MODEH = (GPIO->P[port].MODEH & ~(_GPIO_P_MODEH_MODE8_MASK << ((pin % 8) * 4))) | (GPIO_P_MODEH_MODE8_INPUT << ((pin % 8) * 4)); \
            } else { \
                GPIO->P[port].MODEL = (GPIO->P[port].MODEL & ~(_GPIO_P_MODEL_MODE0_MASK << (pin * 4))) | (GPIO_P_MODEL_MODE0_INPUT << (pin * 4)); \
            } \
        } \
    } while (0)

    // Helper macros for specific buses (optional - for convenience)
    #define GPIO_SIGNAL_BUS0(state)  GPIO_SIGNAL(0, state)
    #define GPIO_SIGNAL_BUS1(state)  GPIO_SIGNAL(1, state)
    #define GPIO_SIGNAL_BUS2(state)  GPIO_SIGNAL(2, state)
    #define GPIO_SIGNAL_BUS3(state)  GPIO_SIGNAL(3, state)
    
    #define GPIO_READ_BUS0()         GPIO_READ(0)
    #define GPIO_READ_BUS1()         GPIO_READ(1)
    #define GPIO_READ_BUS2()         GPIO_READ(2)
    #define GPIO_READ_BUS3()         GPIO_READ(3)

#else
    #error "Only _SILICON_LABS_32B_SERIES_1 is supported at this time..."
#endif

#endif
