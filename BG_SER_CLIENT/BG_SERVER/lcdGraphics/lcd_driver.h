#ifndef LCD_DRIVER_H_
#define LCD_DRIVER_H_

#include "hal-config.h"

#if (HAL_SPIDISPLAY_ENABLE == 1)

#include "bg_types.h"

/**
 *  LCD content can be updated one row at a time using function LCD_write().
 *  Row number is passed as parameter,the possible values are defined below.
 */
#define LCD_ROW_NAME         1    /* 1st row, device name. This row num is used by the header while calling the LCD_init function */
#define LCD_ROW_BTADDR1      2    /* 2nd row, String "BT ADDR" */
#define LCD_ROW_BTADDR2      3    /* 3nd row, BT addr 0-5 bytes ex. "00:0b:57:b5:f2:75" */
#define LCD_ROW_CLIENTADDR   4    /* 4rd row, client address  ex. "Client: f2:75" */
#define LCD_ROW_CONNECTION   5    /* 5rd row, connection status */
#define LCD_ROW_PASSKEY   	 6    /* 6th row, to display passkey while bonding ex. "KEY: {RANDOM_KEY}" */
#define LCD_ROW_ACTION   	 7    /* 7th row, passkey confirm action ex. "Press PB0 to confirm" */
#define LCD_ROW_TEMPVALUE    8    /* 7th row, temp in C */
#define LCD_ROW_MAX          8    /* total number of rows used */

#define LCD_ROW_LEN        32   /* up to 32 characters per each row */

//char *header - a C string that contains the header which is persistent. For ex for a BLE Server -> header = "BLE SERVER"
void LCD_init(char *header);

//char *str  - the C string you want to display in the row number
void LCD_write(char *str, uint8 row);

#endif /* HAL_SPIDISPLAY_ENABLE */

#endif /* LCD_DRIVER_H_ */
