#include "App.h"


#include 	"App_Models.h"
#include	"Ser_argvBind.h"
extern Usart		VOC;
//extern RingBuf	VOCrb;

#define	debug		1

extern u8 Ent_StoreCard_Init(void);
void App_Init(void){
	VOC.Init(&VOC,1);
	
	Usart1.Init(&Usart1,0);
	Usart1.Print("Usart1初始化成功!\r\n");
	Screen.Init(&Screen,1);
	Usart1.Print("Usart2初始化成功!\r\n");
	
	
	
	Bsp_SPI_Init();
	Bsp_ADC_Init();
	
	Bsp_Tim_Init(7200,100);//TIM3 10ms/次---->中断
	
	Bsp_ledPWM_Init();		//TIM4	led呼吸灯
	
	Bsp_GPIO_Init();			//蜂鸣器、水泵
	Dri_GY30_Init();
	
	
	AppModel_Init(&appModel);
	//判断W25Q是否存在或是否连接无误
	if(!Ent_StoreCard_Init()){
		Usart1.Print("[W25Q_Init]W25Q故障或SPI/接线出错 ！\r\n");
	}else{
		Usart1.Print("[W25Q_Init]W25Q初始化成功√\r\n");
	}
	
	
}
#include	"App_Views.h"
static void App_updateAppPage(AppModel * model,HMI_Event *hmi_event){//切换页面数据自动更新
	static u8 hmiRun = 0;
	switch(hmi_event->currPage){
		case hello:
			model->appPage = pageHello;
			if(!isScreenLive){
				Usart1.Print("\r\n[App]屏幕首次启动成功√\r\n");
				isScreenLive = 1;
			}else{
				Usart1.Print("\r\n[App]屏幕重新启动成功√\r\n");
				isScreenLive = 0;
			}
			break;
		case home:
			model->appPage = pageHome;//数据由各传感器事件自动更新
			break;
		case liveData://更新传感器状态
			model->appPage = pageLiveDataPage;
			break;
		case thresSetting:
			model->appPage = pageThresSetting;
			break;
		case setting:
			model->appPage = pageSetting;//设置页面：更新LED当前状态
			break;
		case unknowStatus:
			if(!hmiRun){
				Ser_bindThre1Model(1,model->thresModel->max);
				Ser_bindThre1Model(0,model->thresModel->min);
				hmiRun = !hmiRun;
			}
			Usart1.Print("[App]等待屏幕启动中....\r\n");
			break;
		default:
			Usart1.Print("[屏幕]当前页码有误，已自动跳转到首页！\r\n");
			model->appPage = pageHome;
			break;
	}
	
}
void App_eventRun(AppModel * model,HMI_Event *hmi_event){
	App_updateAppPage(&appModel,&hmiEvent);
	
	switch(hmi_event->evtype)//执行屏幕事件
	{
			case ev_none:
					if(hmi_event->currPage == unknowStatus){
						Usart1.Print("[App]等待屏幕启动中....\r\n");
					
					}else{
						Usart1.Print("[App]当前处于空闲状态！\r\n");
					}
					break;

			case ev_getAdcValue://Timer在达到条件后自动转为此状态
					Usart1.Print("[App]获取实时数据！\r\n");
					break;

			case ev_readAllthres:
					Usart1.Print("[App]读取所有阈值！\r\n");
					View_sendThreshold(model);
					break;

			case ev_readLedSTA:
					Usart1.Print("[App]读取[补光灯]状态！\r\n");
						View_readLedSTA(model,hmi_event);
					break;

			case ev_changeSingleThres:
					Usart1.Print("[App]更改单个阈值！\r\n");
					View_saveSingleThres(model,hmi_event);

					break;

			case ev_recoverMaxThres:
					Usart1.Print("[App]恢复默认最大阈值！\r\n");
					View_recoverThres(1,model,hmi_event);

					break;

			case ev_recoverMinThres:
					Usart1.Print("[App]恢复默认最小阈值！\r\n");
					View_recoverThres(0,model,hmi_event);

					break;

			case ev_recoverAllThres:
					Usart1.Print("[App]恢复默认阈值！\r\n");
					View_recoverThres(2,model,hmi_event);
					break;

			case ev_switchLed:
				model->LedModel->ledSTA = (hmi_event->thresValue == 0x02)?Off:(hmi_event->thresValue == 0x03)?On:Off;
					
						Usart1.Print("[App]用户强制翻转补光灯状态(不记录)！\r\n");
					View_setLed(model);//此函数不记录 用户翻转的状态，需要可以改为Ser_updataLedSTA()
					break;

			case ev_hmiStart:
					Usart1.Print("[App]等待屏幕启动中......\r\n");
					break;

			case ev_IDLE:
//				if(!(timerTimes % 2))Usart1.Print("[App]当前处于空闲状态！\r\n");		
					break;

			default:
					hmi_event->evtype = ev_IDLE;
			Usart1.Print("[App]屏幕当前事件代号出错×已自动恢复为:空闲状态！\r\n");
					break;
	}
	
	hmi_event->evtype = ev_IDLE;
	hmi_event->thresNo = 0xf;hmi_event->thresType = 0xf;hmi_event->thresValue = 0xff;
	//已在Controller中调用清空Usart2(Screen)的串口数组函数
	
}








static u8 App_calAdcValue_Soil(u16 *filterVal,int16_t *Val){//将总采集、滤波后的adc值进行标定
	if (!filterVal || !Val)return 0;
	if(*filterVal <= SOIL_DRY_ADC){*Val = 0;return 1;}
	if(*filterVal >= SOIL_WET_ADC){*Val = Soil_OutMax_Value / 10;return 1;}
	
	u16 range = SOIL_WET_ADC - SOIL_DRY_ADC;
	
	u16 result = (u32)(*filterVal - SOIL_DRY_ADC) * Soil_OutMax_Value;
	
	result += range / 2U;
	result /= range;
	*Val = result;//得到标定结果(有正负)
	#if debug
	Usart1.Print("[debug-App_calAdcValue_Soil],result:%d,value:%d\r\n",result,*Val);
	#endif
	return 1;

}



static u8 App_calCurrGrowth(AppModel *model){//计算生长指数
	
	
	
	
	return 0;
}





#include	"Ser_sensorFilter.h"
u8 sendOnce = 0;

void App_adcTask(AppModel *model){
	static u8 currTimes = 0;
//const u8 adcSampleTimes = 25;
//u16		adcSampleVal[adcSampleTimes] = {0,0,0};
	static u8 filVal0 = 0,errorTimes = 0;
	u16 adcVal = 0;
	
	
	if(model->timerModel->adcSamplFlag && (model->Sensors[3].STA) != sampOk){
		(model->Sensors[3].STA) = startSample;
		model->timerModel->adcSamplFlag = 0;
	}
	
			switch((model->Sensors[3].STA)){
				case busy:
					Usart1.Print("[Sensor-%s]传感器采集数据中!\r\n",model->Sensors[3].name);
					model->Sensors[3].STA = startSample;
				break;
				case sampOk:
					Usart1.Print("[Sensor-%s]数据采集完成,数据处理中...\r\n",model->Sensors[3].name);
						if(filterCycleAdcValue(adcSampleVal,&currTimes,&adcVal)){
							if(App_calAdcValue_Soil(&adcVal,&(model->Sensors[3].Val))){
								Ser_soilFil(model);
								filVal0 = (!(model->Sensors[3].Val))?++filVal0:--filVal0;
								if(model->appPage == pageHome){
									View_sendSoil((model->alermModel[3].filterVal));
								}
								
							}
							Usart1.Print("[Sensor-%s]数据为:%d vol%%\r\n",model->Sensors[3].name,model->Sensors[3].Val);
							(model->Sensors[3].STA) = (filVal0 >= 3)?error:sensorIDLE;
						}else Usart1.Print("当前土壤湿度数据集不可靠！\r\n");
						
					currTimes = 0;
					memset(adcSampleVal,'\0',adcSampleTimes);
						(model->Sensors[3].STA) = (filVal0 >=3)?error:sensorIDLE;
				break;
				case	sensorIDLE:
					Usart1.Print("[Sensor-%s]处于IDLE中!\r\n",model->Sensors[3].name);
					model->Sensors[3].STA = startSample;
				break;
				case startSample://由TIM时钟控制
					if(currTimes >= adcSampleTimes){
						model->Sensors[3].STA = sampOk;
						return;
					}
						Usart1.Print("[Sensor-%s]开始采集数据(%d)!\r\n",model->Sensors[3].name,currTimes);
					if(SensorStartSampl(model,adcSampleVal,currTimes)){
						currTimes++;
						model->Sensors[3].STA = (currTimes >= 25)?sampOk:startSample;
						return;
					}else{//采集失败
////////						--currTimes;
						errorTimes++;
						model->Sensors[3].STA = (errorTimes >=4)?error:startSample;
					}
				break;
				case	error:
					Usart1.Print("[Sensor-%s]传感器连接出错或未置于土壤中，请检查后重试!\r\n",model->Sensors[3].name);
					memset(adcSampleVal,'\0',adcSampleTimes);
					currTimes = 0;
					errorTimes = 0;model->Sensors[3].STA = sensorIDLE;
					
				break;
				default:
					DeInit_theSensor(&(model->Sensors[3]));
					DeInit_theAlermModels(&(model->alermModel[3]));//同时重置对应的Alerm模型
					Usart1.Print("[Sensor-%s]传感器处于未知状态,已被重置!\r\n",model->Sensors[3].name);
			}
	
}

extern RingBuf VOCrb;
void App_vocTask(AppModel *model){
	static	u8 filTemp_times = 0,filHumi_times = 0,filCO2_times = 0;

	(model->Sensors[0].STA) = (VOCrb.cnt >= 11)?sampOk:busy;
	(model->Sensors[1].STA) =  (model->Sensors[0].STA);
	(model->Sensors[4].STA) =  (model->Sensors[0].STA);
	switch((model->Sensors[0].STA)){
		case busy:
			Usart1.Print("[Sensor-室内环境]传感器采集数据中!\r\n");
				(model->Sensors[0].STA) = startSample;
		    (model->Sensors[1].STA) = startSample;
				(model->Sensors[4].STA) = startSample;
		break;
		case sampOk:
			Usart1.Print("[Sensor-室内环境]传感器数据采集完成!\r\n");
			(model->Sensors[0].STA) = sensorIDLE;
		    (model->Sensors[1].STA) = sensorIDLE;
				(model->Sensors[4].STA) = sensorIDLE;
		case startSample:
			Usart1.Print("[Sensor-室内环境]传感器数据开始解析...\r\n");
			Dri_vocPraseData(&VOCrb,&(model->Sensors[0].Val),&(model->Sensors[1].Val),&(model->Sensors[4].Val));
				Ser_humiFil(&appModel);
		    Ser_tempFil(&appModel);
				Ser_CO2Fil(&appModel);
		filTemp_times = (!model->alermModel[0].filterVal)?++filTemp_times:--filTemp_times;
		filHumi_times = (!model->alermModel[1].filterVal)?++filHumi_times:--filHumi_times;
		filCO2_times = (!model->alermModel[4].filterVal)?++filCO2_times:--filCO2_times;
				Usart1.Print("[Sensor-室内环境]数据开始解析完成!\r\n");
				if(model->appPage == pageHome){
					View_sendTemp(model->Sensors[0].Val * 10);
					View_sendHumi(model->Sensors[1].Val * 10);
					View_sendCO2(model->Sensors[4].Val);//CO2数值不需要 * 10倍
				}
				Usart1.Print("[Sensor-室内环境]数据：Temp:%d.%d，Humi:%d.%d%%RH，CO2:%dppm! \r\n",model->Sensors[0].Val / 10,model->Sensors[0].Val % 10,
																									model->Sensors[1].Val / 10, model->Sensors[1].Val % 10, model->Sensors[4].Val);
		 (model->Sensors[0].STA) = (filTemp_times >=3)?error:sensorIDLE;
			(model->Sensors[1].STA) = (filHumi_times >=3)?error:sensorIDLE; 
				(model->Sensors[4].STA) = (filCO2_times >=3)?error:sensorIDLE; 
		case	sensorIDLE:
			Usart1.Print("[Sensor-室内环境]传感器处于IDLE中!\r\n");
				(model->Sensors[0].STA) = startSample;
		    (model->Sensors[1].STA) = startSample;
				(model->Sensors[4].STA) = startSample;
		break;
		case error:
			Usart1.Print("[Sensor-室内环境]传感器连接出错,请检查后重试!\r\n");
				
		default:
			DeInit_theSensor(&(model->Sensors[0]));
				DeInit_theAlermModels(&(model->alermModel[0]));//同时重置对应的Alerm模型
			DeInit_theSensor(&(model->Sensors[1]));
				DeInit_theAlermModels(&(model->alermModel[1]));//同时重置对应的Alerm模型
			DeInit_theSensor(&(model->Sensors[4]));
				DeInit_theAlermModels(&(model->alermModel[4]));//同时重置对应的Alerm模型
		break;
	}

}

extern void Ser_tempFil(AppModel *model);
static u16 App_getValue_Sun(AppModel *model){
	 if ( !model )return 0;
			model->Sensors[2].Val = Dri_GY30_getCurrVal();
			Ser_luminFil(model);
	return model->Sensors[2].Val;
}
void App_getLuminTask(AppModel *model){
	static u8 errTimes = 0;
	u16 result = 0;
	Usart1.Print("errorTime:%d，STA：%d\r\n",errTimes,model->Sensors[2].STA);
	switch((model->Sensors[2].STA)){
		case busy:
			Usart1.Print("[Sensor-%s]传感器采集数据中!\r\n",model->Sensors[2].name);
		break;

		case	startSample:
			Usart1.Print("[Sensor-%s]传感器开始采集数据...\r\n",model->Sensors[2].name);
					result = App_getValue_Sun(model);
		case sampOk:
			Usart1.Print("[Sensor-%s]传感器数据采集完成,解析中...\r\n",model->Sensors[2].name);
				if(result != 0xffff){
					Usart1.Print("[Sensor-%s]数据解析成功√当前光照强度值:%d(lx)\r\n",model->Sensors[2].name,model->Sensors[2].Val);
						if(model->appPage == pageHome){
							View_sendLumin(model->Sensors[2].Val);
						}
						model->Sensors[2].STA = sensorIDLE;
						errTimes = (errTimes>0)?--errTimes:errTimes;
						break;
				}else{
					model->Sensors[2].Val = 0;
					Usart1.Print("[App]光照强度值解析失败×\r\n");
					++errTimes;
				}
			
				model->Sensors[2].STA = (errTimes >=3)?error:sensorIDLE;
		break;
		case sensorIDLE:
			Usart1.Print("[Sensor-%s]传感器处于空闲中,正在分配采集数据任务！\r\n",model->Sensors[2].name);
			model->Sensors[2].STA = startSample;
				break;
		case	error:
			if(errTimes >= 3){
				Usart1.Print("[Sensor-%s]传感器连接出错，或无反应,请检查后重试！\r\n",model->Sensors[2].name);
			}
			errTimes--;
			model->Sensors[2].STA = startSample;
		break;
		default:
			DeInit_theSensor(&(model->Sensors[2]));
				DeInit_theAlermModels(&(model->alermModel[2]));//同时重置对应的Alerm模型
	}

}

void App_hmiDataPrase(Usart *screen,HMI_Event *hmi_event){
			Ser_screenDataParse(screen,hmi_event);
}


#include	"Ser_Alerm.h"
void	App_alermCheckTask(AppModel *model){//更新告警状态
	if(!model){
			Usart1.Print("[App]AppModel为空,无法进行告警监测×\r\n");
			return;
	}
	if(!model->alermModel){
			Usart1.Print("[App]AlermModel为空,无法进行告警监测×\r\n");
			return;
	}

	///未完善相关设定
	Ser_checkSoilHumi(model);			//每600ms执行一次
	Ser_checkVOC(model);          //每1s执行一次
	Ser_checkLumination(model);   //每2s执行一次



	
}
static void tempAlerm(AppModel *model){
	
	
	for(u8 i = 0;i<2;i++){
		if(model->alermModel[i].alermState == over){
			if(i==2){
				Usart1.Print("[App-Alerm]告警监测：当前[%s]超过设定阈值%dlx！\r\n",model->Sensors[i].name,model->thresModel->min[i] * 100);
				continue;
			}
			if(i == 4)continue;
			Usart1.Print("[App-Alerm]告警监测：当前[%s]超过设定阈值:%d！\r\n",model->Sensors[i].name,model->thresModel->max[i]);
		}
		if(model->alermModel[i].alermState == less){
			if(i == 2){
				Usart1.Print("[App-Alerm]告警监测：当前[%s]低于设定阈值%dlx！\r\n",model->Sensors[i].name,model->thresModel->min[i] * 100);
				continue;
			}
			Usart1.Print("[App-Alerm]告警监测：当前[%s]低于设定阈值%d！\r\n",model->Sensors[i].name,model->thresModel->min[i]);
		}
		Usart1.Print("[Debug-View_checkAlerm]%d：%d\r\n",i,model->alermModel[i].filterVal);
	}
	
}
//#include	"Bsp_GPIO.h"
#include	"alermTask.h"
static void	alermTask_temp(AppModel *model){//温度高开风扇，低了响蜂鸣器
	if((model->Sensors[0].STA) != error){
		switch(model->alermModel[0].alermState){//温度告警状态
			case pass:
				//风扇、蜂鸣器关
//				Bsp_setFans(0);
//				Bsp_setBeep(1);//低电平触发
			
			/*测试
				if(model->alermModel[0].timeCNT % 1000 > 1){
					Bsp_toggleBeep();
					//蜂鸣器响
					model->alermModel[0].timeCNT = 0;
				}
			*/
			
			alermTask_tempOver(&(model->alermModel[0].timeCNT),0);
			alermTask_tempLess(&(model->alermModel[1].timeCNT),0);
			break;
			case over:		//高温
				alermTask_tempOver(&(model->alermModel[0].timeCNT),1);
//				model->alermModel[0].timeCNT = 0;
//				if(model->alermModel[0].timeCNT % 100 > 1){
//					Bsp_toggleBeep();
//					//蜂鸣器响
//					model->alermModel[0].timeCNT = 0;
//				}
			break;
			case less://低温
				alermTask_tempLess(&(model->alermModel[1].timeCNT),1);
//				if(model->alermModel[0].timeCNT % 500 > 1){
//					//风扇开？？？？？
//					Bsp_setFans(1);
//					model->alermModel[0].timeCNT = 0;
//				}
			break;
		}
	}
}

void App_alermTask(AppModel *model){//根据告警状态执行对应的任务
	tempAlerm(model);//输出所有传感器状态信息
	View_tempEvent(model);//显示传感器状态到屏幕
	//执行对应事件
	alermTask_temp(model);

}

//void App_viewTask(AppModel *model){//View_tempEvent(model);//显示传感器状态到屏幕


//}

