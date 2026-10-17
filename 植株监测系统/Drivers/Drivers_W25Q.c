#include "Drivers_W25Q.h"

#include	"Delay.h"
#include	"Bsp_Spi.h"

#define W_WRITE_ENABLE												0x06
#define W_WRITE_DISABLE												0x04
#define W_READ_STATUS_REGISTER_1							0x05
#define W_READ_STATUS_REGISTER_2							0x35
#define W_WRITE_STATUS_REGISTER								0x01
#define W_PAGE_PROGRAM												0x02
#define W_QUAD_PAGE_PROGRAM										0x32
#define W_BLOCK_ERASE_64KB										0xD8
#define W_BLOCK_ERASE_32KB										0x52
#define W_SECTOR_ERASE_4KB										0x20
#define W_CHIP_ERASE													0xC7
#define W_ERASE_SUSPEND												0x75
#define W_ERASE_RESUME												0x7A
#define W_POWER_DOWN													0xB9
#define W_HIGH_PERFORMANCE_MODE								0xA3
#define W_CONTINUOUS_READ_MODE_RESET					0xFF
#define W_RELEASE_POWER_DOWN_HPM_DEVICE_ID		0xAB
#define W_MANUFACTURER_DEVICE_ID							0x90
#define W_READ_UNIQUE_ID											0x4B
#define W_JEDEC_ID														0x9F
#define W_READ_DATA														0x03
#define W_FAST_READ														0x0B
#define W_FAST_READ_DUAL_OUTPUT								0x3B
#define W_FAST_READ_DUAL_IO										0xBB
#define W_FAST_READ_QUAD_OUTPUT								0x6B
#define W_FAST_READ_QUAD_IO										0xEB
#define W_OCTAL_WORD_READ_QUAD_IO							0xE3

#define W_DUMMY_BYTE											0xFF

void W25Q64_Init(void){
	Bsp_SPI_Init();					
}

void W25Q_WriteEnable(void){
	Bsp_SPI_Start;								//SPI起始
	Bsp_SPI_SwapByte(W_WRITE_ENABLE);		//交换发送写使能的指令
	Bsp_SPI_Stop;								//SPI终止
}



W_Status W25Q_waitBusy(void){//当使用软件SPI时，应该加入  关中断(__disable_irq())函数
	uint32_t Timeout = 100000;
	u8 status;
	while(1)
	{
		Bsp_SPI_Start;
		Bsp_SPI_SwapByte(W_READ_STATUS_REGISTER_1);
		status = Bsp_SPI_SwapByte(W_DUMMY_BYTE);
		Bsp_SPI_Stop;

		if( (status & 0x01) == 0 )
		{
			return W_Ok; //空闲，退出
		}
		Timeout--;
		if(Timeout == 0)
		{
			return W_timeOut;
		}
	}
}


W_Status W25Q_writeData(u32 Addr_pageLine,u8 data){
	W25Q_WriteEnable();
	Bsp_SPI_Start;								//SPI起始
	Bsp_SPI_SwapByte(W_PAGE_PROGRAM);		//交换发送页编程的指令
	
	Bsp_SPI_SwapByte(Addr_pageLine >> 16);
	Bsp_SPI_SwapByte(Addr_pageLine >> 8);	
	Bsp_SPI_SwapByte(Addr_pageLine);
		Bsp_SPI_SwapByte(data);//硬件SPI自动完成发送数据
	
	Bsp_SPI_Stop;
	return W25Q_waitBusy();
}


W_Status W25Q_readData(u32 Addr_pageLine,u8 *data){
	
	Bsp_SPI_Start;								//SPI起始
	Bsp_SPI_SwapByte(W_READ_DATA);		//交换发送页编程的指令
	
	Bsp_SPI_SwapByte(Addr_pageLine >> 16);
	Bsp_SPI_SwapByte(Addr_pageLine >> 8);	
	Bsp_SPI_SwapByte(Addr_pageLine);
	
		*data = Bsp_SPI_SwapByte(W_DUMMY_BYTE);//硬件SPI自动完成交换的数据
//	Usart1.Print("W25Q_readData,Addr:0x%06X data:0x%X\r\n",Addr_pageLine,*data);
	Bsp_SPI_Stop;
	
	return W_Ok;//W25Q_waitBusy();
}


W_Status W25Q64_ReadID(u8 *MID, u16 *DID){
	Bsp_SPI_Start;								//SPI起始
	Bsp_SPI_SwapByte(W_JEDEC_ID);			//交换发送读取ID的指令
	*MID = Bsp_SPI_SwapByte(W_DUMMY_BYTE);	//交换接收MID，通过输出参数返回
	*DID = Bsp_SPI_SwapByte(W_DUMMY_BYTE);	//交换接收DID高8位
	*DID <<= 8;									//高8位移到高位
	*DID |= Bsp_SPI_SwapByte(W_DUMMY_BYTE);	//或上交换接收DID的低8位，通过输出参数返回
	Bsp_SPI_Stop;								//SPI终止
	return W_Ok;
}

W_Status W25Q64_SectorErase(u32 Addr_Sector){
	W25Q_WriteEnable();						//写使能
	
	Bsp_SPI_Start;									//SPI起始
	Bsp_SPI_SwapByte(W_SECTOR_ERASE_4KB);	//交换发送扇区擦除的指令
	Bsp_SPI_SwapByte(Addr_Sector >> 16);				//交换发送地址23~16位
	Bsp_SPI_SwapByte(Addr_Sector >> 8);				//交换发送地址15~8位
	Bsp_SPI_SwapByte(Addr_Sector);					//交换发送地址7~0位
	Bsp_SPI_Stop;							//SPI终止
	
	return W25Q_waitBusy();							//等待忙
}
















