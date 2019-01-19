//***********************************************************************************
// Include files
//***********************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include<math.h>

//***********************************************************************************
// defined files
//***********************************************************************************
/*Professor Keith Grahm's code was used UNIT32_TO_FLT*/
#define UINT32_TO_FLT(data)   (((float)((uint32_t)( data) & 0x00FFFFFFU)) * (float)pow(10, ((int32_t)(data) >> 24)))
#define EM0 0
#define EM1 1
#define EM2 2
#define EM3 3

#define ULFRCO 1000
#define LFXO  32768
#define period 3
//#define LetimerMode EM3




#define slave_addr (0x40<<1)
#define Temp_th 15
#define  public_address 0
#define Phy 1
#define UUID_len 2



//***********************************************************************************
// global variables
//***********************************************************************************
uint32_t Scheduler;
int wait_time;
int LetimerMode;
//***********************************************************************************
// function prototypes
//***********************************************************************************
