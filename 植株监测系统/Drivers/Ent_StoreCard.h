#ifndef ENT_STORECARD_H
#define ENT_STORECARD_H



#include "stm32f10x.h"                  // Device header

#define sd_ThresNum 						5			
#define sd_dataLength 						(sizeof(u8))
#define sd_lineLength 					255u			
#define sdAddr_defaultMax 				0x020000UL
#define sdAddr_defaultMin 			0x021000UL
#define sdAddr_userMaxThres 			0x030000UL
#define sdAddr_userMinThres 		0x040000UL
#define sdAddr_ledSTA 						0x050000UL

typedef enum SD_Status SD_Status;

enum SD_Status{
	SD_Error,
	SD_Ok,
	SD_Busy,
	SD_emptyPage,
	SD_timeOut,
	SD_nullPointer
};

u8 Ent_StoreCard_Init(void);
u8 Ent_checkHardWare(void);


SD_Status Sd_eraseTheSector(u32 Addr_Sector);//²Á³ýÖ¸¶¨ÉÈÇø

u16 Sd_getPagedataNum(u32 AddrPage);
SD_Status Sd_writData(u32 Addr,u8 data);
void OutUserSectordataNum(u8 maxOrmin);

SD_Status Sd_readSector0Pagedata(u32 Addr_Sector,u8 *dataArray);
SD_Status Sd_readFinaldata(u32 Addr_Page,u8 *data);
SD_Status Sd_cleanUserSector(u8 maxOrmin,u16 whichSector);
#endif



