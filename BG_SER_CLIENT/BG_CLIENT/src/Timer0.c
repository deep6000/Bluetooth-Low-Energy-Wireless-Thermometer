//***********************************************************************************
// Include files
//***********************************************************************************
#include "em_core.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_timer.h"
#include "bg_types.h"
#include "src/tempsensor.h"
#include "src/main.h"
#include "src/I2C.h"
#include"src/Timer0.h"


//***********************************************************************************
// defined files
//***********************************************************************************
/* 47500 Hz -> 19Mhz (clock frequency)*0.08 / 32 (prescaler)*/
#define TOP 47500
//***********************************************************************************
// global variables
//***********************************************************************************



// function prototypes
//***********************************************************************************


//***********************************************************************************
// functions
//***********************************************************************************


void TIMER0_IRQHandler(void)

{
	int Flags;
	  CORE_ATOMIC_IRQ_DISABLE();
	Flags=TIMER_IntGet(TIMER0);
	  if((Flags & TIMER_IF_OF) == TIMER_IF_OF)
	  {
	  wait_time=1;
	  TIMER0->CMD = TIMER_CMD_STOP;
	  TIMER_IntClear(TIMER0, TIMER_IF_OF);
	  }
	  CORE_ATOMIC_IRQ_ENABLE();
}
	void TIMER_setup(void)
	{
	 //  CMU_ClockEnable(cmuClock_HFPER, true);
	 //  CMU_ClockEnable(cmuClock_CORELE, true);
	 // CMU_ClockSelectSet(cmuClock_CORELE, true);
	   CMU_ClockEnable(cmuClock_TIMER0, true);
	  // CMU_ClockEnable(cmuClock_GPIO, true);

	   TIMER_Init_TypeDef timerInit =
	    {
	      .enable     = false,
	      .debugRun   = true,
	      .prescale   = timerPrescale32,
	      .clkSel     = timerClkSelHFPerClk,
	      .fallAction = timerInputActionNone,
	      .riseAction = timerInputActionNone,
	      .mode       = timerModeUp,
	      .dmaClrAct  = false,
	      .quadModeX4 = false,
	      .oneShot    = false,
	      .sync       = false,
	    };

	   TIMER_IntClear(TIMER0,0xFF7);
	   TIMER_IntEnable(TIMER0, TIMER_IF_OF);
	   NVIC_EnableIRQ(TIMER0_IRQn);
	   TIMER_TopSet(TIMER0, TOP);
	   TIMER_Init(TIMER0, &timerInit);

	}
