#include "boot.h"

load_a load_A;

void BOOT_Branch(void)
{
	if(OTA_Info.OTA_flag == OTA_SET_FLAG)
	{
		u0_printf("OTA����\r\n");
		BootStateFlag |= UPDATA_A_FLAG;
		UpDataA.W25Q64_BlockNB = 0;   		//0����OTA����
	}
	else
	{
		u0_printf("��תA����\r\n");
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
//	//HAL_UART_DeInit(&huart1);									//ע���˾�û���ʺ���
//	//HAL_SPI_DeInit(&hspi1);
//	HAL_GPIO_DeInit(GPIOA,GPIO_PIN_9|GPIO_PIN_10);
//	HAL_GPIO_DeInit(GPIOC,GPIO_PIN_11|GPIO_PIN_12);
//}







void LOAD_A(uint32_t addr)
{

	if((*(uint32_t *)addr>=0x20000000)&&(*(uint32_t *)addr<=0x2000BFFF))     //�ж�spջ��ָ��ķ�Χ�Ƿ�Ϸ����ڶ�Ӧ�ͺŵ�RAM�ؼ���Χ��  48K
	{     
		MSR_SP(*(volatile uint32_t *)addr);                                            //����SP
		load_A = (load_a)*(volatile uint32_t *)(addr+4);                               //������ָ��load_Aָ��A���ĸ�λ����
		//BootLoader_Clear();                                                   //���B��ʹ�õ�����
		load_A();                                                             //���ú���ָ��load_A���ı�PCָ�룬�Ӷ�ת��A���ĸ�λ����λ�ã������ת
	}else 
	{
		u0_printf("��תA����ʧ��\r\n"); 
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

//		SCB->VTOR = addr;  // �����ж�������A��

//		MSR_SP(*(uint32_t *)addr);           // ��������ջָ��
//		load_A = (load_a)*(uint32_t *)(addr + 4); // ��λ��ַ

//		BootLoader_Clear();  // �رմ��ں�IO

//		load_A();            // ��ת��A������
//	}
//	else
//	{
//		u0_printf("��תA����ʧ��\r\n");
//	}
//}



