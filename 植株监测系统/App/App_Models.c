#include "App_Models.h"




volatile u8 isScreenLive = 0;			//屏幕重启判断


Status status  = {
	 .receOk = 0,
	 .dataLen = 0//,
//	 .Data = {}
	
};


// 串口实体
Usart Usart1 = {//仅作Debug输出，无Status
	.USARTx = USART1,
	.Baud = 115200,
	.PreemptionPriority = 1,
	.SubPriority = 0,
	.Init = Usartx_Init
};
Usart Screen = {
	.USARTx = USART2,
	.Baud = 115200,
	.PreemptionPriority = 2,
	.SubPriority = 0,
	.Init = Usartx_Init,
	.status = &status
};
Usart VOC = {
		.USARTx = USART3,
		.Baud = 9600,
		.PreemptionPriority = 3,
		.SubPriority = 0,
		.Init = Usartx_Init,
		.status = 0};

		
		
const u8 sensorNum			= 5;		
const u8 adcSampleTimes = 25;
u16		adcSampleVal[25] = {0,0,0};	
HMI_Event			hmiEvent;
ThresModel		thresModel;

Sensor				sensors[sensorNum];//室内温湿度、光照强度、土壤湿度、生长指数
		
AlermModel		alermModel[ALERM_CNT];


AppModel 			appModel;
TimerModel		timerModel;
LedModel			ledModel;
RingBuf				VOCrb = {0};



void DeInit_theSensor(Sensor *sensor){
	//重新初始化为开始状态，等待下一次监测状态
//	for(u8 i = 0;i<3;i++){
		sensor->STA = sensorIDLE;
		sensor->Val = 0;

//	}
	
	Usart1.Print("[App_DeInit]传感器:%s 已重新初始化!\r\n",sensor->name);
}
void DeInit_theAlermModels(AlermModel *alermModel){
	//重新初始化为开始状态，等待下一次监测状态
	for(u8 i = 0;i<3;i++){
		alermModel[i].alermState = pass;
		alermModel[i].filterVal = 0;
		alermModel[i].highCnt = 0;
		alermModel[i].lowCnt = 0;
		alermModel[i].timeCNT = 0;
	}
//	Usart1.Print("[App_AlermModelDeInit]传感器:%s 已重新初始化!\r\n",sensor->name);
}
static void sensorModel_Init(void){
	sensors[0].name = "室内温度";
	sensors[1].name = "室内湿度";
	sensors[2].name = "光照强度";
	sensors[3].name = "土壤湿度";
	sensors[4].name = "生长指数";
	
	for(u8 i = 0;i<sensorNum;i++){
			sensors[i].STA = sensorIDLE;
				sensors[i].Val = 0;
	}
	Usart1.Print("[App_Init]sensorModel_Init OK!\r\n");
}

static void thresModel_Init(void){
	for(u8 i = 0;i < thresNum;i++){
		thresModel.max[i] = defaultMaxThres[i];
	}
	for(u8 i = 0;i < thresNum;i++){
		thresModel.min[i] = defaultMinThres[i];
	}
	thresModel.isFull = 0;
	thresModel.whichThres = 0xf;
}

static void alermModel_Init(void){
	
	for(u8 i =0;i< ALERM_CNT;i++){//0：空气温度、1：空气湿度、2：光照强度、3：土壤湿度、4：生长系数
		alermModel[i].filterVal = 0;
		alermModel[i].highCnt = 0;
		alermModel[i].lowCnt = 0;
		alermModel[i].alermState = pass;
		alermModel[i].timeCNT = 0;
	}
	
//	alermModel.tempAlerm = pass;
//		alermModel.humiAlerm = pass;
//		alermModel.sunAlerm = pass;
//		alermModel.soilHumiAlerm = pass;
//		alermModel.growthAlerm = pass;
	
	
//	typedef struct{
//    union{
//        struct{
//            Alerm tempAlerm;
//            Alerm humiAlerm;
//            Alerm sunAlerm;
//            Alerm soilHumiAlerm;
//            Alerm growthAlerm;
//        };
//        Alerm alermList[5];
//    };
//}AlermModel;
}

static	void timerModel_Init(void){
	//TIM3设定中断周期为：		10 ms/次
	
	timerModel.adcSamplCycle_times  = 3;	//设ADC采集周期为 2个时钟周期： 3 * 10 = 30ms / 次  -->25个周期共750ms采集满
	timerModel.adcSamplFlag = 0;
	timerModel.updateAdcDataCycle_times = 51;		//设串口屏刷新土壤+光照（估算值）、温湿度数据的时钟周期为 51个：51 * 10 = 510ms/次
	timerModel.updateAdcDataFlag = 0;
	timerModel.alermCheckCycle_times = 100;		//告警数值监测任务的启动时间1秒次 -->100个时钟周期
	timerModel.alermCheckFlag = 0;
	
	Usart1.Print("[App_Init]timerModel_Init OK!\r\n");
}

static void ledModel_Init(void){		//Led控制权默认为 告警监测进行控制
		ledModel.Actor = alermCtl;
		ledModel.ledSTA = Off;
		ledModel.currCompareValue = 0;
}
void AppModel_Init(AppModel *Model){
	
	if(!Model)return;
	
	hmiEvent.currPage = unknowStatus;
	hmiEvent.evtype = ev_none;
	hmiEvent.thresNo = 0xf;
	hmiEvent.thresType = 0xf;
	hmiEvent.thresValue = 0xff;
	
	sensorModel_Init();
	thresModel_Init();
	alermModel_Init();
	timerModel_Init();
	ledModel_Init();
	
	Model->appPage = pageUnknow;		
	Model->Sensors 	= 	sensors;
	Model->thresModel = &thresModel;
	Model->timerModel = &timerModel;
	Model->alermModel = alermModel;
	Model->LedModel		= &ledModel;
	
	Usart1.Print("[Model]初始化完成！等待屏幕启动中......\r\n");
}























