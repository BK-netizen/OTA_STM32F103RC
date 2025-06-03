#ifndef __BOOT_H__
#define __BOOT_H__

#include "main.h"
#include "usart.h"
#include "spi.h"


typedef void (*load_a)(void);                            //函数指针类型声明
void LOAD_A(uint32_t addr);
void BOOT_Branch(void);





#endif 



