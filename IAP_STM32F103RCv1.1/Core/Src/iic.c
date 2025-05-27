#include "iic.h"

void IIC_Init(void)
{
	GPIO_InitTypeDef   GPIO_Init_Struct;
	
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	//SCL
	GPIO_Init_Struct.Pin = GPIO_PIN_12;
	GPIO_Init_Struct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_Init_Struct.Speed = GPIO_SPEED_FREQ_HIGH;
	//GPIO_Init_Struct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOC,&GPIO_Init_Struct);
	
	//SDA
	GPIO_Init_Struct.Pin = GPIO_PIN_11;
	GPIO_Init_Struct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_Init_Struct.Speed = GPIO_SPEED_FREQ_HIGH;
	//GPIO_Init_Struct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOC,&GPIO_Init_Struct);
	
	IIC_SCL_H;
	IIC_SDA_H;
}

void IIC_Start(void)
{
	IIC_SCL_H;
	IIC_SDA_H;
	Delay_Us(4);
	IIC_SDA_L;
	Delay_Us(4);
	IIC_SCL_L;
}

void IIC_Stop(void)
{
	//IIC_SCL_L;
	IIC_SDA_L;
	Delay_Us(2);
	IIC_SCL_H;
	Delay_Us(5);
	IIC_SDA_H;
}

//void IIC_Stop(void)
//{
//	IIC_SCL_L;
//	IIC_SDA_L;
//	Delay_Us(2);
//	IIC_SCL_H;        //拉高SCL
//	Delay_Us(5);
////	IIC_SDA_L;        //拉低SDA
////	Delay_Us(2);      //延时
//	IIC_SDA_H;        //拉高SDA
//}

void IIC_Send_Byte(uint8_t txd)
{
	int i;
	for(i = 7;i >=0;i--)
	{
		IIC_SCL_L;
		Delay_Us(2);
		if(txd & BIT(i))
		{
			IIC_SDA_H;
		}
		else
		{
			IIC_SDA_L;
		}
		Delay_Us(2);
		IIC_SCL_H;
	}
	IIC_SCL_L;
	IIC_SDA_H;
}

uint8_t IIC_Wait_Ack(int16_t timeout)
{
	do
	{
		timeout --;
		Delay_Us(2);
	}while((READ_SDA) && (timeout >= 0));
	if(timeout < 0)
	{
		return 1;
	}
	IIC_SCL_H;
	Delay_Us(2);
	if(READ_SDA)   //确保是稳定ack信号
	{
		return 2;
	}
	IIC_SCL_L;
	Delay_Us(2);
	return 0;
}

uint8_t IIC_Read_Byte(uint8_t ack)
{
	int8_t i;
	uint8_t rxd = 0;

	for(i = 7; i >= 0; i--)
	{
		IIC_SCL_L;
		Delay_Us(2);
		IIC_SCL_H;
		if(READ_SDA)
		{
			rxd |= BIT(i);
		}
		Delay_Us(2);
	}
	IIC_SCL_L;
	Delay_Us(2);
	if(ack)
	{
		IIC_SDA_L;
		IIC_SCL_H;
		Delay_Us(2);
		IIC_SCL_L;
		IIC_SDA_H;
		Delay_Us(2);
	}
	else
	{
		IIC_SDA_H;
		IIC_SCL_H;
		Delay_Us(2);
		IIC_SCL_L;
		Delay_Us(2);
	}
	return rxd;
}




