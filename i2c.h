//----------------------------------------------------------------------------//
//                               OBJECT HISTORY                               //
//----------------------------------------------------------------------------//
//  REVISION |    DATE     |                               |      AUTHOR      //
//----------------------------------------------------------------------------//
//  1.00     | 17/Mai/2026 |                               | ALCP             //
// - First version                                                            //
//----------------------------------------------------------------------------//

#ifndef I2C_H
#define I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "led.h"

//----------------------------------------------------------------------------//
// EXTERNAL DEFINITIONS
//----------------------------------------------------------------------------//
#define I2C_REG_ADDR_SIZE   19

//----------------------------------------------------------------------------//
// EXTERNAL TYPES
//----------------------------------------------------------------------------//
typedef union 
{
    // Raw unsigned byte array
    uint8_t bytes[I2C_REG_ADDR_SIZE];
    // Individual Fields
    struct 
    {
        uint8_t rPiCommand;         // byte index 0
        uint8_t rPiWDTEnable;       // byte index 1
        uint16_t resetTimer;        // byte index 2 and 3
        uint16_t resetTimerLimit;   // byte index 4 and 5
        uint16_t resetCounter;      // byte index 6 and 7
        ledConfig ledConfig;        // byte index 8 to 13
        uint8_t resetCause;         // byte index 14
        int16_t adcReading;         // byte index 15 and 16
        int8_t sigrow_offset;       // byte index 17
        uint8_t sigrow_gain;        // byte index 18
    } fields;
} i2cRegisters;

//----------------------------------------------------------------------------//
// EXTERNAL VARIABLES
//----------------------------------------------------------------------------//
extern volatile i2cRegisters g_I2CInData;
extern volatile i2cRegisters g_I2COutData;
extern volatile bool g_isI2CInDataUpdated[I2C_REG_ADDR_SIZE];

//----------------------------------------------------------------------------//
// EXTERNAL FUNCTIONS
//----------------------------------------------------------------------------//
/**
 * Init I2C.
 * 
 * \param   nothing
 * \return  nothing
 */
extern void i2c_init(void);


#ifdef __cplusplus
}
#endif

#endif /* I2C_H */

