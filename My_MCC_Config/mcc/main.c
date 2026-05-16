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
static volatile unsigned char g_tempI2CData;
static volatile i2c_client_error_t g_errorState;

//----------------------------------------------------------------------------//
// INTERNAL FUNCTIONS
//----------------------------------------------------------------------------//

// 20ms periodic callback
void Timer_Callback_20ms(void)
{
    g_mainCounter++;
}

static bool Client_Application(i2c_client_transfer_event_t event)
{
    switch (event)
    {
        //----------------------------
        // Address Match Event
        //----------------------------
        case I2C_CLIENT_TRANSFER_EVENT_ADDR_MATCH:
            if (I2C0_Client.TransferDirGet() == I2C_CLIENT_TRANSFER_DIR_WRITE)
            {
                // Write from Host
            }
            else
            {
                // Read from Host
            }
            break;

        //----------------------------
        // Data from Host to be received
        //----------------------------
        case I2C_CLIENT_TRANSFER_EVENT_RX_READY:
            g_tempI2CData  = I2C0_Client.ReadByte();
            break;

        //----------------------------
        // Data to be sent to Host
        //----------------------------
        case I2C_CLIENT_TRANSFER_EVENT_TX_READY:
            I2C0_Client.WriteByte(g_tempI2CData);
            break;

        //----------------------------
        // End of communication
        //----------------------------
        case I2C_CLIENT_TRANSFER_EVENT_STOP_BIT_RECEIVED:
            break;

        //----------------------------
        // Error Handler
        //----------------------------
        case I2C_CLIENT_TRANSFER_EVENT_ERROR:
            g_errorState = I2C0_Client.ErrorGet();
            if(g_errorState == I2C_CLIENT_ERROR_BUS_ERROR)
            {
                // Bus Error Handling
            }
            else if(g_errorState == I2C_CLIENT_ERROR_COLLISION)
            {
                // Collision Error Handling
            }
            break;

        default:
        break;
    }
    return true;
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
    I2C0_Client.CallbackRegister(Client_Application);
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