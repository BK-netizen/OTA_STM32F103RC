#include "W25Q64.h"


void W25Q64_Init(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef 				  GPIO_InitStruct;
	GPIO_InitStruct.Pin 			= GPIO_PIN_2;
	GPIO_InitStruct.Mode 			= GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull			= GPIO_NOPULL;
	GPIO_InitStruct.Speed			= GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	CS_DISENABLE;
	MX_SPI1_Init();
}

void W25Q64_WaitBusy(void)
{
	uint8_t state;
	do{
		CS_ENABLE;
		SPI1_ReadWriteByte(0x05);
		state = SPI1_ReadWriteByte(0xff);
		CS_DISENABLE;
	}while((state & 0x01) == 0x01);
}

void W25Q64_WriteEnable(void)
{
	W25Q64_WaitBusy();
	CS_ENABLE;
	SPI1_ReadWriteByte(0x06);	
	CS_DISENABLE;
}


void W25Q64_Erase64K(uint8_t blockNB)
{
	uint8_t wdata[4];
	wdata[0]   = 0xD8;
	wdata[1]   = (blockNB*64*1024) >>16;
	wdata[2]   = (blockNB*64*1024) >>8;
	wdata[3]   = (blockNB*64*1024) >>0;
	
	W25Q64_WaitBusy();
	W25Q64_WriteEnable();
	CS_ENABLE;
	SPI1_Write(wdata,4);
	CS_DISENABLE;
	W25Q64_WaitBusy();
}

void W25Q64_PageWrite(uint8_t *wbuff, uint16_t pageNB)
{
	uint8_t wdata[4];                  //保存命令和地址
	
	wdata[0] = 0x02;                   //页写入数据指令0x02
	wdata[1] = (pageNB*256)>>16;       //需要写入数据的页地址
	wdata[2] = (pageNB*256)>>8;        //需要写入数据的页地址
	wdata[3] = (pageNB*256)>>0;        //需要写入数据的页地址
	
	W25Q64_WaitBusy();                 //等待W25Q64空闲
	W25Q64_WriteEnable();                   //使能
	CS_ENABLE;                         //打开CS
	SPI1_Write(wdata,4);               //把wdata发送给W25Q64
	SPI1_Write(wbuff,256);             //接着发送256字节（一页）的数据
	CS_DISENABLE;                      //关闭CS
}


void W25Q64_Read(uint8_t *rbuff, uint32_t addr, uint32_t datalen)
{
	uint8_t wdata[4];          //保存命令和地址
	
	wdata[0] = 0x03;           //读取数据指令0x03
	wdata[1] = (addr)>>16;     //读取数据的地址
	wdata[2] = (addr)>>8;      //读取数据的地址
	wdata[3] = (addr)>>0;      //读取数据的地址
	
	W25Q64_WaitBusy();         //等待W25Q64空闲
	CS_ENABLE;                 //打开CS
	SPI1_Write(wdata,4);       //把wdata发送给W25Q64
	SPI1_Read(rbuff,datalen);  //读取datalen个字节数据
	CS_DISENABLE;              //关闭CS
}




