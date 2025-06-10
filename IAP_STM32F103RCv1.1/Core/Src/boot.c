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
		Delay_Ms(100);
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
	int temp,i;
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
		else if((datalen == 1) && data[0] == '3')
		{
			u0_printf("����OTA�汾��\r\n"); 
			BootStateFlag |= OTA_VER_FLAG;	
		}
		else if((datalen == 1) && data[0] == '4')
		{
			u0_printf("��ѯOTA�汾��\r\n");
			AT24C02_ReadOTAInfo();
			u0_printf("�汾��Ϊ: %s\r\n",OTA_Info.OTA_Version);
			BootLoader_Info();
		}
		else if((datalen == 1) && data[0] == '5')
		{
			u0_printf("���ⲿFlash���س���\r\n");
			BootStateFlag |= CMD5_FLAG;	
		}
		else if((datalen == 1) && data[0] == '6')
		{
			u0_printf("ʹ���ⲿFlash�ڳ���\r\n");
			BootStateFlag |= CMD6_FLAG;	
		}
		else if((datalen == 1) && data[0] == '7')
		{
			u0_printf("����\r\n"); 
			Delay_Ms(10);
			
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
					if(BootStateFlag & CMD5_X_FLAG)
					{
						for(i=0;i<8;i++)  //8�β��ܷ���2k
						{
							W25Q64_PageWrite(&UpDataA.Updatabuff[i*256],UpDataA.W25Q64_BlockNB * 64 * 4 + (UpDataA.XmodemNB/8-2)*4+i);
						}
					}
					else
					{
						FLASH_WRITE(STM32_A_SADDR + ((UpDataA.XmodemNB /(STM32_PAGE_SIZE /128))-1)*STM32_PAGE_SIZE,(uint32_t *)UpDataA.Updatabuff,STM32_PAGE_SIZE);
					}
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
				if(BootStateFlag & CMD5_X_FLAG)
				{
					W25Q64_PageWrite(&UpDataA.Updatabuff[i*256],UpDataA.W25Q64_BlockNB * 64 * 4 + (UpDataA.XmodemNB/8-1)*4+i);
				}
				else
				{
					FLASH_WRITE(STM32_A_SADDR + ((UpDataA.XmodemNB /(STM32_PAGE_SIZE /128))-1)*STM32_PAGE_SIZE,(uint32_t *)UpDataA.Updatabuff,UpDataA.XmodemNB % (STM32_PAGE_SIZE /128)*128);
				}
			}
			BootStateFlag &= ~IAP_XMODEMD_FLAG;
			if(BootStateFlag & CMD5_X_FLAG)
			{                           														//�ж����������5����Xmodem�Ļ�������if
				BootStateFlag &=~ CMD5_X_FLAG;                      							 //���CMD5_XMODEM_FLAG
				OTA_Info.OTA_FileLen[UpDataA.W25Q64_BlockNB] = UpDataA.XmodemNB * 128;   	//���㲢���汾�δ���ĳ����С
				AT24C02_WriteOTAInfo();                                 			 		//���浽24C02
				Delay_Ms(100);                                          					//��ʱ
				u0_printf("%x\r\n",OTA_Info.OTA_FileLen[UpDataA.W25Q64_BlockNB]);
				BootLoader_Info();                                      //�����������Ϣ
			}else{                                                      //�ж������������5����Xmodem�Ļ����Ǿ��Ǵ���IAP�����ģ�����else
				Delay_Ms(100);                                          //��ʱ
				NVIC_SystemReset();                                     //����
			}   
		}
	}
//	else if(BootStateFlag & OTA_VER_FLAG)
//	{
//		if(datalen == 26)
//		{
//			if(sscanf((char *)data,"VER-%d.%d.%d-%d/%d/%d-%d:%d",&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp) == 8)
//			//VER-1.2.3-2020/12/13-12:20
//			{
//				memset(OTA_Info.OTA_Version,0,26);
//				memcpy(OTA_Info.OTA_Version,data,26);
//				AT24C02_WriteOTAInfo();
//				u0_printf("�汾��ȷ\r\n");  
//				BootStateFlag &=~ OTA_VER_FLAG;
//				//u0_printf("%x\r\n",BootStateFlag);
//				BootLoader_Info();
//			}
//			else
//			{
//				u0_printf("�汾�Ÿ�ʽ���ô���\r\n");
//			}
//		}
//		else
//		{
//			u0_printf("�汾�ų������ô���\r\n");
//		}
//	}
	else if(BootStateFlag & CMD5_FLAG)
	{
		if(datalen == 1)
		{
			if((data[0] >= 0x31) && (data[0] <= 0x39))    // ASIC �� 1-9
			{
				UpDataA.W25Q64_BlockNB = data[0] - 0x30;
				BootStateFlag |= (IAP_XMODEMC_FLAG | IAP_XMODEMD_FLAG | CMD5_X_FLAG);  //���ǿ�����ԭ����A����XmodemЭ��Ĵ���,ֻ�����ĳ��˷��ⲿFlash
				UpDataA.XmodemTimeout = 0;
				UpDataA.XmodemNB = 0;
				OTA_Info.OTA_FileLen[UpDataA.W25Q64_BlockNB] = 0;
				W25Q64_Erase64K(UpDataA.W25Q64_BlockNB);
				u0_printf("ͨ��XmodemЭ�飬���ⲿFlash��%d�������س�����ʹ��bin��ʽ�ļ�\r\n",UpDataA.W25Q64_BlockNB);
				BootStateFlag &=~ CMD5_FLAG;   
			}
			else
			{
				u0_printf("�������Ŵ���\r\n");
			}
		}
		else
		{
			u0_printf("���ݳ��ȴ���\r\n"); 
		}
	}
	else if(BootStateFlag & CMD6_FLAG)
	{
		if(datalen == 1)
		{
			if((data[0] >= 0x31) && (data[0] <= 0x39))
			{
				UpDataA.W25Q64_BlockNB = data[0] - 0x30;
				//AT24C02_ReadOTAInfo();				
				BootStateFlag |= UPDATA_A_FLAG;
				BootStateFlag &=~ CMD6_FLAG;  
			}
			else
			{
				u0_printf("��Ŵ���\r\n"); 
			}
		}
		else
		{
			u0_printf("���ݳ��ȴ���\r\n"); 
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



