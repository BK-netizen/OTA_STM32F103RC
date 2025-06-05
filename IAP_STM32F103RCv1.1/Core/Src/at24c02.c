#include "at24c02.h"
#include "string.h"
#include "main.h"
//uint8_t AT24C02_Write_Byte(uint8_t addr,uint8_t wdata)
//{
//	IIC_Start();
//	IIC_Send_Byte(AT24C02_WADDR);					//写设备地址
//	if(IIC_Wait_Ack(100) != 0)	return 1;			//等待应答信号如果出错，则返回
//	IIC_Send_Byte(addr);							//写存储地址
//	if(IIC_Wait_Ack(100) != 0)	return 2;			//等待应答信号如果出错，则返回
//	IIC_Send_Byte(wdata);							//写数据
//	if(IIC_Wait_Ack(100) != 0)	return 3;			//等待应答信号如果出错，则返回
//	IIC_Stop();	
//	return 0;
//}

uint8_t AT24C02_Write_Byte(uint8_t addr,uint8_t wdata)
{
	IIC_Start();                           //发送起始信号
	IIC_Send_Byte(AT24C02_WADDR);           //发送24C02写操作器件地址
	if(IIC_Wait_Ack(100)!=0) return 1;     //等待应答，错误的话，返回1
	IIC_Send_Byte(addr);                   //发送内部存储空间地址
	if(IIC_Wait_Ack(100)!=0) return 2;     //等待应答，错误的话，返回2
	IIC_Send_Byte(wdata);                  //发送数据
	if(IIC_Wait_Ack(100)!=0) return 3;     //等待应答，错误的话，返回3
	IIC_Stop();    

	return 0;
}


uint8_t AT24C02_WritePage(uint8_t addr, uint8_t *wdata)	
{
	uint8_t i;
	
	IIC_Start();
	IIC_Send_Byte(AT24C02_WADDR);					//写设备地址
	if(IIC_Wait_Ack(100) != 0)	return 1;			//等待应答信号如果出错，则返回
	IIC_Send_Byte(addr);							//写存储地址
	if(IIC_Wait_Ack(100) != 0)	return 2;			//等待应答信号如果出错，则返回
	
	for(i = 0;i < 8; i++)
	{
		IIC_Send_Byte(wdata[i]);					//写数据
		if(IIC_Wait_Ack(100) != 0)	return 3+i;		//等待应答信号如果出错，则返回
	}
	
	IIC_Stop();	
	return 0;
}

uint8_t AT24C02_ReadByte(uint8_t addr, uint8_t *rdata)	//
{
	IIC_Start();
	IIC_Send_Byte(AT24C02_WADDR);					//写设备地址
	if(IIC_Wait_Ack(100) != 0)	return 1;	//等待应答信号如果出错，则返回
	IIC_Send_Byte(addr);									//写存储地址
	if(IIC_Wait_Ack(100) != 0)	return 2;	//等待应答信号如果出错，则返回
	IIC_Start();
	IIC_Send_Byte(AT24C02_RADDR);	
	if(IIC_Wait_Ack(100) != 0)	return 3;
	

	*rdata = IIC_Read_Byte(0);
	
	IIC_Stop();	
	return 0;
}

//uint8_t AT24C02_ReadData(uint8_t addr, uint8_t *rdata, uint16_t datalen)
//{
//	uint8_t i;                                  //用于for循环
//	
//	IIC_Start();                                //发送起始信号
//	IIC_Send_Byte(AT24C02_WADDR);                //发送24C02写操作器件地址
//	if(IIC_Wait_Ack(100)!=0) return 1;          //等待应答，错误的话，返回1
//	IIC_Send_Byte(addr);                        //发送内部存储空间地址
//	if(IIC_Wait_Ack(100)!=0) return 2;          //等待应答，错误的话，返回2
//	IIC_Start();                                //再次发送起始信号
//	IIC_Send_Byte(AT24C02_RADDR);                //发送24C02读操作器件地址
//	if(IIC_Wait_Ack(100)!=0) return 3;          //等待应答，错误的话，返回3
//	for(i=0;i<datalen-1;i++)
//	{                   						//循环datalen-1次，一个字节一个字节接收数据
//		rdata[i] = IIC_Read_Byte(1);            //接收数据，发送应答给从机
//	}
//	rdata[datalen-1] = IIC_Read_Byte(0);        //接收最后一个字节，不发送应答信号
//	IIC_Stop();                                 //发送停止信号
//	return 0;		                            //正确，返回0
//}

uint8_t AT24C02_ReadData(uint8_t addr, uint8_t *rdata, uint16_t datalen)
{ 
	while(datalen)
	{
		AT24C02_ReadByte(addr++, rdata);
		rdata++;
		datalen--;
	}
	return 0;
}


void AT24C02_ReadOTAInfo(void)
{
	memset(&OTA_Info,0,OTA_InfoCB_SIZE);
	AT24C02_ReadData(0,(uint8_t *)&OTA_Info.OTA_flag,OTA_InfoCB_SIZE);
}		


//每次写8个字节
void AT24C02_WriteOTAInfo(void)
{
	for(uint8_t i=0;i<OTA_InfoCB_SIZE/8;i++)
	{
		AT24C02_WritePage(i*8,(uint8_t *)&OTA_Info+i*8);
		Delay_Ms(5);				//用HAL库延时出现问题卡死
	}
}






