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

//----------------------------------------------------------------------------//
// INTERNAL DEFINITIONS
//----------------------------------------------------------------------------//
#define VERSION "01.00"

//----------------------------------------------------------------------------//
// INTERNAL TYPES
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// INTERNAL GLOBAL VARIABLES
//----------------------------------------------------------------------------//
static volatile unsigned char g_mainCounter;

//----------------------------------------------------------------------------//
// INTERNAL FUNCTIONS
//----------------------------------------------------------------------------//

// 20ms periodic callback
void Timer_Callback_20ms(void)
{
    g_mainCounter++;
}

// main function
int main(void)
{
    // Init generated code
    SYSTEM_Initialize();
    // Set timer B callback
    TCB0_CaptureCallbackRegister(Timer_Callback_20ms);    
    // Init modules
    IO_SET_SHDN_INACTIVE();
    led_init();
    adc_init();
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
        }
    }    
}