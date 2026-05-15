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
#include <stdio.h>
#include "My_MCC_Config/mcc/mcc_generated_files/system/system.h"
#include "led.h"

//----------------------------------------------------------------------------//
// INTERNAL DEFINITIONS
//----------------------------------------------------------------------------//
// INIT STATE TIMING
#define LED_INIT_TIME           (1700U/20)          // 1,7s @ 20ms period
#define LED_INIT_DUTY_INCREMENT (255U/(1700U/20))   // 1,7s @ 20ms period
#define LED_DUTY_INIT           (127)               // 50%
#define LED_TON_INIT            (40U/20)            // 40ms @ 20ms period
#define LED_TOFF_INIT           (460U/20)           // 460ms @ 20ms period
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
static volatile unsigned char g_state;
static volatile unsigned char g_totalTime;
static volatile unsigned char g_configduty;
static volatile unsigned char g_configOnTime;
static volatile unsigned char g_configOffTime;
static volatile unsigned char g_configTotalTime;
static volatile unsigned char g_duty8;   // 0 = 0%, 255 = 100%
static volatile unsigned int  g_duty16;

//----------------------------------------------------------------------------//
// INTERNAL FUNCTIONS
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
// EXTERNAL FUNCTIONS
//----------------------------------------------------------------------------//

/* LED Initialization */
void led_init(void)
{
    // Init variables
    g_state = LED_STATE_INIT;    
    g_configOnTime = LED_TON_INIT;
    g_configOffTime = LED_TOFF_INIT;
    g_configduty = LED_DUTY_INIT;
    g_configTotalTime = LED_TON_INIT + LED_TOFF_INIT;
    // Set duty to 0%
    g_duty8 = 0;
    g_duty16 = 0;
    // Set total time to 0
    g_totalTime = 0;
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
            g_totalTime++;
            if(g_totalTime > LED_INIT_TIME)
            {
                // End of INIT state
                g_state = LED_STATE_ACTIVE;
                // Set total time to 0
                g_totalTime = 0;
            }
            else if(g_totalTime == LED_INIT_TIME)
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
        // Time OFF, time ON at specific duty cycle
        //--------------------
        case LED_STATE_ACTIVE:
            // Check total time
            g_totalTime++;
            if(g_totalTime >= g_configTotalTime)
            {
                g_totalTime = 0;
            }
            // Check TON and TOFF
            if(g_totalTime >= g_configOffTime)
            {
                // Duty = configured value
                g_duty8 = g_configduty;
            }
            else
            {
                // Duty = 0% during OFF time
                g_duty8 = 0;
            }
            break;
        default:
            led_init();
            break;
    }
    // Update LED output with Duty cycle
    g_duty16 = (unsigned int)(g_duty8);
    // Set Duty Cycle for LED output
    TCA0_Compare0BufferSet(g_duty16);
}

/* Sets LED PWM Duty Cycle. */
void led_setPWMDutyCycle(unsigned char pwm)
{
    g_configduty = pwm;
}

/* Sets LED ON Time. */
void led_setOnTime(unsigned char on_time)
{
    g_configOnTime = on_time;
    g_configTotalTime = g_configOnTime + g_configOffTime;
}

/* Sets LED OFF Time. */
void led_setOffTime(unsigned char off_time)
{
    g_configOffTime = off_time;
    g_configTotalTime = g_configOnTime + g_configOffTime;
}