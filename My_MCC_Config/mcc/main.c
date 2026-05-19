 //----------------------------------------------------------------------------//
//                               OBJECT HISTORY                               //
//----------------------------------------------------------------------------//
//  REVISION |    DATE     |                               |      AUTHOR      //
//----------------------------------------------------------------------------//
//  1.00     | 15/Mai/2026 |                               | ALCP             //
// - First version                                                            //
//----------------------------------------------------------------------------//

#include "mcc_generated_files/system/system.h"
#include "../../adc.h"
#include "../../io.h"
#include "../../led.h"
#include "../../i2c.h"
#include "../../eeprom.h"
#include <stdint.h>
#include <string.h>

//----------------------------------------------------------------------------//
// INTERNAL DEFINITIONS
//----------------------------------------------------------------------------//
// GENERAL
#define VERSION             0
#define SECONDS_TICKS       (1000U/20)
#define RPI_WDT_ENABLE_FLAG 0x63 // Arbitrary number
#define RPI_MIN_RESET_COUNT 300 // 5 minutes
#define POWER_CYCLE_DELAY   30  // 30 seconds
// EEPROM - OFFSETS
#define EEPROM_WDT_OFFSET_ADDR                  0
#define EEPROM_RESET_TIMER_LIMIT_OFFSET_ADDR    1
#define EEPROM_RESET_COUNTER_OFFSET_ADDR        5
// EEPROM - ADDRESSES
#define EEPROM_BASE_ADDR    0x1400
#define EEPROM_WDT_ADDR (EEPROM_BASE_ADDR + EEPROM_WDT_OFFSET_ADDR)
#define EEPROM_RESET_TIMER_LIMIT_ADDR (EEPROM_BASE_ADDR + \
    EEPROM_RESET_TIMER_LIMIT_OFFSET_ADDR)
#define EEPROM_RESET_COUNTER_ADDR (EEPROM_BASE_ADDR + \
    EEPROM_RESET_COUNTER_OFFSET_ADDR)


//----------------------------------------------------------------------------//
// INTERNAL TYPES
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// INTERNAL GLOBAL VARIABLES
//----------------------------------------------------------------------------//
// Periodic tasks flags
static volatile uint8_t g_mainCounter;
static volatile uint8_t g_secondsCounter;
// Control and I2C variables
static i2cRegisters g_locI2CData;
static i2cRegisters g_tempI2CData;
static bool g_locIsI2CUpdated[I2C_REG_ADDR_SIZE];
static bool g_tempIsI2CUpdated[I2C_REG_ADDR_SIZE];

//----------------------------------------------------------------------------//
// INTERNAL FUNCTIONS
//----------------------------------------------------------------------------//
// INTERRUPT CALLBACK: 20ms periodic callback
void Timer_Callback_20ms(void)
{
    g_mainCounter++;
}

void i2cInitVariables(void)
{    
    //-----------------------------
    // COMMAND
    //-----------------------------
    // --> FIELD: Pi Command: No Command
    g_locI2CData.fields.rPiCommand = 0;
    //-----------------------------
    // EEPROM FIELD
    //-----------------------------
    // --> FIELD: Raspbery Pi Watchdog enable - Check EEPROM
    g_locI2CData.fields.rPiWDTEnable = EEPROM_Read(EEPROM_WDT_ADDR);
    //-----------------------------
    // TIMER
    //-----------------------------
    // --> FIELD: Reset Timer: 0 (init)
    g_locI2CData.fields.resetTimer = 0;
    //-----------------------------
    // EEPROM FIELD
    //-----------------------------
    // --> FIELD: Reset Timer Limit - Check EEPROM and boundary check it
    g_locI2CData.bytes[4] = (uint16_t)EEPROM_Read(
        EEPROM_RESET_TIMER_LIMIT_ADDR);
    g_locI2CData.bytes[5] = (uint16_t)EEPROM_Read(
        EEPROM_RESET_TIMER_LIMIT_ADDR + 1);
    if(g_locI2CData.fields.resetTimerLimit < RPI_MIN_RESET_COUNT)
    {
        g_locI2CData.fields.resetTimerLimit = RPI_MIN_RESET_COUNT;
    }
    //-----------------------------
    // EEPROM FIELD
    //-----------------------------
    // --> FIELD: Reset Counter
    g_locI2CData.bytes[6] = (uint16_t)EEPROM_Read(
        EEPROM_RESET_COUNTER_ADDR);
    g_locI2CData.bytes[7] = (uint16_t)EEPROM_Read(
        EEPROM_RESET_COUNTER_ADDR + 1);
    //-----------------------------
    //   - LED
    //-----------------------------    
    // --> FIELD: LED Config
    led_getLedConfig(&g_locI2CData.fields.ledConfig);
    //-----------------------------
    //   - RESET CAUSE
    //-----------------------------
    // --> FIELD: Reset Cause
    // Get reset cause and clear it after reading
    g_locI2CData.fields.resetCause = RSTCTRL_get_reset_cause();
    RSTCTRL_clear_reset_cause();
    //-----------------------------
    //   - ADC
    //-----------------------------
    g_locI2CData.fields.adcReading = 0;
    //-----------------------------
    //   - CALIBRATION
    //-----------------------------
    // --> FIELD: SIGROW OFFSET
    g_locI2CData.fields.sigrow_offset = SIGROW.TEMPSENSE1;
    // --> FIELD: SIGROW GAIN
    g_locI2CData.fields.sigrow_gain = SIGROW.TEMPSENSE0;
    //-----------------------------
    //   - VERSION
    //-----------------------------  
    g_locI2CData.fields.version = VERSION;
}

void i2cInitBufferIN(void)
{
    uint8_t index;
    //-----------------------------
    // Set IN buffer
    //-----------------------------
    memcpy((void*)&(g_I2CInData), (const void*)&(g_locI2CData), 
        I2C_REG_ADDR_SIZE);
    // Last bytes are not applicable to "in buffer" - set with index
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
}

void i2cInitBufferOUT(void)
{
    //-----------------------------
    // Set OUT buffer
    //-----------------------------
    // - Data written by Raspberry Pi and ATTiny: copy from IN Buffer
    memcpy((void*)&(g_I2COutData), (const void*)&(g_locI2CData), 
        I2C_REG_ADDR_SIZE);
}

void i2cSyncData(void)
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
    if(g_tempIsI2CUpdated[0])
    {
        // Update internal data with temporary data
        g_locI2CData.fields.rPiCommand = g_tempI2CData.fields.rPiCommand;
        // Set as clear on the local copy
        g_locIsI2CUpdated[0] = true;
    }
    if(g_tempIsI2CUpdated[2] || g_tempIsI2CUpdated[3])
    {
        // Update internal data with temporary data
        g_locI2CData.fields.resetTimer = g_tempI2CData.fields.resetTimer;
        // Set as clear on the local copy
        g_locIsI2CUpdated[2] = true;
        g_locIsI2CUpdated[3] = true;
    }
    if( g_tempIsI2CUpdated[8] || g_tempIsI2CUpdated[9] || 
        g_tempIsI2CUpdated[10] || g_isI2CInDataUpdated[11] ||
        g_isI2CInDataUpdated[12] || g_isI2CInDataUpdated[13])
    {
        // Update internal data
        memcpy((void*)&(g_locI2CData.fields.ledConfig), 
            (const void*)&(g_tempI2CData.fields.ledConfig), sizeof(ledConfig));        
        led_setLedConfig(&(g_locI2CData.fields.ledConfig));
        // Set as clear on the local copy
        memset((void*)&(g_locIsI2CUpdated[8]), true, sizeof(ledConfig));
    }
    //-----------------------------
    // UPDATE DATA WRITTEN BY HOST
    // - HOST DATA: Check data with EEPROM update
    //-----------------------------
    if(g_tempIsI2CUpdated[1])
    {
        g_locI2CData.fields.rPiWDTEnable = 
            g_tempI2CData.fields.rPiWDTEnable;
        // Save to EEPROM the new value
        eeprom_newWriteRequest(EEPROM_WDT_ADDR, 
            g_locI2CData.fields.rPiWDTEnable);
        // Check if EEPROM Finished
        if(eeprom_requestStatus() == EEPROM_REQUEST_FINISHED)
        {
            // After EEPROM Save: Update internal data with temporary data
            g_locI2CData.fields.rPiWDTEnable = 
                g_tempI2CData.fields.rPiWDTEnable;
            // Set as clear on the local copy
            g_locIsI2CUpdated[1] = true;
        }
    }
    // Has to be else if in order to process one EEPROM byte each function call
    else if(g_tempIsI2CUpdated[4])
    {
        g_locI2CData.bytes[4] = g_tempI2CData.bytes[4];
        // Save to EEPROM the new value
        eeprom_newWriteRequest(EEPROM_WDT_ADDR, 
            g_tempI2CData.bytes[4]);
        // Check if EEPROM Finished
        if(eeprom_requestStatus() == EEPROM_REQUEST_FINISHED)
        {
            // After EEPROM Save: Update internal data with temporary data
            // REMARK: Only if both bytes of resetTimerLimit are updated
            if(!g_tempIsI2CUpdated[5])
            {
                g_locI2CData.fields.resetTimerLimit = 
                    g_tempI2CData.fields.resetTimerLimit;
            }
            // Set as clear on the local copy
            g_locIsI2CUpdated[4] = true;
        }
    }
    // Has to be else if in order to process one EEPROM byte each function call
    else if(g_tempIsI2CUpdated[5])
    {
        g_locI2CData.bytes[5] = g_tempI2CData.bytes[5];
        // Save to EEPROM the new value
        eeprom_newWriteRequest(EEPROM_WDT_ADDR, 
            g_tempI2CData.bytes[5]);
        // Check if EEPROM Finished
        if(eeprom_requestStatus() == EEPROM_REQUEST_FINISHED)
        {
            // After EEPROM Save: Update internal data with temporary data
            // REMARK: Only if both bytes of resetTimerLimit are updated
            if(!g_tempIsI2CUpdated[4])
            {
                g_locI2CData.fields.resetTimerLimit = 
                    g_tempI2CData.fields.resetTimerLimit;
            }
            // Set as clear on the local copy
            g_locIsI2CUpdated[5] = true;
        }
    }
    // Has to be else if in order to process one EEPROM byte each function call
    else if(g_tempIsI2CUpdated[6])
    {
        g_locI2CData.bytes[6] = g_tempI2CData.bytes[6];
        // Save to EEPROM the new value
        eeprom_newWriteRequest(EEPROM_WDT_ADDR, 
            g_tempI2CData.bytes[6]);
        // Check if EEPROM Finished
        if(eeprom_requestStatus() == EEPROM_REQUEST_FINISHED)
        {
            // After EEPROM Save: Update internal data with temporary data
            // REMARK: Only if both bytes of resetCounter are updated
            if(!g_tempIsI2CUpdated[7])
            {
                g_locI2CData.fields.resetCounter = 
                    g_tempI2CData.fields.resetCounter;
            }
            // Set as clear on the local copy
            g_locIsI2CUpdated[6] = true;
        }
    }
    // Has to be else if in order to process one EEPROM byte each function call
    else if(g_tempIsI2CUpdated[7])
    {
        g_locI2CData.bytes[7] = g_tempI2CData.bytes[7];
        // Save to EEPROM the new value
        eeprom_newWriteRequest(EEPROM_WDT_ADDR, 
            g_tempI2CData.bytes[7]);
        // Check if EEPROM Finished
        if(eeprom_requestStatus() == EEPROM_REQUEST_FINISHED)
        {
            // After EEPROM Save: Update internal data with temporary data
            // REMARK: Only if both bytes of resetCounter are updated
            if(!g_tempIsI2CUpdated[6])
            {
                g_locI2CData.fields.resetCounter = 
                    g_tempI2CData.fields.resetCounter;
            }
            // Set as clear on the local copy
            g_locIsI2CUpdated[7] = true;
        }
    }
    // Enter Interrupt protected zone
    DISABLE_INTERRUPTS();
    // Copy from local I2C data to the "Out" buffer
    memcpy((void*)&(g_I2COutData), (const void*)&(g_locI2CData), 
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

void i2cHandleCommands(void)
{
    switch(g_locI2CData.fields.rPiCommand)
    {
        case 0x01:
            // Clear reset timer
            g_locI2CData.fields.resetTimer = 0;
            g_locI2CData.fields.rPiCommand = 0;
            break;
        case 0x11:
            // Restart ATTiny402
            g_locI2CData.fields.rPiCommand = 0;
            // TO DO
            break;
        case 0x21:
            // Cycle Power in POWER_CYCLE_DELAY seconds
            g_locI2CData.fields.rPiCommand = 0;
            // TO DO
            break;
    }
}

void i2cPeriodic(void)
{
    if(g_locI2CData.fields.resetTimer >= g_locI2CData.fields.resetTimerLimit)
    {
        // Cycle Power
        // TO DO
    }
    else 
    {
        g_locI2CData.fields.resetTimer++;
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
    // Init modules
    led_init();
    adc_init();
    eeprom_requestInit();
    // Init I2C data and buffers
    i2cInitVariables();
    i2cInitBufferIN();
    i2cInitBufferOUT();
    // Init i2C
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
            g_locI2CData.fields.adcReading = adc_getADCReading();
            led_periodic();
            // Check if I2c Host updated data
            i2cSyncData();
            // Handle I2C Commands
            i2cHandleCommands();
            // Check for 1 second tasks
            g_secondsCounter++;
            if(g_secondsCounter >= SECONDS_TICKS)
            {
                g_secondsCounter = 0;
                // Every 1 second
                i2cPeriodic();
            }
        }
    }    
}