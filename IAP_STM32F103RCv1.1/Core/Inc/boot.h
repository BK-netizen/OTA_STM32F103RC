#ifndef __BOOT_H__
#define __BOOT_H__

#include "main.h"
#include "usart.h"
#include "spi.h"


typedef void (*load_a)(void);                            //����ָ����������
void LOAD_A(uint32_t addr);
void BOOT_Branch(void);





#endif 



