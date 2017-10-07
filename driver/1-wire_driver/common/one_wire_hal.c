// Include files
#include "one_wire_hal.h"
#include "one_wire_rom_func.h"

uint8_t one_wire_device_num[ONE_WIRE_PORT_NUM];
one_wire_data_type one_wire_data[ONE_WIRE_PORT_NUM][ONE_WIRE_NET_MAX_DEVICE_NUM];

GPIO_TypeDef* GPIOx;
uint16_t GPIO_Pin;
long RCC_AHB1Periph_GPIOx;
void portselect(int port)
{
	switch(port)
	{
		case 2:
			RCC_AHB1Periph_GPIOx=RCC_AHB1Periph_GPIOI;
			GPIOx=GPIOI;
		GPIO_Pin=GPIO_Pin_7;
			break;
		case 1:
			RCC_AHB1Periph_GPIOx=RCC_AHB1Periph_GPIOI;
		GPIOx=GPIOI;
		GPIO_Pin=GPIO_Pin_5;
			break;
		case 0:
			RCC_AHB1Periph_GPIOx=RCC_AHB1Periph_GPIOB;
		GPIOx=GPIOB;
		GPIO_Pin=GPIO_Pin_8;
			break;
		case 3:
			RCC_AHB1Periph_GPIOx=RCC_AHB1Periph_GPIOE;
		GPIOx=GPIOE;
		GPIO_Pin=GPIO_Pin_0;
		break;
		default:
			RCC_AHB1Periph_GPIOx=RCC_AHB1Periph_GPIOI;
			GPIOx=GPIOI;
		GPIO_Pin=GPIO_Pin_7;
	}
}

void hw_one_wire_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOI, &GPIO_InitStructure);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}
void ONE_WIRE_PORT_SWITCH_TO_RX(int port)
{
	
	portselect(port);
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOx, ENABLE);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOx, &GPIO_InitStructure);
	
//		GPIO_InitTypeDef   GPIO_uInitStructure;
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
//	GPIO_uInitStructure.GPIO_Pin = GPIO_Pin_8;                            //设置连接按键的IO端口
//	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_IN;                          //设置端口为输入模式
//	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_100MHz;                    //设置速度为第三级
//	GPIO_uInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;                      //设置输入端口不变化 
//	GPIO_Init(GPIOB, &GPIO_uInitStructure);                                //把设置的参数用到结构体
	
}
void ONE_WIRE_PORT_SWITCH_TO_TX(int port)
{
	portselect(port);
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOx, ENABLE);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOx, &GPIO_InitStructure);
}
void ONE_WIRE_PORT_SET(int port, int port_value)
{
	portselect(port);
	
	if(port_value)
	{
		GPIO_SetBits(GPIOx, GPIO_Pin);
	}
	else
	{
		GPIO_ResetBits(GPIOx, GPIO_Pin);
	}
	
}
int ONE_WIRE_PORT_GET(int port)
{
	portselect(port);
	
	return GPIO_ReadInputDataBit(GPIOx,GPIO_Pin);
}

//--------------------------------------------------------------------------
// Reset all of the devices on the 1-Wire Net and return the result.
//
// Returns: TRUE(1):  presense pulse(s) detected, device(s) reset
//          FALSE(0): no presense pulses detected
//
int8_t one_wire_reset(uint8_t port, one_wire_data_type *one_wire_data)
{
    uint8_t result;

    // Perform a 1-Wire reset
    switch (one_wire_data->speed_mode)
    {
    case MODE_STANDARD:
        ONE_WIRE_PORT_SWITCH_TO_TX(port);
        ONE_WIRE_PORT_SET(port, 0);            // drive bus low.
        ONE_WIRE_DELAY_US(ONE_WIRE_RESET_LOW_TIME_STANDARD);	//Delay 640us
        ONE_WIRE_PORT_SWITCH_TO_RX(port);
        ONE_WIRE_DELAY_US(ONE_WIRE_PRESENCE_DETECT_SAMPLE_TIME_STANDARD);	//Delay 70us
        result=ONE_WIRE_PORT_GET(port);   // get presence detect pulse.
        ONE_WIRE_DELAY_US(ONE_WIRE_RESET_HIGH_TIME_STANDARD);	//Delay 410us
        break;
    case MODE_OVERDRIVE:
		ONE_WIRE_DELAY_US(ONE_WIRE_RESET_WAIT_TIME_OVERDRIVE);	//Delay 2.5us
        ONE_WIRE_PORT_SWITCH_TO_TX(port);
        ONE_WIRE_PORT_SET(port, 0);            // drive bus low.
        ONE_WIRE_DELAY_US(ONE_WIRE_RESET_LOW_TIME_OVERDRIVE);	//Delay 70us
        ONE_WIRE_PORT_SWITCH_TO_RX(port);
        ONE_WIRE_DELAY_US(ONE_WIRE_PRESENCE_DETECT_SAMPLE_TIME_OVERDRIVE);	//Delay 8.5us
        result=ONE_WIRE_PORT_GET(port);   // get presence detect pulse.
        ONE_WIRE_DELAY_US(ONE_WIRE_RESET_HIGH_TIME_OVERDRIVE);	//Delay 40us
        break;
    default:
        break;
    }

    if (result == 0)
    {
        one_wire_data->error_code = ONE_WIRE_ERROR_NO_ERROR;

        return 0;
    }
    else
    {
        one_wire_data->error_code = ONE_WIRE_ERROR_PRESENCE_DETECT_FAILED;

        return -1;
    }
}

//--------------------------------------------------------------------------
// Send 1 bit of communication to the 1-Wire Net. The parameter
// 'sendbit' least significant bit is used and the least
// significant bit of the result is the return bit.
//
// 'sendbit'    - the least significant bit is the bit to send
//
// Returns:  the bit sent to the 1-Wire Net.
//
uint8_t one_wire_write_bit(uint8_t port, one_wire_data_type *one_wire_data, uint8_t sendbit)
{
	ONE_WIRE_PORT_SWITCH_TO_TX(port);
    switch (one_wire_data->speed_mode)
    {
    case MODE_STANDARD:
        if (sendbit == 0)
        {
            
            ONE_WIRE_PORT_SET(port, 0);			// drive bus low.
            ONE_WIRE_DELAY_US(ONE_WIRE_WRITE_ZERO_LOW_TIME_STANDARD);		//Delay 60us
						ONE_WIRE_PORT_SET(port, 1);			// drive bus high.
            ONE_WIRE_DELAY_US(ONE_WIRE_WRITE_ZERO_LOW_WAIT_TIME_STANDARD);	//Delay 10us
		}
        else
        {
            //ONE_WIRE_PORT_SWITCH_TO_TX(port);
            ONE_WIRE_PORT_SET(port, 0);			// drive bus low.
            ONE_WIRE_DELAY_US(ONE_WIRE_WRITE_ONE_LOW_TIME_STANDARD);		//Delay 6us
						ONE_WIRE_PORT_SET(port, 1);			// drive bus high.
            ONE_WIRE_DELAY_US(ONE_WIRE_WRITE_ONE_LOW_WAIT_TIME_STANDARD);	//Delay 64us
		}
        break;
    case MODE_OVERDRIVE:
        if (sendbit == 0)
        {
            //ONE_WIRE_PORT_SWITCH_TO_TX(port);
            ONE_WIRE_PORT_SET(port, 0);            // drive bus low.
            ONE_WIRE_DELAY_US(ONE_WIRE_WRITE_ZERO_LOW_TIME_OVERDRIVE);		//Delay 7.5us
			ONE_WIRE_PORT_SET(port, 1);
            ONE_WIRE_DELAY_US(ONE_WIRE_WRITE_ZERO_LOW_WAIT_TIME_OVERDRIVE);	//Delay 2.5us
        }
        else
        {
            //ONE_WIRE_PORT_SWITCH_TO_TX(port);
            ONE_WIRE_PORT_SET(port, 0);            // drive bus low.
			ONE_WIRE_DELAY_US(ONE_WIRE_WRITE_ONE_LOW_TIME_OVERDRIVE);		//Delay 1us
			ONE_WIRE_PORT_SET(port, 1);
            ONE_WIRE_DELAY_US(ONE_WIRE_WRITE_ONE_LOW_WAIT_TIME_OVERDRIVE);	//Delay 7.5us			
        }
        break;
    default:
        break;
    }

    return sendbit;
}

//--------------------------------------------------------------------------
// Read 1 bit from the 1-Wire Net. The least significant bit
// of the result is the return bit.
//
// Returns:  the bit from from the 1-Wire Net.
//
uint8_t one_wire_read_bit(uint8_t port, one_wire_data_type *one_wire_data)
{
    uint8_t result;

    switch (one_wire_data->speed_mode)
    {
    case MODE_STANDARD:
        ONE_WIRE_PORT_SWITCH_TO_TX(port);
        ONE_WIRE_PORT_SET(port, 0);            // drive bus low.
		ONE_WIRE_DELAY_US(ONE_WIRE_READ_LOW_TIME_STANDARD);		//Delay 6us		
        ONE_WIRE_PORT_SWITCH_TO_RX(port);
		ONE_WIRE_DELAY_US(ONE_WIRE_READ_SAMPLE_TIME_STANDARD);	//Delay 9us	
        result=ONE_WIRE_PORT_GET(port);
		//result=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8);
        ONE_WIRE_DELAY_US(ONE_WIRE_READ_WAIT_TIME_STANDARD);	//Delay 55us
        break;
    case MODE_OVERDRIVE:
        ONE_WIRE_PORT_SWITCH_TO_TX(port);
        ONE_WIRE_PORT_SET(port, 0);            // drive bus low.
		ONE_WIRE_DELAY_US(ONE_WIRE_READ_LOW_TIME_OVERDRIVE);	//Delay 1us
        ONE_WIRE_PORT_SWITCH_TO_RX(port);
		ONE_WIRE_DELAY_US(ONE_WIRE_READ_SAMPLE_TIME_OVERDRIVE);	//Delay 1us
        result=ONE_WIRE_PORT_GET(port);
        ONE_WIRE_DELAY_US(ONE_WIRE_READ_WAIT_TIME_OVERDRIVE);	//Delay 7us
        break;
    default:
        break;
    }

    return result;
}

//--------------------------------------------------------------------------
// Send 8 bits of communication to the 1-Wire Net and verify that the
// 8 bits read from the 1-Wire Net is the same (write operation).
// The parameter 'sendbyte' least significant 8 bits are used.
//
//
// 'sendbyte'   - 8 bits to send (least significant byte)
//
// Returns:  the byte written
//
uint8_t one_wire_write_byte(uint8_t port, one_wire_data_type *one_wire_data, uint8_t sendbyte)
{
    uint8_t i;

    for (i = 0; i < 8; i++)
    {
        one_wire_write_bit(port, one_wire_data, sendbyte & 0x01);
        sendbyte >>= 1;
    }

    return sendbyte;
}

//--------------------------------------------------------------------------
// Send 8 bits of read communication to the 1-Wire Net and and return the
// result 8 bits read from the 1-Wire Net.
//
// Returns:  8 bytes read from 1-Wire Net
//
uint8_t one_wire_read_byte(uint8_t port, one_wire_data_type *one_wire_data)
{
    uint8_t i;
    uint8_t result = 0;

    for (i = 0; i < 8; i++)
    {
        result |= (one_wire_read_bit(port, one_wire_data) << i);
    }

    return result;
}
