
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
/* Sampled Bluetooth project was used as reference for this project
 * https://www.silabs.com/community/wireless/bluetooth/knowledge-base.entry.html/2017/04/13/spp-over-ble_c_examp-mnoe
 */

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
#include "retargetserial.h"
#include "retargetserial.h"


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

#include "src/letimer.h"
#include "src/I2C.h"
#include "src/tempsensor.h"
#include "src/Timer0.h"

//***********************************************************************************
// defined files
//***********************************************************************************

#define DISCONNECTED	0
#define SCANNING		1
#define FIND_SERVICE	2
#define FIND_CHAR		3



// SPP service UUID: 4880c12c-fdcb-4077-8920-a450d7f9b907
const uint8 serviceUUID[2] ={0x09,0x18};
const uint8 charUUID[2] = {0x1C,0x2A};

uint8 Bt_address[6]= {0xE1,0x7E,0xA9,0x9F,0xFD,0x90};

//***********************************************************************************
// global variables
//***********************************************************************************




//***********************************************************************************
// Local Variables
//***********************************************************************************
static uint8_t connHandle ;
static uint8_t currentState = DISCONNECTED;
static uint32_t serviceHandle;
static uint16_t char_handle;


//***********************************************************************************
// function prototypes
//***********************************************************************************
static int process_scan_response(struct gecko_msg_le_gap_scan_response_evt_t *pResp)
{
	// decoding advertising packets is done here. The list of AD types can be found
	// at: https://www.bluetooth.com/specifications/assigned-numbers/Generic-Access-Profile

    int i = 0;
    int ad_match_found = 0;
	int ad_len;
    int ad_type;

    while (i < (pResp->data.len - 1))
    {

        ad_len  = pResp->data.data[i];
        ad_type = pResp->data.data[i+1];
        printf("address length=%d\n",ad_len);
        printf("address_type=%d\n",ad_type);

        if (ad_type == 0x02 || ad_type == 0x03)
        {
        	printf("Entered Address Type\n");

        	if(memcmp(serviceUUID, &(pResp->data.data[i+2]),2) == 0)
        	{
        		printf("Found HTP service\r\n");
        		ad_match_found = 1;
        	}
        }

        //jump to next AD record
        i = i + ad_len + 1;
    }
    printf(" ad_match %d\n",ad_match_found);
    return(ad_match_found);
}

//***********************************************************************************
// main
//***********************************************************************************
int main(void)
{

	struct gecko_msg_system_get_bt_address_rsp_t *btAddrRsp;
	bd_addr *ServiceAddr;
	bd_addr Address;
	uint32_t data_byte;
	uint8_t btAddr[17];
	uint8_t Data[4];
	float Data_Float;
    char data_char[10], temp, passkey[6];


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

  /*UART INIT*/
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(true);


  LCD_init("BLE Client");
  /* Saving Server Address to use in cmd_open*/
  ServiceAddr = &Address;
  for(int i =0; i<=6;  i++)
  {
	 ServiceAddr->addr[i] = Bt_address[i];
  }

 while(1)

 	 {
	    /* Event pointer for handling events */
	    struct gecko_cmd_packet* evt;

	    /* Check for stack event. */
	    evt = gecko_wait_event();

	    switch (BGLIB_MSG_ID(evt->header))
	    {

	    case gecko_evt_system_boot_id:



	    	btAddrRsp = gecko_cmd_system_get_bt_address();
	    	LCD_write("BT Address", LCD_ROW_BTADDR1);

	    	for(uint8_t i=0; i<6; i++)
	    	{

	    		sprintf(&(btAddr[(i)*3]), "%02X",btAddrRsp->address.addr[i]);

	    		  /* Add the ':' character to overwrite the null terminator added by sprintf (except on the last iteration */
	    		 if(i<5)
	    		 {
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
	    	gecko_cmd_sm_delete_bondings();
	    	gecko_cmd_sm_configure(0x0F,  sm_io_capability_displayyesno);
	    	/* enable bondable to accommodate certain mobile OS */
	    	gecko_cmd_sm_set_bondable_mode(1);
	    	gecko_cmd_sm_store_bonding_configuration(3,1);

	    	currentState = SCANNING;
	    	struct gecko_msg_le_gap_discover_rsp_t *Discover = gecko_cmd_le_gap_discover(le_gap_discover_generic);
	    	if (Discover->result == 0)
	    	{
	    		//LCD_write("Scanning Started ", LCD_ROW_ACTION );
	    		printf("Scanning Entered\n");
	    		LCD_write("Scanning Devices", LCD_ROW_CONNECTION);
	    	}
	    	else
	    	{
	    		LCD_write("Scanning Error ", LCD_ROW_ACTION );
	    		printf("Scanning error");
	 		}

	    	break;

	    case gecko_evt_le_gap_scan_response_id:
	    	if(process_scan_response(&(evt->data.evt_le_gap_scan_response)) > 0)
	    	{
	    		printf("Device Found");
	    		LCD_write("Device Found ", LCD_ROW_ACTION );
	    	}
	    	else
	    	{
	    	printf("Device Not found!");
	    	}

	    	struct gecko_msg_le_gap_open_rsp_t *pResp;
	    	gecko_cmd_le_gap_end_procedure();
	    	pResp = gecko_cmd_le_gap_connect(Address, public_address,Phy);
	    	// make copy of connection handle for later use (for example, to cancel the connection attempt)
	    	connHandle = pResp->connection;
	    	break;


        /*This event indicates that a new connection was opened,*/
	    case gecko_evt_le_connection_opened_id:
	    	 LCD_write("Connected", LCD_ROW_CONNECTION);
	    	 printf("Connected\n");
			connHandle = evt->data.evt_le_connection_opened.connection;
			gecko_cmd_sm_increase_security(connHandle);
			gecko_cmd_gatt_discover_primary_services_by_uuid(connHandle, UUID_len, serviceUUID);
	    break;

	    case gecko_evt_sm_confirm_passkey_id:
	    	LCD_write("PassKey", LCD_ROW_CONNECTION);
	    	sprintf(&passkey,"%lu", evt->data.evt_sm_confirm_passkey.passkey);
	    	 passkey[6]='\0';
	    	LCD_write(passkey, LCD_ROW_PASSKEY);
	    	LCD_write("PRESS PB0 TO CONFIRM ", LCD_ROW_ACTION );
	    	while(GPIO_PinInGet(gpioPortF, 6) != 0);
	    	gecko_cmd_sm_passkey_confirm(connHandle,1);
	    	break;


	    case  gecko_evt_sm_bonded_id:
	    	LCD_write(" ", LCD_ROW_PASSKEY);
	 //   	bondHandler = evt->data.evt_sm_bonded.bonding;
	    	LCD_write("Bonded", LCD_ROW_CONNECTION);
	    	LCD_write(" ", LCD_ROW_ACTION );
	    	//LETIMER_Enable(LETIMER0,true);
	    break;

	    case gecko_evt_sm_bonding_failed_id:
	    	LCD_write("Bond Fail", LCD_ROW_CONNECTION);
	    	gecko_cmd_endpoint_close(connHandle);
	    	break;


	    case gecko_evt_gatt_service_id:
	    		if (!memcmp(evt->data.evt_gatt_service.uuid.data, serviceUUID, 2))
	    		{
	    			 LCD_write("Service Found", LCD_ROW_ACTION);
	    			 printf("Service handled found\n");
	    			serviceHandle = evt->data.evt_gatt_service.service;
	    			printf("Service handle is\n");

	    			 currentState = FIND_SERVICE;

	    		}
	    			break;


	    case gecko_evt_gatt_procedure_completed_id:

	        	switch(currentState)
	        	{
	        	case FIND_SERVICE:

	        		if (serviceHandle > 0)
	        		{
	        			// Service found, next search for characteristics
	        			printf("Current state Find Service");
	        			gecko_cmd_gatt_discover_characteristics(connHandle, serviceHandle);

	        		}
	        		else
	        		{
	        			// no service found -> disconnect
	        			printf("SPP service not found?\r\n");
	        			gecko_cmd_endpoint_close(connHandle);
	        		}

	        		break;

	        	case FIND_CHAR:
	        		if (char_handle > 0)
	        		{
	        			printf("Current state Find characteristics\n");
	        			// Char found, turn on indications
	        			gecko_cmd_gatt_set_characteristic_notification(connHandle, char_handle, gatt_indication);

	        		}
	        		else
	        		{
	        			// no characteristic found? -> disconnect
	        			printf("SPP char not found?\r\n");
	        			gecko_cmd_endpoint_close(connHandle);
	        		}
	        		break;


	        	default:
	        		break;
	        	}
	        	break;



	    case gecko_evt_gatt_characteristic_id:
	    			currentState= FIND_CHAR;
	        		if(evt->data.evt_gatt_characteristic.uuid.len == 2)
	        		{
	        			if(memcmp(charUUID, evt->data.evt_gatt_characteristic.uuid.data,2) == 0)
	        			{

	        				printf("Char MAtched\n");
	        				char_handle = evt->data.evt_gatt_characteristic.characteristic;
	        			}
	        		}
	       break;

	    case gecko_evt_gatt_characteristic_value_id:

	    	printf("Entered characteristic Value id\n");

    	    if(evt->data.evt_gatt_characteristic_value.att_opcode == 29)
    	    	{
    	    		gecko_cmd_gatt_send_characteristic_confirmation(connHandle);
    	    		printf("attopcode received");

    	    		if(evt->data.evt_gatt_characteristic_value.characteristic == char_handle)
    	    		{
	        	    		for(int j=0;j < evt->data.evt_gatt_server_attribute_value.value.len ; j++)
	        	    		{
	        	    			Data[j]= evt->data.evt_gatt_server_attribute_value.value.data[j];

	        	    		}
	        	    			data_byte= (uint32_t)(BYTES_TO_UINT32(Data[1],Data[2],Data[3],Data[4]));
	        	    			printf("Data in UINT32 is %d\n", data_byte);
	        	    			Data_Float= UINT32_TO_FLT(data_byte);
	        	    			sprintf(&data_char,"%4.2f",Data_Float);
	        	    		    LCD_write("temperature", LCD_ROW_ACTION );
	        	    			LCD_write(data_char, LCD_ROW_TEMPVALUE);
	        	    			printf("Temperature is %f",Data_Float);
	        		}

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
	        //   gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable);
	           gecko_cmd_le_gap_discover(le_gap_discover_generic);
	           LETIMER_Enable(LETIMER0,false);
	           LCD_write("", LCD_ROW_ACTION);
	           LCD_write("", LCD_ROW_TEMPVALUE );
	           LCD_write("Disconnected", LCD_ROW_CONNECTION);
	         }
	         break;

	    }
 	 }
}


