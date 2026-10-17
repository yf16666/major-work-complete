#ifndef Drivers_W25Q_H
#define Drivers_W25Q_H

#include	 	<stm32f10x.h>


typedef enum W_Status W_Status;
enum W_Status{
	W_Error,
	W_Ok,
	W_None,
	W_Busy,
	W_timeOut
};

void W25Q64_Init(void);
void W25Q_WriteEnable(void);
W_Status	W25Q_waitBusy(void);
W_Status W25Q_writeData(u32 Addr_pageLine,u8 data);
W_Status W25Q_readData(u32 Addr_pageLine,u8 *data);
W_Status W25Q64_ReadID(u8 *MID, u16 *DID);

W_Status W25Q64_SectorErase(u32 Addr_Sector);
#endif
