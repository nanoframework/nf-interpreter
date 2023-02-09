#include "sl_iostream.h"
#include "sl_iostream_handles.h"
#include "string.h"
#include <target_platform.h>

const sl_iostream_instance_info_t *sl_iostream_instances_info[] = {

#if HAL_WP_USE_SERIAL == TRUE
    &sl_iostream_instance_vcom_info,
#endif

#if HAL_USE_ONEWIRE == TRUE
    &sl_iostream_instance_onewire_info,
#endif

};

const uint32_t sl_iostream_instances_count = sizeof(sl_iostream_instances_info) / sizeof(sl_iostream_instances_info[0]);

/***************************************************************************//**
 * Get iostream instance handle for a given name
 *
 * @return  Instance handle if it exist, NULL otherwise.
 ******************************************************************************/
sl_iostream_t *sl_iostream_get_handle(char *name)
{
  for (uint32_t i = 0; i < sl_iostream_instances_count; i++) {
    if (strcmp(sl_iostream_instances_info[i]->name, name) == 0) {
      return sl_iostream_instances_info[i]->handle;
    }
  }

  return NULL;
}
