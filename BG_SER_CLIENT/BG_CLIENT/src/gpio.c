//***********************************************************************************
// Include files
//***********************************************************************************
#include "src/gpio.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************


//***********************************************************************************
// functions
//***********************************************************************************
void gpio_init(void){

	// Set LED ports to be standard output drive with default off (cleared)
	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthStrongAlternateStrong);
//	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, LED0_default);

	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthStrongAlternateStrong);
//	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, LED1_default);
	//GPIO_PinOutClear(LED0_port, LED0_pin);

	// Set LED ports to be standard output drive with default off (cleared)
	GPIO_DriveStrengthSet(gpioPortD, gpioDriveStrengthStrongAlternateStrong);
//	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthWeakAlternateWeak);

	GPIO_PinModeSet(gpioPortD, 15, gpioModePushPull, 0);
	GPIO_DriveStrengthSet(PB0_port, gpioDriveStrengthStrongAlternateStrong);
	    GPIO_PinModeSet(PB0_port, PB0_pin, gpioModeInput, PB0_default);
}
