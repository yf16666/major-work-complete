#ifndef HIM_PROTOCOL_H
#define HIM_PROTOCOL_H

#include "Bsp_M_Usart.h"

typedef enum{
	ev_none,
	ev_getAdcValue,//1
	ev_readAllthres,
	ev_readLedSTA,//3
	ev_changeSingleThres,
	ev_recoverMaxThres,//5
	ev_recoverMinThres,
	ev_recoverAllThres,//7
	ev_switchLed,
	ev_hmiStart,//9
	ev_IDLE
}himEventType;

typedef enum{
	hello,
	home,
	liveData,
	thresSetting,
	setting,
	unknowStatus
}hmiPage;

typedef struct HMI_Event HMI_Event;

struct HMI_Event{
	himEventType evtype;
	hmiPage currPage;
	u8	thresNo;//阈值序号
	u8 	thresType;//阈值类型：最大小
	u8 	thresValue;
};





//void clearScreenData(Usart *screen);

//u8 ParseScreenProtocol(const u8 *data,const u8 dataLen,HMI_Event *hmiEvent);


void	Ser_screenDataParse(Usart *screen,HMI_Event *event);




#endif




