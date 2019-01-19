//***********************************************************************************
// Include files
//***********************************************************************************

#include "em_core.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "bg_types.h"
#include "src/tempsensor.h"
#include "src/main.h"
#include "src/I2C.h"


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

void sensor_enable()
{
	 GPIO_PinModeSet(gpioPortD, 15, gpioModePushPull, 1);
}

void sensor_disable()
{
	GPIO_PinModeSet(gpioPortD, 15, gpioModePushPull, 0);
}

void Load_management_on()
{
	int j=0;
//	 sensor_enable();
	// for (i = 0; i <160000;i++); 							// 80ms sensor time
	 GPIO_PinModeSet(gpioPortC, 10, gpioModeWiredAnd, 1);   // SCL
	 GPIO_PinModeSet(gpioPortC, 11, gpioModeWiredAnd, 1);   // SDA


	 // toggle SCL pin 9 times to reset any I2C slave that may require it
	 for ( j = 0; j < 9; j++)
	 {
		GPIO_PinModeSet(gpioPortC, 10, gpioModeWiredAnd, 0);
	    GPIO_PinModeSet(gpioPortC, 10, gpioModeWiredAnd, 1);
	 }

}

void Load_management_off()
{
	 GPIO_PinModeSet(gpioPortC, 10, gpioModeDisabled, 0);   // disable SCL
	 GPIO_PinModeSet(gpioPortC, 11, gpioModeDisabled, 0);   // disable SDA
	 sensor_disable();
}
