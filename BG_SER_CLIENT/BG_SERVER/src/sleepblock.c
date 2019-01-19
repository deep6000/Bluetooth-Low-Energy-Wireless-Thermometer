//***********************************************************************************
// Include files
//***********************************************************************************
#include "src/sleepblock.h"
#include "em_core.h"
#include "em_emu.h"
#include "bg_types.h"

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

void  blockSleepMode( uint8 minimumMode)
{

	CORE_ATOMIC_IRQ_DISABLE();

	   sleep_block[minimumMode]++;
	CORE_ATOMIC_IRQ_ENABLE();

}

void  unblockSleepMode( uint8 minimumMode )
{
	CORE_ATOMIC_IRQ_DISABLE();
	    if(sleep_block[minimumMode]>0)
	    {
	        sleep_block[minimumMode]--;
	    }
	    CORE_ATOMIC_IRQ_ENABLE();

}
void sleep_mode()
{
	if(sleep_block[0]>0)
{
    return;
}

else if(sleep_block[1]>0)
{
    EMU_EnterEM1();
}
else if(sleep_block[2]>0)
{
    EMU_EnterEM2(true);
}
else
{
    EMU_EnterEM3(true);
}
return;
	}
