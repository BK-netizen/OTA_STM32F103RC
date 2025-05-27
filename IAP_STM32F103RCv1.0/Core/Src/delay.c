#include "delay.h"


void Delay_Us(uint16_t us)
{
	SysTick->LOAD = us * 72;
	SysTick->VAL = 0x00;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	while(!(SysTick->CTRL&SysTick_CTRL_COUNTFLAG_Msk));    //�ȵ�Systick��LOAD����ֵ������0����־λ��λ����ʾ��ʱʱ�䵽
	SysTick->CTRL &=~ SysTick_CTRL_ENABLE_Msk;             //��Systick	
	
}

void Delay_Ms(uint16_t ms)
{
	while(ms--){             //ѭ��
		Delay_Us(1000);      //ÿ����ʱ1000us
	}
}





