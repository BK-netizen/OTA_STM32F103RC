#include "delay.h"


void Delay_Us(uint16_t us)
{
	SysTick->LOAD = us * 72;
	SysTick->VAL = 0x00;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	while(!(SysTick->CTRL&SysTick_CTRL_COUNTFLAG_Msk));    //等到Systick从LOAD重载值倒数到0，标志位置位，表示延时时间到
	SysTick->CTRL &=~ SysTick_CTRL_ENABLE_Msk;             //关Systick	
	
}

void Delay_Ms(uint16_t ms)
{
	while(ms--){             //循环
		Delay_Us(1000);      //每次延时1000us
	}
}





