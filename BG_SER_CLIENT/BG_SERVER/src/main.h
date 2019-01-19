//***********************************************************************************
// Include files
//***********************************************************************************

#include <stdint.h>
#include <stdbool.h>

//***********************************************************************************
// defined files
//***********************************************************************************
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

#define Max_interval 12
#define Min_interval 12
#define slave_latency 4
#define Min_adv_interval 400
#define Max_adv_interval 400
#define Timeout 80
//***********************************************************************************
// global variables
//***********************************************************************************
uint32_t Scheduler;
int wait_time;
int LetimerMode;
//***********************************************************************************
// function prototypes
//***********************************************************************************
