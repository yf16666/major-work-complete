#include "Ent_StoreCard.h"


#include <string.h>

#include "Drivers_W25Q.h"
//#define sd_LineLength				0x8

#define debug		1
#if debug
	#include	"App_Models.h"
#endif

//#define sd_ThresNum 						5			
//#define sd_dataLength 						(sizeof(u8))
//#define sd_lineLength 					255u			
//#define sdAddr_defaultMax 				0x020000UL
//#define sdAddr_defaultMin 			0x021000UL
//#define sdAddr_userMaxThres 			0x030000UL	//Max[0]   固定每个阈值独占一个扇区且只写在第0页
//																	0x031000UL	//Max[1]   固定每个阈值独占一个扇区且只写在第0页

//#define sdAddr_userMinThres 		0x040000UL   //Min[0]    固定每个阈值独占一个扇区且只写在第0页
//																0x041000UL	//Min[1]     固定每个阈值独占一个扇区且只写在第0页

/*
扇区：0x000000 -->0x000FFF
扇区个数：0x000FFF-->0xFFFfff即共0xFFF（4095）个扇区  
page：0x000 0 00 --> 0x00 F ff 共0xF  (1 + 15)页
line：0x000 0 00 --> 0x00 0 FF  共 0 ->0xFF (1 + 255)行
*/
//#define sdAddr_ledSTA 						0x050000UL

//#include		"App_View.h"


//extern Usart  Usart1;

u8 Ent_checkHardWare(void){
	u8 a=0;u16 b = 0;
	if(W25Q64_ReadID(&a,&b) == W_Ok)return 1;else return 0;
}

u8 Ent_StoreCard_Init(void){
	W25Q64_Init();
	
	Delay_ms(500);
	return Ent_checkHardWare();
	
}
static SD_Status getW_Status(W_Status wResult){
	switch(wResult){
	case	W_Error:
		return SD_Error;
	case	W_Ok:
		return SD_Ok;
	case	W_None:
		return SD_emptyPage;
	case	W_Busy:
		return SD_Busy;
	case	W_timeOut:
		return SD_timeOut;
	default:
		return SD_Error;
	}
}

SD_Status Sd_writData(u32 Addr,u8 data){
	return getW_Status(W25Q_writeData(Addr,data));
}


#define W_READ_DATA														0x03
#define W_DUMMY_BYTE											0xFF
/*
先连续获取当前扇区-页中已经写的次数，
从而根据设计的数据存储类型的得到每个数据的行偏移量
以此完成业务所需的条件：1，读最后一次更新的数据（读取）；2，写新数据到最新行（保存）
*/
u16 Sd_getPagedataNum(u32 AddrPage){//获取指定扇区-页的数据数量,不能利用单次读数据的函数，读一次拉高又拉低CS
	u8 Num = 0;u8 data = 0;
//	__disable_irq();  //非软件模拟SPI下无需进行中断保护
    Bsp_SPI_Start;   // CS只拉低一次
    Bsp_SPI_SwapByte(W_READ_DATA);
    Bsp_SPI_SwapByte(AddrPage >> 16);
    Bsp_SPI_SwapByte(AddrPage >> 8);
    Bsp_SPI_SwapByte(AddrPage);

    for(u8 i = 0; i < sd_lineLength; i++)
    {
        data = Bsp_SPI_SwapByte(W_DUMMY_BYTE); // 只读1字节，临时变量
        if(data != 0xFF)
        {
            Num++;
        }else break;
        // 可选：提前终止：一旦连续N个0xFF，直接break退出循环
        // else break;
    }
    Bsp_SPI_Stop;    // 读完这一页，CS拉高一次
//		__enable_irq()
	Usart1.Print("getPagedataNum,AddrPage:0x%06X,Num:%d\r\n",AddrPage,Num);
	
	return Num;
}

//先获取指定扇区-页的数量
//从0读到数量的行偏移
SD_Status Sd_readSector0Pagedata(u32 Addr_Sector,u8 *dataArray){//读整个扇区-页的数据到指定的数值中 0x031?00确定的-->固定每个阈值独占一个扇区且只写在第0页
	if(!dataArray)return SD_nullPointer;//空指针
	Addr_Sector = Addr_Sector & 0xFFF000;
	u16 dataNum = Sd_getPagedataNum(Addr_Sector);//得到当前页有效（不包含0xFF的数据的数量）;
	if(!dataNum)return SD_emptyPage;//else if(dataLength == 0xFFFF  ??????)return SD_Error;
	
	Usart1.Print("-->Sd_readPagedata,AddrPage:0x%6X,lineNum:%d\r\n",Addr_Sector,dataNum);
	
	for(u8 i = 0;i < dataNum;i++){ //循环读取，最大数量是获取到的数据的数量；地址的行偏移要加上  0x031?00 + (? &0xFF)
		 W25Q_readData(Addr_Sector + i,(dataArray+i));
		Usart1.Print("Sd_readPagedata,AddrPage:0x%06X,data:%d,lineNum:%d\r\n",Addr_Sector  + i,*(dataArray+i),dataNum);
	}
	return SD_Ok;
}


/*
	先查找该扇区中所有写了的数量->得到行数
	行数 =>最新数据的行偏移
*/
SD_Status Sd_readFinaldata(u32 Addr_Page,u8 *data){
	if(!data)return SD_nullPointer;//空指针
	Addr_Page = Addr_Page & 0xFFF000;
	u16 dataNum = Sd_getPagedataNum(Addr_Page);//得到当前页有效（不包含0xFF的数据）的数量;
	
	if(!dataNum)return SD_emptyPage;//else if(dataLength == 0xFFFF  ?????)return SD_Error;
	
	u8 lineOffset = ((dataNum - 1) * sd_dataLength) & 0xFF;//页偏移：得到的是从1开始的数，所以要减去【存储的数据类型的长度】，得到行的地址

	u32 AddrLine = Addr_Page + lineOffset;
	Usart1.Print("Sd_readPagedata,AddrLine:0x%06X,lineNum:%d\r\n",AddrLine,dataNum);
	return getW_Status(W25Q_readData(AddrLine,data));
	
}


SD_Status Sd_eraseTheSector(u32 Addr_Sector){//擦除指定扇区
	W_Status Wre = W_Error;
	Wre = W25Q64_SectorErase(Addr_Sector);
	return getW_Status(Wre);
}

SD_Status Sd_cleanUserSector(u8 maxOrmin,u16 whichSector){
	if(whichSector > sd_ThresNum)return SD_nullPointer;
	W_Status Wre = W_Error;
	SD_Status re = SD_Error;

		Wre = W25Q64_SectorErase(((maxOrmin)?sdAddr_userMaxThres:sdAddr_userMinThres) + (((u32)whichSector) << 12));
		re = getW_Status(Wre);
	//maxOrmin > 1;
	return re;
}



void OutUserSectordataNum(u8 maxOrmin){
	u16 userSector = 0;
	if(maxOrmin == 1){//0x03? 000
		for(u8 i=0;i<sd_ThresNum;i++){
			userSector = (i << 12);
			Usart1.Print("\r\n[OutUserSectordataNum]i:%d，Num:%d,0x%06X\r\n",i,Sd_getPagedataNum(sdAddr_userMaxThres + userSector),sdAddr_userMaxThres + userSector);
		}
		
	}else if(!maxOrmin){
		for(u8 i=0;i<sd_ThresNum;i++){
			userSector = (i << 12);
			Usart1.Print("\r\n[OutUserSectordataNum]i:%d，Num:%d,0x%06X\r\n",i,Sd_getPagedataNum(sdAddr_userMinThres + userSector),sdAddr_userMinThres + userSector);
		}
	}
}


