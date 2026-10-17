#ifndef Bsp_SPI_H
#define Bsp_SPI_H

#include <stm32f10x.h>

#define Bsp_SPI_SS(x)		GPIO_WriteBit(GPIOA,GPIO_Pin_4,(BitAction)x);//SS默认高电平  低电平->开始时序
#define	Bsp_SPI_Start		GPIO_WriteBit(GPIOA,GPIO_Pin_4,(BitAction)0)
#define	Bsp_SPI_Stop		GPIO_WriteBit(GPIOA,GPIO_Pin_4,(BitAction)1)

void Bsp_SPI_Init(void);

u8 Bsp_SPI_SwapByte(u8 byte);


#endif




