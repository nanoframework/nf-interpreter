//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

///////////////////////////////////////////////////////////////////////////////////////////
// At this time(Azure RTOS v6.1.7_rel) it's not possible to have a different ux_user file
// for nanoBooter and another for nanoCLR.
///////////////////////////////////////////////////////////////////////////////////////////

#ifndef UX_USER_H
#define UX_USER_H

/* Define various build options for the USBX port.  The application should either make changes
   here by commenting or un-commenting the conditional compilation defined OR supply the defines 
   though the compiler's equivalent of the -D option.  */
/* #define UX_THREAD_STACK_SIZE                                (2 * 1024) */

/* Define USBX Host Enum Thread Stack Size. The default is to use UX_THREAD_STACK_SIZE */
/* 
#define UX_HOST_ENUM_THREAD_STACK_SIZE                      UX_THREAD_STACK_SIZE 
*/


/* Define USBX Host Thread Stack Size.  The default is to use UX_THREAD_STACK_SIZE */
/*
#define UX_HOST_HCD_THREAD_STACK_SIZE                       UX_THREAD_STACK_SIZE
*/

/* Define USBX Host HNP Polling Thread Stack Size. The default is to use UX_THREAD_STACK_SIZE */
/*
#define UX_HOST_HNP_POLLING_THREAD_STACK                    UX_THREAD_STACK_SIZE
*/

/* Override various options with default values already assigned in ux_api.h or ux_port.h. Please 
   also refer to ux_port.h for descriptions on each of these options.  */

/* Defined, this value represents how many ticks per seconds for a specific hardware platform. 
   The default is 1000 indicating 1 tick per millisecond.  */

/* #define UX_PERIODIC_RATE 1000
*/
#define UX_PERIODIC_RATE (TX_TIMER_TICKS_PER_SECOND)

/* Define control transfer timeout value in millisecond.
   The default is 10000 milliseconds.  */
/*
#define UX_CONTROL_TRANSFER_TIMEOUT                         10000
*/

/* Define non control transfer timeout value in millisecond.
   The default is 50000 milliseconds.  */
/*
#define UX_NON_CONTROL_TRANSFER_TIMEOUT                     50000
*/


/* Defined, this value is the maximum number of classes that can be loaded by USBX. This value
   represents the class container and not the number of instances of a class. For instance, if a
   particular implementation of USBX needs the hub class, the printer class, and the storage
   class, then the UX_MAX_CLASSES value can be set to 3 regardless of the number of devices 
   that belong to these classes.  */

#define UX_MAX_CLASSES  1


/* Defined, this value is the maximum number of classes in the device stack that can be loaded by
   USBX.  */

/* #define UX_MAX_SLAVE_CLASS_DRIVER    1
*/

/* Defined, this value is the maximum number of interfaces in the device framework.  */

/* #define UX_MAX_SLAVE_INTERFACES    16
*/

/* Defined, this value represents the number of different host controllers available in the system. 
   For USB 1.1 support, this value will usually be 1. For USB 2.0 support, this value can be more 
   than 1. This value represents the number of concurrent host controllers running at the same time. 
   If for instance there are two instances of OHCI running, or one EHCI and one OHCI controller
   running, the UX_MAX_HCD should be set to 2.  */

/* #define UX_MAX_HCD  1
*/


/* Defined, this value represents the maximum number of devices that can be attached to the USB.
   Normally, the theoretical maximum number on a single USB is 127 devices. This value can be 
   scaled down to conserve memory. Note that this value represents the total number of devices 
   regardless of the number of USB buses in the system.  */

/* #define UX_MAX_DEVICES  127
*/


/* Defined, this value represents the current number of SCSI logical units represented in the device
   storage class driver.  */

/* #define UX_MAX_SLAVE_LUN    1
*/


/* Defined, this value represents the maximum number of SCSI logical units represented in the
   host storage class driver.  */
   
/* #define UX_MAX_HOST_LUN 1
*/


/* Defined, this value represents the maximum number of bytes received on a control endpoint in
   the device stack. The default is 256 bytes but can be reduced in memory constrained environments.  */

/* #define UX_SLAVE_REQUEST_CONTROL_MAX_LENGTH 256
*/


/* Defined, this value represents the maximum number of bytes that can be received or transmitted
   on any endpoint. This value cannot be less than the maximum packet size of any endpoint. The default 
   is 4096 bytes but can be reduced in memory constrained environments. For cd-rom support in the storage 
   class, this value cannot be less than 2048.  */

#define UX_SLAVE_REQUEST_DATA_MAX_LENGTH    (1024 * 2)


/* Defined, this value includes code to handle storage Multi-Media Commands (MMC). E.g., DVD-ROM.
*/

/* #define UX_SLAVE_CLASS_STORAGE_INCLUDE_MMC   */


/* Defined, this value represents the maximum number of bytes that a storage payload can send/receive.
   The default is 8K bytes but can be reduced in memory constrained environments.  */
#define UX_HOST_CLASS_STORAGE_MEMORY_BUFFER_SIZE            (1024 * 8)

/* Define USBX Mass Storage Thread Stack Size. The default is to use UX_THREAD_STACK_SIZE. */

/* #define UX_HOST_CLASS_STORAGE_THREAD_STACK_SIZE             UX_THREAD_STACK_SIZE 
 */

/* Defined, this value represents the maximum number of Ed, regular TDs and Isochronous TDs. These values
   depend on the type of host controller and can be reduced in memory constrained environments.  */

#define UX_MAX_ED                                           80
#define UX_MAX_TD                                           128
#define UX_MAX_ISO_TD                                       1

/* Defined, this value represents the maximum size of the HID decompressed buffer. This cannot be determined
   in advance so we allocate a big block, usually 4K but for simple HID devices like keyboard and mouse
   it can be reduced a lot. */

#define UX_HOST_CLASS_HID_DECOMPRESSION_BUFFER              1024

/* Defined, this value represents the maximum number of HID usages for a HID device. 
   Default is 2048 but for simple HID devices like keyboard and mouse it can be reduced a lot. */

#define UX_HOST_CLASS_HID_USAGES                            512


/* By default, each key in each HID report from the device is reported by ux_host_class_hid_keyboard_key_get 
   (a HID report from the device is received whenever there is a change in a key state i.e. when a key is pressed
   or released. The report contains every key that is down). There are limitations to this method such as not being
   able to determine when a key has been released.

   Defined, this value causes ux_host_class_hid_keyboard_key_get to only report key changes i.e. key presses
   and key releases. */

/* #define UX_HOST_CLASS_HID_KEYBOARD_EVENTS_KEY_CHANGES_MODE */

/* Works when UX_HOST_CLASS_HID_KEYBOARD_EVENTS_KEY_CHANGES_MODE is defined.

   Defined, this value causes ux_host_class_hid_keyboard_key_get to only report key pressed/down changes;
   key released/up changes are not reported.
 */

/* #define UX_HOST_CLASS_HID_KEYBOARD_EVENTS_KEY_CHANGES_MODE_REPORT_KEY_DOWN_ONLY */

/* Works when UX_HOST_CLASS_HID_KEYBOARD_EVENTS_KEY_CHANGES_MODE is defined.

   Defined, this value causes ux_host_class_hid_keyboard_key_get to report lock key (CapsLock/NumLock/ScrollLock) changes.
 */

/* #define UX_HOST_CLASS_HID_KEYBOARD_EVENTS_KEY_CHANGES_MODE_REPORT_LOCK_KEYS */

/* Works when UX_HOST_CLASS_HID_KEYBOARD_EVENTS_KEY_CHANGES_MODE is defined.

   Defined, this value causes ux_host_class_hid_keyboard_key_get to report modifier key (Ctrl/Alt/Shift/GUI) changes.
 */

/* #define UX_HOST_CLASS_HID_KEYBOARD_EVENTS_KEY_CHANGES_MODE_REPORT_MODIFIER_KEYS */


/* Defined, this value represents the maximum number of media for the host storage class. 
   Default is 8 but for memory constrained resource systems this can ne reduced to 1. */

#define UX_HOST_CLASS_STORAGE_MAX_MEDIA                     2

/* Defined, this value includes code to handle storage devices that use the CB
   or CBI protocol (such as floppy disks). It is off by default because these 
   protocols are obsolete, being superseded by the Bulk Only Transport (BOT) protocol
   which virtually all modern storage devices use.
*/

/* #define UX_HOST_CLASS_STORAGE_INCLUDE_LEGACY_PROTOCOL_SUPPORT */

/* Defined, this value forces the memory allocation scheme to enforce alignment
   of memory with the UX_SAFE_ALIGN field.
*/

/* #define UX_ENFORCE_SAFE_ALIGNMENT   */

/* Defined, this value represents the number of packets in the CDC_ECM device class.
   The default is 16.
*/

#define UX_DEVICE_CLASS_CDC_ECM_NX_PKPOOL_ENTRIES           4

/* Defined, this value represents the number of packets in the CDC_ECM host class.
   The default is 16.
*/

/* #define UX_HOST_CLASS_CDC_ECM_NX_PKPOOL_ENTRIES             16 */

/* Defined, this value represents the number of milliseconds to wait for packet
   allocation until invoking the application's error callback and retrying.
   The default is 1000 milliseconds.
*/

/* #define UX_HOST_CLASS_CDC_ECM_PACKET_POOL_WAIT           10 */

/* Defined, this value represents the number of milliseconds to wait for packet
   pool availability checking loop.
   The default is 100 milliseconds.
*/

/* #define UX_HOST_CLASS_CDC_ECM_PACKET_POOL_INSTANCE_WAIT  10 */

/* Defined, this enables CDC ECM class to use the packet pool from NetX instance.  */

/* #define UX_HOST_CLASS_CDC_ECM_USE_PACKET_POOL_FROM_NETX */

/* Defined, this value represents the number of milliseconds to wait for packet
   allocation until invoking the application's error callback and retrying.
*/

/* #define UX_DEVICE_CLASS_CDC_ECM_PACKET_POOL_WAIT         10 */

/* Defined, this value represents the the maximum length of HID reports on the
   device.
 */

/* #define UX_DEVICE_CLASS_HID_EVENT_BUFFER_LENGTH          64 */

/* Defined, this value represents the the maximum number of HID events/reports 
   that can be queued at once.                   
 */

/* #define UX_DEVICE_CLASS_HID_MAX_EVENTS_QUEUE             8  */


/* Defined, this macro will disable DFU_UPLOAD support.  */

/* #define UX_DEVICE_CLASS_DFU_UPLOAD_DISABLE  */

/* Defined, this macro will enable DFU_GETSTATUS and DFU_GETSTATE in dfuERROR.  */

/* #define UX_DEVICE_CLASS_DFU_ERROR_GET_ENABLE  */

/* Defined, this macro will change status mode.
   0 - simple mode,
       status is queried from application in dfuDNLOAD-SYNC and dfuMANIFEST-SYNC state,
       no bwPollTimeout.
   1 - status is queried from application once requested,
       b0-3 : media status
       b4-7 : bStatus
       b8-31: bwPollTimeout
       bwPollTimeout supported.
*/

/* #define UX_DEVICE_CLASS_DFU_STATUS_MODE                  (1)  */

/* Defined, this value represents the default DFU status bwPollTimeout.
   The value is 3 bytes long (max 0xFFFFFFu).
   By default the bwPollTimeout is 1 (means 1ms).
 */

/* #define UX_DEVICE_CLASS_DFU_STATUS_POLLTIMEOUT           (1)  */

/* Defined, this macro will enable custom request process callback.  */

/* #define UX_DEVICE_CLASS_DFU_CUSTOM_REQUEST_ENABLE   */

/* Defined, this macro disables CDC ACM non-blocking transmission support.  */

/* #define UX_DEVICE_CLASS_CDC_ACM_TRANSMISSION_DISABLE  */

/* Defined, this macro enables device bi-directional-endpoint support.  */

/* #define UX_DEVICE_BIDIRECTIONAL_ENDPOINT_SUPPORT  */

/* Defined, this value will only enable the host side of usbx.  */
/* #define UX_HOST_SIDE_ONLY   */

/* Defined, this value will only enable the device side of usbx.  */
/* #define UX_DEVICE_SIDE_ONLY   */

/* Defined, this value will include the OTG polling thread. OTG can only be active if both host/device are present.
*/

#ifndef UX_HOST_SIDE_ONLY 
#ifndef UX_DEVICE_SIDE_ONLY 

/* #define UX_OTG_SUPPORT */

#endif 
#endif 

/* Defined, this value represents the maximum size of single tansfers for the SCSI data phase.
*/

#define UX_HOST_CLASS_STORAGE_MAX_TRANSFER_SIZE             (1024 * 1)

/* Defined, this value represents the size of the log pool.
*/
#define UX_DEBUG_LOG_SIZE                                   (1024 * 16)


/* Defined, this enables the assert checks inside usbx.  */
#define UX_ENABLE_ASSERT

/* Defined, this defines the assert action taken when failure detected. By default
   it halts without any output.  */
/* #define UX_ASSERT_FAIL  for (;;) {tx_thread_sleep(UX_WAIT_FOREVER); }  */


/* DEBUG includes and macros for a specific platform go here.  */
#ifdef UX_INCLUDE_USER_DEFINE_BSP
#include "usb_bsp.h"
#include "usbh_hcs.h"
#include "usbh_stdreq.h"
#include "usbh_core.h"
#endif 

#endif 

