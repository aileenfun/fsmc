#include "delay.h"
#include "..\fwlib\inc\stm32f4xx_syscfg.h"
void delay_us(long nus)
{
    long ticks;
    long tick_old, tick_now, tick_cnt = 0;
    long reload = SysTick->LOAD;                     //LOAD的值

    tick_cnt = 0;
	ticks = nus * (SystemCoreClock / 1000000);      //需要的节拍数
    tick_old = SysTick->VAL;                                            //刚进入时的计数器值
    while (1)
    {
        tick_now = SysTick->VAL;

        if (tick_now != tick_old)                                       //SYSTICK为递减计数器
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

            if (tick_cnt >= ticks)                                      //达到延时时间
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
