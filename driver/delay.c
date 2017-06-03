#include "delay.h"
#include "..\fwlib\inc\stm32f4xx_syscfg.h"
void delay_us(long nus)
{
    long ticks;
    long tick_old, tick_now, tick_cnt = 0;
    long reload = SysTick->LOAD;                     //LOAD��ֵ

    tick_cnt = 0;
	ticks = nus * (SystemCoreClock / 1000000);      //��Ҫ�Ľ�����
    tick_old = SysTick->VAL;                                            //�ս���ʱ�ļ�����ֵ
    while (1)
    {
        tick_now = SysTick->VAL;

        if (tick_now != tick_old)                                       //SYSTICKΪ�ݼ�������
        {
            if (tick_now < tick_old)
            {
                tick_cnt += tick_old - tick_now;
            }
            else
            {
                tick_cnt += reload - tick_now + tick_old;
            }

            tick_old = tick_now;

            if (tick_cnt >= ticks)                                      //�ﵽ��ʱʱ��
            {
                break;
            }
        }
    }
}

void delay_ms(long nms)
{
    delay_us(nms * 1000);
}
