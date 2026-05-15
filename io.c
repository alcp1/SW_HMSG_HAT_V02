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
#include "io.h"

//----------------------------------------------------------------------------//
// INTERNAL DEFINITIONS
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// INTERNAL TYPES
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// INTERNAL GLOBAL VARIABLES
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// INTERNAL FUNCTIONS
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// EXTERNAL FUNCTIONS
//----------------------------------------------------------------------------//
/* IO Initialization */
void io_init(void)
{
}

/* Sets SHDN */
void io_setSHDN(unsigned char state)
{
    if(state)
    {
        // Set SHDN as output with 0
        SUP_SHDN_SetLow();
        SUP_SHDN_SetDigitalOutput();
        SUP_SHDN_SetLow();
    }
    else
    {
        // Set SHDN as input
        SUP_SHDN_SetDigitalInput();
    }
}

/* Sets LED Duty Cycle */
void io_setLED(unsigned char duty)
{
    unsigned int temp = 0;
    // Casting (uchar to uint)
    temp = (unsigned int)(duty);
    // Set Duty Cycle for LED output
    TCA0_Compare0BufferSet(temp);
}