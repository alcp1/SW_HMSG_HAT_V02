//----------------------------------------------------------------------------//
//                               OBJECT HISTORY                               //
//----------------------------------------------------------------------------//
//  REVISION |    DATE     |                               |      AUTHOR      //
//----------------------------------------------------------------------------//
//  1.00     | 15/Mai/2026 |                               | ALCP             //
// - First version                                                            //
//----------------------------------------------------------------------------//

#ifndef EEPROM_H
#define EEPROM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "My_MCC_Config/mcc/mcc_generated_files/nvm/nvm.h"

//----------------------------------------------------------------------------//
// EXTERNAL DEFINITIONS
//----------------------------------------------------------------------------//
#define EEPROM_REQUEST_FINISHED     0
#define EEPROM_REQUEST_STARTED      1

//----------------------------------------------------------------------------//
// EXTERNAL FUNCTIONS
//----------------------------------------------------------------------------//
/**
 * EEPROM Request Init.
 * 
 * \param   nothing
 * \return  nothing
 */
extern void eeprom_requestInit(void);

/**
 * New EEPROM Write Request. REMARK: Will ignore new requests while a request 
 * is still pending.
 * 
 * \param   address EEPROM Address to be written
 * \param   data EEPROM Data to be written
 * \return  nothing
 */
extern void eeprom_newWriteRequest(eeprom_address_t address, 
    eeprom_data_t data);

/**
 * Get EEPROM Request Status.
 * 
 * \param   nothing
 * \return  EEPROM_REQUEST_STARTED: Request in progress
 * \return  EEPROM_REQUEST_FINISHED: Request finished
 */
extern uint8_t eeprom_requestStatus(void);


#ifdef __cplusplus
}
#endif

#endif /* EEPROM_H */

