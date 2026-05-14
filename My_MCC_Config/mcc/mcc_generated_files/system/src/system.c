/**
 * System Driver Source File
 * 
 * @file system.c
 * 
 * @ingroup systemdriver
 * 
 * @brief This file contains the API implementation for the System driver.
 *
 * @version Driver Version 1.0.2
 *
 * @version Package Version 3.1.7
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
   Section: Included Files
 */
#include "../system.h"

/**
 * Initializes MCU, drivers and middleware in the project
**/

int8_t BOD_Initialize();

int8_t WDT_Initialize();

static void (*bod_vlm_callback)(void) = NULL;

void SYSTEM_Initialize(void)
{
    CLOCK_Initialize();
    
    PIN_MANAGER_Initialize();
    ADC0_Initialize();
    TCA0_Initialize();
    TCB0_Initialize();
    BOD_Initialize();
    I2C0_Client_Initialize();
    NVM_Initialize();
    VREF_Initialize();
    WDT_Initialize();
    CPUINT_Initialize();
}

int8_t BOD_Initialize()
{
    //SLEEP Enabled; 
    ccp_write_io((void*)&(BOD.CTRLA),0x5);
    //
    BOD.CTRLB = 0x2;
    //VLMCFG Interrupt when supply goes below VLM level; VLMIE disabled; 
    BOD.INTCTRL = 0x0;
    //VLMIF disabled; 
    BOD.INTFLAGS = 0x0;
    //
    BOD.STATUS = 0x0;
    //VLMLVL VLM threshold 5% above BOD level; 
    BOD.VLMCTRLA = 0x0;

    return 0;
}

void BOD_VLM_Set_Callback(void (*handler)(void))
{
	if(handler != NULL)
    {
		bod_vlm_callback = handler;
	}
}

ISR(BOD_VLM_vect)
{
	if(bod_vlm_callback != NULL)
    {
        bod_vlm_callback();
    }

	/* The interrupt flag has to be cleared manually */
	BOD.INTFLAGS = BOD_VLMIE_bm;
}
int8_t WDT_Initialize()
{
    //PERIOD Watch-Dog timer Off; WINDOW Window mode off; 
    ccp_write_io((void*)&(WDT.CTRLA),0x0);
    //LOCK enabled; 
    ccp_write_io((void*)&(WDT.STATUS),0x80);

    return 0;
}


