#include "boot.h"

load_a load_A;

void BOOT_Branch(void)
{
	if(OTA_Info.OTA_flag == OTA_SET_FLAG)
	{
		u0_printf("OTA更新\r\n");
		BootStateFlag |= UPDATA_A_FLAG;
		UpDataA.W25Q64_BlockNB = 0;   		//0块是OTA代码
	}
	else
	{
		u0_printf("跳转A分区\r\n");
		LOAD_A(STM32_A_SADDR);
	}
}

__asm void MSR_SP(uint32_t addr)
{
	MSR MSP,r0
	BX r14
}

//void BootLoader_Clear(void)
//{
//	//HAL_UART_DeInit(&huart1);									//注释了就没有问号了
//	//HAL_SPI_DeInit(&hspi1);
//	HAL_GPIO_DeInit(GPIOA,GPIO_PIN_9|GPIO_PIN_10);
//	HAL_GPIO_DeInit(GPIOC,GPIO_PIN_11|GPIO_PIN_12);
//}







void LOAD_A(uint32_t addr)
{

	if((*(uint32_t *)addr>=0x20000000)&&(*(uint32_t *)addr<=0x2000BFFF))     //判断sp栈顶指针的范围是否合法，在对应型号的RAM控件范围内  48K
	{     
		MSR_SP(*(volatile uint32_t *)addr);                                            //设置SP
		load_A = (load_a)*(volatile uint32_t *)(addr+4);                               //将函数指针load_A指向A区的复位向量
		//BootLoader_Clear();                                                   //清除B区使用的外设
		load_A();                                                             //调用函数指针load_A，改变PC指针，从而转向A区的复位向量位置，完成跳转
	}else 
	{
		u0_printf("跳转A分区失败\r\n"); 
	}
}
//void LOAD_A(uint32_t addr)
//{
//	if (((*(uint32_t *)addr) >= 0x20000000) && ((*(uint32_t *)addr) <= 0x2000BFFF))
//	{
//		__disable_irq();

//		HAL_RCC_DeInit();
//		HAL_DeInit();

//		SysTick->CTRL = 0;
//		SysTick->LOAD = 0;
//		SysTick->VAL = 0;

//		for (int i = 0; i < 8; i++)
//		{
//			NVIC->ICER[i] = 0xFFFFFFFF;
//			NVIC->ICPR[i] = 0xFFFFFFFF;
//		}

//		SCB->VTOR = addr;  // 设置中断向量表到A区

//		MSR_SP(*(uint32_t *)addr);           // 设置主堆栈指针
//		load_A = (load_a)*(uint32_t *)(addr + 4); // 复位地址

//		BootLoader_Clear();  // 关闭串口和IO

//		load_A();            // 跳转到A区程序
//	}
//	else
//	{
//		u0_printf("跳转A分区失败\r\n");
//	}
//}



