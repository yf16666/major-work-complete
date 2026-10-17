#include "App_Views.h"

#include "App_Controllers.h"

#define  debug		0

//#include	"Bsp_Tim.h"
#include	"Ser_argvBind.h"
//更新阈值页面
//更新main页面（数据：温湿度、光照强度、土壤湿度、生长指数）
//更新liveData页面√  View_tempEvent()

static void LedOn(void){
			Screen.Print("setting.ledBtn.pic=5\xff\xff\xff");
			Screen.Print("setting.ledBtn.pic2=4\xff\xff\xff");
			Screen.Print("setting.ledStatus.val=1\xff\xff\xff");
}

static void LedOff(void){
			Screen.Print("setting.ledBtn.pic=4\xff\xff\xff");
			Screen.Print("setting.ledBtn.pic2=5\xff\xff\xff");
			Screen.Print("setting.ledStatus.val=0\xff\xff\xff");
}
void View_setLed(AppModel *model)
{
    if(!model || !model->LedModel)
        return;

    switch(model->LedModel->Actor){
			case userCtl:
					switch(model->LedModel->ledSTA){
						case On:
							Drivers_ledSetCompare(100);
							LedOn();
							Usart1.Print("[屏幕]用户打开补光灯\r\n");
						break;
						case Off:
							Drivers_ledSetCompare(0);
							LedOff();
							Usart1.Print("[屏幕]用户关闭补光灯\r\n");
						break;
					}
			break;
			case alermCtl:
					switch(model->LedModel->ledSTA){
						case On:
							Drivers_ledSetCompare(model->LedModel->currCompareValue);
							Usart1.Print("[App]自动补光 PWM=%d\r\n",
													 model->LedModel->currCompareValue);
							LedOn();
						break;
						case Off:
							Drivers_ledSetCompare(0);
							LedOff();
							Usart1.Print("[App]已自动关闭补光灯\r\n");
							
						break;
					}
			break;
		}
    
}


void View_readLedSTA(AppModel *model,HMI_Event *event){
	
	if(model->LedModel->Actor == userCtl){
		if(event->evtype == ev_switchLed){
				u8 newLedSTA = hmiEvent.thresValue - 2;//02关、03开
							model->LedModel->ledSTA = (AppActorSTA)newLedSTA;
							View_setLed(model);
							if(Ser_updataLedSTA(&newLedSTA) == SD_Ok){
								Usart1.Print("[Model]用户翻转[补光灯】状态：%d,成功√\r\n",newLedSTA);
								View_setLed(model);
								return;
							}else Usart1.Print("[Model]用户翻转[补光灯】状态：%d,失败×系统不会记录当前补光灯状态!\r\n",newLedSTA);	
	//						hmi_event->evtype = ev_IDLE;	View事件中自动闭合状态
							return;
			}
	}else
			Usart1.Print("[Model]绑定[补光灯】状态失败×当前处于告警状态,用户无法操作补光灯!\r\n");//break;
}
//更新main页面（数据：温湿度、光照强度、土壤湿度、生长指数）
void View_sendSoil(u16 soilVal){
	Usart1.Print("[App]土壤湿度值:%d %%Vol,已更新\r\n",soilVal);
		Screen.Print("main.humiSoil.val=%d\xff\xff\xff",soilVal);
}
void View_sendTemp(int16_t temp){
	Usart1.Print("[App]室内温度值:%d ℃,已更新\r\n",temp);
		Screen.Print("main.tempIndoor.val=%d\xff\xff\xff",temp);
}
void View_sendHumi(int16_t Humi){
	
		Screen.Print("main.humiIndoor.val=%d\xff\xff\xff",Humi);
}
void View_sendLumin(int16_t Lumin){
	Usart1.Print("[App]光照强度值:%d lx,已更新\r\n",Lumin);
		Screen.Print("main.sun.val=%d\xff\xff\xff",Lumin);
}
void View_sendGrowth(int16_t grow){
	Usart1.Print("[App]生长指数:%d,已更新\r\n",grow);
		Screen.Print("main.sun.val=%d\xff\xff\xff",grow);
}

void View_sendThreshold(AppModel *model){
	if(!model->thresModel)return;
	
	#if debug
		Usart1.Print("[DEBUG]\r\n");
		for(u8 i = 0; i < 5; i ++){
			Usart1.Print("%d ",model->thresModel->max[i]);
		}
		Usart1.Print("\r\n");
		for(u8 i = 0; i < 5; i ++){
			Usart1.Print("%d ",model->thresModel->min[i]);
		}
		Usart1.Print("\r\n[DEBUG]\r\n");
	#endif
	
	
	u8 t =0;//大小阈值显示共8个
	for(u8 i = 0; i <= 6; i += 2){//model->thresModel->max 
		Usart1.Print("ind:%d,val:%d;",i,model->thresModel->max[t]);
		Screen.Print("n%d.val=%d\xff\xff\xff",i,model->thresModel->max[t++]);
		
	}
	Usart1.Print("\r\n");
	Usart1.Print("[屏幕-阈值控制页]已下发最大阈值！\r\n");
	t = 0;
	for(u8 i = 1; i <= 7; i += 2){//model->thresModel->max
		Usart1.Print("ind:%d,val:%d;",i,model->thresModel->min[t]);
		Screen.Print("thresSetting.n%d.val=%d\xff\xff\xff",i,model->thresModel->min[t++]);
		
	}
	Usart1.Print("\r\n");
	Usart1.Print("[屏幕-阈值控制页]已下发最小阈值！\r\n");
	return;
}




const char *alermName[] = {
	"室内温度","室内湿度","光照强度","土壤湿度","生长指数"
};
const char *thName[] ={"最小阈值","最大阈值"};



void View_saveSingleThres(AppModel *model,HMI_Event *event){
	u8 *target;
    SD_Status result;

    if(!model || !model->thresModel || !event)return;
    if(event->thresType > 1 || event->thresNo >= thresNum || event->thresValue == 0xFF){
        Usart1.Print("[Screen-SaveThres]invalid parameter!\r\n");
        return;
    }

    target = event->thresType ? model->thresModel->max : model->thresModel->min;
    result = Ser_updateTheThres(event->thresType,event->thresNo,event->thresValue);
    if(result != SD_Ok){
        Usart1.Print("[Screen-Event]Flash write/verify failed,type:%s,ind:%d,val:%d,res:%d\r\n",
                     thName[!event->thresType],event->thresNo,event->thresValue,result);
        View_sendThreshold(model);
        return;
    }


    target[event->thresNo] = event->thresValue;
    Usart1.Print("[Screen-Event]saved %s Ind:%d = %d\r\n",
											 thName[!event->thresType],event->thresNo,event->thresValue);
}


void View_recoverThres(u8 maxOrmin,AppModel *model,HMI_Event *event){//恢复所有阈值
	if(!model || event->evtype != ev_recoverMaxThres
		|| event->evtype != ev_recoverAllThres
		|| event->evtype != ev_recoverMinThres
	)return;
	Usart1.Print("[App]正在恢复所选的阈值,无法撤销....\r\n");
	//恢复：最大阈值：0x01、最小阈值：0x02、全部阈值：0x03
	SD_Status res = SD_Error;
	//先清空用户阈值,再将当前appModel的阈值替换为默认阈值，最后下发
	
	if(maxOrmin == 1){
		res = Ser_cleanAlluserThres(maxOrmin,model->thresModel->max);
	}else if(!maxOrmin){
		res = Ser_cleanAlluserThres(maxOrmin,model->thresModel->min);
	}else if(maxOrmin == 3){
		res = Ser_cleanAlluserThres(maxOrmin,model->thresModel->max);
		res = Ser_cleanAlluserThres(maxOrmin,model->thresModel->min);
	}
	if(res == SD_Ok){
		if(maxOrmin == 1){
			Usart1.Print("[App]恢复默认最大阈值成功√\r\n");
		}else if(!maxOrmin){
			Usart1.Print("[App]恢复默认最小阈值成功√\r\n");
		}else{
			Usart1.Print("[App]恢复所有默认阈值成功√\r\n");
		}
		return;
	}
	Usart1.Print("[App]恢复默认最大或最小阈值时出错×请检查相关设置！\r\n");
}



extern Usart Screen;
void View_tempEvent(AppModel *model){//更新室内温度事件至页面 liveData
	extern const u8 sensorNum;
	for(u8 i=0;i<sensorNum;i++){
		if(model->appPage != pageLiveDataPage)return;
		switch(model->alermModel[i].alermState){
			case less:
				Screen.Print("liveDataPage.t%d.pco=RED\xff\xff\xff",i);
				Screen.Print("liveDataPage.t%d.txt=\"当前%s低于设定阈值!\"\xff\xff\xff",i,model->Sensors[i].name);
			break;
			case over:
				Screen.Print("liveDataPage.t%d.pco=RED\xff\xff\xff",i);
				Screen.Print("liveDataPage.t%d.txt=\"当前%s高于设定阈值!\"\xff\xff\xff",i,model->Sensors[i].name);
			break;
			case pass:
				Screen.Print("liveDataPage.t%d.pco=GREEN\xff\xff\xff",i);
				Screen.Print("liveDataPage.t%d.txt=\"当前%s处于阈值范围内!\"\xff\xff\xff",i,model->Sensors[i].name);
			break;
		}
	}
	
	

}








