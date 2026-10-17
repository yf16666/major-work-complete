#ifndef APP_VIEWS_H
#define APP_VIEWS_H

#include <stm32f10x.h>


#include	"Hmi_Protocol.h"
#include 	"App_Models.h"
#include 	"Bsp_M_Usart.h"
#include	"Drivers_ADC.h"


void 	 View_setLed(AppModel *model);
void 	 View_sendThreshold(AppModel *model);
void	 View_saveSingleThres(AppModel *model,HMI_Event *event);
void	 View_showCurrrEvent(HMI_Event *event);
void 	 View_recoverThres(u8 maxOrmin,AppModel *model,HMI_Event *event);


void View_sendSoil(u16 soilVal);
void View_sendTemp(int16_t temp);
void View_sendHumi(int16_t Humi);
void View_sendLumin(int16_t Lumin);
void View_sendGrowth(int16_t grow);

void View_readLedSTA(AppModel *model,HMI_Event *event);

void View_updateHomePageValue(AppModel *model,u8 adcOrIndoor);/////修改发送标志位为 满足TIM3的发送周期
void 	View_processAdcFillter(AppModel *model);

void View_checkAlermStatus(AppModel *model);

void View_tempEvent(AppModel *model);
#endif
