#ifndef __W25Q64_H__
#define __W25Q64_H__

#include "main.h"
#include "spi.h"

#define CS_ENABLE				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_RESET)
#define CS_DISENABLE			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET)


void W25Q64_Init(void);
void W25Q64_WaitBusy(void);
void W25Q64_WriteEnable(void);
void W25Q64_Erase64K(uint8_t blockNB);
void W25Q64_PageWrite(uint8_t *wbuff, uint16_t pageNB);
void W25Q64_Read(uint8_t *rbuff, uint32_t addr, uint32_t datalen);

#endif
