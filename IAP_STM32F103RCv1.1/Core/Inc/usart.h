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
#define U0_RX_SIZE 2048                 //���ջ���������
#define U0_TX_SIZE 2048                 //���ͻ���������
#define U0_RX_MAX  256                  //���ν��������
#define NUM        10                   //seָ��Խṹ�����鳤��


/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);

/* USER CODE BEGIN Prototypes */
typedef struct{                         
	uint8_t *start;                     //s���ڱ����ʼλ��
	uint8_t *end;                       //e���ڱ�ǽ���λ��
}UCB_URxBuffptr;                        //seָ��Խṹ��

typedef struct{
	uint16_t URxCounter;                //�ۼƽ���������
	UCB_URxBuffptr URxDataPtr[NUM];     //seָ��Խṹ������
	UCB_URxBuffptr *URxDataIN;          //ָ�����ڱ�ǽ�������
	UCB_URxBuffptr *URxDataOUT;         //OUTָ��������ȡ���յ�����
	UCB_URxBuffptr *URxDataEND;         //IN��OUTָ��Ľ�β��־
}UCB_CB;                                //���ڿ��ƽṹ��

extern UCB_CB  U0CB;                    //�����ⲿ����
extern uint8_t U0_RxBuff[U0_RX_SIZE];   //�����ⲿ����
void U0Rx_PtrInit(void);
void u0_printf(char *format, ...);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
