//----------------------------------------------------------------------------//
//                               OBJECT HISTORY                               //
//----------------------------------------------------------------------------//
//  REVISION |    DATE     |                               |      AUTHOR      //
//----------------------------------------------------------------------------//
//  1.00     | 15/Mai/2026 |                               | ALCP             //
// - First version                                                            //
//----------------------------------------------------------------------------//

#ifndef LED_H
#define LED_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
//----------------------------------------------------------------------------//
// EXTERNAL TYPES
//----------------------------------------------------------------------------//
// HAPCAN frame
typedef struct  
{
    uint8_t activeDuty;
    uint8_t activeONtime;
    uint8_t activeOFFtime;
    uint8_t transmitDuty;
    uint8_t transmitONtime;
    uint8_t transmitOFFtime;
} ledConfig;


//----------------------------------------------------------------------------//
// EXTERNAL FUNCTIONS
//----------------------------------------------------------------------------//
/**
 * LED Initialization.
 * 
 * \param   nothing
 * \return  nothing
 */
extern void led_init(void);

/**
 * Periodic function.
 * 
 * \param   nothing
 * \return  nothing
 */
extern void led_periodic(void);

/**
 * Set LED Config.
 * 
 * \param   config LED configuration
 * \return  nothing
 */
extern void led_setLedConfig(volatile ledConfig* config);

/**
 * Get LED Config.
 * 
 * \param   config LED configuration
 * \return  nothing
 */
extern void led_getLedConfig(volatile ledConfig* config);

/**
 * Request LED Transition signaling.
 * 
 * \param   nothing
 * \return  nothing
 */
extern void led_requestTransmitSignaling(void);


#ifdef __cplusplus
}
#endif

#endif /* LED_H */

