#ifndef __FSL_GPIO_EXT_H_
#define __FSL_GPIO_EXT_H_

#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

typedef struct
{
    const uint32_t CNF;
    const uint32_t MUX;
    const uint8_t  NUM_OF_GPIO;
} GPIO_Ext;


/* GPIO1 */
#define GPIO1_CNF_BASE (0x401F82ACu)
#define GPIO1_MUX_BASE (0x401F80BCu)
#define GPIO1_EXT      { .CNF = GPIO1_CNF_BASE, .MUX = GPIO1_MUX_BASE, .NUM_OF_GPIO = 32u }
/* GPIO1 */


/* GPIO2 */
#define GPIO2_CNF_BASE (0x401F832Cu)
#define GPIO2_MUX_BASE (0x401F813Cu)
#define GPIO2_EXT      { .CNF = GPIO2_CNF_BASE, .MUX = GPIO2_MUX_BASE, .NUM_OF_GPIO = 32u }
/* GPIO2 */


/* GPIO3_1 */
#define GPIO3_CNF_1_BASE (0x401F83C4u)
#define GPIO3_MUX_1_BASE (0x401F81D4u)
#define GPIO3_EXT_1      { .CNF = GPIO3_CNF_1_BASE, .MUX = GPIO3_MUX_1_BASE, .NUM_OF_GPIO = 12u }
/* GPIO3_1 */


/* GPIO3_2 */
#define GPIO3_CNF_2_BASE (0x401F83ACu)
#define GPIO3_MUX_2_BASE (0x401F81BCu)
#define GPIO3_EXT_2      { .CNF = GPIO3_CNF_2_BASE, .MUX = GPIO3_MUX_2_BASE, .NUM_OF_GPIO = 6u }
/* GPIO3_2 */


/* GPIO3_3 */
#define GPIO3_CNF_3_BASE (0x401F8094u)
#define GPIO3_MUX_3_BASE (0x401F8284u)
#define GPIO3_EXT_3      { .CNF = GPIO3_CNF_3_BASE, .MUX = GPIO3_MUX_3_BASE, .NUM_OF_GPIO = 10u }
/* GPIO3_3 */


/* GPIO4 */
#define GPIO4_CNF_BASE (0x401F8204u)
#define GPIO4_MUX_BASE (0x401F8014u)
#define GPIO4_EXT      { .CNF = GPIO4_CNF_BASE, .MUX = GPIO4_MUX_BASE, .NUM_OF_GPIO = 32u }
/* GPIO4 */


/* GPIO5 */
#define GPIO5_CNF_BASE (0x400A8018u)
#define GPIO5_MUX_BASE (0x400A8000u)
#define GPIO5_EXT      { .CNF = GPIO5_CNF_BASE, .MUX = GPIO5_MUX_BASE, .NUM_OF_GPIO = 3u }
/* GPIO 5 */

#define GPIO_EXT_1245   { GPIO1_EXT, GPIO2_EXT, {0} ,GPIO4_EXT, GPIO5_EXT }
#define GPIO_EXT_3      { GPIO3_EXT_1, GPIO3_EXT_2, GPIO3_EXT_3 }

#define MUX_REGISTER (0u)
#define CNF_REGISTER (1u)

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

void GPIO_PinMux(uint32_t base, uint32_t pin, const uint32_t muxMode);
void GPIO_PinConfig(uint32_t base, uint32_t pin, const uint32_t configValue);

#if defined(__cplusplus)
}
#endif

#endif /* __FSL_GPIO_EXT_H_ */