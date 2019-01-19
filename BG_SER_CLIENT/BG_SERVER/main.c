
/***********************************************************************************************//**
 * \file   main.c
 * \brief  Silicon Labs Empty Example Project
 *
 * This example demonstrates the bare minimum needed for a Blue Gecko C application
 * that allows Over-the-Air Device Firmware Upgrading (OTA DFU). The application
 * starts advertising after boot and restarts advertising after a connection is closed.
 ***************************************************************************************************
 * <b> (C) Copyright 2016 Silicon Labs, http://www.silabs.com</b>
 ***************************************************************************************************
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 **************************************************************************************************/
/*A bluetooth server sample code  by Silicon Labs was used as a reference for this code*/

/* Board headers */
#include<stdlib.h>
#include<stdio.h>
#include "init_mcu.h"
#include "init_board.h"
#include "ble-configuration.h"
#include "board_features.h"
#include "infrastructure.h"
#include "lcd_driver.h"


/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"

/* Libraries containing default Gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"
#include "em_core.h"
#include "sleep.h"
#include "em_letimer.h"
#include "em_timer.h"
/* Device initialization header */
#include "hal-config.h"

#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#else
#include "bspconfig.h"
#endif

/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup app
 * @{
 **************************************************************************************************/

#ifndef MAX_CONNECTIONS
#define MAX_CONNECTIONS 4
#endif
uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS)];

// Gecko configuration parameters (see gecko_configuration.h)
static const gecko_configuration_t config = {
  .config_flags = 0,
  .sleep.flags = SLEEP_FLAGS_DEEP_SLEEP_ENABLE,
  .bluetooth.max_connections = MAX_CONNECTIONS,
  .bluetooth.heap = bluetooth_stack_heap,
  .bluetooth.heap_size = sizeof(bluetooth_stack_heap),
  .bluetooth.sleep_clock_accuracy = 100, // ppm
  .gattdb = &bg_gattdb_data,
  .ota.flags = 0,
  .ota.device_name_len = 3,
  .ota.device_name_ptr = "OTA",
#if (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
  .pa.config_enable = 1, // Enable high power PA
  .pa.input = GECKO_RADIO_PA_INPUT_VBAT, // Configure PA input to VBAT
#endif // (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
};

// Flag for indicating DFU Reset must be performed
uint8_t boot_to_dfu = 0;
//***********************************************************************************
// Include files
//***********************************************************************************

#include "src/main.h"
#include "src/gpio.h"
#include "src/cmu.h"
#include "src/sleepblock.h"
#include "src/letimer.h"
#include "src/I2C.h"
#include "src/tempsensor.h"
#include "src/Timer0.h"

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



//***********************************************************************************
// main
//***********************************************************************************


void temperatureMeasure()
{
  uint16_t Temperature_code=0;
  uint8_t htmTempBuffer[5]; /* Stores the temperature data in the Health Thermometer (HTM) format. */
  uint8_t flags = 0x00;   /* HTM flags set as 0 for Celsius, no time stamp and no temperature type. */
  float TEMPERATURE_F;   /* Stores the temperature data read from the sensor in the correct format */
  uint8_t *p = htmTempBuffer; /* Pointer to HTM temperature buffer needed for converting values to bitstream. */
  char temperature1[10];
  uint32_t temperature;
  /* Convert flags to bitstream and append them in the HTM temperature data buffer (htmTempBuffer) */

  	  UINT8_TO_BITSTREAM(p, flags);

     EMU_EnterEM1();
     sensor_enable();
     TIMER_Enable(TIMER0,true);
     while(wait_time == 0);
     wait_time=0;
  	 I2C_setup();
  	 I2C_Measure_Temp();
  	 Temperature_code=Receive_temp_code();
  	TEMPERATURE_F= Temperature_Cal(Temperature_code);
  	 if(TEMPERATURE_F < Temp_th)
  	 {
  		GPIO_PinOutSet(LED1_port, LED1_pin);
  	 }
  	 else
  	 {
  		GPIO_PinOutClear(LED1_port, LED1_pin);
  	 }
  // Load_management_off();
    /* Convert temperature to bitstream and place it in the HTM temperature data buffer (htmTempBuffer) */
  	temperature = (uint32_t)FLT_TO_UINT32(TEMPERATURE_F*1000,-3);
  	 UINT32_TO_BITSTREAM(p, temperature);
    /* Send indication of the temperature in htmTempBuffer to all "listening" clients.
    0xFF is to send all clients */
    gecko_cmd_gatt_server_send_characteristic_notification(0xFF, gattdb_temperature_measurement, 5, htmTempBuffer);
   // temperature1[3]='\0';
    LCD_write("temperature", LCD_ROW_ACTION );
    sprintf(&temperature1,"%4.2f",TEMPERATURE_F);
    LCD_write(temperature1, LCD_ROW_TEMPVALUE );

	}



int main(void)
{
	uint8 con_handle=0, evt_temp_read=0, status_rssi=0;
	int8 rssi_power=0;
	int16 tx_power;
	uint8_t btAddr[17];
	uint8_t cl_btAddr[17];
	char temp;
	  uint8_t bondHandler =0xFF ;
	struct gecko_msg_system_get_bt_address_rsp_t *btAddrRsp;
	//uint32 pass_key=0;
	char passkey[6];
	bd_addr Cl_addr;



  // Initialize device
  initMcu();
  // Initialize board
  initBoard();

  /* Initialize GPIO */
  gpio_init();

  // Initialize clocks
  cmu_init();

  // Initialize stack
  gecko_init(&config);


  /* Initialize LETIMER */
  	 LetimerMode = EM3;
  	 LETIMER_setup();
  	 TIMER_setup();
  	LCD_init("BLE Server");

 while(1)

 	 {
	    /* Event pointer for handling events */
	    struct gecko_cmd_packet* evt;

	    /* Check for stack event. */
	    evt = gecko_wait_event();

	    switch (BGLIB_MSG_ID(evt->header))
	    {

	    case gecko_evt_system_boot_id:

	    	gecko_cmd_system_halt(1);
	    	gecko_cmd_system_set_tx_power(0);
	    	gecko_cmd_system_halt(0);

	    	btAddrRsp = gecko_cmd_system_get_bt_address();
	    	LCD_write("BT Address", LCD_ROW_BTADDR1);


	    	for(uint8_t i=0; i<6; i++)
	    	{

	    		sprintf(&(btAddr[(i)*3]), "%02X",btAddrRsp->address.addr[i]);

	    		  /* Add the ':' character to overwrite the null terminator added by sprintf (except on the last iteration */
	    		  if(i<5) {
	    			  btAddr[i*3+2] = ':';// adding : at every 3rd byte
	    		  }
	    	  }
	    	for(int i=0;i<8;)
	    	               {
	    						temp = btAddr[i];
	    	                   btAddr[i] = btAddr[15-i];
	    	                   btAddr[15-i] = temp;
	    	                   temp = btAddr[i+1];
	    	                   btAddr[i+1] = btAddr[15-i+1];
	    	                   btAddr[15-i+1] = temp;
	    	                   i+=3;
	    	               }
	    	LCD_write(btAddr, LCD_ROW_BTADDR2);

	    	/*Delete All Bondings*/
	      	gecko_cmd_sm_delete_bondings();
	    	gecko_cmd_sm_configure(0x0F,  sm_io_capability_displayyesno);
	    	 /* enable bondable to accommodate certain mobile OS */
	    	 gecko_cmd_sm_set_bondable_mode(1);
	    	 gecko_cmd_sm_store_bonding_configuration(3,1);
	    	 LCD_write("Advertising", LCD_ROW_CONNECTION);

	    	 gecko_cmd_le_gap_set_advertise_timing(0, Min_adv_interval, Max_adv_interval, 0, 0);
	    	/* Start general advertising and enable connections. */
	    	gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable);
            break;


        /*This event indicates that a new connection was opened,*/
	    case gecko_evt_le_connection_opened_id:
	    	 LCD_write("Connected", LCD_ROW_CONNECTION);
	    	 for(int i =0; i<6; i++)
	    	Cl_addr.addr[i]= evt->data.evt_le_connection_opened.address.addr[i];

	    	 for(uint8_t i=0; i<2; i++)
	    	 	    	{

	    	 	    		sprintf(&(cl_btAddr[(i)*3]), "%02X",Cl_addr.addr[i]);

	    	 	    		  /* Add the ':' character to overwrite the null terminator added by sprintf (except on the last iteration */
	    	 	    		  if(i<1)
	    	 	    		  {
	    	 	    			  cl_btAddr[i*3+2] = ':';// adding : at every 3rd byte
	    	 	    		  }
	    	 	    	  }
	    	 for (int i =0; i< 2;i++)
	    	 {
	    				 temp = cl_btAddr[i];
	    	 	 	 	 cl_btAddr[i]= cl_btAddr[i+3];
	    	 	 	 	cl_btAddr[i+3]=temp;
	    	 }
	    		    	LCD_write(cl_btAddr, LCD_ROW_CLIENTADDR);
	    	 // get value of connection_ handle

	    	con_handle =evt->data.evt_le_connection_opened.connection;
	    	// Slave Latency is 300/75=4 , Connection interval = 75ms and timeout >75
	    	gecko_cmd_le_connection_set_parameters(con_handle,Max_interval,Min_interval,slave_latency,Timeout);
	   // 	gecko_cmd_sm_bonding_confirm(con_handle,1);
	    	gecko_cmd_sm_increase_security(con_handle);
	    	if(bondHandler != 0xff)
	    	{
	    		LETIMER_Enable(LETIMER0,true);
	    	}

	    	break;


	    case gecko_evt_sm_confirm_passkey_id:
	    	LCD_write("PassKey", LCD_ROW_CONNECTION);
	    	sprintf(&passkey,"%lu", evt->data.evt_sm_confirm_passkey.passkey);
	    	 passkey[6]='\0';
	    	LCD_write(passkey, LCD_ROW_PASSKEY);
	    	LCD_write("PRESS PB0 TO CONFIRM ", LCD_ROW_ACTION );
	    	while(GPIO_PinInGet(gpioPortF, 6) != 0);
	    	gecko_cmd_sm_passkey_confirm(con_handle,1);
	    	break;


	    case  gecko_evt_sm_bonded_id:
	    	LCD_write(" ", LCD_ROW_PASSKEY);
	    	LCD_write(" ", LCD_ROW_ACTION );
	    	bondHandler = evt->data.evt_sm_bonded.bonding;
	    	LCD_write("Bonded", LCD_ROW_CONNECTION);
	    	LETIMER_Enable(LETIMER0,true);
	    break;

	    case gecko_evt_sm_bonding_failed_id:
	    	LCD_write("Bond Fail", LCD_ROW_CONNECTION);
	    	gecko_cmd_endpoint_close(con_handle);
	    	break;


	    case gecko_evt_le_connection_rssi_id:
	    	// get status of the command execution
	    	status_rssi=evt->data.evt_le_connection_rssi.status;
	    	if(status_rssi==0x00) // 0x00 is completed
	    	{
	    		// Get RSSI
	    		rssi_power=evt->data.evt_le_connection_rssi.rssi;
	    	}
	    	if(rssi_power >= 35)
	    	{
	    		tx_power=-300;
	    	}

	    	else if(rssi_power > -45 && rssi_power < -35)
	    	{
	    		tx_power=-200;
	    	}

	    	else if(rssi_power > -55 && rssi_power < -45)
	    	{
	   			tx_power=-150;
	    	}
	    	else if(rssi_power > -65 && rssi_power < -55)
	    	{
	   			tx_power=-50;
	    	}
	    	else if(rssi_power > -75 && rssi_power < -65)
	    	{
	    		tx_power=0;
	    	}
	    	else if(rssi_power > -85 && rssi_power < -75)
	    	{
	    		tx_power=50;
	    	}
	    	else if(rssi_power <= -85)
	   		{
	   			tx_power=80;
	    	}
	    	// halt the process to set TX power
    		gecko_cmd_system_halt(1);
    		gecko_cmd_system_set_tx_power(tx_power);
    		gecko_cmd_system_halt(0);

	    		break;



	    case gecko_evt_system_external_signal_id:

	    	evt_temp_read = evt->data.evt_system_external_signal.extsignals;

	    	if( evt_temp_read == 0x0F)
	    	{

	    		temperatureMeasure();

	    		gecko_cmd_le_connection_get_rssi(con_handle);
	    	}
	    	break;


	    case gecko_evt_le_connection_closed_id:
	         /* Check if need to boot to dfu mode */
	         if (boot_to_dfu)
	         {

	           /* Enter to DFU OTA mode */
	           gecko_cmd_system_reset(2);
	           // on reset Tx power is set to 0
	    		gecko_cmd_system_halt(1);
	    		gecko_cmd_system_set_tx_power(0);
	    		gecko_cmd_system_halt(0);
	         }
	         else
	         {

	           /* Restart advertising after client has disconnected */
	           gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable);
	           LETIMER_Enable(LETIMER0,false);
	           LCD_write("", LCD_ROW_ACTION);
	           LCD_write("", LCD_ROW_TEMPVALUE );
	           LCD_write("Disconnected", LCD_ROW_CONNECTION);
	         }
	         break;

	    }
 	 }
}


