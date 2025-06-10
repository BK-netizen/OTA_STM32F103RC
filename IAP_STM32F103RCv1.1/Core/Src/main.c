/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "delay.h"
#include "at24c02.h"
#include "spi.h"
#include "W25Q64.h"
#include "fmc.h"
#include "boot.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
uint32_t    BootStateFlag;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
OTA_InfoCB OTA_Info;  
UpDataA_CB UpDataA;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint32_t wbuff[1024];
uint8_t rbuff[50];
uint8_t crcbuf[5] = {0x01,0x02,0x03,0x04,0x05};
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  //HAL_Delay(10);
  W25Q64_Init();
  HAL_UART_Receive_DMA(&huart1, (uint8_t *)U0_RxBuff, U0_RX_MAX + 1);
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
  U0Rx_PtrInit();
  IIC_Init();
  
//  OTA_Info.OTA_flag =0xAABB1122;
//  for(uint8_t i=0;i<11;i++)
//  {
//	OTA_Info.OTA_FileLen[i] = i;
//  }
//  AT24C02_WriteOTAInfo();
//  AT24C02_ReadOTAInfo();
//  AT24C02_ReadData(0,rbuff,OTA_InfoCB_SIZE);
//  for(uint8_t i=0;i<50;i++)
//  {
//	u0_printf("OTA_flag = %x\r\n",rbuff[i]);
//  }
  BOOT_Branch();

  //static uint32_t wbuff[1024];
//  uint32_t i;
//  for(i=0;i<1024;i++)
//  {
//	wbuff[i] = 0x12345678;
//  }
//  FLASH_ERASE(50,2);
//  HAL_Delay(10);
//  FLASH_WRITE(50*2048+0x08000000,wbuff,1024*4);
//  HAL_Delay(10);
////  u0_printf("write success\r\n");
//  for(i=0;i<1024;i++)
//  {
//	u0_printf("%x\r\n",(*((uint32_t *)(50*2048+0x08000000+(i*4)))));
//  }
//  u0_printf("%x\r\n",*((uint32_t *)(0x08019FF2)));

 
//  uint16_t i,j; 
//  uint8_t wdata[256];
//  uint8_t rdata[256];
//  W25Q64_Erase64K(0);
//  for(i= 0;i< 256;i++)
//  {
//	for(j = 0;j<256;j++)
//	{
//		wdata[j] = i;
//	}
//	W25Q64_PageWrite(wdata,i);
//  }
//  HAL_Delay(50);
//  for(i= 0;i< 256;i++)
//  {
//	W25Q64_Read(rdata,i*256,256);
//	for(j = 0;j<256;j++)
//	{
//		u0_printf("地址%d =%x\r\n",i*256+j,rdata[j]);
//	}
//  }
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  Delay_Ms(10);
		if(U0CB.URxDataOUT != U0CB.URxDataIN)
		{                                                                 										//IN 和 OUT不相等的时候进入if，说明缓冲区有数据了
			BootLoader_Event(U0CB.URxDataOUT->start,U0CB.URxDataOUT->end - U0CB.URxDataOUT->start + 1);        //调用BootLoader_Event处理数据
			U0CB.URxDataOUT++;                                                                                 //OUT后挪
			if(U0CB.URxDataOUT == U0CB.URxDataEND)
			{                                                            										//如果挪到了END标记的最后一个成员，进入if
				U0CB.URxDataOUT = &U0CB.URxDataPtr[0];                      									//重新回到数组0号成员
			}
		}
		if(BootStateFlag&IAP_XMODEMC_FLAG)
		{     										//如果IAP_XMODEMC_FLAG标志位置位，表明需要发送C
			if(UpDataA.XmodemTimeout>=100)
			{     	
				u0_printf("C");               
				UpDataA.XmodemTimeout = 0;     	
			}
			UpDataA.XmodemTimeout++;           	
		}	
	  
	  
	  //OTA事件
	  if(BootStateFlag & UPDATA_A_FLAG)
	  {
		  AT24C02_ReadOTAInfo();										//如果单独用6，而不用5命令，不加此段代码会有bug，OTA的值没读过来，导致更新失败
		  if(OTA_Info.OTA_FileLen[UpDataA.W25Q64_BlockNB] % 4 == 0)   //四字节对齐
		  {
				uint8_t  i =0;
			    FLASH_ERASE(STM_A_START_PAGE,STM32_A_PAGE_NUM);
				u0_printf("OTA_FileLen: %x\r\n",OTA_Info.OTA_FileLen[UpDataA.W25Q64_BlockNB]);
			    for(i=0;i<OTA_Info.OTA_FileLen[UpDataA.W25Q64_BlockNB] /STM32_PAGE_SIZE;i++)   //按页读写	
				{
					W25Q64_Read(UpDataA.Updatabuff,i*STM32_PAGE_SIZE + UpDataA.W25Q64_BlockNB*64*1024,STM32_PAGE_SIZE);    //按block分，一个block64K
					FLASH_WRITE(STM32_A_SADDR+i*STM32_PAGE_SIZE,(uint32_t *)UpDataA.Updatabuff,STM32_PAGE_SIZE);
				}
				if((OTA_Info.OTA_FileLen[UpDataA.W25Q64_BlockNB] % STM32_PAGE_SIZE) != 0)
				{
					W25Q64_Read(UpDataA.Updatabuff,i*STM32_PAGE_SIZE + UpDataA.W25Q64_BlockNB*64*1024,(OTA_Info.OTA_FileLen[UpDataA.W25Q64_BlockNB] % STM32_PAGE_SIZE));    //按block分，一个block64K
					FLASH_WRITE(STM32_A_SADDR+i*STM32_PAGE_SIZE,(uint32_t *)UpDataA.Updatabuff,(OTA_Info.OTA_FileLen[UpDataA.W25Q64_BlockNB] % STM32_PAGE_SIZE));
				}
				if(UpDataA.W25Q64_BlockNB == 0)
				{
					OTA_Info.OTA_flag = 0;
					AT24C02_WriteOTAInfo();
				}
				u0_printf("A区更新完毕\r\n");
				NVIC_SystemReset();
		  }
		  else
		  {
				u0_printf("长度错误\r\n");
				BootStateFlag &= ~UPDATA_A_FLAG;
		  }
	  }

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
