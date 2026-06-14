//----------------------------------------------------------------------------//
// INCLUDES
//----------------------------------------------------------------------------//
#include "eeprom.h"

//----------------------------------------------------------------------------//
// INTERNAL DEFINITIONS
//----------------------------------------------------------------------------//
enum 
{
    E_ST_FINISHED = 0,  // No Requests pending | Request Finished
    E_ST_WAIT_BUSY,     // Wait for EEPROM to not be busy
    E_ST_CHECK_DATA,    // Check if data has changed
    E_ST_WAIT_WRITE,    // Wait Write finished
};

//----------------------------------------------------------------------------//
// INTERNAL TYPES
//----------------------------------------------------------------------------//
static eeprom_address_t g_address;
static eeprom_data_t g_data;
static uint8_t g_state;

//----------------------------------------------------------------------------//
// INTERNAL GLOBAL VARIABLES
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// INTERNAL FUNCTIONS
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// EXTERNAL FUNCTIONS
//----------------------------------------------------------------------------//
/* EEPROM Request Init */
void eeprom_requestInit(void)
{
    // Init state
    g_state = E_ST_FINISHED;
}

/* New EEPROM Write Request */
void eeprom_newWriteRequest(eeprom_address_t address, eeprom_data_t data)
{
    // Check if state is finished
    if(g_state == E_ST_FINISHED)
    {
        // Set new request
        g_address = address;
        g_data = data;
        g_state = E_ST_WAIT_BUSY;
    }
    // REMARK: Will ignore new requests while a request is still pending
}

/* Get EEPROM Request Status */
uint8_t eeprom_requestStatus(void)
{
    uint8_t ret;
    uint8_t l_data;
    switch(g_state)
    {
        case E_ST_WAIT_BUSY:
            if(!EEPROM_IsBusy())
            {
                g_state = E_ST_CHECK_DATA;
            }
            break;
        case E_ST_CHECK_DATA:
            // Read the EEPROM contents
            l_data = EEPROM_Read(g_address);
            if(l_data == g_data)
            {
                // Same data, do not write
                g_state = E_ST_FINISHED;
            }
            else 
            {
                // Write Request
                EEPROM_Write(g_address, g_data);
                g_state = E_ST_WAIT_WRITE;
            }
            break;
        case E_ST_WAIT_WRITE:
            if(!EEPROM_IsBusy())
            {
                // Write finished
                g_state = E_ST_FINISHED;
            }
            break;
        default:
            g_state = E_ST_WAIT_BUSY;
            break;
        
    }
    if(g_state != E_ST_FINISHED)
    {
        ret = EEPROM_REQUEST_STARTED;
    }
    else 
    {
        ret = EEPROM_REQUEST_FINISHED;
    }
    return ret;
}