/*
 * FILE                : adc.c
 * DESCRIPTION         : --
 * Author              : ysloveivy.
 * Copyright           :
 *
 * History
 * --------------------
 * Rev                 : 0.00
 * Date                : 11/21/2015
 *
 * create.
 * --------------------
 */
//------------------------ Include files ------------------------//
#include "..\FWlib\inc\stm32f4xx_adc.h"
#include "..\FWlib\inc\stm32f4xx_gpio.h"
#include "..\FWlib\inc\stm32f4xx_rcc.h"
#include "..\include\adc.h"

//--------------------- Function Prototype ----------------------//
static int initialize(void);
static int read_adc(int);

//-------------------------- Variable ---------------------------//
SYS_ADC_T adc = {
	.initialize = initialize,
	.read = read_adc
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
 * Date                : 11/21/2015
 * 
 * create.
 * --------------------
 */
static int initialize(void)
{
		GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef  ADC_CommonInitStructure;
	ADC_InitTypeDef  ADC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);                            //使能ADC1时钟 
	

	//ADC1_PC4<----->channel 14, ADC1_PC5<----->channel 15
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;                                    //模拟输入
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;                        //ADC为独立模式
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;    //设置两个采样阶段之间的延迟周期数为5个周期
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;         //禁止DMA 
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div6;                     //设置分频系数为6分频 
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	//初始化ADC
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;                          //ADC转换分辨率12位
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;                                   //非扫描模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                             //关闭连续转换模式
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;     //禁止触发检测
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                          //右对齐方式
	ADC_InitStructure.ADC_NbrOfConversion = 1;                                      //1个转换在规则序列中
	ADC_Init(ADC1, &ADC_InitStructure);
	//ADC_Init(ADC3, &ADC_InitStructure);
	
	//使能ADC
	ADC_Cmd(ADC1, ENABLE);

	return 0;
}
/*
 * Name                  : sort
 * Description           : ---
 * Author                : XiaomaGee.
 *
 * History
 * ----------------------
 * Rev                   : 0.00
 * Date                  : 11/21/2015
 *
 * create.
 * ----------------------
 */
void sort(unsigned short int a[], int n)
{
	int i, j, t;
	
	//元素从小到大排列
	for (i = 0; i < n - 1; i++) {
		for (j = 0; j < n - i - 1; j++) {
			if (a[j] > a[j + 1]) {
				t = a[j];
				a[j] = a[j + 1];
				a[j + 1] = t;
			}
		}
	}
}
/*
 * Name                  : read_adc
 * Description           : ---
 * Author                : ysloveivy.
 *
 * History
 * ----------------------
 * Rev                   : 0.00
 * Date                  : 11/21/2015
 *
 * create.
 * ----------------------
 */
#define channels 2
static int read_adc(int channel)
{
	int i,k;
	unsigned long int temp[20] = {0};
	unsigned long int value;
	unsigned short int data[100];
	unsigned char channel_remap[channels] = {ADC_Channel_14,ADC_Channel_15};//,ADC_Channel_15,ADC_Channel_4,ADC_Channel_5};

	//取得到的100个转换值的中间20个的平均值作为结果
	//连续取20次这样的平均值，再求平均值作为最终结果
	for(k = 0;k < 20;k++){
		for(i = 0;i < 100;i++){
			if(channel == 0 || channel == 1){
				ADC_RegularChannelConfig(ADC1, channel_remap[channel], 1, ADC_SampleTime_480Cycles); //ADC1规则通道为通道14，15，一个序列，采样时间为480个周期

				ADC_SoftwareStartConv(ADC1);                                                         //使能指定的ADC1的软件转换启动功能

				while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));                                       //等待转换结束

				data[i] = ADC_GetConversionValue(ADC1);	                                             //将结果保存

			}
		}

		sort(data,100);

		for(i = 40;i < 60;i++){
			temp[k] += data[i];
		}

		temp[k] = temp[k] / 20;
	}

	value = 0;
	for(k = 0;k < 20;k++){
		value += temp[k]; 
	}
	value /= 20;

	adc.value[channel] = value * ADC_REF / 4096; 

	return value;
}
