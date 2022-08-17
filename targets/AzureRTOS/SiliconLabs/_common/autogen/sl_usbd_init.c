//****************************************************************************
// Includes.

#include "sl_status.h"


#include "sl_usbd_core.h"







#include "sl_usbd_class_hid.h"








//****************************************************************************
// Global functions.

/* USB initialization function */
void sli_usbd_init(void)
{

  sl_usbd_core_init();







  sl_usbd_hid_init();







}
