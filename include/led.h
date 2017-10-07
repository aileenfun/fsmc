/*
 * FILE                : led.h
 * DESCRIPTION         : This file is for led.c
 * Author              : ysloveivy
 * Copyright           : 
 *
 * History
 * --------------------
 * Rev                 : 0.00
 * Date                : 01/03/2016
 *
 * create.
 * --------------------
 */
#ifndef __led_h__
#define __led_h__

//--------------------------- Define ---------------------------//
//ºìµÆ<----->PI5
#define LED_RED_OFF GPIO_SetBits(GPIOA,GPIO_Pin_9)
#define LED_RED_ON  GPIO_ResetBits(GPIOA,GPIO_Pin_9)
//ÂÌµÆ<----->PI6
#define LED_GREEN_OFF GPIO_SetBits(GPIOA,GPIO_Pin_10)
#define LED_GREEN_ON  GPIO_ResetBits(GPIOA,GPIO_Pin_10)
//À¶µÆ<----->PI7
#define LED_BLUE_OFF GPIO_SetBits(GPIOI,GPIO_Pin_7)
#define LED_BLUE_ON  GPIO_ResetBits(GPIOI,GPIO_Pin_7)

#define PW_GREEN_ON GPIO_ResetBits(GPIOF,GPIO_Pin_7)
#define PW_ORG_ON GPIO_ResetBits(GPIOF,GPIO_Pin_8)
#define GET_PW_GREEN_STATUS GPIO_ReadOutputDataBit(GPIOF,GPIO_Pin_7)
#define GET_PW_ORG_STATUS GPIO_ReadOutputDataBit(GPIOF,GPIO_Pin_8)
#define PW_ORG_TOGGLE GPIO_ToggleBits(GPIOF,GPIO_Pin_8)

#define PW_GREEN_OFF GPIO_SetBits(GPIOF,GPIO_Pin_7)
#define PW_ORG_OFF GPIO_SetBits(GPIOF,GPIO_Pin_8)//org

#define BATCHARGE_INPUT  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)
//----------------------- Include files ------------------------//

//-------------------------- Typedef----------------------------//
typedef struct {
	int (* initialize)(void);
}LED_T;

//--------------------------- Extern ---------------------------//
extern LED_T led;

#endif //__led_h__
