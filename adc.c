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
#include "adc.h"

//----------------------------------------------------------------------------//
// INTERNAL DEFINITIONS
//----------------------------------------------------------------------------//
#define ADC_INITIAL_TEMP    27
#define ADC_MAX_DELTA       4
#define ADC_MIN_VALUE       -40
#define ADC_MAX_VALUE       120

//----------------------------------------------------------------------------//
// INTERNAL TYPES
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// INTERNAL GLOBAL VARIABLES
//----------------------------------------------------------------------------//
static volatile signed int g_LastTemperature;
static volatile signed int g_Temperature;
static volatile signed int g_ADCReading;

//----------------------------------------------------------------------------//
// INTERNAL FUNCTIONS
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// EXTERNAL FUNCTIONS
//----------------------------------------------------------------------------//
/* ADC Initialization */
void adc_init(void)
{
    g_LastTemperature = ADC_INITIAL_TEMP;
    g_Temperature = ADC_INITIAL_TEMP;
}

/* Periodic function */
void adc_periodic(void)
{    
    //-------------------------------------------
    // Start convertion, wait for result to be ready, convert to Celsius
    //-------------------------------------------
    // ADC conversion result with 1.1 V internal reference    
    g_ADCReading = ADC0_ChannelSelectAndConvert(ADC0_CHANNEL_TEMPSENSE);    
}

/* Returns latest ADC reading */
signed int adc_getADCReading(void)
{
    return g_ADCReading;
}

/* Returns current temperature in Celsius */
signed int adc_getTemperature(void)
{
    //-------------------------------------------
    // See datasheet - Temperature Measurement
    //-------------------------------------------
    volatile int8_t sigrow_offset = 0;
    volatile uint8_t sigrow_gain = 0;
    volatile adc_result_t adc_reading = 0;
    volatile uint32_t temp;
    volatile uint16_t temperature_in_K = 0;
    volatile int16_t temperature_in_C = 0;
    volatile int16_t delta = 0;
    //-------------------------------------------
    // Get last ADC convertion
    adc_reading = g_ADCReading;
    // Read signed value from signature row    
    sigrow_offset = SIGROW.TEMPSENSE1;
    // Read unsigned value from signature row
    sigrow_gain = SIGROW.TEMPSENSE0; 
    // Adjust ADC reading with calibration
    temp = adc_reading - sigrow_offset;
    // Result might overflow 16 bit variable (10bit+8bit)
    temp *= sigrow_gain;
    // Add 1/2 to get correct rounding on division below
    temp += 0x80; 
    // Divide result to get Kelvin 
    temp >>= 8; 
    temperature_in_K = temp;
    // Kelvin to Celsius
    temperature_in_C = temperature_in_K - 273;
    //-------------------------------------------
    // Validate value (min, max, delta)
    //-------------------------------------------
    if((temperature_in_C < ADC_MAX_VALUE) && (temperature_in_C > ADC_MIN_VALUE))
    {
        if(temperature_in_C > g_LastTemperature)
        {
            delta = temperature_in_C - g_LastTemperature;
        }
        else        
        {
            delta = g_LastTemperature - temperature_in_C;
        }
        if(delta > ADC_MAX_DELTA)
        {
            // Do not update temperature for now (could be invalid)
            // Only update last temperature
            g_LastTemperature = temperature_in_C;
        }
        else
        {
            // All is OK: Update last temperature
            // Current temperature is simple average with last valid temperature
            g_LastTemperature = temperature_in_C;
            g_Temperature = g_Temperature + g_LastTemperature;
            g_Temperature = g_Temperature / 2;
        }
    }
    return g_Temperature;
}