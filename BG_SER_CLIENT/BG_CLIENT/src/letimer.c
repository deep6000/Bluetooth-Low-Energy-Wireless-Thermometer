//***********************************************************************************
// Include files
//***********************************************************************************
#include "init_mcu.h"
#include "init_board.h"
#include "ble-configuration.h"
#include "board_features.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"

/* Libraries containing default Gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"
#include "em_core.h"
#include "sleep.h"
#include "em_core.h"
#include "em_emu.h"
#include "src/main.h"
#include "src/gpio.h"
#include "em_letimer.h"  // Peripheral Headers
//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


// function prototypes
//***********************************************************************************


//***********************************************************************************
// functions
//***********************************************************************************
void LETIMER0_IRQHandler(void)
{
 int  Flags;
	CORE_ATOMIC_IRQ_DISABLE();

	Flags=LETIMER_IntGet(LETIMER0);

	if((Flags & LETIMER_IF_UF) == LETIMER_IF_UF)
	{
		//Scheduler |= event_sensor_enable;
		gecko_external_signal(0x0F);
		LETIMER_IntClear(LETIMER0, LETIMER_IFC_UF);
	}

	CORE_ATOMIC_IRQ_ENABLE();
}

/**************************************************************************//**
 * @brief  LETIMER_setup
 * Configures and starts the LETIMER0
 *****************************************************************************/

int prescaler_LETIMER()
{
	int max_ticks , osc_freq, COMP0;
		int counter= 0;
		if(LetimerMode == EM3)
		{
		osc_freq=  ULFRCO;// Ultra low freq osc
		}
		else
		{
		osc_freq= LFXO; // low freq osc

		}

		max_ticks = (period * osc_freq)-1;
		while(max_ticks > 65535 && counter < 15)  // prescaler can be done max 15times
		{
		counter++;
		max_ticks = max_ticks/2;
		}
		COMP0= max_ticks-1; // counter is from 65535 to 0
		CMU->LFAPRESC0 = counter; // load prescalar reg
		return COMP0;

}




void LETIMER_setup(void)
{
int top_counter;
  /* Enable necessary clocks */
   if(LetimerMode == EM3)
		CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);
   else
		CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
   CMU_ClockEnable(cmuClock_CORELE, true);
   CMU_ClockSelectSet(cmuClock_CORELE, true);
   CMU_ClockEnable(cmuClock_LETIMER0, true);
   CMU_ClockEnable(cmuClock_GPIO, true);


   top_counter=prescaler_LETIMER();
   LETIMER_CompareSet(LETIMER0, 0, top_counter); // load comp0
 //  LETIMER_CompareSet(LETIMER0, 1,COMP1);
   while (LETIMER0->SYNCBUSY!=0);  			// wait for	 sync busy

   	   LETIMER_IntClear(LETIMER0, 0x1F);
 /*      LETIMER_IntClear(LETIMER0, LETIMER_IFC_REP0);
       LETIMER_IntClear(LETIMER0, LETIMER_IFC_COMP1);
       LETIMER_IntClear(LETIMER0, LETIMER_IFC_REP1);
       LETIMER_IntClear(LETIMER0, LETIMER_IFC_UF);			// clear all interrupts*/

  // LETIMER_IntEnable(LETIMER0, LETIMER_IEN_COMP0);// enable underflow flag
   LETIMER_IntEnable(LETIMER0, LETIMER_IEN_UF);
   NVIC_EnableIRQ(LETIMER0_IRQn);



  const LETIMER_Init_TypeDef letimerInit =
   {
   .enable         = false,
   .debugRun       = true,                  /* Counter shall not keep running during debug halt. */
   .comp0Top       = true,                   /* Load COMP0 register into CNT when counter underflows. COMP0 is used as TOP */
   .bufTop         = false,                  /* Don't load COMP1 into COMP0 when REP0 reaches 0. */
   .out0Pol        = 0,                      /* Idle value for output 0. */
   .out1Pol        = 0,                      /* Idle value for output 1. */
   .ufoa0          = letimerUFOANone,
   .ufoa1          = letimerUFOANone,
   .repMode        = letimerRepeatFree       /* Count until stopped */
   };
  LETIMER_Init(LETIMER0, &letimerInit);
 // LETIMER_Enable(LETIMER0,true);
}
