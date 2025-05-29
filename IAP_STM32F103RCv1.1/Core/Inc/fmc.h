#ifndef __FMC_H__
#define __FMC_H__

#include "main.h"

#define FLASH_ADDR			0X08000000

void FLASH_ERASE(uint16_t start_page,uint16_t num);
void FLASH_WRITE(uint32_t waddr,uint32_t *wdata,uint32_t datalen);

#endif
