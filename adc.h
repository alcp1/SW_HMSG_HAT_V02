//----------------------------------------------------------------------------//
//                               OBJECT HISTORY                               //
//----------------------------------------------------------------------------//
//  REVISION |    DATE     |                               |      AUTHOR      //
//----------------------------------------------------------------------------//
//  1.00     | 15/May/2026 |                               | ALCP             //
// - First version                                                            //
//----------------------------------------------------------------------------//

#ifndef ADC_H
#define ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
//----------------------------------------------------------------------------//
// EXTERNAL DEFINITIONS
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// EXTERNAL FUNCTIONS
//----------------------------------------------------------------------------//
/**
 * ADC Initialization.
 * 
 * \param   nothing
 * \return  nothing
 */
extern void adc_init(void);

/**
 * Periodic function.
 * 
 * \param   nothing
 * \return  nothing
 */
extern void adc_periodic(void);

/**
 * Returns last ADC Reading.
 * 
 * \param   nothing
 * \return  ADC Convertion result
 */
extern int16_t adc_getADCReading(void);

/**
 * Returns current temperature.
 * 
 * \param   nothing
 * \return  temperature in Celsius
 */
extern int16_t adc_getTemperature(void);

#ifdef __cplusplus
}
#endif

#endif /* ADC_H */

