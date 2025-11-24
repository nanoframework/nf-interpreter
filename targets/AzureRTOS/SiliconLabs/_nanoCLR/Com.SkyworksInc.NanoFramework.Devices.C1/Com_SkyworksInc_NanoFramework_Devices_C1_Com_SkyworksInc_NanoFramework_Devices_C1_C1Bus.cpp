//-----------------------------------------------------------------------------
//
//                   ** WARNING! **
//    This file was generated automatically by a tool.
//    Re-running the tool will overwrite this file.
//    You should copy this file to a custom location
//    before adding any customization in the copy to
//    prevent loss of your changes when the tool is
//    re-run.
//
//-----------------------------------------------------------------------------

#include "sl_sleeptimer.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_timer.h"
#include "Com_SkyworksInc_NanoFramework_Devices_C1_C1Instruction.h"
#include "Com_SkyworksInc_NanoFramework_Devices_C1.h"
#include "Com_SkyworksInc_NanoFramework_Devices_C1_Com_SkyworksInc_NanoFramework_Devices_C1_C1Bus.h"
#include "Com_SkyworksInc_NanoFramework_Devices_C1_Native_Target.h"

using namespace Com_SkyworksInc_NanoFramework_Devices_C1::Com_SkyworksInc_NanoFramework_Devices_C1;

// We only support 4 C1 buses
#define MAX_C1_BUSES 4

C1BusConfig_t c1BusConfigs[MAX_C1_BUSES] = {
    {gpioPortB, 0, false},  // Bus 0 (DUT1)
    {gpioPortB, 3, false},  // Bus 1 (DUT2)
    {gpioPortB, 6,  false},  // Bus 2 (DUT3)
    {gpioPortB, 9,  false},  // Bus 3 (DUT4) or Bus 0 (DUT1) for single DUT board
};

// Use TIMER0 for all buses
#define C1_TIMER        TIMER0
#define C1_TIMER_IRQ    TIMER0_IRQn

// Current active bus index
static volatile uint8_t currentBusIndex = 0;

enum C1_States
{
    STATE_INIT,
    STATE_RUN,
    START_1,
    START_2,
    ADDRESS_FOLLOW,     // allows multiple devices on C1 line, but we only use one, so always 0
    SLOW_ACCESS,        // Newer chips (from Si5302 on) only use FastPath so must be set to 0
    INSTRUCTION_1,
    INSTRUCTION_2,
    INCREMENT,
    READ_1,
    READ_2,
    READ_3,
    READ_4,
    READ_5,
    READ_6,
    DATA_1,
    DATA_2,
    DATA_3,
    DATA_4,
    DATA_5,
    DATA_6,
    DATA_7,
    DATA_8,
    STATE_END
};

enum C1_States_RD
{
    RD_STATE_INIT,
    RD_STATE_RUN,
    RD_START_1,
    RD_START_1_2,
    RD_START_2,
    RD_START_2_2,
    RD_ADDRESS_FOLLOW,     // allows multiple devices on C1 line, but we only use one, so always 0
    RD_ADDRESS_FOLLOW_2,
    RD_SLOW_ACCESS,        // Newer chips (from Si5302 on) only use FastPath so must be set to 0
    RD_SLOW_ACCESS_2,
    RD_INSTRUCTION_1,
    RD_INSTRUCTION_1_2,
    RD_INSTRUCTION_2,
    RD_INSTRUCTION_2_2,
    RD_INCREMENT,
    RD_INCREMENT_2,
    RD_READ_1,
    RD_READ_1_2,
    RD_READ_2,
    RD_READ_2_2,
    RD_READ_3,
    RD_READ_3_2,
    RD_READ_4,
    RD_READ_4_2,
    RD_READ_5,
    RD_READ_5_2,
    RD_READ_6,
    RD_READ_6_2,
    RD_STATE_END
};

void init(uint8_t busIndex);
void initInstruction(C1InstructionName instructionType);
void setIRQHandler(C1InstructionName instructionType);
void setupTimer();
void setupGPIO(uint8_t busIndex);
void TIMER0_IRQHandler();
void TIMER0_Read_IRQHandler();
void cleanUp(); 
void handleInstructionWrite(int instr_value, int index, C1_States nextState);
void handleDataWrite(int index, uint8_t* data, C1_States nextState);
void setNthBit(uint8_t &data, int n, bool logicValue);
int getNthBit(uint8_t data, int n);

// current and previous state place holders
volatile enum C1_States currentState = STATE_INIT;
volatile enum C1_States_RD currentStateRD = RD_STATE_INIT;
volatile enum C1_States previousState = STATE_INIT;

// GPIO_Port_TypeDef gpioPort = gpioPortB;
// unsigned int gpioPin = 10;

volatile C1InstructionName instruction;

uint8_t transfer_data = 0;

// hold the instruction values for C1 protocol
C1Instruction instructions[4];

// allow us to only run init function once
volatile int ignoreInit = 0;

// Logical 1 is three times the duration of logical 0, therefore we need to wait 3 periods
// we will use this variable as a counter for the number of periods we have waited.
volatile int writeLogical1 = 0;

// Count to help iterate the read sequence of a bit
volatile int readCount = 0;

volatile bool pinValue = false;

// tells if the next write state shoud set the pin to 0 or 1
volatile int next_write_gpio_state;

CMU_Select_TypeDef lastClockSelect;

volatile int bitToRead = 0;

volatile int tRest = 0;

volatile int instruction0 = 0;
volatile int instruction1 = 0;

volatile int write_bit;

volatile uint32_t start_cnt = 0;
// variables to calculate a delay in microseconds
uint32_t core_clock_speed = 0;
uint32_t delay_iterations = 0;

TIMER_Prescale_TypeDef prescaler_slow = timerPrescale16;
TIMER_Prescale_TypeDef prescaler_fast = timerPrescale8;


void C1Bus::NativeTransmitWriteWithAddress( uint16_t param0, uint8_t param1, uint8_t param2, CLR_RT_TypedArray_UINT8 param3, HRESULT &hr )
{

    (void)param0;
    (void)param1;
    (void)param2;
    (void)param3;
    (void)hr;

    ////////////////////////////////
    // implementation starts here //

    // param0 is bus index for c1
    // param1 is register address
    // param2 is write value for address
    // param3 is response array

    currentBusIndex = param0;  // Set the active bus
    if(currentBusIndex >= MAX_C1_BUSES) {
        hr = CLR_E_INVALID_PARAMETER;
        return;
    }

    init(currentBusIndex);
    setupGPIO(currentBusIndex);

    transfer_data = 0;
    bitToRead = 0;

    setupTimer();

    /*
     *  First address write
     */
    // Reset state machine for the next run currentState = STATE_INIT;
    next_write_gpio_state = 1;
    instruction = ADDRESS_WRITE;
    currentState = STATE_INIT;
    previousState = STATE_INIT;
    transfer_data = param1;
    initInstruction(instruction);
    TIMER_Enable(C1_TIMER, true);     // Start TIMER0

    // Wait for the state machine to finish
    while(currentState != STATE_END){}

    // Wait in between Address Write and Data Read

    for (volatile uint32_t i = 0; i < 64; i++) {
        // Empty loop to create delay
    }

    /*
     *  Second data read
     */
    // Reset state machine for the next run currentState = STATE_INIT;
    next_write_gpio_state = 1;
    instruction = DATA_WRITE;
    currentState = STATE_INIT;
    previousState = STATE_INIT;
    transfer_data = param2;

    initInstruction(instruction);
    TIMER_Enable(C1_TIMER, true);     // Start TIMER0

    // Wait for the state machine to finish
    while(currentState != STATE_END){}

    param3[0] = 0x01;

    for (volatile uint32_t i = 0; i < 64; i++) {
        // Empty loop to create delay
    }

    cleanUp();

    // implementation ends here   //
    ////////////////////////////////
}

void C1Bus::NativeTransmitReadWithAddress( uint16_t param0, uint8_t param1, CLR_RT_TypedArray_UINT8 param2, HRESULT &hr )
{

    (void)param0;
    (void)param1;
    (void)param2;
    (void)hr;

    ////////////////////////////////
    // implementation starts here //

    // param0 is bus index for c1
    // param1 is register address
    // param2 is response array

    currentBusIndex = param0;  // Set the active bus
    if(currentBusIndex >= MAX_C1_BUSES) {
        hr = CLR_E_INVALID_PARAMETER;
        return;
    }

    init(currentBusIndex);
    setupGPIO(currentBusIndex);

    transfer_data = 0;
    bitToRead = 0;

    setupTimer();

    /*
     *  First address write
     */
    // Reset state machine for the next run currentState = STATE_INIT;
    next_write_gpio_state = 1;
    instruction = ADDRESS_WRITE;
    currentState = STATE_INIT;
    previousState = STATE_INIT;
    transfer_data = param1;
    initInstruction(instruction);
    TIMER_Enable(C1_TIMER, true);     // Start TIMER0

    // Wait for the state machine to finish
    while(currentState != STATE_END){}

    // Wait in between Address Write and Data Read

    for (volatile uint32_t i = 0; i < 64; i++) {
        // Empty loop to create delay
    }

    /*
     *  Second data read
     */
    // Reset state machine for the next run currentState = STATE_INIT;
    next_write_gpio_state = 1;
    instruction = DATA_READ;
    currentState = STATE_INIT;
    previousState = STATE_INIT;
    transfer_data = 0;
    initInstruction(instruction);
    TIMER_Enable(C1_TIMER, true);     // Start TIMER0

    // Wait for the state machine to finish
    while(currentState != STATE_END){}

    param2[0] = 0x01;
    param2[1] = transfer_data;

    for (volatile uint32_t i = 0; i < 64; i++) {
        // Empty loop to create delay
    }

    cleanUp();

    // implementation ends here   //
    ////////////////////////////////
}

void C1Bus::NativeTransmitRead( uint16_t param0, CLR_RT_TypedArray_UINT8 param1, HRESULT &hr )
{

    (void)param0;
    (void)param1;
    (void)hr;


    ////////////////////////////////
    // implementation starts here //

    
    // param0 is bus index for c1
    // param1 is response array

    // setupGPIO();
    
    // Set the active bus index
    currentBusIndex = param0;
    if(currentBusIndex >= MAX_C1_BUSES) {
        hr = CLR_E_INVALID_PARAMETER;
        return;
    }

    transfer_data = 0;
    bitToRead = 0;

    // setupTimer();

    // Reset state machine for the next run currentState = STATE_INIT;
    next_write_gpio_state = 1;
    instruction = DATA_READ;
    currentStateRD = RD_STATE_INIT;
    transfer_data = 0;
    init(currentBusIndex);
    initInstruction(instruction);
    TIMER_Enable(C1_TIMER, true);     // Start TIMER0

     // Wait for the state machine to finish
    while(currentState != STATE_END){}

    for (volatile uint32_t i = 0; i < 64; i++) {
        // Empty loop to create delay
    }

    param1[0] = 0x01;
    param1[1] = transfer_data;

    for (volatile uint32_t i = 0; i < 64; i++) {
        // Empty loop to create delay
    }

    cleanUp();
    
    // implementation ends here   //
    ////////////////////////////////


}

void C1Bus::NativeTransmitWrite( uint16_t param0, uint8_t param1, CLR_RT_TypedArray_UINT8 param2, HRESULT &hr )
{

    (void)param0;
    (void)param1;
    (void)param2;
    (void)hr;


    ////////////////////////////////
    // implementation starts here //

    // param0 is bus index for c1
    // param1 is write value
    // param0 is response array
    
    // setupGPIO();
    
    // Set the active bus index
    currentBusIndex = param0;
    if(currentBusIndex >= MAX_C1_BUSES) {
        hr = CLR_E_INVALID_PARAMETER;
        return;
    }

    transfer_data = 0;
    bitToRead = 0;

    // setupTimer();

    // Reset state machine for the next run currentState = STATE_INIT;
    next_write_gpio_state = 1;
    instruction = DATA_WRITE;
    currentState = STATE_INIT;
    previousState = STATE_INIT;
    transfer_data = param1;
    init(currentBusIndex);
    initInstruction(instruction);
    TIMER_Enable(C1_TIMER, true);     // Start TIMER0

    // Wait for the state machine to finish
    while(currentState != STATE_END){}

    param2[0] = 0x01;

    for (volatile uint32_t i = 0; i < 64; i++) {
        // Empty loop to create delay
    }

    cleanUp();

    // implementation ends here   //
    ////////////////////////////////


}

void C1Bus::NativeTransmitWriteAddress( uint16_t param0, uint8_t param1, CLR_RT_TypedArray_UINT8 param2, HRESULT &hr )
{

    (void)param0;
    (void)param1;
    (void)param2;
    (void)hr;


    ////////////////////////////////
    // implementation starts here //

    // param0 is bus index for c1
    // param1 is write value for address
    // param2 is response array
    
    // setupGPIO();
    
    // Set the active bus index
    currentBusIndex = param0;
    if(currentBusIndex >= MAX_C1_BUSES) {
        hr = CLR_E_INVALID_PARAMETER;
        return;
    }

    transfer_data = 0;
    bitToRead = 0;

    // setupTimer();

    // Reset state machine for the next run currentState = STATE_INIT;
    next_write_gpio_state = 1;
    instruction = ADDRESS_WRITE;
    currentState = STATE_INIT;
    previousState = STATE_INIT;
    transfer_data = param1;
    init(currentBusIndex);
    initInstruction(instruction);
    TIMER_Enable(C1_TIMER, true);     // Start TIMER0

    // Wait for the state machine to finish
    while(currentState != STATE_END){}

    param2[0] = 0x01;

    for (volatile uint32_t i = 0; i < 64; i++) {
        // Empty loop to create delay
    }

    cleanUp();
    // implementation ends here   //
    ////////////////////////////////


}

void C1Bus::NativeTransmitReadAddress( uint16_t param0, CLR_RT_TypedArray_UINT8 param1, HRESULT &hr )
{

    (void)param0;
    (void)param1;
    (void)hr;


    ////////////////////////////////
    // implementation starts here //

    // param0 is bus index for c1
    // param1 is response array
    
    // setupGPIO();

    // Set the active bus index
    currentBusIndex = param0;
    if(currentBusIndex >= MAX_C1_BUSES) {
        hr = CLR_E_INVALID_PARAMETER;
        return;
    }

    transfer_data = 0;
    bitToRead = 0;

    // setupTimer();

    // Reset state machine for the next run currentState = STATE_INIT;
    next_write_gpio_state = 1;
    instruction = ADDRESS_READ;
    currentStateRD = RD_STATE_INIT;
    previousState = STATE_INIT;
    transfer_data = 0x00;
    init(currentBusIndex);
    initInstruction(instruction);
    TIMER_Enable(C1_TIMER, true);     // Start TIMER0

    // Wait for the state machine to finish
    while(currentState != STATE_END){}

    param1[0] = 0x01;
    param1[1] = transfer_data;

    for (volatile uint32_t i = 0; i < 64; i++) {
        // Empty loop to create delay
    }

    cleanUp();

    // implementation ends here   //
    ////////////////////////////////


}

void init(uint8_t busIndex)
{
    if(busIndex >= MAX_C1_BUSES) return;

    for (volatile uint32_t i = 0; i < 64; i++) {
        // Empty loop to create delay
    }

    // Disable interrupts globally
    // CORE_ATOMIC_IRQ_DISABLE();NativeTransmitWriteWithAddress

    // Get the core clock frequency
    core_clock_speed = CMU_ClockFreqGet(cmuClock_CORE);
    delay_iterations = 1 * (core_clock_speed / 1000000) / 4;

    if(ignoreInit > 0) {
        return;
    }

    ignoreInit++;

    setupGPIO(busIndex);
    setupTimer();
}

void setIRQHandler(C1InstructionName instruction) 
{
    // Get the address of the vector table
    uint32_t *vectorTable = (uint32_t *)SCB->VTOR;

    if(instruction == DATA_READ || instruction == ADDRESS_READ)
    {
        // Set the TIMER0 IRQ handler to your custom handler
        vectorTable[C1_TIMER_IRQ + 16] = (uint32_t)TIMER0_Read_IRQHandler;
    }
    else if(instruction == DATA_READ || instruction == ADDRESS_READ)
    {
        vectorTable[C1_TIMER_IRQ + 16] = (uint32_t)TIMER0_IRQHandler;
    }

}

void initInstruction(C1InstructionName instructionType) 
{
    //setIRQHandler(instructionType);

    switch(instructionType)
    {
        case DATA_WRITE:
            instruction0 = 0;
            instruction1 = 0;
            break;
        case DATA_READ:
            instruction0 = 0;
            instruction1 = 1;
            break;
        case ADDRESS_WRITE:
            instruction0 = 1;
            instruction1 = 0;
            break;
        case ADDRESS_READ:
            instruction0 = 1;
            instruction1 = 1;
            break;
    }
}

void setupGPIO(uint8_t busIndex)
{
    if(busIndex >= MAX_C1_BUSES) return;
    
    // Enable clock for GPIO
    CMU_ClockEnable(cmuClock_GPIO, true);

    C1BusConfig_t* config = &c1BusConfigs[busIndex];
    
    // Configure pin as push-pull output using the GPIO_SIGNAL macro
    GPIO_SIGNAL(busIndex, 1);  // Set HIGH and configure as push-pull
    
    config->initialized = true;
}

void cleanUp(void)
{
    // Re-enable interrupts globally
    // CORE_ATOMIC_IRQ_ENABLE();

    for (volatile uint32_t i = 0; i < 64; i++) {
        // Empty loop to create delay
    }
}

void setupTimer()
{
    // Determine frequency based on instruction type
    int frequency = 0;
    if(instruction == DATA_READ || instruction == ADDRESS_READ)
    {
        frequency = 1000000; // 1MHz for read operations
    }
    else if(instruction == DATA_WRITE || instruction == ADDRESS_WRITE)
    {
        frequency = 500000; // 500kHz for write operations
    }

    // Enable clock for TIMER0
    CMU_ClockEnable(cmuClock_TIMER0, true);

    // Enable peripheral clock
    CMU_ClockEnable(cmuClock_HFPERB, true);

    // Timer initialization structure
    TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;

    // Configure TIMER0
    timerInit.enable     = false;    // Do not enable timer immediately
    timerInit.prescale   = timerPrescale8;  // Set prescaler (adjust as needed)
    timerInit.clkSel     = timerClkSelHFPerClk;  // Use HFPERB clock branch
    timerInit.mode       = timerModeUp; // Up-counting mode

    TIMER_Init(C1_TIMER, &timerInit);

    // Set TIMER0 Top value for desired interrupt frequency
    uint32_t timerFreq = CMU_ClockFreqGet(cmuClock_HFPERB) / (1 << timerInit.prescale);
    uint32_t topValue = (timerFreq / frequency) - 1; // 500 kHz for 2 microseconds
    TIMER_TopSet(C1_TIMER, topValue);  // Example: 1 kHz interrupt rate

    // Enable busIndex TIMER interrupts
    TIMER_IntEnable(C1_TIMER, TIMER_IF_OF);  // Enable overflow interrupt
    NVIC_SetPriority(C1_TIMER_IRQ, 0);      // Set TIMER0 interrupt priority to highest
    NVIC_EnableIRQ(C1_TIMER_IRQ);
}

void TIMER0_Read_IRQHandler()
{
    // Clear TIMER0 interrupt flag
    TIMER_IntClear(C1_TIMER, TIMER_IF_OF);

    // Disable interrupts
    // CORE_ATOMIC_IRQ_DISABLE();
    __disable_irq();

    switch (currentStateRD)
    {
        case RD_STATE_INIT:
            currentStateRD = RD_START_1;
            break;
        case RD_START_1:
            // GPIO_PinOutClear(gpioPort, gpioPin);
            GPIO_SIGNAL(currentBusIndex, 0);
            currentStateRD = RD_START_1_2;
            break;
        case RD_START_1_2:
            currentStateRD = RD_START_2;
            break;
        case RD_START_2:
            GPIO_SIGNAL(currentBusIndex, 1);
            currentStateRD = RD_START_2_2;
            break;
        case RD_START_2_2:
            currentStateRD = RD_ADDRESS_FOLLOW;
            break;
        case RD_ADDRESS_FOLLOW:
            GPIO_SIGNAL(currentBusIndex, 0);
            currentStateRD = RD_ADDRESS_FOLLOW_2;
            break;
        case RD_ADDRESS_FOLLOW_2:
            currentStateRD = RD_SLOW_ACCESS;
            break;
        case RD_SLOW_ACCESS:
            GPIO_SIGNAL(currentBusIndex, 1);
            currentStateRD = RD_SLOW_ACCESS_2;
            break;
        case RD_SLOW_ACCESS_2:
            currentStateRD = RD_INSTRUCTION_1;
            break;
        case RD_INSTRUCTION_1:
            GPIO_SIGNAL(currentBusIndex, 0);

            if(instruction0 == 1 && writeLogical1 < 6) {
                writeLogical1++;
            }
            else {
                currentStateRD = RD_INSTRUCTION_2;
                writeLogical1 = 0;
            }

            break;
        case RD_INSTRUCTION_2:
            GPIO_SIGNAL(currentBusIndex, 1);

            if(instruction1 == 1 && writeLogical1 < 6) {
                writeLogical1++;
            }
            else {
                currentStateRD = RD_INCREMENT;
                writeLogical1 = 0;
            }
            break;
        case RD_INCREMENT:
            GPIO_SIGNAL(currentBusIndex, 0);
            currentStateRD = RD_INCREMENT_2;
            break;
        case RD_INCREMENT_2:
            currentStateRD = RD_READ_1;
            break;
        case RD_READ_1:
            GPIO_SIGNAL(currentBusIndex, 1);
            currentStateRD = RD_READ_1_2;
            break;
        case RD_READ_1_2:
            currentStateRD = RD_READ_2;
            break;
        case RD_READ_2:
            currentStateRD = RD_READ_2_2;            
            break;
        case RD_READ_2_2:
            currentStateRD = RD_READ_3;
            break;
        case RD_READ_3:
            currentStateRD = RD_READ_3_2;
            break;
        case RD_READ_3_2:
            pinValue = GPIO_READ(currentBusIndex);

            // keep the signal LOW or HIGH depending on what the DUT signal was
            if(pinValue){
                GPIO_SIGNAL(currentBusIndex, 1);
            }
            else {
                GPIO_SIGNAL(currentBusIndex, 0);
            }

            currentStateRD = RD_READ_4;
            break;
        case RD_READ_4:            
            // modify the current bit of transfer_data which holds the register
            // value
            if(pinValue) {
                transfer_data |= (1 << bitToRead);
            }
            else {
                transfer_data &= ~(1 << bitToRead);
            }

            bitToRead++;
            currentStateRD = RD_READ_4_2;
            break;
        case RD_READ_4_2:
            currentStateRD = RD_READ_5;
            break;
        case RD_READ_5:
            currentStateRD = RD_READ_5_2;
            break;
        case RD_READ_5_2:
            // if value is 1 we need to pull the line low for an extra cycle in READ_6
            if(pinValue) {
                // GPIO_SIGNAL(currentBusIndex, 1);
                currentStateRD = RD_READ_6;
            }
            else {
                // GPIO_SIGNAL(currentBusIndex, 0);
                // byte value is 0 and there's still values to read go back to
                // READ_1, if not go to STATE_END
                if(bitToRead < 8) {
                    currentStateRD = RD_READ_1;
                }
                else {
                    currentStateRD = RD_STATE_END;
                }
            }
            break;
        case RD_READ_6:
            // set pin to low
            GPIO_SIGNAL(currentBusIndex, 0);

            // stay at this state for 2 cycles
            if(tRest < 2) {
                tRest++;
            }
            else {
                tRest = 0;
                // if there's more bits to read go back to READ_1
                // else go to end
                if(bitToRead < 8){
                    currentStateRD = RD_READ_1;
                }
                else {
                    currentStateRD = RD_STATE_END;
                }
            }
            break;
        
        case RD_STATE_END:
            GPIO_SIGNAL(currentBusIndex, 1);
            TIMER_Enable(C1_TIMER, false);
            break;

        default:
            // Default actions
            TIMER_Enable(C1_TIMER, false);
            break;  
    }

    // Re-enable interrupts
    __enable_irq();
}

void TIMER0_IRQHandler()
{
    // Clear TIMER0 interrupt flag
    TIMER_IntClear(C1_TIMER, TIMER_IF_OF);

    // Disable interrupts
    // CORE_ATOMIC_IRQ_DISABLE();
    __disable_irq();


    if(instruction == DATA_READ)
    {
        switch (currentStateRD)
        {
            case RD_STATE_INIT:
                currentStateRD = RD_START_1;
                break;
            case RD_START_1:
                // GPIO_PinOutClear(gpioPort, gpioPin);
                GPIO_SIGNAL(currentBusIndex, 1);
                currentStateRD = RD_START_1_2;
                break;
            case RD_START_1_2:
                currentStateRD = RD_START_2;
                break;
            case RD_START_2:
                GPIO_SIGNAL(currentBusIndex, 0);
                currentStateRD = RD_START_2_2;
                break;
            case RD_START_2_2:
                currentStateRD = RD_ADDRESS_FOLLOW;
                break;
            case RD_ADDRESS_FOLLOW:
                GPIO_SIGNAL(currentBusIndex, 1);
                currentStateRD = RD_ADDRESS_FOLLOW_2;
                break;
            case RD_ADDRESS_FOLLOW_2:
                currentStateRD = RD_SLOW_ACCESS;
                break;
            case RD_SLOW_ACCESS:
                GPIO_SIGNAL(currentBusIndex, 0);
                currentStateRD = RD_SLOW_ACCESS_2;
                break;
            case RD_SLOW_ACCESS_2:
                currentStateRD = RD_INSTRUCTION_1;
                break;
            case RD_INSTRUCTION_1:
                GPIO_SIGNAL(currentBusIndex, 1);

                if(instruction0 == 1 && writeLogical1 < 3) {
                    writeLogical1++;
                }
                else {
                    currentStateRD = RD_INSTRUCTION_1_2;
                    writeLogical1 = 0;
                }

                break;
            case RD_INSTRUCTION_1_2:
                if(instruction0 == 1 && writeLogical1 < 3) {
                    writeLogical1++;
                }
                else {
                    currentStateRD = RD_INSTRUCTION_2;
                    writeLogical1 = 0;
                }
                break;
            case RD_INSTRUCTION_2:
                GPIO_SIGNAL(currentBusIndex, 0);

                if(instruction1 == 1 && writeLogical1 < 3) {
                    writeLogical1++;
                }
                else {
                    currentStateRD = RD_INSTRUCTION_2_2;
                    writeLogical1 = 0;
                }
                break;
            case RD_INSTRUCTION_2_2:
                if(instruction0 == 1 && writeLogical1 < 3) {
                    writeLogical1++;
                }
                else {
                    currentStateRD = RD_INCREMENT;
                    writeLogical1 = 0;
                }
                break;
            case RD_INCREMENT:
                GPIO_SIGNAL(currentBusIndex, 1);
                currentStateRD = RD_INCREMENT_2;
                break;
            case RD_INCREMENT_2:
                currentStateRD = RD_READ_1;
                break;
            case RD_READ_1:
                GPIO_SIGNAL(currentBusIndex, 0);
                currentStateRD = RD_READ_1_2;
                break;
            case RD_READ_1_2:
                currentStateRD = RD_READ_2;
                break;
            case RD_READ_2:
                currentStateRD = RD_READ_2_2;
                GPIO_SIGNAL(currentBusIndex, 2);            
                break;
            case RD_READ_2_2:
                currentStateRD = RD_READ_3;
                break;
            case RD_READ_3:
                currentStateRD = RD_READ_3_2;
                break;
            case RD_READ_3_2:
                // pinValue = GPIO_READ();

                // // keep the signal LOW or HIGH depending on what the DUT signal was
                // if(pinValue){
                //     GPIO_SIGNAL(currentBusIndex, 1);
                // }
                // else {
                //     GPIO_SIGNAL(currentBusIndex, 0);
                // }

                currentStateRD = RD_READ_4;
                break;
            case RD_READ_4:            
                // modify the current bit of transfer_data which holds the register
                // value
                // if(pinValue) {
                //     transfer_data |= (1 << bitToRead);
                // }
                // else {
                //     transfer_data &= ~(1 << bitToRead);
                // }

                // bitToRead++;
                currentStateRD = RD_READ_4_2;
                break;
            case RD_READ_4_2:
                pinValue = GPIO_READ(currentBusIndex);

                // keep the signal LOW or HIGH depending on what the DUT signal was
                if(pinValue){
                    GPIO_SIGNAL(currentBusIndex, 1);
                }
                else {
                    GPIO_SIGNAL(currentBusIndex, 0);
                }

                currentStateRD = RD_READ_5;
                break;
            case RD_READ_5:
                if(pinValue) {
                    transfer_data |= (1 << bitToRead);
                }
                else {
                    transfer_data &= ~(1 << bitToRead);
                }

                bitToRead++;

                currentStateRD = RD_READ_5_2;
                break;
            case RD_READ_5_2:
                // if value is 1 we need to pull the line low for an extra cycle in READ_6
                if(!pinValue) {
                    // GPIO_SIGNAL(currentBusIndex, 1);
                    currentStateRD = RD_READ_6;
                }
                else {
                    // GPIO_SIGNAL(currentBusIndex, 0);
                    // byte value is 0 and there's still values to read go back to
                    // READ_1, if not go to STATE_END
                    if(bitToRead < 8) {
                        currentStateRD = RD_READ_1;
                    }
                    else {
                        currentStateRD = RD_STATE_END;
                    }
                }
                break;
            case RD_READ_6:
                // set pin to low
                GPIO_SIGNAL(currentBusIndex, 1);

                // stay at this state for 2 cycles
                if(tRest < 2) {
                    tRest++;
                }
                else {
                    tRest = 0;
                    // if there's more bits to read go back to READ_1
                    // else go to end
                    if(bitToRead < 8){
                        currentStateRD = RD_READ_1;
                    }
                    else {
                        currentStateRD = RD_STATE_END;
                    }
                }
                break;
            
            case RD_STATE_END:
                GPIO_SIGNAL(currentBusIndex, 0);
                TIMER_Enable(C1_TIMER, false);
                break;

            default:
                // Default actions
                TIMER_Enable(C1_TIMER, false);
                break;  
        }
    }
    else 
    {
        switch (currentState)
        {
            case STATE_INIT:
                currentState = START_1;
                break;
            case START_1:
                // GPIO_PinOutClear(gpioPort, gpioPin);
                GPIO_SIGNAL(currentBusIndex, 1);
                currentState = START_2;
                break;
            case START_2:
                GPIO_SIGNAL(currentBusIndex, 0);
                currentState = ADDRESS_FOLLOW;
                break;
            case ADDRESS_FOLLOW:
                GPIO_SIGNAL(currentBusIndex, 1);
                currentState = SLOW_ACCESS;
                break;
            case SLOW_ACCESS:
                GPIO_SIGNAL(currentBusIndex, 0);
                currentState = INSTRUCTION_1;
                break;
            case INSTRUCTION_1:
                GPIO_SIGNAL(currentBusIndex, 1);

                if(instruction0 == 1 && writeLogical1 < 3) {
                    writeLogical1++;
                }
                else {
                    currentState = INSTRUCTION_2;
                    writeLogical1 = 0;
                }

                break;
            case INSTRUCTION_2:
                GPIO_SIGNAL(currentBusIndex, 0);

                if(instruction1 == 1 && writeLogical1 < 3) {
                    writeLogical1++;
                }
                else {
                    currentState = INCREMENT;
                    writeLogical1 = 0;
                }
                break;
            case INCREMENT:
                GPIO_SIGNAL(currentBusIndex, 1);

                // Determine the next state
                switch(instruction) {
                    case DATA_READ:
                    case ADDRESS_READ:
                        currentState = READ_1;
                        break;
                    case DATA_WRITE:
                    case ADDRESS_WRITE:
                        currentState = DATA_1;
                        break;
                }
                break;
            case DATA_1:
                GPIO_SIGNAL(currentBusIndex, 0);

                write_bit = (transfer_data & (1 << 0)) >> 0;
            
                if(write_bit == 1 && writeLogical1 < 3) {
                    writeLogical1++;
                }
                else {
                    writeLogical1 = 0;
                    currentState = DATA_2;
                }

                break;
            case DATA_2:
                GPIO_SIGNAL(currentBusIndex, 1);

                write_bit = (transfer_data & (1 << 1)) >> 1;
            
                if(write_bit == 1 && writeLogical1 < 3) {
                    writeLogical1++;
                }
                else {
                    writeLogical1 = 0;
                    currentState = DATA_3;
                }
                break;
            case DATA_3:
                GPIO_SIGNAL(currentBusIndex, 0);

                write_bit = (transfer_data & (1 << 2)) >> 2;
            
                if(write_bit == 1 && writeLogical1 < 3) {
                    writeLogical1++;
                }
                else {
                    writeLogical1 = 0;
                    currentState = DATA_4;
                }
                break;
            case DATA_4:
                GPIO_SIGNAL(currentBusIndex, 1);

                write_bit = (transfer_data & (1 << 3)) >> 3;
            
                if(write_bit == 1 && writeLogical1 < 3) {
                    writeLogical1++;
                }
                else {
                    writeLogical1 = 0;
                    currentState = DATA_5;
                }
                break;
            case DATA_5:
                GPIO_SIGNAL(currentBusIndex, 0);

                write_bit = (transfer_data & (1 << 4)) >> 4;
            
                if(write_bit == 1 && writeLogical1 < 3) {
                    writeLogical1++;
                }
                else {
                    writeLogical1 = 0;
                    currentState = DATA_6;
                }
                break;
            case DATA_6:
                GPIO_SIGNAL(currentBusIndex, 1);

                write_bit = (transfer_data & (1 << 5)) >> 5;
            
                if(write_bit == 1 && writeLogical1 < 3) {
                    writeLogical1++;
                }
                else {
                    writeLogical1 = 0;
                    currentState = DATA_7;
                }
                break;
            case DATA_7:
                GPIO_SIGNAL(currentBusIndex, 0);

                write_bit = (transfer_data & (1 << 6)) >> 6;
            
                if(write_bit == 1 && writeLogical1 < 3) {
                    writeLogical1++;
                }
                else {
                    writeLogical1 = 0;
                    currentState = DATA_8;
                }
                break;
            case DATA_8:
                GPIO_SIGNAL(currentBusIndex, 1);

                write_bit = (transfer_data & (1 << 7)) >> 7;
            
                if(write_bit == 1 && writeLogical1 < 3) {
                    writeLogical1++;
                }
                else {
                    writeLogical1 = 0;
                    currentState = STATE_END;
                }
                break;
            case STATE_END:
                GPIO_SIGNAL(currentBusIndex, 0);
                TIMER_Enable(C1_TIMER, false);
                break;

            default:
                // Default actions
                TIMER_Enable(C1_TIMER, false);
                break;  
        }
    }

    // Re-enable interrupts
    __enable_irq();
}

void handleInstructionWrite(int instr_value, int index, C1_States nextState)
{
    GPIO_SIGNAL(currentBusIndex, index);

    if(instr_value == 1 && writeLogical1 < 3) {
        writeLogical1++;
    }
    else {
        currentState = nextState;
        writeLogical1 = 0;
    }
}

void handleDataWrite(int index, uint8_t* data, C1_States nextState)
{
    GPIO_SIGNAL(currentBusIndex, next_write_gpio_state);

    int write_bit = (*data & (1 << index)) >> index;;
 
    if(write_bit == 1 && writeLogical1 < 3) {
        writeLogical1++;
    }
    else {
        writeLogical1 = 0;
        currentState = nextState;

        int next_index = index + 1;

        // once it goes to the next state we need to change it
        if(( next_index % 2) == 0) {
            next_write_gpio_state = 1;
        }
        else {
            next_write_gpio_state = 0;
        }
    }
}

int getNthBit(uint8_t data, int n)
{
    return (data & (1 << n)) >> n;
}

void setNthBit(uint8_t &data, int n, bool logicValue)
{
    if(logicValue) {
        data |= (1 << n);
    }
    else {
        data &= ~(1 << n);
    }
}


