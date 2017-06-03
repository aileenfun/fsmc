/*
 * FILE               : systick.c
 * DESCRIPTION        : This file is systick driver.
 * Author             : ysloveivy
 * Copyright          :
 *
 * History
 * -------------------
 * Rev                : 0.00
 * Date               : 11/21/2015
 *
 * create.
 * -------------------
 */
//--------------------------- Include ---------------------------//
#include "..\include\systick.h"
#include "..\fwlib\inc\stm32f4xx_syscfg.h"

//--------------------- Function Prototype ----------------------//
static int initialize(void);

//--------------------------- Variable --------------------------//
SYSTICK_T systick = {
	.initialize = initialize,
	.ms_flag=0,
	.second_flag = 0
};
//--------------------------- Function --------------------------//
/*
 * Name               : initialize
 * Description        : ---
 * Author             : ysloveivy.
 *
 * History
 * -------------------
 * Rev                : 0.00
 * Date               : 11/21/2015
 * 
 * create.
 * -------------------
 */
static int initialize(void)
{
	//¶¨Ê±1ms
	if(SysTick_Config(SystemCoreClock / 1000)){
		while(1);
	}
	return 0;
}
/*
 * Name               : SysTick_Handler
 * Description        : ---
 * Author             : ysloveivy.
 *
 * History
 * -------------------
 * Rev                : 0.00
 * Date               : 11/21/2015
 * 
 * create.
 * -------------------
 */
void SysTick_Handler(void)
{
	systick.counter=0;
	systick.ms_flag=1;
	if((systick.counter ++ % 1000) == 0){
		systick.second_flag = 1;
	}
}
