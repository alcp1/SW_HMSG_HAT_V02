
/**
  * TCB0 Generated Driver File
  *
  * @file tcb0.c
  *
  * @ingroup tcb0
  *
  * @brief This file contains the API implementation for the TCB0 module driver.
  *
  * @version TCB0 Driver Version 2.0.0
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

/**
 * Section: Included Files
*/

#include "../tcb0.h"

const struct TIMER_INTERFACE Timer0 = {    
    .Initialize = TCB0_Initialize,
    .Deinitialize = TCB0_Deinitialize,
    .Start = TCB0_Start,
    .Stop = TCB0_Stop,
    .PeriodSet = TCB0_PeriodSet,
    .PeriodGet = TCB0_PeriodGet,
    .CounterGet = TCB0_CounterGet,
    .CounterSet = TCB0_CounterSet,
    .MaxCountGet = TCB0_MaxCountGet,
    .TimeoutCallbackRegister = TCB0_CaptureCallbackRegister,
    .Tasks = NULL
};

static void TCB0_DefaultCaptureCallback(void);
static void (*TCB0_CAPT_cb)(void);

void TCB0_Initialize(void)
{
    TCB0.CTRLA &= ~TCB_ENABLE_bm;   // Stop Timer 

    TCB0.CTRLB = (0 << TCB_ASYNC_bp)   // ASYNC disabled
        | (0 << TCB_CCMPEN_bp)   // CCMPEN disabled
        | (0 << TCB_CCMPINIT_bp)   // CCMPINIT disabled
        | (TCB_CNTMODE_INT_gc);  // CNTMODE INT

    TCB0.DBGCTRL = (0 << TCB_DBGRUN_bp);  // DBGRUN disabled

    TCB0.EVCTRL = (0 << TCB_CAPTEI_bp)   // CAPTEI disabled
        | (0 << TCB_EDGE_bp)   // EDGE disabled
        | (0 << TCB_FILTER_bp);  // FILTER disabled

    TCB0.CCMP = 0xC350U;  // CCMP 0xC350

    TCB0.CNT = 0x0;  // CNT 0xNAN

    TCB0.INTFLAGS = (0 << TCB_CAPT_bp);  // CAPT disabled

    //Set default callback for TCB0 capture interrupt
	/* cppcheck-suppress misra-c2012-8.7 */
    TCB0_CaptureCallbackRegister (TCB0_DefaultCaptureCallback);

    TCB0.INTCTRL = (1 << TCB_CAPT_bp);  // CAPT enabled

    TCB0.CTRLA = (TCB_CLKSEL_CLKDIV2_gc)   // CLKSEL CLKDIV2
        | (1 << TCB_ENABLE_bp)   // ENABLE enabled
        | (0 << TCB_RUNSTDBY_bp)   // RUNSTDBY disabled
        | (0 << TCB_SYNCUPD_bp);  // SYNCUPD disabled
}

void TCB0_Deinitialize(void)
{
    TCB0.CTRLA &= ~TCB_ENABLE_bm;   // Stop Timer 
    
    TCB0.CCMP = 0x0;

    TCB0.CNT = 0x0;
    
    TCB0.CTRLB = 0x0;

    TCB0.DBGCTRL = 0x0;

    TCB0.EVCTRL = 0x0;

    TCB0.INTCTRL = 0x0;

    TCB0.TEMP = 0x0;

    TCB0.CTRLA = 0x0;
    
    TCB0.INTFLAGS = ~0x0;
}

void TCB0_Start(void)
{
    TCB0.CTRLA |= TCB_ENABLE_bm; 
}

void TCB0_Stop(void)
{
    TCB0.CTRLA &= ~TCB_ENABLE_bm;
}

void TCB0_CounterSet(uint32_t timerVal)
{
    TCB0.CNT = (uint16_t) timerVal;
}

uint32_t TCB0_CounterGet(void)
{
    uint32_t readVal;

    readVal = (uint32_t) TCB0.CNT;

    return readVal;
}

void TCB0_PeriodSet(uint32_t periodVal)
{
    TCB0.CCMP = (uint16_t) periodVal;
}

uint32_t TCB0_PeriodGet(void)
{
    return (uint32_t)TCB0.CCMP;
}

uint32_t TCB0_MaxCountGet(void)
{
    return (uint32_t) TCB0_MAX_COUNT;
}

/* cppcheck-suppress misra-c2012-8.7 */
void TCB0_CaptureCallbackRegister(void (* CallbackHandler)(void))
{
    TCB0_CAPT_cb = CallbackHandler;
}

static void TCB0_DefaultCaptureCallback(void)
{
    //Add your ISR code here
}

/* cppcheck-suppress misra-c2012-2.7 */
/* cppcheck-suppress misra-c2012-8.2 */
/* cppcheck-suppress misra-c2012-8.4 */
ISR(TCB0_INT_vect)
{
    if (0 != (TCB0.INTFLAGS & TCB_CAPT_bm))
    {
        if ( NULL != TCB0_CAPT_cb)
        {
            (*TCB0_CAPT_cb)();
        }

        TCB0.INTFLAGS = TCB_CAPT_bm;
    }
}
