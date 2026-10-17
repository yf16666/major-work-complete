#ifndef BSP_GPIO_H
#define BSP_GPIO_H

#include "stm32f10x.h" 

void Bsp_GPIO_Init(void);

void Bsp_setFans(u8 status);//单独的风扇(DC)
void Bsp_setHumidifier(u8 status);//加湿器

void Bsp_setBeep(u8 status);//蜂鸣器（低电平触发）
void Bsp_toggleBeep(void);


void Bsp_setMotor(u8 status);//马达风扇(马达加扇叶）
#endif



