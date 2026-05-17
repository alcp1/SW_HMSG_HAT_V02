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
#include <stdint.h>

//----------------------------------------------------------------------------//
// INTERNAL DEFINITIONS
//----------------------------------------------------------------------------//
// GENERAL
#define VERSION             "01.00"
#define SECONDS_TICKS       (1000U/20)
#define I2C_REG_ADDR_SIZE   20
#define RPI_WDT_ENABLE_FLAG 0x63 // Arbitrary number
#define RPI_MIN_RESET_COUNT 300 // 5 minutes
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
static volatile uint8_t g_rPiCommand;
static volatile uint8_t g_rPiWDTEnable;
static volatile uint16_t g_resetTimer;
static volatile uint16_t g_resetTimerLimit;
static volatile uint8_t g_resetCause;
static volatile uint16_t g_resetCounter;
static volatile int8_t g_ADCReading;
static volatile int8_t g_sigrow_offset;
static volatile uint8_t g_sigrow_gain;
static volatile ledConfig g_ledConfigTemp;

//----------------------------------------------------------------------------//
// INTERNAL FUNCTIONS
//----------------------------------------------------------------------------//
// INTERRUPT CALLBACK: 20ms periodic callback
void Timer_Callback_20ms(void)
{
    g_mainCounter++;
}

void initI2CVariables(void)
{
    //-----------------------------
    //   - Init Control variables
    //-----------------------------
    // Pi Command: No Command
    g_rPiCommand = 0;
    //-----------------------------
    //   - EEPROM READ
    //-----------------------------
    // Raspbery Pi Watchdog enable - Check EEPROM
    g_rPiWDTEnable = EEPROM_Read(EEPROM_WDT_ADDR);
    // Reset Timer: 0 (init)
    g_resetTimer = 0;
    //Reset Timer Limit - Check EEPROM and boundary check it
    g_resetTimerLimit = (uint16_t)EEPROM_Read(
        EEPROM_RESET_TIMER_LIMIT_ADDR + 1);
    g_resetTimerLimit = g_resetTimerLimit << 8;
    g_resetTimerLimit += (uint16_t)EEPROM_Read(
        EEPROM_RESET_TIMER_LIMIT_ADDR);
    if(g_resetTimerLimit < RPI_MIN_RESET_COUNT)
    {
        g_resetTimerLimit = RPI_MIN_RESET_COUNT;
    }
    g_resetCounter = (uint16_t)EEPROM_Read(EEPROM_RESET_COUNTER_ADDR);
    //-----------------------------
    //   - RESET CAUSE
    //-----------------------------
    // Get reset cause and clear it after reading
    g_resetCause = RSTCTRL_get_reset_cause();
    RSTCTRL_clear_reset_cause();
    //-----------------------------
    //   - CALIBRATION
    //-----------------------------
    // Calibration values for temperature
    g_sigrow_offset = SIGROW.TEMPSENSE1;
    g_sigrow_gain = SIGROW.TEMPSENSE0; 
    //-----------------------------
    //   - LED
    //-----------------------------
    led_getLedConfig(&g_ledConfigTemp);
}

void initI2CInBuffer(void)
{
    uint8_t index;
    //-----------------------------
    // Set IN buffer
    //-----------------------------
    g_I2CInData[0] = g_rPiCommand;
    g_I2CInData[1] = g_rPiWDTEnable;
    g_I2CInData[2] = (uint8_t)(g_resetTimer & 0xFF);
    g_I2CInData[3] = (uint8_t)((g_resetTimer >> 8) & 0xFF);
    g_I2CInData[4] = (uint8_t)(g_resetTimerLimit & 0xFF);
    g_I2CInData[5] = (uint8_t)((g_resetTimerLimit >> 8) & 0xFF);
    g_I2CInData[6] = (uint8_t)(g_resetCounter & 0xFF);
    g_I2CInData[7] = (uint8_t)((g_resetCounter >> 8) & 0xFF);
    g_I2CInData[8] = g_ledConfigTemp.activeDuty;
    g_I2CInData[9] = g_ledConfigTemp.activeONtime;
    g_I2CInData[10] = g_ledConfigTemp.activeOFFtime;
    g_I2CInData[11] = g_ledConfigTemp.transmitDuty;
    g_I2CInData[12] = g_ledConfigTemp.transmitONtime;
    g_I2CInData[13] = g_ledConfigTemp.transmitOFFtime;
    // Remaining bytes are not applicable to in buffer - set with index
    for(index = 14; index < I2C_REG_ADDR_SIZE; index++)
    {
        g_I2CInData[index] = index;
    }
    //-----------------------------
    // Set IN buffer Host Update Request
    //-----------------------------
    for(index = 0; index < I2C_REG_ADDR_SIZE; index++)
    {
        g_isI2CInDataUpdated[index] = false;
    }
}

void initI2COutBuffer(void)
{
    //-----------------------------
    // Set OUT buffer
    //-----------------------------
    // - Data written by Raspberry Pi and ATTiny: copy from IN Buffer
    g_I2COutData[0] = g_I2CInData[0];
    g_I2COutData[1] = g_I2CInData[1];
    g_I2COutData[2] = g_I2CInData[2];
    g_I2COutData[3] = g_I2CInData[3];
    g_I2COutData[4] = g_I2CInData[4];
    g_I2COutData[5] = g_I2CInData[5];
    g_I2COutData[6] = g_I2CInData[6];
    g_I2COutData[7] = g_I2CInData[7];
    g_I2COutData[8] = g_I2CInData[8];
    g_I2COutData[9] = g_I2CInData[9];
    g_I2COutData[10] = g_I2CInData[10];
    g_I2COutData[11] = g_I2CInData[11];
    g_I2COutData[12] = g_I2CInData[12];
    g_I2COutData[13] = g_I2CInData[13];
    // - Data to be updated by ATTiny periodically / on power up
    g_I2COutData[14] = g_resetCause; // power up only
    g_I2COutData[15] = (uint8_t)(g_ADCReading); //periodic update
    g_I2COutData[16] = (uint8_t)(g_sigrow_offset); // power up only
    g_I2COutData[17] = g_sigrow_gain; // power up only    
}

void initI2CSyncData(void)
{
    // Check data
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
    // Init I2C data and buffers
    initI2CVariables();
    initI2CInBuffer();
    initI2COutBuffer();
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
            led_periodic();
            // Check if I2c Host updated data
            initI2CSyncData();
            // Check for 1second tasks
            g_secondsCounter++;
            if(g_secondsCounter >= SECONDS_TICKS)
            {
                g_secondsCounter = 0;
                // Every 1 second

            }
        }
    }    
}