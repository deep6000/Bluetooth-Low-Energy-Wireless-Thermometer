//***********************************************************************************
// Include files
//***********************************************************************************
#include "em_i2c.h"
#include "em_gpio.h"
#include "em_emu.h"
#include "native_gecko.h"
#include "gatt_db.h"
#include "infrastructure.h"
#include "em_timer.h"


#include "src/cmu.h"
#include "src/gpio.h"
#include "src/sleepblock.h"
#include "src/main.h"
#include "src/I2C.h"
#include "src/Timer0.h"
#include "src/tempsensor.h"
#include "bg_types.h"

//***********************************************************************************
// defined files
//***********************************************************************************
#define read_slave  (0x01)
#define write_slave (0x00)
//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//**********************************************************************************

void I2C_setup(void)
 {

	// Select clocks
	 Load_management_on();
	 CMU_ClockEnable(cmuClock_HFPER, true);
	 CMU_ClockEnable(cmuClock_I2C0, true);

	 I2C0->ROUTEPEN  = I2C_ROUTEPEN_SDAPEN | I2C_ROUTEPEN_SCLPEN;
	 I2C0->ROUTELOC0 = ((SCL_locReg << SCL_Route_LOC_shift ) | (SDA_locReg << SDA_Route_LOC_shift)); // ) SCL is at location 16(5:0 in RouteLOC0)

	 I2C_IntClear(I2C0,0x0007FFF);
	 I2C_IntEnable(I2C0,0x0007FFF);



	 const I2C_Init_TypeDef i2cInit =
	 {
	     false,                  /* Enable when initialization done. */
	     true,                  /* Set to master mode. */
	     0,                     /* Use currently configured reference clock. */
	     I2C_FREQ_STANDARD_MAX, /* Set to standard rate assuring being */
	     /*                        within I2C specification. */
	     i2cClockHLRStandard    /* Set to use 4:4 low/high duty cycle. */
	 };


	 I2C_Init(I2C0, &i2cInit);
	 I2C_Enable(I2C0,true);
	 if(I2C0->STATE & I2C_STATE_BUSY)
	 {
	   I2C0->CMD=I2C_CMD_ABORT;
	 }
 }

void I2C_Measure_Temp(void)
 {


	I2C0->CMD = I2C_CMD_START;
	I2C0->TXDATA = (slave_addr)| write_slave ;

	while((I2C0->IF & I2C_IF_ACK)== 0);
	I2C_IntClear(I2C0,I2C_IFC_ACK);  // clear the ack flag

	I2C0->TXDATA = (0xE3);
//	for (i = 0; i <16000;i++);
	while((I2C0->IF & I2C_IF_ACK)== 0);
	I2C_IntClear(I2C0,I2C_IFC_ACK);

	I2C0->CMD = I2C_CMD_START;       // Start bit
	I2C0->TXDATA = (slave_addr)|read_slave;   // slave address to read

	while((I2C0->IF & I2C_IF_ACK)== 0);
	I2C_IntClear(I2C0,I2C_IFC_ACK);
 }


int Receive_temp_code()
 {

		uint8_t MSB_Data = 0;
		uint8_t LSB_Data = 0;
		uint16_t Temp_code = 0;

		while(( I2C0->STATUS & I2C_STATUS_RXDATAV) == 0);
		MSB_Data = I2C0->RXDATA;

		I2C0->CMD = I2C_CMD_ACK;

		while(( I2C0->STATUS & I2C_STATUS_RXDATAV) == 0);
		LSB_Data= I2C0->RXDATA;

		I2C0->CMD=I2C_CMD_NACK;
		I2C0->CMD=I2C_CMD_STOP;

		Temp_code = MSB_Data<<8 | LSB_Data	;
		return Temp_code;
  }

float Temperature_Cal(uint16_t code)
 {
	float Temp_cal=0;
	Temp_cal = (175.72 * code);
	Temp_cal /= 65536;
	Temp_cal -= 46.85;
	return Temp_cal;
 }
