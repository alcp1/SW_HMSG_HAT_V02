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
#include "io.h"
#include "led.h"

//----------------------------------------------------------------------------//
// INTERNAL DEFINITIONS
//----------------------------------------------------------------------------//
// INIT STATE TIMING
#define LED_INIT_TIME           (1500U/20)      // 1,5s @ 20ms period
#define LED_INIT_DUTY_INCREMENT (255U/1500U/20) // 1,5s @ 20ms period
#define LED_DUTY_INIT           (127)           // 50%
#define LED_TON_INIT            (40U/20)        // 40ms @ 20ms period
#define LED_TOFF_INIT           (460U/20)       // 460ms @ 20ms period
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
static volatile unsigned char g_duty;   // 0 = 0%, 255 = 100%

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
    g_duty = 0;
    // Set total time to 0
    g_totalTime = 0;
    // Init LED with 0% duty
    io_setLED(0);
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
                // Duty = 100%
                g_duty = 255;
            }
            else
            {
                // Duty = linear increase
                g_duty += LED_INIT_DUTY_INCREMENT;
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
                g_duty = g_configduty;
            }
            else
            {
                // Duty = 0% during OFF time
                g_duty = 0;
            }
            break;
        default:
            led_init();
            break;
    }
    // Update LED output with Duty cycle
    io_setLED(g_duty);
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