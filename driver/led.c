/*
 * FILE                : led.c
 * DESCRIPTION         : This file is led driver.
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
//--------------------------- Include ---------------------------//
#include "..\include\led.h"
#include "..\fwlib\inc\stm32f4xx_gpio.h"
#include "..\fwlib\inc\stm32f4xx_rcc.h"

//--------------------- Function Prototype ----------------------//
static int initialize(void);

//--------------------------- Variable --------------------------//
LED_T led = {
	.initialize = initialize
};
//--------------------------- Function --------------------------//
/*
 * Name                : initialize
 * Description         : ---
 * Author              : ysloveivy.
 *
 * History
 * --------------------
 * Rev                 : 0.00
 * Date                : 01/03/2016
 * 
 * create.
 * --------------------
 */
static int initialize(void)
{
	GPIO_InitTypeDef   GPIO_uInitStructure;

	//LED IO��ʼ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	GPIO_uInitStructure.GPIO_Pin = GPIO_Pin_7	| GPIO_Pin_8;    //PF7->PW_LED2 PF8->PW_LED1
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_OUT;                          //���ö˿�Ϊ���ģʽ
	GPIO_uInitStructure.GPIO_OType = GPIO_OType_PP;                         //�������
	GPIO_uInitStructure.GPIO_PuPd = GPIO_PuPd_UP;                           //����
	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_100MHz;                     //�����ٶ�Ϊ������

	GPIO_Init(GPIOF,&GPIO_uInitStructure);

	//PI5��PI6��PI7����ɫLED�ƣ�PI5��PI6��PI7�øߵ�λ����Ϩ��
	GPIO_SetBits(GPIOF,GPIO_Pin_7|GPIO_Pin_8);

	
	//PA11 for battery charge test
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	GPIO_uInitStructure.GPIO_Pin = GPIO_Pin_11;    
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_IN;  
	GPIO_uInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_100MHz; 

	GPIO_Init(GPIOA,&GPIO_uInitStructure);


	GPIO_SetBits(GPIOA,GPIO_Pin_7|GPIO_Pin_8);
	return 0;
}
