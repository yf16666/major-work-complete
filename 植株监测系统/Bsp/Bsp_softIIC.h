#ifndef BSP_SOFTIIC_H
#define BSP_SOFTIIC_H

#include <stm32f10x.h>
#include	"Delay.h"


void Bsp_softIIC_Init(void);

void BspIIC_start(void);

void BspIIC_senAck(u8 ack);

u8 BspIIC_waitAck(void);

void BspIIC_senByte(u8 byte);

u8 BspIIC_receByte(void);

void BspIIC_Stop(void);
















#endif

