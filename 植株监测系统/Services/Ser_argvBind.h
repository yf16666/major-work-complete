#ifndef		SER_ARGVBIND_H
#define		SER_ARGVBIND_H


#include "stm32f10x.h"                  // Device header

typedef enum SD_Status SD_Status;

SD_Status Ser_checkHardFault(volatile u8 *status);

SD_Status Ser_bindThre1Model(u8 maxOrmin,u8 *model);
SD_Status Ser_updateTheThres(u8 maxOrmin,u8 thresInd,u8 newThres);


SD_Status Ser_getNewLedSTA(u8 *ledSTA);
SD_Status Ser_updataLedSTA(u8 *newSTA);

SD_Status Ser_cleanAlluserThres(u8 maxOrmin,u8 *modelArray);



#endif
