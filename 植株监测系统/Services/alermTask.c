#include		"alermTask.h"

#include			"Bsp_GPIO.h"

#define		timPeriod						10			//用到的定时器周期是 10ms/周期

//告警动作执行机构：1，Beep、2，Motor（风扇马达）、3，LED（补光灯）

#define		timeBase								(1000 / timPeriod)			// 除去定时器的周期	//告警动作机构执行时间基准为1000ms ==>1S
//#define		tempOverTime		timeBase
//#define			tempOverBeeptimes		1				//告警动作机构在执行时间基准下执行次数
//#define		tempLessTime		timeBase
//#define			tempLessBeeptimes		2

//#define		humiOverTime		(10 * timeBase)
//#define			humiOverBeeptimes		1				//告警动作机构在执行时间基准下执行次数
//#define		humiLessTime		(3 * timeBase)
//#define			humiLessBeeptimes		1

//#define		luminOverTime		(5 * timeBase)
//#define			luminOverBeeptimes		1				//告警动作机构在执行时间基准下执行次数
////#define		luminLessTime		timeBase			//光强过低则开LED（PWM调节）
////#define			luminLessBeeptimes		2


//#define		soilOverTime		( 5 * timeBase)
//#define			soilOverBeeptimes		2				//告警动作机构在执行时间基准下执行次数
//#define		soilLessTime		(2 * timeBase)
//#define			soilLessBeeptimes		1

#define		beepPersistTime					(100 / timPeriod)				//蜂鸣器持续触发时间：100ms
typedef struct {
	u32 period;			//周期
	u8 beepTimes;		//次数
}AlermTaskConf;

static AlermTaskConf alermTaskCof[] = {
	{timeBase,1},						//室内温度Over
	{timeBase,2},						//室内温度Less
	{(10 * timeBase),1},		//室内湿度Over
	{(3 * timeBase),1},			//室内湿度Less
	{(5 * timeBase),1},			//光照强度Over
	{(5 * timeBase),2},		//土壤湿度Over
	{(2 * timeBase),1}			//土壤湿度Less
};


/*
	湿 + 热条件：
		风扇开启
*/
void alermTask_wetAndhot(u8 Action){
//		static u8 lastSTA = 0;//默认上次状态关闭
			Bsp_setFans(Action);
//		lastSTA = Action;
}

/*
	温度Over条件：
		Beep每秒响 1 次

	--->被调用！
*/
void alermTask_tempOver(u16 *currTime,u8 flag){//alermTaskCof[0]
	static u8 temO_STA = 0;//0：静默、1：正在响、
	static u8 okTimes = 0;		//Bsp_setBeep(0);
	static u16 lastTime = 0;u16 delta = 0;//*currTime;
	
	if(flag){
		if(okTimes >= alermTaskCof[0].beepTimes){
			Bsp_setBeep(1);
			okTimes = 0;lastTime = *currTime;  // 不要清零
			temO_STA = 0;//*currTime = 0;//数据类型为u16可溢出
			return;
		}
		delta = *currTime - lastTime;
		if(!temO_STA){//静默中
				if(delta >= alermTaskCof[0].period){
					Bsp_setBeep(0);
					temO_STA = 1;
					lastTime = *currTime;
					okTimes++;
				}
			
		}else{//正在响
				if(delta >= beepPersistTime){
					Bsp_setBeep(1);
					temO_STA = 0;
					lastTime = *currTime;
				}
		}
		
	}else{
		okTimes = 0;lastTime = *currTime;  // 不要清零
		temO_STA = 0;//*currTime = 0;
	}
}

/*
	温度Less条件：
		Beep每秒响 2 次
*/
void alermTask_tempLess(u16 *currTime,u8 flag){//alermTaskCof[1]
	static u8 temO_STA = 0;//0：静默、1：正在响、
	static u8 okTimes = 0;		//Bsp_setBeep(0);
	static u16 lastTime = 0;u16 delta = 0;//*currTime;
	
	if(flag){
		if(okTimes >= alermTaskCof[1].beepTimes){
			Bsp_setBeep(1);
			okTimes = 0;lastTime = *currTime;  // 不要清零//lastTime = 0;
			temO_STA = 0;//*currTime = 0;//数据类型为u16可溢出
			return;
		}
		delta = *currTime - lastTime;
		if(!temO_STA){//静默中
				if(delta >= (alermTaskCof[1].period / alermTaskCof[1].beepTimes)){
					Bsp_setBeep(0);
					temO_STA = 1;
					lastTime = *currTime;
					okTimes++;
				}
			
		}else{//正在响
				if(delta >= beepPersistTime){
					Bsp_setBeep(1);
					temO_STA = 0;
					lastTime = *currTime;
				}
		}
		
	}else{
		okTimes = 0;lastTime = *currTime;  // 不要清零lastTime = 0;
		temO_STA = 0;//*currTime = 0;
	}
}

/*
	湿度Over条件：
		Beep每 10 秒响 1 次
*/
void alermTask_humiOver(u16 *currTime,u8 flag){//alermTaskCof[2]
	static u8 temO_STA = 0,okTimes = 0;		//Bsp_setBeep(0);
	static u16 lastTime = 0;//*currTime;
	if(flag){
		if((*currTime - lastTime) / beepPersistTime >= 1){
			Bsp_setBeep(1);
			lastTime  = 0;okTimes = (okTimes < alermTaskCof[2].beepTimes)?okTimes:0;
		}else{return ;}
		if(*currTime / alermTaskCof[2].period >= 1 && okTimes < alermTaskCof[2].beepTimes){
			Bsp_setBeep(0);
			okTimes++;
		}
		lastTime = *currTime;
	}else{
			Bsp_setBeep(1);
			lastTime  = 0;okTimes = 0;
	}
}
/*
	湿度Less条件：
		Beep每 3 秒响 1 次
*/
void alermTask_humiLess(u16 *currTime,u8 flag){//alermTaskCof[3]
	static u8 temO_STA = 0,okTimes = 0;		//Bsp_setBeep(0);
	static u16 lastTime = 0;//*currTime;
	if(flag){
		if((*currTime - lastTime) / beepPersistTime >= 1){
			Bsp_setBeep(1);
			lastTime  = 0;okTimes = (okTimes < alermTaskCof[3].beepTimes)?okTimes:0;
		}else{return ;}
		if(*currTime / alermTaskCof[3].period >= 1 && okTimes < alermTaskCof[3].beepTimes){
			Bsp_setBeep(0);
			okTimes++;
		}
		lastTime = *currTime;
	}else{
			Bsp_setBeep(1);
			lastTime  = 0;okTimes = 0;
	}
}
/*
	光照强度Over条件：
		Beep每 5 秒响 1 次
*/
void alermTask_luminOver(u16 *currTime,u8 flag){//alermTaskCof[4]
	static u8 temO_STA = 0,okTimes = 0;		//Bsp_setBeep(0);
	static u16 lastTime = 0;//*currTime;
	if(flag){
		if((*currTime - lastTime) / beepPersistTime >= 1){
			Bsp_setBeep(1);
			lastTime  = 0;okTimes = (okTimes < alermTaskCof[4].beepTimes)?okTimes:0;
		}else{return ;}
		if(*currTime / alermTaskCof[4].period >= 1 && okTimes < alermTaskCof[4].beepTimes){
			Bsp_setBeep(0);
			okTimes++;
		}
		lastTime = *currTime;
	}else{
			Bsp_setBeep(1);
			lastTime  = 0;okTimes = 0;
	}
}
/*
	光照强度Less条件：
		LED-Pwm条件
		已在App_alermTask中完成
*/


/*
	土壤湿度Over条件：
		Beep每 5 秒响 2 次
*/
void alermTask_soilOver(u16 *currTime,u8 flag){//alermTaskCof[5]
	static u8 temO_STA = 0,okTimes = 0;		//Bsp_setBeep(0);
	static u16 lastTime = 0;//*currTime;
	if(flag){
		if((*currTime - lastTime) / beepPersistTime >= 1){
			Bsp_setBeep(1);
			lastTime  = 0;okTimes = (okTimes < alermTaskCof[5].beepTimes)?okTimes:0;
		}else{return ;}
		if(*currTime / alermTaskCof[5].period >= 1 && okTimes < alermTaskCof[5].beepTimes){
			Bsp_setBeep(0);
			okTimes++;
		}
		lastTime = *currTime;
	}else{
			Bsp_setBeep(1);
			lastTime  = 0;okTimes = 0;
	}
}
/*
	土壤湿度Less条件：
		Beep每秒响 1 次
*/
void alermTask_soilLess(u16 *currTime,u8 flag){//alermTaskCof[6]
	static u8 temO_STA = 0,okTimes = 0;		//Bsp_setBeep(0);
	static u16 lastTime = 0;//*currTime;
	if(flag){
		if((*currTime - lastTime) / beepPersistTime >= 1){
			Bsp_setBeep(1);
			lastTime  = 0;
			okTimes = (okTimes < alermTaskCof[6].beepTimes)?okTimes:0;
		}else{return ;}
		if(*currTime / alermTaskCof[6].period >= 1 && okTimes < alermTaskCof[6].beepTimes){
			Bsp_setBeep(0);
			okTimes++;
		}
		lastTime = *currTime;
	}else{
			Bsp_setBeep(1);
			lastTime  = 0;okTimes = 0;
	}
}




