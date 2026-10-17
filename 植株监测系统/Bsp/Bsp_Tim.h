#ifndef BSP_TIM_H
#define BSP_TIM_H

#include "stm32f10x.h" 

void Bsp_Tim_Init(u16 Psc,u16 Arr);			
//		¡¾TIM3¡ª¡ªIRQHandler¡¿---->APP_Controller.c

void Bsp_ledPWM_Init(void);
void Drivers_ledSetCompare(u16 ledCompare);
#endif














