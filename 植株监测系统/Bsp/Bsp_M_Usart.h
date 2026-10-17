#ifndef Bsp_M_USART_H
#define Bsp_M_USART_H


#include "stm32f10x.h"                  // Device header
#include "stdio.h"
#include "string.h"
#include <stdarg.h>

#define USART_Buf_Len 256

/*
typedef struct{
	USART_TypeDef* USARTx;
	u32	Baud;
	u8	PreemptionPriority;
	u8	SubPriority;
	
}Usart;

void Usartx_Init(Usart* Usartx);
*/

typedef void (*M_Print)(char *fmt, ...);//声明一个函数类型(函数指针类型)
typedef struct Usart	Usart;
typedef struct Status	Status;
struct Status{
	u8	receOk;
	u8	dataLen;
	u8	Data[255];
};

struct Usart{
	USART_TypeDef* USARTx;
	u32						Baud;
	u8						PreemptionPriority;//抢占
	u8						SubPriority;//响应
	void (*Init)	(Usart*,u8);
	M_Print 			Print;
	Status 		*status;
};
void Usartx_Init(Usart* Usartx,u8 idle_status);


//extern Usart Usart1 ;
//extern Usart Screen ;
















#endif


