/*
 * FILE                : timer3.c
 * DESCRIPTION         : This file is timer3 driver.
 * Author              : ysloveivy
 * Copyright           :
 *
 * History
 * --------------------
 * Rev                 : 0.00
 * Date                : 11/30/2015
 *
 * create.
 * --------------------
 */
//--------------------------- Include ---------------------------//
#include "..\include\timer3.h"
#include "..\include\led.h"
#include "..\fwlib\inc\stm32f4xx_gpio.h"
#include "..\fwlib\inc\stm32f4xx_rcc.h"
#include "..\fwlib\inc\stm32f4xx_tim.h"

//--------------------- Function Prototype ----------------------//
static int initialize(void);

//--------------------------- Variable --------------------------//
TIMER3_T timer3 = {
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
 * Date                : 11/30/2015
 * 
 * create.
 * --------------------
 */

static int initialize(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);                         //ʹ��TIM3ʱ��

	TIM_TimeBaseInitStructure.TIM_Period = 4999;                                //�����Զ�����ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = 8399;                             //���ö�ʱ��Ԥ��Ƶϵ��
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;                 //����ʱ�ӷ�Ƶ����
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;             //���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);

	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);                                    //ʹ��TIM3�����ж�
	TIM_Cmd(TIM3,ENABLE);                                                       //ʹ��TIM3

	//�ж����ȼ�����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	return 0;
}

/*
 * Name                : TIM3_IRQHandler
 * Description         : ---
 * Author              : ysloveivy.
 *
 * History
 * -------------------
 * Rev                 : 0.00
 * Date                : 30/11/2015
 * 
 * create.
 * -------------------
 */
void TIM3_IRQHandler(void)
{
	static int counter = 0;

	//��ʱ500ms��ÿ��500ms��ɫLED����˸һ��
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)
	{
//		if(counter % 3 == 0){
//			LED_RED_ON;
//			LED_BLUE_OFF;
//			LED_GREEN_OFF;
//		}else if(counter % 3 == 1){
//			LED_RED_OFF;
//			LED_BLUE_ON;
//			LED_GREEN_OFF;
//		}else if(counter % 3 == 2){
//			LED_RED_OFF;
//			LED_BLUE_OFF;
//			LED_GREEN_ON;
//		}
//		counter ++;
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
}
