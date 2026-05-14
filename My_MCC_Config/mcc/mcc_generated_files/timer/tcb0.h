/**
 * TCB0 Generated Driver API Header File
 *
 * @file tcb0.h
 *
 * @ingroup timerdriver
 *
 * @brief This file contains the API prototypes and other data types for the TCB0 Timer driver.
 *
 * @version TCB0 Timer Driver Version 2.0.0
 * 
 * @version Package Version 6.0.0
 */
/*
© [2026] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

#ifndef TCB0_H
#define TCB0_H

#include <stdbool.h>
#include <stdint.h>
#include "../system/utils/compiler.h"
#include "timer_interface.h"

/**
 * @misradeviation{@advisory,2.5}
 * MCC Melody drivers provide macros that can be added to an application. 
 * It depends on the application whether a macro is used or not. 
 */

/**
 * @ingroup timerdriver
 * @brief Defines the maximum count of the timer
 */
#define TCB0_MAX_COUNT (65535U)
/**
 * @ingroup timerdriver
 * @brief Defines the TCB0 frequency in hertz
 */
 /* cppcheck-suppress misra-c2012-2.5 */
#define TCB0_CLOCK_FREQ (2500000UL)
/**
 * @ingroup timerdriver
 * @brief Defines the Custom Name for the \ref TCB0_MAX_COUNT.
 */
/* cppcheck-suppress misra-c2012-2.5 */
#define TIMER0_MAX_COUNT TCB0_MAX_COUNT
/**
 * @ingroup timerdriver
 * @brief Defines the Custom Name for the \ref TCB0_CLOCK_FREQ.
 */
/* cppcheck-suppress misra-c2012-2.5 */
#define TIMER0_CLOCK_FREQ TCB0_CLOCK_FREQ
/**
 * @ingroup timerdriver
 * @brief Defines the Custom Name for the \ref TCB0_Initialize API
 */
 /* cppcheck-suppress misra-c2012-2.5 */
#define Timer0_Initialize TCB0_Initialize
/**
 * @ingroup timerdriver
 * @brief Defines the Custom Name for the \ref TCB0_Deinitialize API
 */
 /* cppcheck-suppress misra-c2012-2.5 */
#define Timer0_Deinitialize TCB0_Deinitialize
/**
 * @ingroup timerdriver
 * @brief Defines the Custom Name for the \ref TCB0_Start API
 */
 /* cppcheck-suppress misra-c2012-2.5 */
#define Timer0_Start TCB0_Start
/**
 * @ingroup timerdriver
 * @brief Defines the Custom Name for the \ref TCB0_Stop API
 */
 /* cppcheck-suppress misra-c2012-2.5 */
#define Timer0_Stop TCB0_Stop
/**
 * @ingroup timerdriver
 * @brief Defines the Custom Name for the \ref TCB0_CounterGet API
 */
 /* cppcheck-suppress misra-c2012-2.5 */
#define Timer0_CounterGet TCB0_CounterGet
/**
 * @ingroup timerdriver
 * @brief Defines the Custom Name for the \ref TCB0_CounterSet API
 */
 /* cppcheck-suppress misra-c2012-2.5 */
#define Timer0_CounterSet TCB0_CounterSet
/**
 * @ingroup timerdriver
 * @brief Defines the Custom Name for the \ref TCB0_PeriodSet API
 */
 /* cppcheck-suppress misra-c2012-2.5 */
#define Timer0_PeriodSet TCB0_PeriodSet
/**
 * @ingroup timerdriver
 * @brief Defines the Custom Name for the \ref TCB0_PeriodGet API
 */
 /* cppcheck-suppress misra-c2012-2.5 */
#define Timer0_PeriodGet TCB0_PeriodGet
/**
 * @ingroup timerdriver
 * @brief Defines the Custom Name for the \ref TCB0_MaxCountGet API
 */
 /* cppcheck-suppress misra-c2012-2.5 */
#define Timer0_MaxCountGet TCB0_MaxCountGet
/**
 * @ingroup timerdriver
 * @brief Defines the Custom Name for the \ref TCB0_CaptureCallbackRegister API
 */
 /* cppcheck-suppress misra-c2012-2.5 */
#define Timer0_CaptureCallbackRegister TCB0_CaptureCallbackRegister

/**
 * @ingroup timerdriver
 * @struct TIMER_INTERFACE
 * @brief This is an instance of TIMER_INTERFACE for the TIMER module
 */
extern const struct TIMER_INTERFACE Timer0;

/**
 * @ingroup timerdriver
 * @brief Initializes the TCB0 module.
 *        This routine must be called before any other TCB0 routines.
 * @param None.
 * @return None.
 */ 
 void TCB0_Initialize(void);

/**
 * @ingroup timerdriver
 * @brief    Deinitializes the TCB0 to POR values
 * @param    None.
 * @return   None.
 */
void TCB0_Deinitialize(void);

/**
 * @ingroup timerdriver
 * @brief Starts the TCB0.
 * @pre Initialize TCB0 with TCB0_Initialize() before calling this API.
 * @param None.
 * @return None.
 */
void TCB0_Start(void);

/**
 * @ingroup timerdriver
 * @brief Stops the TCB0.
 * @pre Initialize TCB0 with TCB0_Initialize() before calling this API.
 * @param None.
 * @return None.
 */
void TCB0_Stop(void);

/**
 * @ingroup timerdriver
 * @brief Writes the counter value to the CNT register.
 * @pre Initialize TCB0 with TCB0_Initialize() before calling this API.
 * @param counterValue - Counter value to be written to the CNT register
 * @return None.
 */
void TCB0_CounterSet(uint32_t CounterValue);

/**
 * @ingroup timerdriver
 * @brief Reads the counter value from the CNT register
 * @pre Initialize TCB0 with TCB0_Initialize() before calling this API.
 * @param None.
 * @return Counter value from the CNT register
 */
uint32_t TCB0_CounterGet(void);

/**
 * @ingroup timerdriver
 * @brief Loads the 16-bit value to the timerTCB0ReloadVal variable.
 * @pre Initialize TCB0 with TCB0_Initialize() before calling this API.
 * @param periodCount - Period count value 
 * @return None.
 */
void TCB0_PeriodSet(uint32_t periodCount);

/**
 * @ingroup timerdriver
 * @brief Returns the TCB0 period count value.
 * @pre Initialize TCB0 with TCB0_Initialize() before calling this API.
 * @param None.
 * @return TCB0 period count value
 */
uint32_t TCB0_PeriodGet(void);

/**
 * @ingroup timerdriver
 * @brief Returns the TCB0 maximum timer count value.
 * @param None.
 * @return TCB0 maximum count value
 */
uint32_t TCB0_MaxCountGet(void);

/**
 * @ingroup timerdriver
 * @brief Setter function for the TCB0 overflow callback.
 * @param void (* CallbackHandler)(void) - Pointer to the custom callback
 * @return None.
 */
 void TCB0_CaptureCallbackRegister(void (* CallbackHandler)(void));


#endif // TCB0_H