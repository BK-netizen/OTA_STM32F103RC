#ifndef __IIC_H__
#define __IIC_H__

#include "main.h"
#include "delay.h"

#define IIC_SDA_H     HAL_GPIO_WritePin(GPIOC,GPIO_PIN_11,GPIO_PIN_SET)
#define IIC_SDA_L     HAL_GPIO_WritePin(GPIOC,GPIO_PIN_11,GPIO_PIN_RESET)

#define IIC_SCL_H     HAL_GPIO_WritePin(GPIOC,GPIO_PIN_12,GPIO_PIN_SET)
#define IIC_SCL_L     HAL_GPIO_WritePin(GPIOC,GPIO_PIN_12,GPIO_PIN_RESET)

#define READ_SDA	  HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_11)

#define BIT(x)                       ((uint32_t)((uint32_t)0x01U<<(x)))


void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Send_Byte(uint8_t txd);
uint8_t IIC_Wait_Ack(int16_t timeout);
uint8_t IIC_Read_Byte(uint8_t ack);


#endif


