#ifndef __ONE_WIRE_HAL_H__
#define __ONE_WIRE_HAL_H__

#include "one_wire_def.h"
#include "..\fwlib\inc\stm32f4xx_gpio.h"
#include "..\fwlib\inc\stm32f4xx_rcc.h"
#define ONE_WIRE_PORT_NUM								 4

#define ONE_WIRE_NET_MAX_DEVICE_NUM						 4
#define DS2401_FAMILY_CODE 1


#define ONE_WIRE_RESET_LOW_TIME_STANDARD                 635	//640us
#define ONE_WIRE_PRESENCE_DETECT_SAMPLE_TIME_STANDARD    67		//70us
#define ONE_WIRE_RESET_HIGH_TIME_STANDARD                407	//410us
#define ONE_WIRE_WRITE_ZERO_LOW_TIME_STANDARD            57		//60us
#define ONE_WIRE_WRITE_ZERO_LOW_WAIT_TIME_STANDARD       6		//10us
#define ONE_WIRE_WRITE_ONE_LOW_TIME_STANDARD             3		//6us
#define ONE_WIRE_WRITE_ONE_LOW_WAIT_TIME_STANDARD        60		//64us
#define ONE_WIRE_READ_LOW_TIME_STANDARD                  3		//6us
#define ONE_WIRE_READ_SAMPLE_TIME_STANDARD               5		//9us
#define ONE_WIRE_READ_WAIT_TIME_STANDARD				 52		//55us

//Note: Overdrive mode is not impelemented for now.
#define ONE_WIRE_RESET_WAIT_TIME_OVERDRIVE               12		//2.5us
#define ONE_WIRE_RESET_LOW_TIME_OVERDRIVE                552	//70us
#define ONE_WIRE_PRESENCE_DETECT_SAMPLE_TIME_OVERDRIVE   61		//8.5us
#define ONE_WIRE_RESET_HIGH_TIME_OVERDRIVE               312	//40us
#define ONE_WIRE_WRITE_ZERO_LOW_TIME_OVERDRIVE           53		//7.5us
#define ONE_WIRE_WRITE_ZERO_LOW_WAIT_TIME_OVERDRIVE      12		//2.5us
#define ONE_WIRE_WRITE_ONE_LOW_TIME_OVERDRIVE            1		//1us
#define ONE_WIRE_WRITE_ONE_LOW_WAIT_TIME_OVERDRIVE       53		//7.5us
#define ONE_WIRE_READ_LOW_TIME_OVERDRIVE                 1		//1us
#define ONE_WIRE_READ_SAMPLE_TIME_OVERDRIVE              1		//1us
#define ONE_WIRE_READ_WAIT_TIME_OVERDRIVE				 49		//7us

void ONE_WIRE_PORT_SWITCH_TO_RX(int port);
void ONE_WIRE_PORT_SWITCH_TO_TX(int port);
void ONE_WIRE_PORT_SET(int port, int port_value);
int ONE_WIRE_PORT_GET(int port, int port_value);


/*
#define ONE_WIRE_PORT_SET(port, port_value)	{\
												switch(port)\
												{\
													case 0:\
														port_value ? (GPIOF->BSRR = GPIO_Pin_0) : (GPIOF->BRR = GPIO_Pin_0);\
														break;\
													case 1:\
														port_value ? (GPIOF->BSRR = GPIO_Pin_1) : (GPIOF->BRR = GPIO_Pin_1);\
														break;\
													case 2:\
														port_value ? (GPIOF->BSRR = GPIO_Pin_5) : (GPIOF->BRR = GPIO_Pin_5);\
														break;\
													case 3:\
														port_value ? (GPIOF->BSRR = GPIO_Pin_3) : (GPIOF->BRR = GPIO_Pin_3);\
														break;\
													default:\
														break;\
												}\
											}	//Set port value

#define ONE_WIRE_PORT_GET(port, port_value) {\
												switch(port)\
												{\
													case 0:\
														port_value = (((GPIOF->IDR & GPIO_Pin_0) != (uint32_t)Bit_RESET) ? 1 : 0);\
														break;\
													case 1:\
														port_value = (((GPIOF->IDR & GPIO_Pin_1) != (uint32_t)Bit_RESET) ? 1 : 0);\
														break;\
													case 2:\
														port_value = (((GPIOF->IDR & GPIO_Pin_5) != (uint32_t)Bit_RESET) ? 1 : 0);\
														break;\
													case 3:\
														port_value = (((GPIOF->IDR & GPIO_Pin_3) != (uint32_t)Bit_RESET) ? 1 : 0);\
														break;\
													default:\
														break;\
												}\
											}	//Get port value

#define ONE_WIRE_PORT_SWITCH_TO_RX(port)    {\
												switch(port)\
												{\
													case 0:\
														GPIOF->CRL &= 0xFFFFFFF0;\
														GPIOF->CRL |= 0x00000008;\
														break;\
													case 1:\
														GPIOF->CRL &= 0xFFFFFF0F;\
														GPIOF->CRL |= 0x00000080;\
														break;\
													case 2:\
														GPIOF->CRL &= 0xFFFFF0FF;\
														GPIOF->CRL |= 0x00000800;\
														break;\
													case 3:\
														GPIOF->CRL &= 0xFFFF0FFF;\
														GPIOF->CRL |= 0x00008000;\
														break;\
													default:\
														break;\
												}\
											}	//Pull-up input

#define ONE_WIRE_PORT_SWITCH_TO_TX(port)	{\
												switch(port)\
												{\
													case 0:\
														GPIOF->CRL &= 0xFFFFFFF0;\
														GPIOF->CRL |= 0x00000003;\
														break;\
													case 1:\
														GPIOF->CRL &= 0xFFFFFF0F;\
														GPIOF->CRL |= 0x00000030;\
														break;\
													case 2:\
														GPIOF->CRL &= 0xFFFFF0FF;\
														GPIOF->CRL |= 0x00000300;\
														break;\
													case 3:\
														GPIOF->CRL &= 0xFFFF0FFF;\
														GPIOF->CRL |= 0x00003000;\
														break;\
													default:\
														break;\
												}\
											}	//Push-pull output
*/
#include "delay.h"
#define ONE_WIRE_DELAY_US(n)            (delay_us(n))
#define ONE_WIRE_DELAY_MS(n)            (delay_ms(n))

extern uint8_t one_wire_device_num[ONE_WIRE_PORT_NUM];
extern one_wire_data_type one_wire_data[ONE_WIRE_PORT_NUM][ONE_WIRE_NET_MAX_DEVICE_NUM];
											
void hw_one_wire_init(void);
int8_t one_wire_reset(uint8_t port, one_wire_data_type *one_wire_data);
uint8_t one_wire_write_bit(uint8_t port, one_wire_data_type *one_wire_data, uint8_t sendbit);
uint8_t one_wire_read_bit(uint8_t port, one_wire_data_type *one_wire_data);
uint8_t one_wire_write_byte(uint8_t port, one_wire_data_type *one_wire_data, uint8_t sendbyte);
uint8_t one_wire_read_byte(uint8_t port, one_wire_data_type *one_wire_data);

#endif
