#ifndef Drivers_ADC_H
#define Drivers_ADC_H

#include "Bsp_ADC.h"
#include "App_Models.h"


u8 SensorStartSampl(AppModel *model,u16 *sampVal,u8 currSampTimes);

u8 filterCycleAdcValue(u16 *sampVal,u8 *currSamlTimes,u16 *filterVal);



#endif
