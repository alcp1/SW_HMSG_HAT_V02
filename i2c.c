 //----------------------------------------------------------------------------//
//                               OBJECT HISTORY                               //
//----------------------------------------------------------------------------//
//  REVISION |    DATE     |                               |      AUTHOR      //
//----------------------------------------------------------------------------//
//  1.00     | 17/Mai/2026 |                               | ALCP             //
// - First version                                                            //
//----------------------------------------------------------------------------//

#include "My_MCC_Config/mcc/mcc_generated_files/i2c_client/twi0.h"
#include "i2c.h"

//----------------------------------------------------------------------------//
// INTERNAL DEFINITIONS
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// INTERNAL TYPES
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// INTERNAL GLOBAL VARIABLES
//----------------------------------------------------------------------------//
// I2C - INTERNAL
static volatile bool g_I2CWaitingRegAddr;
static volatile uint8_t g_I2CRegAddr;
static volatile i2c_client_error_t g_errorState;
// I2C - EXTERNAL
volatile i2cRegisters g_I2CInData;
volatile i2cRegisters g_I2COutData;
volatile bool g_isI2CInDataUpdated[I2C_REG_ADDR_SIZE];

//----------------------------------------------------------------------------//
// INTERNAL FUNCTIONS
//----------------------------------------------------------------------------//

// INTERRUPT CALLBACK: I2C Event
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
                // Write from Host - Host will then send the Register Address
                g_I2CWaitingRegAddr = true;
            }
            else
            {
                // Read from Host - Host already sent the Register Address
                g_I2CWaitingRegAddr = false;
            }
            break;

        //----------------------------
        // Data sent by I2C Host is available
        //----------------------------
        case I2C_CLIENT_TRANSFER_EVENT_RX_READY:            
            if(g_I2CWaitingRegAddr)
            {
                // If this is the first write by Host after Address Match, the
                // Host will send the register address
                g_I2CWaitingRegAddr = false;
                g_I2CRegAddr = I2C0_Client.ReadByte();
                // Check boundaries
                if(g_I2CRegAddr >= I2C_REG_ADDR_SIZE)
                {
                    // Error - Register Address out of range - Send NACK
                    g_I2CRegAddr = 0;
                    return false;
                }
            }
            else
            {
                // Register address already sent - write data to buffer
                g_I2CInData.bytes[g_I2CRegAddr] = I2C0_Client.ReadByte();
                g_isI2CInDataUpdated[g_I2CRegAddr] = true;
                g_I2CRegAddr++;
                if(g_I2CRegAddr >= I2C_REG_ADDR_SIZE)
                {
                    // Go to first address after the last address
                    g_I2CRegAddr = 0;
                }
            }
            break;

        //----------------------------
        // I2C client can respond to data read request from I2C Host
        //----------------------------
        case I2C_CLIENT_TRANSFER_EVENT_TX_READY:
            I2C0_Client.WriteByte(g_I2COutData.bytes[g_I2CRegAddr]);
            g_I2CRegAddr++;
            if(g_I2CRegAddr >= I2C_REG_ADDR_SIZE)
            {
                // Go to first address after the last address
                g_I2CRegAddr = 0;
            }
            break;

        //----------------------------
        // I2C stop bit received
        //----------------------------
        case I2C_CLIENT_TRANSFER_EVENT_STOP_BIT_RECEIVED:
            // Go to first address after the last address
            g_I2CRegAddr = 0;
            // Signal transmision
            led_requestTransmitSignaling();
            break;

        //----------------------------
        // I2C Bus error occurred
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
    // return true to send ACK
    return true;
}

//----------------------------------------------------------------------------//
// EXTERNAL FUNCTIONS
//----------------------------------------------------------------------------//

/* Initialization */
void i2c_init(void)
{
    // Set I2C Client callback
    I2C0_Client.CallbackRegister(Client_Application);
}
