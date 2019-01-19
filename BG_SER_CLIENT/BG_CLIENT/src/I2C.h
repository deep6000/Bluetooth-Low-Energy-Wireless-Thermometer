//***********************************************************************************
// Include files
//***********************************************************************************
#include "main.h"
#include "em_cmu.h"

//***********************************************************************************
// defined files
//***********************************************************************************
#define SCL_locReg	 			14   //  SCL location register
#define SDA_locReg 				16   // SDA location Register
#define SCL_Route_LOC_shift 	8 	 // (13:8 in RouteLOC0)
#define SDA_Route_LOC_shift 	0    //



//***********************************************************************************
// global variables
//***********************************************************************************

//***********************************************************************************
// function prototypes
//***********************************************************************************
void I2C_setup(void);
void I2C_driver(void);
void I2C_Measure_Temp(void);
int Receive_temp_code();
float Temperature_Cal(uint16_t code);
void sensor_enable();
void sensor_disable();
void temperatureMeasure();
