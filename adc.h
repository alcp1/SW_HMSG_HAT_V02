//----------------------------------------------------------------------------//
//                               OBJECT HISTORY                               //
//----------------------------------------------------------------------------//
//  REVISION |    DATE     |                               |      AUTHOR      //
//----------------------------------------------------------------------------//
//  1.00     | 15/Mai/2026 |                               | ALCP             //
// - First version                                                            //
//----------------------------------------------------------------------------//

#ifndef ADC_H
#define ADC_H

#ifdef __cplusplus
extern "C" {
#endif

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
extern signed int adc_getADCReading(void);

/**
 * Returns current temperature.
 * 
 * \param   nothing
 * \return  temperature in Celsius
 */
extern signed int adc_getTemperature(void);

#ifdef __cplusplus
}
#endif

#endif /* ADC_H */

