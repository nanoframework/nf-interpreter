//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <zephyr.h>
#include <drivers/uart.h>
#include <device.h>

#include <nanoHAL_v2.h>
#include <WireProtocol.h>
#include <WireProtocol_Message.h>

#if defined(TRACE_MASK) && (TRACE_MASK & TRACE_VERBOSE) != 0
// used WP_Message_Process() and methods it calls to avoid flooding TRACE
extern uint32_t traceLoopCounter;
#endif

// Handle to wire protcol uart driver.
//    The driver is opened in the lib_main code
//    to confirm the uart is valid before starting
//    debugger.
const struct device *g_wire_uart = 0;

// Defines for lower level serial IO
void *wire_port = 0;
int wire_port_init(const struct device *uart_dev);
size_t wire_port_readto(const void *inst, char *buf, int size, int timeout);
int wire_port_writeto(const void *inst, char *buf, int size, int timeout);

void WP_ReceiveBytes(uint8_t **ptr, uint32_t *size)
{
    // save for later comparison
    uint32_t requestedSize = *size;
    (void)requestedSize;

    if (wire_port == 0)
    {
        wire_port_init(g_wire_uart);
    }
    if (((uint32_t)wire_port == -1) || ((uint32_t)wire_port == 0))
        return;

    // check for request with 0 size
    if (*size)
    {
        // non blocking read from serial port with 100ms timeout
        size_t read = wire_port_readto(wire_port, *ptr, requestedSize, 100);

        *ptr += read;
        *size -= read;
    }
}
int printCmdName(WP_Message *message);
uint8_t WP_TransmitMessage(WP_Message *message)
{
    uint32_t writeResult;
    bool operationResult = false;

    TRACE_WP_HEADER(WP_TXMSG, message);

    if (wire_port == 0)
    {
        wire_port_init(g_wire_uart);
    }
    if (((uint32_t)wire_port == -1) || (wire_port == 0))
        return 0;

    // write header to output stream
    writeResult = wire_port_writeto(wire_port, (char *)&message->m_header, sizeof(message->m_header), 10);

    if (writeResult == sizeof(message->m_header))
    {
        operationResult = true;

        // if there is anything on the payload send it to the output stream
        if (message->m_header.m_size && message->m_payload)
        {
            // reset flag
            operationResult = false;

            writeResult = wire_port_writeto(wire_port, message->m_payload, message->m_header.m_size, 50);
            if (writeResult == message->m_header.m_size)
            {
                operationResult = true;

                TRACE0(TRACE_ERRORS, "TXMSG: OK\n");
            }
        }
    }
//#define DUMP_ACK_MSGS
#ifdef DUMP_ACK_MSGS
    printk(
        "xmsg-- %d f:%08x %s cmd ",
        operationResult,
        message->m_header.m_flags,
        (message->m_header.m_flags & WP_Flags_c_ACK) ? "ACK" : "   ");
    printCmdName(message);
// printk("xmsg-- cmd %x %d\n", message->m_header.m_cmd, operationResult);
#endif
    return operationResult;
}
// --------------------------
// Zephyr-specific routines
// --------------------------
#ifdef DUMP_ACK_MSGS

#define PRTDBGCMD(cmd)                                                                                                 \
    case cmd:                                                                                                          \
        printk(##cmd "\n");                                                                                            \
        break;

int printCmdName(WP_Message *message)
{
    static const unsigned int c_Debugging_Execution_BasePtr =
        0x00020000; // Returns the pointer for the ExecutionEngine object.
    static const unsigned int c_Debugging_Execution_ChangeConditions =
        0x00020001;                                                           // Sets/resets the state of the debugger.
    static const unsigned int c_Debugging_Execution_SecurityKey = 0x00020002; // Sets security key.
    static const unsigned int c_Debugging_Execution_Unlock =
        0x00020003; // Unlock the low-level command, for mfg. test programs.
    static const unsigned int c_Debugging_Execution_Allocate = 0x00020004;    // Permanently allocate some memory.
    static const unsigned int c_Debugging_Execution_Breakpoints = 0x00020005; // Sets breakpoints.
    static const unsigned int c_Debugging_Execution_BreakpointHit =
        0x00020006; // Notification that a breakpoint was hit.
    static const unsigned int c_Debugging_Execution_BreakpointStatus = 0x00020007; // Queries last breakpoint hit.
    static const unsigned int c_Debugging_Execution_QueryCLRCapabilities =
        0x00020008; // Queries capabilities of the CLR.
    static const unsigned int c_Debugging_Execution_SetCurrentAppDomain =
        0x00020009; // Sets the current AppDomain.  This is required before
    int rc = 0;

    switch (message->m_header.m_cmd)
    {
        case 0:
            printk("ping\n");
            break;
        case 1:
            printk("str: ");
            //(db) too chatty
            // for (int i = 0; i < message->m_header.m_size; i++)
            // {
            // 	printk("%c", message->m_payload[i]);
            // }
            printk("\n");
            break;
        case 2:
            printk("read mem\n");
            break;
        case 3:
            printk("write mem\n");
            break;
        case 4:
            printk("chk mem\n");
            break;
        case 5:
            printk("erase mem\n");
            break;
        case 6:
            printk("execute\n");
            break;
        case 7:
            printk("reboot\n");
            break;
        case 8:
            printk("mem map\n");
            break;
        case 9:
            printk("prog exit\n");
            break;
        case 11:
            printk("deployment map\n");
            break;
        case 12:
            printk("flash sector\n");
            break;
        case 14:
            printk("OEM Info\n");
            break;
        case 15:
            printk("Q config\n");
            break;
        case 16:
            printk("Update config\n");
            break;
        case 32:
            printk("Targ info\n");
            break;
            PRTDBGCMD(c_Debugging_Execution_BasePtr)
            PRTDBGCMD(c_Debugging_Execution_ChangeConditions)
            PRTDBGCMD(c_Debugging_Execution_SecurityKey)
            PRTDBGCMD(c_Debugging_Execution_Unlock)
            PRTDBGCMD(c_Debugging_Execution_Allocate)
            PRTDBGCMD(c_Debugging_Execution_Breakpoints)
            PRTDBGCMD(c_Debugging_Execution_BreakpointHit)
            PRTDBGCMD(c_Debugging_Execution_BreakpointStatus)
            PRTDBGCMD(c_Debugging_Execution_QueryCLRCapabilities)
            PRTDBGCMD(c_Debugging_Execution_SetCurrentAppDomain)
        default:
            printk("%x  *****\n", message->m_header.m_cmd);
            break;
    }

    return rc;
}
#endif // DUMP_ACK_MSGS

#define SERBUF_SIZE 1536
//#define SERBUF_SIZE_TX 1024
#define SERBUF_SIZE_TX 1536

/*
 * structure for buffer management.
 */
struct buf_struct
{
    char *pBuf;
    char *pNext;
    char *pLast;
    int buf_size;
    struct k_sem lock;
};

struct buf_struct *init_buf(int size);
int buf_chars_avail(struct buf_struct *pb);
int add_buf_char(struct buf_struct *pb, char ch);
int get_buf_char(struct buf_struct *pb, char *ch);

#define SER_FLAG_INITIALIZED 0x0001
#define SER_FLAG_ACTIVE      0x0002
#define SER_FLAG_POWER_DOWN  0x0004

static struct serial_dev
{
    const struct device *dev;
    struct buf_struct *rx_buf;
    struct buf_struct *tx_buf;
    struct k_sem rx_avail_sem;
    uint32_t flags;
} devs;

/*
 * Function prototype for custom ISR handler
 */
typedef void (*uart_isr_handler)(const struct device *dev, void *user_data);
/*
 * Structure to be declared in app that defines the callback
 * function for the ISR as well as the user_data value that is
 * passed to the ISR when it is called.
 */
struct serial_isr_info
{
    uart_isr_handler irq_fn;
    void *user_data;
};

// Structures used to provide isr forwarding info
struct serial_isr_info isr_info;

/*
 * uart_interrupt_handler - All UARTs on the 52840
 * must share one ISR. This code receives all isr
 * callbacks and forwards them to the approperate
 * routine using the saved function pointer provided
 * when the callback was initialized. Using this for
 * the 9160 as it doesn't hurt and maintains compat.
 *
 */
static void uart_interrupt_handler(const struct device *dev, void *user_data)
{
    struct serial_isr_info *isr_data = user_data;

    isr_data->irq_fn(dev, isr_data->user_data);
    return;
}

/*
 * serial_lib_register_isr - register an isr
 *
 */
void serial_lib_register_isr(const struct device *dev, struct serial_isr_info *isr_info)
{
    uart_irq_callback_user_data_set(dev, uart_interrupt_handler, isr_info);
}

/*
 * uart_isr - All UARTs on the 52840
 * must share one ISR. This code was orignally designed
 * to pass data from 2 uarts to 2 instances of the USB
 * virtual serial ports.
 *
 * In this design we use a single UART for for log
 * data and the console. The ISR reads chars looking
 * for an escape char. When found, the next char goes
 * to the USB port designated for console IO. Otherwise,
 * the char goes to the USB port designated for logging.
 *
 */
static void uart_isr(const struct device *unused, void *user_data)
{
    struct serial_dev *sd = user_data;
    const struct device *dev = sd->dev;

    char ch = 0;
    while (uart_irq_update(dev) && uart_irq_is_pending(dev) && uart_irq_rx_ready(dev))
    {
        int len = uart_fifo_read(dev, &ch, 1);
        if (len == 0)
            break;
        add_buf_char(sd->rx_buf, ch);
    }
    if (buf_chars_avail(sd->rx_buf) > 0)
    {
        k_sem_give(&sd->rx_avail_sem);
    }

    while (uart_irq_tx_ready(dev))
    {
        uint16_t written = 0;
        if (get_buf_char(sd->tx_buf, &ch) == 0)
        {
            written = uart_fifo_fill(dev, &ch, 1);
            if (written == 0)
                break;
        }
    }
    if (buf_chars_avail(sd->tx_buf) == 0)
    {
        uart_irq_tx_disable(dev);
    }
}

int wire_port_init(const struct device *uart_dev)
{
    // Make sure we have a UART to use.
    if (!uart_dev)
    {
        return -1;
    }

    devs.dev = uart_dev;
    devs.tx_buf = init_buf(SERBUF_SIZE_TX);
    devs.rx_buf = init_buf(SERBUF_SIZE);
    k_sem_init(&devs.rx_avail_sem, 0, 1);
    k_sem_take(&devs.rx_avail_sem, K_MSEC(0));

    devs.flags = SER_FLAG_INITIALIZED;

    // Now init the struct to pass to the isr handler.
    isr_info.irq_fn = uart_isr;
    isr_info.user_data = &devs;

    // Register the ISR handler
    serial_lib_register_isr(uart_dev, &isr_info);

    devs.flags |= SER_FLAG_ACTIVE;
    wire_port = (void *)&devs;

    // Enable receive interrrupts.
    uart_irq_rx_enable(uart_dev);
    printk("wire uart init complete\n");
    return 0;
}

size_t wire_port_readto(const void *inst, char *buf, int size, int timeout)
{
    struct serial_dev *devstruct;
    int chars = 0;
    // int want = size;
    char ch;

    devstruct = (struct serial_dev *)inst;
    if ((devstruct == 0) || (-1 == (uint32_t)devstruct))
    {
        return 0;
    }
    uint64_t start = k_uptime_get();
    uint64_t now;
    while (size > 0)
    {
        // Copy chars to out buffer.
        while (get_buf_char(devstruct->rx_buf, &ch) == 0)
        {
            *buf++ = ch;
            size--;
            chars++;
            if (size == 0)
                break;
        }
        now = k_uptime_get();
        if (now - start > timeout)
        {
            break;
        }
        k_sleep(K_MSEC(1));
    }
    return chars;
}

/*
 * wire_port_writeto - Write debug port with timeout.
 */
int wire_port_writeto(const void *inst, char *buf, int size, int timeout)
{
    struct serial_dev *devstruct;
    int chars = 0;
    if ((inst == 0) || (-1 == (uint32_t)inst))
    {
        printk("\nwriteto FAIL\n");
        return -1;
    }
    devstruct = (struct serial_dev *)inst;

    // TODO:: Cause timeout if not written...
    while (size > 0)
    {
        if (add_buf_char(devstruct->tx_buf, *buf++) == 0)
        {
            size--;
            chars++;
        }
        else
        {
            printk("wire buf tx overflow\n");
            break;
        }
    }
    uart_irq_tx_enable(devstruct->dev);

    return chars;
}

/*
 * init_buf - Allocs a buffer and sets the pointers
 */
struct buf_struct *init_buf(int size)
{
    // Buffer needs to be large enough to hold pointer structure.
    if (size < sizeof(struct buf_struct) + 2)
        return 0;

    // alloc buffer.
    char *mem = k_malloc(size);
    if (mem == 0)
        return 0;
    char *dat = mem + sizeof(struct buf_struct);

    struct buf_struct *buf = (struct buf_struct *)mem;

    buf->pBuf = dat;
    buf->pNext = dat;
    buf->pLast = dat;

    buf->buf_size = size - sizeof(struct buf_struct);
    return buf;
}

int buf_chars_avail(struct buf_struct *pb)
{
    if (pb->pNext >= pb->pLast)
    {
        return (pb->pNext - pb->pLast);
    }
    return ((pb->pBuf + pb->buf_size) - pb->pLast) + (pb->pNext - pb->pBuf);
}

int add_buf_char(struct buf_struct *pb, char ch)
{
    int rc = 0;
    // Overwrite the oldest data.
    // See if room in buf
    if (pb->pNext + 1 == pb->pLast)
    {
        pb->pLast++;
        if (pb->pLast >= pb->pBuf + pb->buf_size)
        {
            pb->pLast = pb->pBuf;
        }
        rc = -1;
    }
    *pb->pNext++ = ch;
    if (pb->pNext >= pb->pBuf + pb->buf_size)
    {
        pb->pNext = pb->pBuf;
    }
    return rc;
}
/*
 * get_buf_char
 */
int get_buf_char(struct buf_struct *pb, char *ch)
{
    // See if char in buf
    if (pb->pLast == pb->pNext)
    {
        return -1;
    }
    *ch = *pb->pLast++;
    if (pb->pLast >= pb->pBuf + pb->buf_size)
    {
        pb->pLast = pb->pBuf;
    }
    return 0;
}
