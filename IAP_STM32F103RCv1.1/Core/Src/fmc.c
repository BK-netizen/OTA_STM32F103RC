#include "fmc.h"

void FLASH_ERASE(uint16_t start_page,uint16_t num)
{
	uint32_t erase_addr;
	FLASH_EraseInitTypeDef Flash_InitStruct;
	Flash_InitStruct.TypeErase 			= FLASH_TYPEERASE_PAGES;
	Flash_InitStruct.PageAddress 		= FLASH_ADDR + start_page*2048;
	Flash_InitStruct.NbPages 			= num;    			//ÒÔ2KÀ´²Á³ý
	
	HAL_FLASH_Unlock();
	HAL_FLASHEx_Erase(&Flash_InitStruct,&erase_addr);
	HAL_FLASH_Lock();
}

void FLASH_WRITE(uint32_t waddr,uint32_t *wdata,uint32_t datalen)
{
	HAL_FLASH_Unlock();
//	for(i=0;i<datalen;i+=4)
//	{
//		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(waddr + FLASH_ADDR + (i*1024*4)),*wdata);
//		waddr += 4;
//		wdata++;
//	}
	while(datalen)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,waddr,*wdata);
		datalen -= 4;
		waddr += 4;
		wdata ++;
	}
	
	HAL_FLASH_Lock();
}
	
