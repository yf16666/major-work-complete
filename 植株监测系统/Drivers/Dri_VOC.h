#ifndef		DRI_VOC_H
#define		DRI_VOC_H

#include "stm32f10x.h"                  // Device header


typedef	struct RingBuf	RingBuf;
#define		rbSize	100
struct RingBuf{
	u8 cnt;
	u8 wIndex;
	u8 rIndex;
	u8 data[rbSize];
};


u8 Dri_readByte(RingBuf *rb,u8 *data);
u8 Dri_writeByte(RingBuf *rb,u8 ch);
void Dri_vocPraseData(RingBuf *rb,int16_t *temp,int16_t *humi);


#endif



