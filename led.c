//----------------------------------------------------------------------------//
//                               OBJECT HISTORY                               //
//----------------------------------------------------------------------------//
//  REVISION |    DATE     |                               |      AUTHOR      //
//----------------------------------------------------------------------------//
//  1.00     | 15/Mai/2026 |                               | ALCP             //
// - First version                                                            //
//----------------------------------------------------------------------------//

/*
 * Includes
 */
#include "My_MCC_Config/mcc/mcc_generated_files/timer/tca0.h"
#include "led.h"

//----------------------------------------------------------------------------//
// INTERNAL DEFINITIONS
//----------------------------------------------------------------------------//
// INIT STATE TIMING
#define LED_INIT_TIME           (1700U/20)          // 1,7s @ 20ms period
#define LED_INIT_DUTY_INCREMENT (255U/(1700U/20))   // 1,7s @ 20ms period
// ACTIVE STATE INITIAL CONFIG
#define LED_ACTIVE_DUTY_INIT    (127)               // 50%
#define LED_ACTIVE_TON_INIT     (40U/20)            // 40ms @ 20ms period
#define LED_ACTIVE_TOFF_INIT    (460U/20)           // 460ms @ 20ms period
// TRASNMISSTION SIGNALING INITIAL CONFIG
#define LED_TRANSM_DUTY_INIT    (191)               // 75%
#define LED_TRANSM_TON_INIT     (60U/20)            // 60ms @ 20ms period
#define LED_TRANSM_TOFF_INIT    (40U/20)            // 40ms @ 20ms period
// STATE
enum
{
    LED_STATE_INIT = 0,
    LED_STATE_ACTIVE
};

//----------------------------------------------------------------------------//
// INTERNAL TYPES
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// INTERNAL GLOBAL VARIABLES
//----------------------------------------------------------------------------//
static volatile ledConfig g_ledConfig;
static volatile uint8_t g_state;
static volatile uint8_t g_activeTotalTime;
static volatile uint8_t g_configActiveDuty;
static volatile uint8_t g_configActiveOnTime;
static volatile uint8_t g_configActiveOffTime;
static volatile uint8_t g_configActiveTotalTime;
static volatile bool g_transmitRequested;
static volatile uint8_t g_transmitTotalTime;
static volatile uint8_t g_configTransmitDuty;
static volatile uint8_t g_configTransmitOnTime;
static volatile uint8_t g_configTransmitOffTime;
static volatile uint8_t g_configTransmitTotalTime;
static volatile uint8_t g_duty8;   // 0 = 0%, 255 = 100%
static volatile uint16_t g_duty16;

//----------------------------------------------------------------------------//
// INTERNAL FUNCTIONS
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
// EXTERNAL FUNCTIONS
//----------------------------------------------------------------------------//

/* LED Initialization */
void led_init(void)
{
    //------------------------
    // Init variables
    //------------------------
    // Initial State (when module starts, initial transition on LED)
    g_state = LED_STATE_INIT;
    // Initial active config
    g_configActiveOnTime = LED_ACTIVE_TON_INIT;
    g_configActiveOffTime = LED_ACTIVE_TOFF_INIT;
    g_configActiveDuty = LED_ACTIVE_DUTY_INIT;
    g_configActiveTotalTime = LED_ACTIVE_TON_INIT + LED_ACTIVE_TOFF_INIT;
    // Initial transmit config
    g_configTransmitDuty = LED_TRANSM_DUTY_INIT;
    g_configTransmitOnTime = LED_TRANSM_TON_INIT;
    g_configTransmitOffTime = LED_TRANSM_TOFF_INIT;
    g_configTransmitTotalTime = LED_TRANSM_TON_INIT + LED_TRANSM_TOFF_INIT;
    // Set duty to 0%
    g_duty8 = 0;
    g_duty16 = 0;
    // Set total time to 0
    g_activeTotalTime = 0;
    g_transmitTotalTime = 0;
    // Clear transmit request
    g_transmitRequested = false;
    // Init LED with 0% duty
    TCA0_Compare0BufferSet(g_duty16);
}

/* Periodic function */
void led_periodic(void)
{
    // Check mode set
    switch(g_state)
    {
        //--------------------
        // Increase duty cycle up to 100% during INIT state
        //--------------------
        case LED_STATE_INIT:        
            g_activeTotalTime++;
            if(g_activeTotalTime > LED_INIT_TIME)
            {
                // End of INIT state
                g_state = LED_STATE_ACTIVE;
                // Set total time to 0
                g_activeTotalTime = 0;
            }
            else if(g_activeTotalTime == LED_INIT_TIME)
            {
                // Duty = 100% = 255
                g_duty8 = 255;
            }
            else
            {
                // Duty = linear increase
                g_duty8 += LED_INIT_DUTY_INCREMENT;
            }
            break;        
        //--------------------
        // Time OFF, time ON at specific duty cycle on active modules.
        // If a trasmit signaling was requestes, use transmit values for Duty,
        // ON and OFF without disrupting the Active ON / OFF timers 
        //--------------------
        case LED_STATE_ACTIVE:
            //--------------------------------------------------------
            // ACTIVE LED SIGNALING - Active all the time after init
            //--------------------------------------------------------
            // Check total time
            g_activeTotalTime++;
            if(g_activeTotalTime >= g_configActiveTotalTime)
            {
                g_activeTotalTime = 0;
            }
            // Check TON and TOFF
            if(g_activeTotalTime >= g_configActiveOffTime)
            {
                // Duty = configured value
                g_duty8 = g_configActiveDuty;
            }
            else
            {
                // Duty = 0% during OFF time
                g_duty8 = 0;
            }
            //--------------------------------------------------------
            // TRANSMIT LED SIGNALING - Active when there is a transmit 
            // event after init
            // TRANSMIT has priority over ACTIVE
            //--------------------------------------------------------
            if(g_transmitRequested)
            {
                // Check total time
                g_transmitTotalTime++;
                if(g_transmitTotalTime >= g_configTransmitTotalTime)
                {
                    // End of transmit signaling
                    g_transmitTotalTime = 0;
                    g_transmitRequested = false;
                }
                // Check TON and TOFF
                if(g_transmitTotalTime >= g_configTransmitOffTime)
                {
                    // Duty = configured value
                    g_duty8 = g_configTransmitDuty;
                }
                else
                {
                    // Duty = 0% during OFF time
                    g_duty8 = 0;
                }
            }
            break;
        default:
            led_init();
            break;
    }
    // Update LED output with Duty cycle
    g_duty16 = (uint16_t)(g_duty8);
    // Set Duty Cycle for LED output
    TCA0_Compare0BufferSet(g_duty16);
}

/* Set LED Config */
void led_setLedConfig(volatile ledConfig* config)
{
    // Copy data
    g_ledConfig.activeDuty = config->activeDuty;
    g_ledConfig.activeONtime = config->activeONtime;
    g_ledConfig.activeOFFtime = config->activeOFFtime;
    g_ledConfig.transmitDuty = config->transmitDuty;
    g_ledConfig.transmitONtime = config->transmitONtime;
    g_ledConfig.transmitOFFtime = config->transmitOFFtime;
    // Set variables
    g_configActiveDuty = g_ledConfig.activeDuty;
    g_configActiveOnTime = g_ledConfig.activeONtime;
    g_configActiveOffTime = g_ledConfig.activeOFFtime;
    g_configActiveTotalTime = g_configActiveOnTime + g_configActiveOffTime;
    g_configTransmitDuty = g_ledConfig.transmitDuty;
    g_configTransmitOnTime = g_ledConfig.transmitONtime;
    g_configTransmitOffTime = g_ledConfig.transmitOFFtime;
    g_configTransmitTotalTime = g_configTransmitOnTime +
        g_configTransmitOffTime;
}

/* Get LED Config */
void led_getLedConfig(volatile ledConfig* config)
{
    // Copy data
    config->activeDuty = g_ledConfig.activeDuty;
    config->activeONtime = g_ledConfig.activeONtime;
    config->activeOFFtime = g_ledConfig.activeOFFtime;
    config->transmitDuty = g_ledConfig.transmitDuty;
    config->transmitONtime = g_ledConfig.transmitONtime;
    config->transmitOFFtime = g_ledConfig.transmitOFFtime;
}

/* Request LED Transition signaling */
void led_requestTransmitSignaling(void)
{
    g_transmitRequested = true;    
}
