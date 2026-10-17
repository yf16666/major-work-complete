#ifndef APP_H
#define APP_H

#include <stm32f10x.h>
#include	"App_Models.h"


void 	App_Init(void);
void 	App_adcTask(AppModel *model);
void 	App_vocTask(AppModel *model);
void 	App_getLuminTask(AppModel *model);
void	App_alermCheckTask(AppModel *model);
void 	App_alermTask(AppModel *model);
void 	App_viewTask(AppModel *model);
void 	App_eventRun(AppModel * model,HMI_Event *hmi_event);
void App_hmiDataPrase(Usart *screen,HMI_Event *hmi_event);
//u8 App_calAdcValue_Soil(AppModel *model);
u16 App_getValue_Sun(AppModel *model);



#endif
