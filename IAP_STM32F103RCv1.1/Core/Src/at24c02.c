#include "at24c02.h"
#include "string.h"
#include "main.h"
//uint8_t AT24C02_Write_Byte(uint8_t addr,uint8_t wdata)
//{
//	IIC_Start();
//	IIC_Send_Byte(AT24C02_WADDR);					//д�豸��ַ
//	if(IIC_Wait_Ack(100) != 0)	return 1;			//�ȴ�Ӧ���ź���������򷵻�
//	IIC_Send_Byte(addr);							//д�洢��ַ
//	if(IIC_Wait_Ack(100) != 0)	return 2;			//�ȴ�Ӧ���ź���������򷵻�
//	IIC_Send_Byte(wdata);							//д����
//	if(IIC_Wait_Ack(100) != 0)	return 3;			//�ȴ�Ӧ���ź���������򷵻�
//	IIC_Stop();	
//	return 0;
//}

uint8_t AT24C02_Write_Byte(uint8_t addr,uint8_t wdata)
{
	IIC_Start();                           //������ʼ�ź�
	IIC_Send_Byte(AT24C02_WADDR);           //����24C02д����������ַ
	if(IIC_Wait_Ack(100)!=0) return 1;     //�ȴ�Ӧ�𣬴���Ļ�������1
	IIC_Send_Byte(addr);                   //�����ڲ��洢�ռ��ַ
	if(IIC_Wait_Ack(100)!=0) return 2;     //�ȴ�Ӧ�𣬴���Ļ�������2
	IIC_Send_Byte(wdata);                  //��������
	if(IIC_Wait_Ack(100)!=0) return 3;     //�ȴ�Ӧ�𣬴���Ļ�������3
	IIC_Stop();    

	return 0;
}


uint8_t AT24C02_WritePage(uint8_t addr, uint8_t *wdata)	
{
	uint8_t i;
	
	IIC_Start();
	IIC_Send_Byte(AT24C02_WADDR);					//д�豸��ַ
	if(IIC_Wait_Ack(100) != 0)	return 1;			//�ȴ�Ӧ���ź���������򷵻�
	IIC_Send_Byte(addr);							//д�洢��ַ
	if(IIC_Wait_Ack(100) != 0)	return 2;			//�ȴ�Ӧ���ź���������򷵻�
	
	for(i = 0;i < 8; i++)
	{
		IIC_Send_Byte(wdata[i]);					//д����
		if(IIC_Wait_Ack(100) != 0)	return 3+i;		//�ȴ�Ӧ���ź���������򷵻�
	}
	
	IIC_Stop();	
	return 0;
}

uint8_t AT24C02_ReadByte(uint8_t addr, uint8_t *rdata)	//
{
	IIC_Start();
	IIC_Send_Byte(AT24C02_WADDR);					//д�豸��ַ
	if(IIC_Wait_Ack(100) != 0)	return 1;	//�ȴ�Ӧ���ź���������򷵻�
	IIC_Send_Byte(addr);									//д�洢��ַ
	if(IIC_Wait_Ack(100) != 0)	return 2;	//�ȴ�Ӧ���ź���������򷵻�
	IIC_Start();
	IIC_Send_Byte(AT24C02_RADDR);	
	if(IIC_Wait_Ack(100) != 0)	return 3;
	

	*rdata = IIC_Read_Byte(0);
	
	IIC_Stop();	
	return 0;
}

//uint8_t AT24C02_ReadData(uint8_t addr, uint8_t *rdata, uint16_t datalen)
//{
//	uint8_t i;                                  //����forѭ��
//	
//	IIC_Start();                                //������ʼ�ź�
//	IIC_Send_Byte(AT24C02_WADDR);                //����24C02д����������ַ
//	if(IIC_Wait_Ack(100)!=0) return 1;          //�ȴ�Ӧ�𣬴���Ļ�������1
//	IIC_Send_Byte(addr);                        //�����ڲ��洢�ռ��ַ
//	if(IIC_Wait_Ack(100)!=0) return 2;          //�ȴ�Ӧ�𣬴���Ļ�������2
//	IIC_Start();                                //�ٴη�����ʼ�ź�
//	IIC_Send_Byte(AT24C02_RADDR);                //����24C02������������ַ
//	if(IIC_Wait_Ack(100)!=0) return 3;          //�ȴ�Ӧ�𣬴���Ļ�������3
//	for(i=0;i<datalen-1;i++)
//	{                   						//ѭ��datalen-1�Σ�һ���ֽ�һ���ֽڽ�������
//		rdata[i] = IIC_Read_Byte(1);            //�������ݣ�����Ӧ����ӻ�
//	}
//	rdata[datalen-1] = IIC_Read_Byte(0);        //�������һ���ֽڣ�������Ӧ���ź�
//	IIC_Stop();                                 //����ֹͣ�ź�
//	return 0;		                            //��ȷ������0
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


//ÿ��д8���ֽ�
void AT24C02_WriteOTAInfo(void)
{
	for(uint8_t i=0;i<OTA_InfoCB_SIZE/8;i++)
	{
		AT24C02_WritePage(i*8,(uint8_t *)&OTA_Info+i*8);
		Delay_Ms(5);				//��HAL����ʱ�������⿨��
	}
}






