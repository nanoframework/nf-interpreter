//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _SPI_TO_DISPLAY_
#define _SPI_TO_DISPLAY_ 1

#include "spi_master.h"
#include <Esp32_os.h>
#include "DisplayInterface.h"

#include <nanoPAL.h>
#include <target_platform.h>

// ESP32 - Documentation
// https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/spi_master.html

// ESP32 Wrover-Kit

struct DisplayInterface g_DisplayInterface;

static void IRAM_ATTR spi_ready(spi_transaction_t* trans);
static spi_device_handle_t spi;
static volatile bool spi_trans_in_progress;
static spi_transaction_t t;

void ESP_IDF_spi_send_bytes(CLR_UINT8* data, CLR_UINT32 length);

const int SPI_MOSI = GPIO_NUM_23;            // MOSI_1   GPIO23   SDA
const int SPI_MISO = GPIO_NUM_25;            // MISO_1   GPIO25   SDO
const int SPI_CLK = GPIO_NUM_19;             // CLK_1   GPIO19   SCL
const int SPI_CS = GPIO_NUM_22;              // CS_1       GPIO22   CS

const gpio_num_t  LCD_RESET = GPIO_NUM_18;   // RST_1   GPIO18   RESET
const gpio_num_t LCD_D_CX = GPIO_NUM_21;     // D/CX_1   GPIO21   D/C
const gpio_num_t LCD_BACKLIGHT = GPIO_NUM_5; // GPIO5   Backlight

#define NUMBER_OF_LINES 16
#define SPI_MAX_TRANSFER_SIZE  (320 * 2 * NUMBER_OF_LINES)  // 320 pixels 2 words wide ( 16 bit colour)

CLR_INT16 OutputBufferSize;
CLR_UINT8 SpiBuffer[SPI_MAX_TRANSFER_SIZE];

// Display Interface
void DisplayInterface::Initialize()
{
    esp_err_t ret = NULL;

    spi_bus_config_t buscfg;
    buscfg.flags = 0;                                // Abilities of bus to be checked by the driver 
    buscfg.intr_flags = 0;                           // Interrupt flag for the bus to set the priority, and IRAM attribute
    buscfg.max_transfer_sz = SPI_MAX_TRANSFER_SIZE;
    buscfg.miso_io_num = -1;                         // (not used) -> Master In, Slave Out, a.k.a.Q.Data transmission from a Device to Host.
    buscfg.mosi_io_num = SPI_MOSI;                   // Master Out, Slave In, a.k.a. D. Data transmission from a Host to Device.
    buscfg.quadhd_io_num = -1;                       // Hold signal. Only used for 4-bit (qio/qout) transactions.
    buscfg.quadwp_io_num = -1;                       // Write Protect signal. Only used for 4-bit (qio/qout) transactions.
    buscfg.sclk_io_num = SPI_CLK;

    spi_device_interface_config_t devcfg;
    devcfg.address_bits = 0;                         // Default amount of bits in address phase (0-64), used when SPI_TRANS_VARIABLE_ADDR is not used.
                                                     // Otherwise ignored.
    devcfg.clock_speed_hz = 40 * 1000 * 1000;        // Clock speed, divisors of 80MHz, in Hz.
    devcfg.command_bits = 0;                         // Default amount of bits in command phase (0-16), used when SPI_TRANS_VARIABLE_CMD is not used
                                                     // Otherwise ignored.  
    devcfg.cs_ena_posttrans = 0;                     // Amount of SPI bit-cycles the cs should stay active after the transmission (0-16) 
    devcfg.cs_ena_pretrans = 0;                      // Amount of SPI bit-cycles the cs should be activated before the transmission (0-16). 
                                                     // This only works on half-duplex transactions. 
    devcfg.dummy_bits = 0;
    devcfg.duty_cycle_pos = 0;                       // Duty cycle of positive clock, in 1/256th increments (128 = 50%/50% duty). 
                                                     // Setting this to 0 (=not setting it) is equivalent to setting this to 128. 
    devcfg.flags = SPI_DEVICE_HALFDUPLEX;            // Bitwise OR of SPI_DEVICE_* flags. 
    devcfg.input_delay_ns = 0;                       // The time required between SCLK and MISO valid, including the possible clock delay from slave to master
    devcfg.mode = 0;                                 // SPI mode (0-3) 
    devcfg.post_cb = spi_ready;                      // Callback to be called after a transmission has completed.
                                                     // This callback is called within interrupt context should be in IRAM for best performance
    devcfg.pre_cb = NULL;                            // Callback to be called before a transmission is started
                                                     // This callback is called within interrupt context should be in IRAM for best performance
    devcfg.queue_size = 1;                           // How many that can be queued using spi_device_queue_trans but not yet finished using spi_device_get_trans_result
    devcfg.spics_io_num = SPI_CS;                    // CS GPIO pin for this device, or -1 if not used. 

    //Initialize the SPI bus
    ret = spi_bus_initialize(HSPI_HOST, &buscfg, 1);
    ASSERT(ret == ESP_OK);

    //Attach the LCD to the SPI bus
    ret = spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
    ASSERT(ret == ESP_OK);

    //Initialize non-SPI GPIOs
    gpio_set_direction(LCD_D_CX, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_RESET, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_BACKLIGHT, GPIO_MODE_OUTPUT);

    //Reset the display
    gpio_set_level(LCD_RESET, 0);
    OS_DELAY(100);
    gpio_set_level(LCD_RESET, 1);
    OS_DELAY(100);

    return;
}

void DisplayInterface::GetTransferBuffer(CLR_UINT8*& TransferBuffer, CLR_UINT32& TransferBufferSize)
{
    TransferBuffer = SpiBuffer;
    TransferBufferSize = sizeof(SpiBuffer);
}
void DisplayInterface::ClearFrameBuffer()
{
   // Not Used
}

void DisplayInterface::WriteToFrameBuffer(CLR_UINT8 command, CLR_UINT8 data[], CLR_UINT32 dataCount)
{
    gpio_set_level(LCD_D_CX, 0);    //Command mode
    ESP_IDF_spi_send_bytes(&command, 1);
    gpio_set_level(LCD_D_CX, 1);    // Data mode
    ESP_IDF_spi_send_bytes(data, dataCount);
    return;
}

void DisplayInterface::SendCommand(CLR_UINT8 arg_count, ...)
{
    va_list ap;
    va_start(ap, arg_count);

    // First byte is the command
    CLR_UINT8 command = va_arg(ap, int);
    gpio_set_level(LCD_D_CX, 0);    //Command mode
    ESP_IDF_spi_send_bytes(&command, 1);


    // Following bytes are data related to the command
    int parameterCount = arg_count - 1;
    CLR_UINT8 parameters[parameterCount];
    for (int i = 0; i < parameterCount; i++)
    {
        parameters[i] = va_arg(ap, int);
    }
    gpio_set_level(LCD_D_CX, 1);    // Data mode
    ESP_IDF_spi_send_bytes(parameters, parameterCount);

    return;
}

void DisplayInterface::DisplayBacklight(bool on) // true = on
{
    if (on)
    {
        gpio_set_level(LCD_BACKLIGHT, 1);
    }
    else
    {
        gpio_set_level(LCD_BACKLIGHT, 0);
    }
    return;
}

// esp-idf functions
void ESP_IDF_spi_send_bytes(CLR_UINT8* data, CLR_UINT32 length)
{
    if (length == 0) return;            //no need to send anything
    while (spi_trans_in_progress);
    t.length = length * 8;            // transaction length is in bits
    t.tx_buffer = data;

    spi_trans_in_progress = true;
    spi_device_queue_trans(spi, &t, portMAX_DELAY);

    // Theoretically, we could do work after queueing and waiting for the Spi transfer to complete.
    // For the moment, wait until completion, continuing starts to overwrite buffer before it is completed
    while (spi_trans_in_progress);
    return;
}

static void IRAM_ATTR spi_ready(spi_transaction_t* trans)
{
    spi_trans_in_progress = false;
    if (trans->tx_buffer == NULL) {}; // avoid unused parameter, maybe used in the future
    return;
}
#endif  // _SPI_TO_DISPLAY_

