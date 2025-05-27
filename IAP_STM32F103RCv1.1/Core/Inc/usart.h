/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "stdint.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

/* USER CODE BEGIN Private defines */
#define U0_RX_SIZE 2048                 //接收缓冲区长度
#define U0_TX_SIZE 2048                 //发送缓冲区长度
#define U0_RX_MAX  256                  //单次接收最大量
#define NUM        10                   //se指针对结构体数组长度


/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);

/* USER CODE BEGIN Prototypes */
typedef struct{                         
	uint8_t *start;                     //s用于标记起始位置
	uint8_t *end;                       //e用于标记结束位置
}UCB_URxBuffptr;                        //se指针对结构体

typedef struct{
	uint16_t URxCounter;                //累计接收数据量
	UCB_URxBuffptr URxDataPtr[NUM];     //se指针对结构体数组
	UCB_URxBuffptr *URxDataIN;          //指针用于标记接收数据
	UCB_URxBuffptr *URxDataOUT;         //OUT指针用于提取接收的数据
	UCB_URxBuffptr *URxDataEND;         //IN和OUT指针的结尾标志
}UCB_CB;                                //串口控制结构体

extern UCB_CB  U0CB;                    //变量外部声明
extern uint8_t U0_RxBuff[U0_RX_SIZE];   //变量外部声明
void U0Rx_PtrInit(void);
void u0_printf(char *format, ...);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
