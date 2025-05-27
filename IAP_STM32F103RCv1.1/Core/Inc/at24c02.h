#ifndef __AT24C02_H__
#define __AT24C02_H__

#include "main.h"
#include "iic.h"

#define AT24C02_WADDR  0xA0     //24C02写操作器件地址
#define AT24C02_RADDR  0xA1     //24C02读操作器件地址


uint8_t AT24C02_Write_Byte(uint8_t addr,uint8_t wdata);
uint8_t AT24C02_WritePage(uint8_t addr, uint8_t *wdata);
uint8_t AT24C02_ReadByte(uint8_t addr, uint8_t *rdata);
uint8_t AT24C02_ReadData(uint8_t addr, uint8_t *rdata, uint16_t datalen);

#endif


