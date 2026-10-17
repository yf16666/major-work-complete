#ifndef APP_MODELS_H
#define APP_MODELS_H

#include <stm32f10x.h>


#include "Bsp_M_Usart.h"
#include "Hmi_Protocol.h"
//#include "StoreCard.h"

#include	"Bsp_Spi.h"
#include	"Dri_GY30.h"
#include	"Bsp_ADC.h"
#include 	"Bsp_Tim.h"
#include	"Ent_StoreCard.h"
#include	"Bsp_GPIO.h"

#include	"Dri_VOC.h"

#define thresNum				6				//阈值x个数
#define adcSamplTimes		25			//共采集25次-->500ms更新到屏幕，500 / 20 = 25次

#define	Light_OutMax_Value		2500U		//滤波值超过极大值，说明比最大值还大，没必要继续估算

#define SOIL_DRY_ADC  				400U   		// 探头处于干燥状态时实测
#define SOIL_WET_ADC  				4095U   // 探头插入湿润土壤或水中时实测

#define	Soil_OutMax_Value			1000U		//百分比；滤波值超过极大值，说明比最大值还大，没必要继续估算

//0：空气温度度、1：空气湿度、2：光照强度、3：土壤湿度、4：生长系数（有无阈值暂定！）
//上电自动监测用户是否设置最大最小阈值，否则AppModel.thresArray使用下列默认阈值
static const u8 defaultMaxThres[thresNum] = {32, 65, 25, 35 ,8 , 0};//0：空气温度、1：空气湿度、2：光照强度(缩小了100倍)、3：土壤湿度、4：环境CO2(缩小了100倍)、5：生长系数（有无阈值暂定！）
static const u8 defaultMinThres[thresNum] = {15, 40, 4, 18 ,12 , 0};//0：空气温度、1：空气湿度、2：光照强度(缩小了100倍)、3：土壤湿度、4：环境CO2(缩小了100倍)、5：生长系数（有无阈值暂定！）

static const char *alermName[] = {
	"室内温度","室内湿度","光照强度","土壤湿度","环境CO2","生长指数"
};
static const char *thName[] ={"最小阈值","最大阈值"};


extern const u8 sensorNum;
extern const u8 adcSampleTimes;
extern u16		adcSampleVal[25];


typedef enum{
	pageHello,
	pageHome,
	pageLiveDataPage,
	pageThresSetting,
	pageSetting,
	pageUnknow
}AppPage;

typedef enum{
	Off,
	On,
}AppActorSTA;
typedef enum{
	userCtl,
	alermCtl,
}Actor;


typedef enum{
	pass,
	over,
	less
}AlermState;

typedef struct ThresModel 				ThresModel;
typedef struct AppModel						AppModel;
//typedef struct SensorModel				SensorModel;//ADC：土壤湿度、光敏传感器*1，Usart3：室内温湿度(打印得值要/=10倍)
typedef struct AlermModel					AlermModel;
//typedef struct bestThresModel			bestThresModel;//最优阈值
typedef struct TimerModel 				TimerModel;
typedef struct LedModel						LedModel;
typedef	struct	Sensor						Sensor;
typedef	enum	SensorSTA						SensorSTA;

struct ThresModel{
	u8	max[thresNum];
	u8  min[thresNum];
	u8	whichThres;
	u8	isFull;
};
enum SensorSTA{
	busy,					//采样中
	sampOk,						//采样完成
	startSample,	//开始采样
	sensorIDLE,		//模块空闲中
	error					//模块连接错误
};
struct Sensor{
	SensorSTA		STA;
	char				*name;
	int16_t			Val;
};

struct AlermModel{
	int16_t	 		filterVal;
	u8					lowCnt;
	u8					highCnt;
	AlermState	alermState;
	u16					timeCNT;
};

// AlermModel的数量
#define ALERM_CNT 6		//0：空气温度、1：空气湿度、2：光照强度、3：土壤湿度、4：环境CO2、5：生长系数
//typedef struct{
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

struct	TimerModel{
	u8 	adcSamplCycle_times;					//adc采集的时钟周期次数 10ms/周期
	volatile u8 	adcSamplFlag;							//adc采集开始标志，adc采集完成就自行关闭，开启时在TIM3中断开启
	u8	updateAdcDataCycle_times;			//更新ADC数据至屏幕的周期次数	10ms/周期
	volatile u8  updateAdcDataFlag;				//更新ADC数据至屏幕标志，更新完之后屏幕处自行关闭，开启时在TIM3中断开启
	u16	alermCheckCycle_times;				//告警数值监测的周期次数		10ms/周期
	volatile u8	alermCheckFlag;						//告警数值监测任务启动标志位	1000ms/次 / 10ms/周期 = 100周期/次
};

struct LedModel{
	volatile AppActorSTA	 	ledSTA;
	Actor									Actor;//决定Led属于谁控制
	volatile	u16					currCompareValue;
};

struct AppModel{//调试模式，watch 窗口直接输入 sizeof(AppModel)，直接读出真实大小
	ThresModel 						*thresModel;
	volatile AppPage 	 		appPage;
	Sensor								*Sensors;	
	AlermModel						*alermModel;			//根据阈值调控
	TimerModel						*timerModel;
	LedModel							*LedModel;
	volatile u8 					W25HardError;
};				

extern AppModel appModel;


extern Usart Usart1 ;
extern Usart Screen ;
extern HMI_Event		hmiEvent;

extern volatile u8 isScreenLive;


void AppModel_Init(AppModel *appmodel);


void DeInit_theSensor(Sensor *sensor);
void DeInit_theAlermModels(AlermModel *alermModel);
#endif
