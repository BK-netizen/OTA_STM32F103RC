#include "boot.h"

load_a load_A;

void BOOT_Branch(void)
{
	if(BootLoader_Enter(20) == 0)
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
	u0_printf("进入BootLoader命令行\r\n");           //串口0输出信息
	BootLoader_Info();
}

__asm void MSR_SP(uint32_t addr)
{
	MSR MSP,r0
	BX r14
}

void BootLoader_Clear(void)
{
	//HAL_UART_DeInit(&huart1);									//注释了就没有问号了
	//HAL_SPI_DeInit(&hspi1);
	HAL_GPIO_DeInit(GPIOA,GPIO_PIN_9|GPIO_PIN_10);
	HAL_GPIO_DeInit(GPIOC,GPIO_PIN_11|GPIO_PIN_12);
}

uint8_t BootLoader_Enter(uint8_t timeout)
{
	u0_printf("%d ms内输入小写字母 w ,进入BootLoader命令行\r\n",timeout *100);
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
	u0_printf("\r\n");                           //串口0输出信息
	u0_printf("[1]擦除A区\r\n");                 //串口0输出信息
	u0_printf("[2]串口IAP下载A区程序\r\n");      //串口0输出信息
	u0_printf("[3]设置OTA版本号\r\n");           //串口0输出信息
	u0_printf("[4]查询OTA版本号\r\n");           //串口0输出信息
	u0_printf("[5]向外部Flash下载程序\r\n");     //串口0输出信息
	u0_printf("[6]使用外部Flash内程序\r\n");     //串口0输出信息
	u0_printf("[7]重启\r\n");
}

void BootLoader_Event(uint8_t *data, uint16_t datalen)
{
	int temp,i;
	if(BootStateFlag == 0)  								//没发生任何事件
	{
		if((datalen == 1) && data[0] == '1')
		{
			u0_printf("擦除A区\r\n"); 
			FLASH_ERASE(STM_A_START_PAGE,STM32_A_PAGE_NUM);
		}
		else if((datalen == 1) && data[0] == '2')
		{
			u0_printf("串口IAP下载A区程序\r\n"); 
			FLASH_ERASE(STM_A_START_PAGE,STM32_A_PAGE_NUM);
			BootStateFlag |= (IAP_XMODEMC_FLAG | IAP_XMODEMD_FLAG);
			UpDataA.XmodemTimeout = 0;
			UpDataA.XmodemNB = 0;
		}
		else if((datalen == 1) && data[0] == '3')
		{
			u0_printf("设置OTA版本号\r\n"); 
			BootStateFlag |= OTA_VER_FLAG;	
		}
		else if((datalen == 1) && data[0] == '4')
		{
			u0_printf("查询OTA版本号\r\n");
			AT24C02_ReadOTAInfo();
			u0_printf("版本号为: %s\r\n",OTA_Info.OTA_Version);
			BootLoader_Info();
		}
		else if((datalen == 1) && data[0] == '5')
		{
			u0_printf("向外部Flash下载程序\r\n");
			BootStateFlag |= CMD5_FLAG;	
		}
		else if((datalen == 1) && data[0] == '6')
		{
			u0_printf("使用外部Flash内程序\r\n");
			BootStateFlag |= CMD6_FLAG;	
		}
		else if((datalen == 1) && data[0] == '7')
		{
			u0_printf("重启\r\n"); 
			Delay_Ms(10);
			
			NVIC_SystemReset();
		}
	}
	else  if(BootStateFlag & IAP_XMODEMD_FLAG)
	{
		if(datalen == 133 && data[0] == 0x01)
		{
			BootStateFlag &= ~IAP_XMODEMC_FLAG;					//此时不发C了，直接等接收数据了，清标志位
			UpDataA.XmodemCrc = XmodemCRC16(&data[3],128);
			if(UpDataA.XmodemCrc == data[131]*256 + data[132])
			{
				UpDataA.XmodemNB++;					
				memcpy(&UpDataA.Updatabuff[((UpDataA.XmodemNB-1)%(STM32_PAGE_SIZE/128))*128],&data[3],128);   //2K 最多传16次，然后回卷
				//由于先++了，所以这里要减1
				if(UpDataA.XmodemNB % (STM32_PAGE_SIZE /128) == 0)    //意味着塞满了2k，可以传了
				{
					if(BootStateFlag & CMD5_X_FLAG)
					{
						for(i=0;i<8;i++)  //8次才能发完2k
						{
							W25Q64_PageWrite(&UpDataA.Updatabuff[i*256],UpDataA.W25Q64_BlockNB * 64 * 4 + (UpDataA.XmodemNB/8-2)*4+i);
						}
					}
					else
					{
						FLASH_WRITE(STM32_A_SADDR + ((UpDataA.XmodemNB /(STM32_PAGE_SIZE /128))-1)*STM32_PAGE_SIZE,(uint32_t *)UpDataA.Updatabuff,STM32_PAGE_SIZE);
					}
				}
				u0_printf("\x06");			//应答信号
			}
			else
			{
				u0_printf("\x15");			//非应答信号
			}
		}
		if(datalen == 1 && data[0] == 0x04)  //EOT
		{
			u0_printf("\x06");
			if(UpDataA.XmodemNB % (STM32_PAGE_SIZE /128) != 0)   //不满足2K部分
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
			{                           														//判断如果是命令5启动Xmodem的话，进入if
				BootStateFlag &=~ CMD5_X_FLAG;                      							 //清除CMD5_XMODEM_FLAG
				OTA_Info.OTA_FileLen[UpDataA.W25Q64_BlockNB] = UpDataA.XmodemNB * 128;   	//计算并保存本次传输的程序大小
				AT24C02_WriteOTAInfo();                                 			 		//保存到24C02
				Delay_Ms(100);                                          					//延时
				u0_printf("%x\r\n",OTA_Info.OTA_FileLen[UpDataA.W25Q64_BlockNB]);
				BootLoader_Info();                                      //输出命令行信息
			}else{                                                      //判断如果不是命令5启动Xmodem的话，那就是串口IAP启动的，进入else
				Delay_Ms(100);                                          //延时
				NVIC_SystemReset();                                     //重启
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
//				u0_printf("版本正确\r\n");  
//				BootStateFlag &=~ OTA_VER_FLAG;
//				//u0_printf("%x\r\n",BootStateFlag);
//				BootLoader_Info();
//			}
//			else
//			{
//				u0_printf("版本号格式设置错误\r\n");
//			}
//		}
//		else
//		{
//			u0_printf("版本号长度设置错误\r\n");
//		}
//	}
	else if(BootStateFlag & CMD5_FLAG)
	{
		if(datalen == 1)
		{
			if((data[0] >= 0x31) && (data[0] <= 0x39))    // ASIC 码 1-9
			{
				UpDataA.W25Q64_BlockNB = data[0] - 0x30;
				BootStateFlag |= (IAP_XMODEMC_FLAG | IAP_XMODEMD_FLAG | CMD5_X_FLAG);  //还是可以用原来发A区的Xmodem协议的代码,只不过改成了发外部Flash
				UpDataA.XmodemTimeout = 0;
				UpDataA.XmodemNB = 0;
				OTA_Info.OTA_FileLen[UpDataA.W25Q64_BlockNB] = 0;
				W25Q64_Erase64K(UpDataA.W25Q64_BlockNB);
				u0_printf("通过Xmodem协议，向外部Flash第%d个块下载程序，请使用bin格式文件\r\n",UpDataA.W25Q64_BlockNB);
				BootStateFlag &=~ CMD5_FLAG;   
			}
			else
			{
				u0_printf("块输入编号错误\r\n");
			}
		}
		else
		{
			u0_printf("数据长度错误\r\n"); 
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
				u0_printf("编号错误\r\n"); 
			}
		}
		else
		{
			u0_printf("数据长度错误\r\n"); 
		}
	}
	

}



void LOAD_A(uint32_t addr)
{

	if((*(uint32_t *)addr>=0x20000000)&&(*(uint32_t *)addr<=0x2000BFFF))     //判断sp栈顶指针的范围是否合法，在对应型号的RAM控件范围内  48K
	{     
		MSR_SP(*(volatile uint32_t *)addr);                                            //设置SP
		load_A = (load_a)*(volatile uint32_t *)(addr+4);                               //将函数指针load_A指向A区的复位向量
		BootLoader_Clear();                                                   //清除B区使用的外设
		load_A();                                                             //调用函数指针load_A，改变PC指针，从而转向A区的复位向量位置，完成跳转
	}else 
	{
		u0_printf("跳转A分区失败\r\n"); 
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



