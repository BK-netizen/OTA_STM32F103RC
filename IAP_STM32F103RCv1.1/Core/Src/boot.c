#include "boot.h"

load_a load_A;

void BOOT_Branch(void)
{
	if(BootLoader_Enter(20) == 0)
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
	u0_printf("����BootLoader������\r\n");           //����0�����Ϣ
	BootLoader_Info();
}

__asm void MSR_SP(uint32_t addr)
{
	MSR MSP,r0
	BX r14
}

void BootLoader_Clear(void)
{
	//HAL_UART_DeInit(&huart1);									//ע���˾�û���ʺ���
	//HAL_SPI_DeInit(&hspi1);
	HAL_GPIO_DeInit(GPIOA,GPIO_PIN_9|GPIO_PIN_10);
	HAL_GPIO_DeInit(GPIOC,GPIO_PIN_11|GPIO_PIN_12);
}

uint8_t BootLoader_Enter(uint8_t timeout)
{
	u0_printf("%d ms������Сд��ĸ w ,����BootLoader������\r\n",timeout *100);
	while(timeout--)
	{
		HAL_Delay(100);
		if(U0_RxBuff[0] == 'w')
		{
			return 1;
		}
	}
	return 0;

}

void BootLoader_Info(void)
{ 
	u0_printf("\r\n");                           //����0�����Ϣ
	u0_printf("[1]����A��\r\n");                 //����0�����Ϣ
	u0_printf("[2]����IAP����A������\r\n");      //����0�����Ϣ
	u0_printf("[3]����OTA�汾��\r\n");           //����0�����Ϣ
	u0_printf("[4]��ѯOTA�汾��\r\n");           //����0�����Ϣ
	u0_printf("[5]���ⲿFlash���س���\r\n");     //����0�����Ϣ
	u0_printf("[6]ʹ���ⲿFlash�ڳ���\r\n");     //����0�����Ϣ
	u0_printf("[7]����\r\n");
}

void BootLoader_Event(uint8_t *data, uint16_t datalen)
{
	if(BootStateFlag == 0)  								//û�����κ��¼�
	{
		if((datalen == 1) && data[0] == '1')
		{
			u0_printf("����A��\r\n"); 
			FLASH_ERASE(STM_A_START_PAGE,STM32_A_PAGE_NUM);
		}
		else if((datalen == 1) && data[0] == '2')
		{
			u0_printf("����IAP����A������\r\n"); 
			FLASH_ERASE(STM_A_START_PAGE,STM32_A_PAGE_NUM);
			BootStateFlag |= (IAP_XMODEMC_FLAG | IAP_XMODEMD_FLAG);
			UpDataA.XmodemTimeout = 0;
			UpDataA.XmodemNB = 0;

		}
		else if((datalen == 1) && data[0] == '7')
		{
			u0_printf("����\r\n"); 
			HAL_Delay(10);
			NVIC_SystemReset();
		}
	}
	else  if(BootStateFlag & IAP_XMODEMD_FLAG)
	{
		if(datalen == 133 && data[0] == 0x01)
		{
			BootStateFlag &= ~IAP_XMODEMC_FLAG;					//��ʱ����C�ˣ�ֱ�ӵȽ��������ˣ����־λ
			UpDataA.XmodemCrc = XmodemCRC16(&data[3],128);
			if(UpDataA.XmodemCrc == data[131]*256 + data[132])
			{
				UpDataA.XmodemNB++;					
				memcpy(&UpDataA.Updatabuff[((UpDataA.XmodemNB-1)%(STM32_PAGE_SIZE/128))*128],&data[3],128);   //2K ��ഫ16�Σ�Ȼ��ؾ�
				//������++�ˣ���������Ҫ��1
				if(UpDataA.XmodemNB % (STM32_PAGE_SIZE /128) == 0)    //��ζ��������2k�����Դ���
				{
					FLASH_WRITE(STM32_A_SADDR + ((UpDataA.XmodemNB /(STM32_PAGE_SIZE /128))-1)*STM32_PAGE_SIZE,(uint32_t *)UpDataA.Updatabuff,STM32_PAGE_SIZE);
					
				}
				u0_printf("\x06");			//Ӧ���ź�
			}
			else
			{
				u0_printf("\x15");			//��Ӧ���ź�
			}
		}
		if(datalen == 1 && data[0] == 0x04)  //EOT
		{
			u0_printf("\x06");
			if(UpDataA.XmodemNB % (STM32_PAGE_SIZE /128) != 0)   //������2K����
			{
				FLASH_WRITE(STM32_A_SADDR + ((UpDataA.XmodemNB /(STM32_PAGE_SIZE /128))-1)*STM32_PAGE_SIZE,(uint32_t *)UpDataA.Updatabuff,UpDataA.XmodemNB % (STM32_PAGE_SIZE /128)*128);
			}
			BootStateFlag &= ~IAP_XMODEMD_FLAG;
			NVIC_SystemReset();     
		}
		
		
	}
	

}



void LOAD_A(uint32_t addr)
{

	if((*(uint32_t *)addr>=0x20000000)&&(*(uint32_t *)addr<=0x2000BFFF))     //�ж�spջ��ָ��ķ�Χ�Ƿ�Ϸ����ڶ�Ӧ�ͺŵ�RAM�ؼ���Χ��  48K
	{     
		MSR_SP(*(volatile uint32_t *)addr);                                            //����SP
		load_A = (load_a)*(volatile uint32_t *)(addr+4);                               //������ָ��load_Aָ��A���ĸ�λ����
		BootLoader_Clear();                                                   //���B��ʹ�õ�����
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


uint16_t XmodemCRC16(uint8_t *data,uint16_t datalen)
{
	uint8_t i;
	uint16_t Crcinit = 0x0000;
	uint16_t Crcploy = 0x1021;
	
	while(datalen--)
	{
		Crcinit = (*data <<8) ^ Crcinit;
		for(i = 0;i<8;i++)
		{
			if(Crcinit & 0x8000)
			{
				Crcinit = (Crcinit<<1) ^ Crcploy;
			}
			else
			{
				Crcinit = (Crcinit<<1);
			}
		}
		data++;
	}
	return Crcinit;
}



