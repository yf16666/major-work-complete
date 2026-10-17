#ifndef ALSERTASK_H
#define ALSERTASK_H

#include "stm32f10x.h"                  // Device header


void alermTask_tempOver(u16 *currTime,u8 flag);
void alermTask_tempLess(u16 *currTime,u8 flag);
void alermTask_humiOver(u16 *currTime,u8 flag);
void alermTask_humiLess(u16 *currTime,u8 flag);
void alermTask_luminOver(u16 *currTime,u8 flag);
void alermTask_soilOver(u16 *currTime,u8 flag);
void alermTask_soilLess(u16 *currTime,u8 flag);

#endif


