#ifndef __BOOT_H__
#define __BOOT_H__

#include "main.h"
#include "usart.h"
#include "spi.h"
#include "fmc.h"


typedef void (*load_a)(void);                            //函数指针类型声明
void LOAD_A(uint32_t addr);
void BOOT_Branch(void);
uint8_t BootLoader_Enter(uint8_t timeout);
void BootLoader_Info(void);
void BootLoader_Event(uint8_t *data, uint16_t datalen);
uint16_t XmodemCRC16(uint8_t *data,uint16_t datalen);




#endif 



