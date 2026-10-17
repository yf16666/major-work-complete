#include "App_Controllers.h"



#include	"Bsp_GPIO.h"



#include	"App.h"
#include	"Ser_argvBind.h"

//W25Q数据读取不正确时，请检查：是否有外部干扰的线接在W25Q上
void App_Run(void){//屏幕数据等 1s/一次的温湿度更新
	
	App_eventRun(&appModel,&hmiEvent);
		Ser_checkHardFault(&appModel.W25HardError);//检查W25Q状态



	App_adcTask(&appModel);
	App_vocTask(&appModel);
	App_getLuminTask(&appModel);
	App_alermCheckTask(&appModel);
	App_alermTask(&appModel);


	App_hmiDataPrase(&Screen,&hmiEvent);
	
}






void TIM3_IRQHandler(void)
{
	static u8 a = 0;//ADC采集任务时钟周期次数
//	static u16 screenCycleTimes = 0;
//	static u16 alermCheckCycleTimes = 0;//告警数据监测时钟周期次数
	u8 i=0;
	
	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		//每秒钟清零时钟周期次数
		a++;
		if(a >= appModel.timerModel->adcSamplCycle_times){//adcSamplCycle_times 周期/次
			appModel.timerModel->adcSamplFlag = 1;
			a = 0;
		}
		for(i=0;i<sensorNum;i++)// && (appModel.alermModel[i].alermState != pass)
			if((appModel.Sensors[i].STA) != error)appModel.alermModel[i].timeCNT++;else appModel.alermModel[i].timeCNT = 0;
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}





































