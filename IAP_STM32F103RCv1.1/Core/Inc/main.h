/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
#define 		STM32_FLASH_SADDR		0X08000000
#define 		STM32_PAGE_SIZE			2048
#define 		STM32_PAGE_NUM			128
#define 		STM32_B_PAGE_NUM		10
#define 		STM32_A_PAGE_NUM		STM32_PAGE_NUM - STM32_B_PAGE_NUM
#define			STM_A_START_PAGE		STM32_B_PAGE_NUM
//#define 		STM32_A_SADDR			STM32_FLASH_SADDR + STM_A_START_PAGE * STM32_PAGE_SIZE
#define 		STM32_A_SADDR           0x08010000
#define 		OTA_SET_FLAG			0xAABB1122    //小端模式，低字节在低地址
//#define 		OTA_SET_FLAG			0x03020100


#define 		UPDATA_A_FLAG			0x00000001

typedef struct{
	uint32_t OTA_flag;
	uint32_t OTA_FileLen[11];
}OTA_InfoCB;

#define			OTA_InfoCB_SIZE			sizeof(OTA_InfoCB)
	
typedef struct{
	uint8_t  Updatabuff[STM32_PAGE_SIZE];
	uint32_t W25Q64_BlockNB; 
}UpDataA_CB;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
extern OTA_InfoCB OTA_Info;
extern UpDataA_CB UpDataA;
extern uint32_t  BootStateFlag;
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
