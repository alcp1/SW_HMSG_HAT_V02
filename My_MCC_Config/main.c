//----------------------------------------------------------------------------//
// INCLUDES
//----------------------------------------------------------------------------//
#include "mcc_generated_files/system/system.h"
#include "../adc.h"
#include "../io.h"
#include "../led.h"
#include "../i2c.h"
#include "../eeprom.h"
#include <stdint.h>
#include <string.h>

//----------------------------------------------------------------------------//
// INTERNAL DEFINITIONS
//----------------------------------------------------------------------------//
// GENERAL
#define VERSION                 0
#define SECONDS_TICKS           (1000U/20)
#define RPI_WDT_ENABLE_FLAG     0x63 // Arbitrary number
#define DEFAULT_RESET_TIML      (60*60) // 60 minutes for Reset Timer Limit
#define DEFAULT_PWRC_TIME       30 // 30 seconds for power cycle
#define MAX_I2C_ERROR_TIME      25 // 25 seconds for I2C error before SW reset
#define MAX_PC_EEP_UPDATE_TIME  10 // 10 seconds for Reset Counter update
// EEPROM - OFFSETS
#define EEPROM_WDT_OFFSET_ADDR                  0
#define EEPROM_RESET_COUNTER_OFFSET_ADDR        2
// EEPROM - ADDRESSES
#define EEPROM_BASE_ADDR    0x1400
#define EEPROM_WDT_ADDR \
    (EEPROM_BASE_ADDR + EEPROM_WDT_OFFSET_ADDR)
#define EEPROM_RESET_COUNTER_ADDR \
    (EEPROM_BASE_ADDR + EEPROM_RESET_COUNTER_OFFSET_ADDR)


//----------------------------------------------------------------------------//
// INTERNAL TYPES
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// INTERNAL GLOBAL VARIABLES
//----------------------------------------------------------------------------//
//----------------------------------
// Periodic tasks flags
//----------------------------------
static volatile uint8_t g_mainCounter;
static volatile uint8_t g_secondsCounter;
//----------------------------------
// Control and I2C variables
//----------------------------------
// Local I2C data used by the application
static i2cRegisters g_appI2CData;
// "Is Updated array" used to signal when a register address was updated
static bool g_locIsI2CUpdated[I2C_REG_ADDR_SIZE];
// "Copy" variables from the original ones that are updated inside interrupts
static i2cRegisters g_tempI2CData;
static bool g_tempIsI2CUpdated[I2C_REG_ADDR_SIZE];
//----------------------------------
// Power Cycle Control
//----------------------------------
static bool powerCycleRequested;
static uint8_t powerCycleTimer;
static uint8_t resetBytesToUpdate;
//----------------------------------
// I2C Error Control
//----------------------------------
static uint8_t i2cErrorTimer;

//----------------------------------------------------------------------------//
// INTERNAL FUNCTIONS
//----------------------------------------------------------------------------//
// INTERRUPT CALLBACK: 20ms periodic callback
void Timer_Callback_20ms(void)
{
    g_mainCounter++;
}

// Init I2C app buffer / variable
void appI2CInit(void)
{    
    uint8_t index;
    //-----------------------------
    // PART 1: Update I2C Data
    //-----------------------------    
    // COMMAND
    // --> FIELD: Pi Command: No Command
    g_appI2CData.fields.rPiCommand = 0;
    // --> FIELD: Pi Aux: Default Cycle Power Timer Delay
    g_appI2CData.fields.rPiPCDelay = DEFAULT_PWRC_TIME;
    // EEPROM WDT ENABLE
    // --> FIELD: Raspbery Pi Watchdog enable - Check EEPROM
    g_appI2CData.fields.rPiWDTEnable = EEPROM_Read(EEPROM_WDT_ADDR);
    // TIMER
    // --> FIELD: Reset Timer: 0 (init)
    g_appI2CData.fields.resetTimer = 0;
    // RESET TIMER LIMIT
    // --> FIELD: Reset Timer Limit
    g_appI2CData.fields.resetTimerLimit = DEFAULT_RESET_TIML;
    // EEPROM FIELD - Reset Counter
    // --> FIELD: Reset Counter
    g_appI2CData.bytes[I2C_REG_RCNT] = (uint16_t)EEPROM_Read(
        EEPROM_RESET_COUNTER_ADDR);
    g_appI2CData.bytes[I2C_REG_RCNT + 1] = (uint16_t)EEPROM_Read(
        EEPROM_RESET_COUNTER_ADDR + 1);
    // LED
    // --> FIELD: LED Config
    led_getLedConfig(&g_appI2CData.fields.ledConfig);
    // RESET CAUSE
    // --> FIELD: Reset Cause
    // Get reset cause and clear it after reading
    g_appI2CData.fields.resetCause = RSTCTRL_get_reset_cause();
    RSTCTRL_clear_reset_cause();
    // ADC
    g_appI2CData.fields.adcReading = 0;
    // CALIBRATION
    // --> FIELD: SIGROW OFFSET
    g_appI2CData.fields.sigrow_offset = SIGROW.TEMPSENSE1;
    // --> FIELD: SIGROW GAIN
    g_appI2CData.fields.sigrow_gain = SIGROW.TEMPSENSE0;
    // SW VERSION
    //-----------------------------  
    g_appI2CData.fields.version = VERSION;

    //-----------------------------
    // PART 2: Update I2C IN Buffer
    //----------------------------- 
    memcpy((void*)&(g_I2CInData), (const void*)&(g_appI2CData), 
        I2C_REG_ADDR_SIZE);
    // Last bytes are not applicable to "IN Buffer" - set with index
    for(index = 14; index < I2C_REG_ADDR_SIZE; index++)
    {
        g_I2CInData.bytes[index] = index;
    }
    //-----------------------------
    // Set IN buffer Host Update Request
    //-----------------------------
    for(index = 0; index < I2C_REG_ADDR_SIZE; index++)
    {
        g_isI2CInDataUpdated[index] = false;
    }

    //-----------------------------
    // PART 3: Update I2C OUT Buffer
    //----------------------------- 
    // - Data written by Raspberry Pi and ATTiny: copy from IN Buffer
    memcpy((void*)&(g_I2COutData), (const void*)&(g_appI2CData), 
        I2C_REG_ADDR_SIZE);
}

// Sync I2C buffers and update data based on app I2C data and data written 
// by the HOST
void appSyncI2CData(void)
{    
    uint8_t index;
    // Init local "Is Updated" flags
    memset((void*)&(g_locIsI2CUpdated[0]), false, I2C_REG_ADDR_SIZE);
    // Enter Interrupt protected zone
    DISABLE_INTERRUPTS();
    memcpy((void*)&(g_tempI2CData), (const void*)&(g_I2CInData), 
        I2C_REG_ADDR_SIZE);
    memcpy((void*)&(g_tempIsI2CUpdated), (const void*)&(g_isI2CInDataUpdated), 
        I2C_REG_ADDR_SIZE);
    // Leave Interrupt protected zone
    ENABLE_INTERRUPTS();
    //-----------------------------
    // UPDATE DATA WRITTEN BY HOST
    // - HOST DATA: Check data with immediate update
    //-----------------------------
    // FIELD: rPiCommand
    if(g_tempIsI2CUpdated[I2C_REG_RPICOM])
    {
        // Update internal data with temporary data
        g_appI2CData.fields.rPiCommand = g_tempI2CData.fields.rPiCommand;
        // Set as clear on the local copy
        g_locIsI2CUpdated[I2C_REG_RPICOM] = true;
    }
    // FIELD: rPiPCDelay
    if(g_tempIsI2CUpdated[I2C_REG_RPIDLY])
    {
        // Update internal data with temporary data
        g_appI2CData.fields.rPiPCDelay = g_tempI2CData.fields.rPiPCDelay;
        // Set as clear on the local copy
        g_locIsI2CUpdated[I2C_REG_RPIDLY] = true;
    }
    // FIELD: resetTimer
    if(g_tempIsI2CUpdated[I2C_REG_TIM] || g_tempIsI2CUpdated[I2C_REG_TIM + 1])
    {
        // Update internal data with temporary data
        g_appI2CData.fields.resetTimer = g_tempI2CData.fields.resetTimer;
        // Set as clear on the local copy
        g_locIsI2CUpdated[I2C_REG_TIM] = true;
        g_locIsI2CUpdated[I2C_REG_TIM + 1] = true;
    }
    // FIELD: resetTimerLimit
    if(g_tempIsI2CUpdated[I2C_REG_TIML] || g_tempIsI2CUpdated[I2C_REG_TIML + 1])
    {
        // Update internal data with temporary data
        g_appI2CData.fields.resetTimerLimit = 
            g_tempI2CData.fields.resetTimerLimit;
        // Set as clear on the local copy
        g_locIsI2CUpdated[I2C_REG_TIML] = true;
        g_locIsI2CUpdated[I2C_REG_TIML + 1] = true;
    }
    // FIELD: ledConfig
    if( g_tempIsI2CUpdated[I2C_REG_LEDC] || 
        g_tempIsI2CUpdated[I2C_REG_LEDC + 1] || 
        g_tempIsI2CUpdated[I2C_REG_LEDC + 2] || 
        g_tempIsI2CUpdated[I2C_REG_LEDC + 3] ||
        g_tempIsI2CUpdated[I2C_REG_LEDC + 4] || 
        g_tempIsI2CUpdated[I2C_REG_LEDC + 5] )
    {
        // Update internal data
        memcpy((void*)&(g_appI2CData.fields.ledConfig), 
            (const void*)&(g_tempI2CData.fields.ledConfig), sizeof(ledConfig));
        led_setLedConfig(&(g_appI2CData.fields.ledConfig));
        // Set as clear on the local copy
        memset((void*)&(g_locIsI2CUpdated[I2C_REG_LEDC]), true, 
            sizeof(ledConfig));
    }
    //-----------------------------
    // UPDATE DATA WRITTEN BY HOST
    // - HOST DATA: Check data with EEPROM update
    //-----------------------------
    // FIELD: rPiWDTEnable
    if(g_tempIsI2CUpdated[I2C_REG_WDTEN])
    {
        g_appI2CData.fields.rPiWDTEnable = 
            g_tempI2CData.fields.rPiWDTEnable;
        // Save to EEPROM the new value
        eeprom_newWriteRequest(EEPROM_WDT_ADDR, 
            g_appI2CData.fields.rPiWDTEnable);
        // Check if EEPROM Finished
        if(eeprom_requestStatus() == EEPROM_REQUEST_FINISHED)
        {
            // After EEPROM Save: Update internal data with temporary data
            g_appI2CData.fields.rPiWDTEnable = 
                g_tempI2CData.fields.rPiWDTEnable;
            // Set as clear on the local copy
            g_locIsI2CUpdated[I2C_REG_WDTEN] = true;
        }
    }
    // FIELD: resetCounter
    // Has to be else if in order to process one EEPROM byte each function call
    else if(g_tempIsI2CUpdated[I2C_REG_RCNT])
    {
        g_appI2CData.bytes[I2C_REG_RCNT] = g_tempI2CData.bytes[I2C_REG_RCNT];
        // Save to EEPROM the new value
        eeprom_newWriteRequest(EEPROM_WDT_ADDR, 
            g_tempI2CData.bytes[I2C_REG_RCNT]);
        // Check if EEPROM Finished
        if(eeprom_requestStatus() == EEPROM_REQUEST_FINISHED)
        {
            // After EEPROM Save: Update internal data with temporary data
            // REMARK: Only if both bytes of resetCounter are updated
            if(!g_tempIsI2CUpdated[I2C_REG_RCNT + 1])
            {
                g_appI2CData.fields.resetCounter = 
                    g_tempI2CData.fields.resetCounter;
            }
            // Set as clear on the local copy
            g_locIsI2CUpdated[I2C_REG_RCNT] = true;
        }
    }
    // Has to be else if in order to process one EEPROM byte each function call
    else if(g_tempIsI2CUpdated[I2C_REG_RCNT + 1])
    {
        g_appI2CData.bytes[I2C_REG_RCNT + 1] = 
            g_tempI2CData.bytes[I2C_REG_RCNT + 1];
        // Save to EEPROM the new value
        eeprom_newWriteRequest(EEPROM_WDT_ADDR, 
            g_tempI2CData.bytes[I2C_REG_RCNT + 1]);
        // Check if EEPROM Finished
        if(eeprom_requestStatus() == EEPROM_REQUEST_FINISHED)
        {
            // After EEPROM Save: Update internal data with temporary data
            // REMARK: Only if both bytes of resetCounter are updated
            if(!g_tempIsI2CUpdated[I2C_REG_RCNT])
            {
                g_appI2CData.fields.resetCounter = 
                    g_tempI2CData.fields.resetCounter;
            }
            // Set as clear on the local copy
            g_locIsI2CUpdated[I2C_REG_RCNT + 1] = true;
        }
    }
    // Enter Interrupt protected zone
    DISABLE_INTERRUPTS();
    // Copy from local I2C data to the "Out" buffer
    memcpy((void*)&(g_I2COutData), (const void*)&(g_appI2CData), 
        I2C_REG_ADDR_SIZE);
    // Update "Is Updated" buffer
    for(index = 0; index < I2C_REG_ADDR_SIZE; index++)
    {
        if(g_locIsI2CUpdated[index])
        {
            // Field was updated
            g_isI2CInDataUpdated[index] = false;
        }
    }
    // Leave Interrupt protected zone
    ENABLE_INTERRUPTS();
}

// Handle I2C commands sent by the Host - rPiCommand
void appHandleI2CCommands(void)
{
    switch(g_appI2CData.fields.rPiCommand)
    {
        case 0x01:
            // Clear command
            g_appI2CData.fields.rPiCommand = 0;
            // Clear reset timer
            g_appI2CData.fields.resetTimer = 0;
            break;
        case 0x11:
            // Restart ATTiny402
            RSTCTRL_SoftwareReset();
            break;
        case 0x21:
            // Clear command
            g_appI2CData.fields.rPiCommand = 0;
            // Cycle Power in POWER_CYCLE_DELAY seconds
            powerCycleRequested = true;
            powerCycleTimer = g_appI2CData.fields.rPiPCDelay;
            // Do NOT update Reset Counter in EEPROM
            resetBytesToUpdate = 0;
            break;
        default:
            break;
    }
}

// App I2C task every 20ms
void appI2CPeriodic20ms(void)
{
    // Check I2C and update variables only if a power cycle was not requested
    if(!powerCycleRequested)
    {
        // Update ADC reading
        g_appI2CData.fields.adcReading = adc_getADCReading();
        // Check if I2c Host updated data
        appSyncI2CData();
        // Handle I2C Commands
        appHandleI2CCommands();
    }
    else
    {
        //------------------------------------        
        // Check if EEPROM has to be updated before a Power Cycle
        //------------------------------------
        // PART 1: Update Reset Counter (First Byte)
        if(resetBytesToUpdate >= 2)
        {
            // Save to EEPROM the new value
            eeprom_newWriteRequest(EEPROM_RESET_COUNTER_ADDR, 
                g_tempI2CData.bytes[I2C_REG_RCNT]);
            // Check if EEPROM Finished
            if(eeprom_requestStatus() == EEPROM_REQUEST_FINISHED)
            {
                // First EEPROM byte updated
                resetBytesToUpdate = 1;
            }
        }
        // PART 2: Update Reset Counter (Second Byte)
        // Has to be else if in order to process one EEPROM byte each 
        // function call
        else if(resetBytesToUpdate == 1)
        {
            // Save to EEPROM the new value
            eeprom_newWriteRequest(EEPROM_RESET_COUNTER_ADDR + 1, 
                g_tempI2CData.bytes[I2C_REG_RCNT + 1]);
            // Check if EEPROM Finished
            if(eeprom_requestStatus() == EEPROM_REQUEST_FINISHED)
            {
                // Finished updating EEPROM
                resetBytesToUpdate = 0;
                // No more waiting for shutdown
                powerCycleTimer = 0;
            }
        }
    }
}

// App I2C task every second
void appI2CPeriodic1s(void)
{
    //------------------------------
    // Check if WDT is enabled
    //------------------------------
    if(g_appI2CData.fields.rPiWDTEnable == RPI_WDT_ENABLE_FLAG)
    {
        //------------------------------
        // Check Restart Timer and Restart Timer Limit
        //------------------------------
        if(g_appI2CData.fields.resetTimer >= 
            g_appI2CData.fields.resetTimerLimit)
        {
            // Update Reset Counter
            g_appI2CData.fields.resetCounter++;            
            // Cycle Power Now (as soon as reset counter is updated in EEPROM)
            powerCycleRequested = true;
            powerCycleTimer = MAX_PC_EEP_UPDATE_TIME;
            // Reset Counter to be updated in EEPROM
            resetBytesToUpdate = 2;
        }
        else 
        {
            g_appI2CData.fields.resetTimer++;
        }
    }    
    //------------------------------
    // Check for Power Cycle
    //------------------------------
    if(powerCycleRequested)
    {
        // When power cycle requsted, decrement power cycle timer
        if(powerCycleTimer > 0)
        {
            powerCycleTimer--;
        }
        // If power cycle timer elapsed elapsed: cycle power anyway (even 
        // without EEPROM update - the time for it to get updated already 
        // elapsed)
        if(powerCycleTimer == 0)
        {
            // Shutdown
            IO_SET_SHDN_ACTIVE();
        }        
    }
    //------------------------------
    // Check for I2C Errors
    //------------------------------
    if(g_errorState != I2C_CLIENT_ERROR_NONE)
    {
        if(i2cErrorTimer > 0)
        {
            i2cErrorTimer--;
        }
        else
        {
            // Restart ATTiny402
            RSTCTRL_SoftwareReset();
        }
    }
    else
    {
        // Restart Timer
        i2cErrorTimer = MAX_I2C_ERROR_TIME;
    }
}

// main function
int main(void)
{
    // Init generated code
    SYSTEM_Initialize();
    // Set timer B callback
    TCB0_CaptureCallbackRegister(Timer_Callback_20ms);    
    // Init SHDN pin as input
    IO_SET_SHDN_INACTIVE();
    // Init Restart Control
    powerCycleRequested = false;
    // Init I2c Error Control
    i2cErrorTimer = MAX_I2C_ERROR_TIME;
    // Init modules
    led_init();
    adc_init();
    eeprom_requestInit();
    // Init I2C app data and buffers
    appI2CInit();
    // Init i2C module / interrupts
    i2c_init();
    // Endless loop
    g_mainCounter = 0;
    while(1)
    {
        // Check counter
        if(g_mainCounter > 0)
        {
            //-------------------------
            // CODE RUNS EVERY 20ms
            //-------------------------
            // reset counter            
            g_mainCounter = 0;
            // Clear Watchdog
            __builtin_avr_wdr();
            // periodic functions
            adc_periodic();
            led_periodic();
            appI2CPeriodic20ms();
            // Check for 1 second tasks
            g_secondsCounter++;
            if(g_secondsCounter >= SECONDS_TICKS)
            {
                g_secondsCounter = 0;
                // Every 1 second
                appI2CPeriodic1s();
            }
        }
    }    
}