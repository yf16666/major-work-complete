#include		"Ser_Alerm.h"


#define		bestMin		1200
#define		bestMax		2000		//最优光照强度区间
#define		fabs(a,b)		{if(}

//extern AppModel 	AppModel;

void Ser_checkLumination(AppModel *model){//得到的光照强度阈值都应 *= 100倍

    extern void View_setLed(AppModel *model);

    static u8 regulateNum = 25;

	
	
    u16 fVal = model->alermModel[2].filterVal,
				thresVal_Min = model->thresModel->min[2] * 100,thresVal_Max = model->thresModel->max[2] * 100;
//	Usart1.Print("【Ser_checkLumination】fVal:%d，maxThr:%d\r\n",fVal,thresVal_Max);
		if(fVal >= bestMin &&  fVal <= bestMax){//处于最优光照强度区间，LED控制权交出
			
			model->alermModel[2].alermState = pass;
			model->LedModel->Actor = userCtl;return;
		}
		
    if(fVal <= thresVal_Min + 200){ /* 光照过低 */
			//边界值判断
			model->alermModel[2].highCnt = 0;
			if(thresVal_Min - fVal >= 100){//最小临界判断：离最小边界太远
				model->alermModel[2].lowCnt =
            (model->alermModel[2].lowCnt < 3) ? model->alermModel[2].lowCnt + 1 : model->alermModel[2].lowCnt;
        

        if(model->alermModel[2].lowCnt >= 3){
					regulateNum = (regulateNum < 100)?regulateNum + 5: 100;
					
            model->LedModel->currCompareValue = regulateNum;
            model->alermModel[2].alermState = less;//提交状态，进行播报
						model->LedModel->Actor = alermCtl;
            model->LedModel->ledSTA = On;
            View_setLed(model);
        }
			}else{
				model->alermModel[2].lowCnt =
            (model->alermModel[2].lowCnt >0) ? model->alermModel[2].lowCnt - 1 : model->alermModel[2].lowCnt;
				if(!model->alermModel[2].lowCnt){model->LedModel->Actor = userCtl;model->LedModel->currCompareValue = 0;}
			}
			
        
    }else if(fVal >= (thresVal_Max - bestMax)){/* 光照过高 */
				//边界判断
//			Usart1.Print("【Debug】fVal:%d\r\n",fVal);
        model->alermModel[2].highCnt = (model->alermModel[2].highCnt < 3) ? model->alermModel[2].highCnt + 1 : model->alermModel[2].highCnt;
        model->alermModel[2].lowCnt = 0;

        if(model->alermModel[2].highCnt >= 3){
        
							regulateNum = (regulateNum >= 5)?regulateNum - 5: 0;
            model->LedModel->currCompareValue = regulateNum;
            model->alermModel[2].alermState = over;

            if(regulateNum == 0)
                model->LedModel->ledSTA = Off;
            else  
                model->LedModel->ledSTA = On;
						
						model->LedModel->Actor = alermCtl;
            View_setLed(model);model->LedModel->Actor = userCtl;
        }
    }else{/* 回到正常区间 */
   
        model->alermModel[2].highCnt = 0;
        model->alermModel[2].lowCnt = 0;

        regulateNum = 0;
        model->LedModel->currCompareValue = 0;
				model->LedModel->Actor = alermCtl;
        model->LedModel->ledSTA = Off;

				View_setLed(model);
        model->alermModel[2].alermState = pass;
       
				model->LedModel->Actor = userCtl;
    }

}

static void Ser_checkVOC_CO2(AppModel *model){
	//CO2：  1，小于最小阈值，					 2，大于最大阈值
	//void	Ser_CO2Fil(AppModel *model);
	int16_t CO2 = (int16_t)model->alermModel[4].filterVal;//值，不需要*10倍，保存的CO2阈值才需要*100倍
	u8 CO2_threMax = model->thresModel->max[4] * 100,CO2_threMin = model->thresModel->min[4] * 100;
//	Usart1.Print("[Debug-Ser_VOC]fv:%d,ft:%d，h:%d hu_L:%d,H:%d\r\n",
	if(CO2 <= CO2_threMin){
		model->alermModel[4].highCnt = 0;
		if(CO2_threMin - CO2 >= 5){//边界判断
			model->alermModel[4].lowCnt = (!model->alermModel[4].lowCnt || model->alermModel[4].lowCnt <3)?model->alermModel[4].lowCnt + 1:model->alermModel[4].lowCnt -1;
		}else{
			model->alermModel[4].lowCnt = (model->alermModel[4].lowCnt >0)?model->alermModel[4].lowCnt -1:model->alermModel[4].lowCnt;
		}
			
			
		if(model->alermModel[4].lowCnt >=3)model->alermModel[4].alermState = less;

		
	}else if( CO2 > CO2_threMax){
		model->alermModel[4].lowCnt = 0;
		if(CO2 - CO2_threMax >= 5){//边界判断
			model->alermModel[4].highCnt = (!model->alermModel[4].highCnt || model->alermModel[4].highCnt <3)?model->alermModel[4].highCnt + 1:model->alermModel[0].highCnt -1;
		}else{
			model->alermModel[4].highCnt = (model->alermModel[4].highCnt <3)?model->alermModel[4].highCnt - 1:model->alermModel[4].highCnt;
		}
			if(model->alermModel[4].highCnt >=3)model->alermModel[4].alermState = over;
				
	}else{
		if(!model->alermModel[4].highCnt || !model->alermModel[4].lowCnt){
			model->alermModel[4].alermState = pass;//解除室内湿度过高/过低或低温/高温告警状态
		}else{
			model->alermModel[4].lowCnt = (model->alermModel[4].lowCnt > 0)?model->alermModel[4].lowCnt-1:model->alermModel[4].lowCnt;
			model->alermModel[4].highCnt = (model->alermModel[4].highCnt > 0)?model->alermModel[4].highCnt-1:model->alermModel[4].highCnt;
		}
  }	
	
}
void Ser_checkVOC(AppModel *model){//model中的VOC数值应 /= 10倍再进行判断

	//判断空气温湿度		
		//温度：	1，小于最小阈值, 蜂鸣器响， 2，大于最大阈值,开风扇	
		//湿度： 1，小于最小阈值，空气加湿   2，大于最大阈值,开风扇 *100
		//CO2：  1，小于最小阈值，					 2，大于最大阈值
//	static u8 tempTime_H = 0,humiTime_H = 0,tempTime_L = 0,humiTime_L = 0;
	//滤波值连续超过3次及以上才认为触发报警阈值或解除报警阈值
	
	int16_t temp = (int16_t)model->alermModel[0].filterVal,humi = (int16_t)model->alermModel[1].filterVal;
	u8 temp_threMax = model->thresModel->max[0],temp_threMin = model->thresModel->min[0];
	u8 humi_threMax = model->thresModel->max[1],humi_threMin = model->thresModel->min[1];
	Usart1.Print("[Debug-Ser_VOC]fv:%d,ft:%d，h:%d hu_L:%d,H:%d\r\n",temp,model->alermModel[0].
													filterVal,humi,model->thresModel->min[1],model->thresModel->max[1]);
	
	if(temp <= temp_threMin){
		model->alermModel[0].highCnt = 0;
		if(temp_threMin - temp >= 5){//边界判断
			model->alermModel[0].lowCnt = (!model->alermModel[0].lowCnt || model->alermModel[0].lowCnt <3)?model->alermModel[0].lowCnt + 1:model->alermModel[0].lowCnt -1;
		}else{
			model->alermModel[0].lowCnt = (model->alermModel[0].lowCnt >0)?model->alermModel[0].lowCnt -1:model->alermModel[0].lowCnt;
		}
			
			
		if(model->alermModel[0].lowCnt >=3)model->alermModel[0].alermState = less;

		
	}else if( temp > temp_threMax){
		model->alermModel[0].lowCnt = 0;
		if(temp - temp_threMax >= 5){//边界判断
			model->alermModel[0].highCnt = (!model->alermModel[0].highCnt || model->alermModel[0].highCnt <3)?model->alermModel[0].highCnt + 1:model->alermModel[0].highCnt -1;
		}else{
			model->alermModel[0].highCnt = (model->alermModel[0].highCnt <3)?model->alermModel[0].highCnt - 1:model->alermModel[0].highCnt;
		}
			if(model->alermModel[0].highCnt >=3)model->alermModel[0].alermState = over;
				
	}else{
		if(!model->alermModel[0].highCnt || !model->alermModel[0].lowCnt){
			model->alermModel[0].alermState = pass;//解除室内湿度过高/过低或低温/高温告警状态
		}else{
			model->alermModel[0].lowCnt = (model->alermModel[0].lowCnt > 0)?model->alermModel[0].lowCnt-1:model->alermModel[0].lowCnt;
			model->alermModel[0].highCnt = (model->alermModel[0].highCnt > 0)?model->alermModel[0].highCnt-1:model->alermModel[0].highCnt;
		}
  }	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	if(humi <= humi_threMin){
		
		model->alermModel[1].highCnt = 0;
		if(humi_threMin - humi){
			model->alermModel[1].lowCnt = (!model->alermModel[1].lowCnt || model->alermModel[1].lowCnt <3)?model->alermModel[1].lowCnt + 1:model->alermModel[1].lowCnt -1;
		}else{
			model->alermModel[1].lowCnt = (model->alermModel[1].lowCnt >0)?model->alermModel[1].lowCnt - 1:model->alermModel[1].lowCnt;
		}
		
		if(model->alermModel[1].lowCnt >=3)model->alermModel[1].alermState = less;
		
	}else if(humi > humi_threMax ){
		
		model->alermModel[1].lowCnt = 0;
		if(humi - humi_threMax >= 5){
			model->alermModel[1].highCnt = (!model->alermModel[1].highCnt || model->alermModel[1].highCnt <3)?model->alermModel[1].highCnt + 1:model->alermModel[1].highCnt-1;
		}else{
			model->alermModel[1].highCnt = (model->alermModel[1].highCnt >0)?model->alermModel[1].highCnt -1:model->alermModel[1].highCnt;
		}
			
			if(model->alermModel[1].highCnt >= 3)model->alermModel[1].alermState = over;	
				
	}else{

				model->alermModel[1].highCnt = (model->alermModel[1].highCnt > 0)?model->alermModel[1].highCnt-1:model->alermModel[1].highCnt;
				model->alermModel[1].lowCnt = (model->alermModel[1].lowCnt > 0)?model->alermModel[1].lowCnt-1:model->alermModel[1].lowCnt;
			
	}
	
	Ser_checkVOC_CO2(model);

}
void Ser_checkSoilHumi(AppModel *model){


//	static u8 soilHumi_H = 0,soilHumi_L = 0;
	//判断土壤湿度：1，小于最小阈值，水泵启动（滴灌?直接放水？用户自行操作√）2,大于最大阈值，蜂鸣器，风扇加速空气流通，多余水分蒸发
	//阈值：	0：空气温度、1：空气湿度、2：光照强度、3：土壤湿度、4：生长系数（有无阈值暂定！）
		//最大阈值：model->thresModel->max[0-4]
		//最小阈值：model->thresModel->min[0-4]
	//土壤湿度已经过25次采集、均值滤波得到值
	
	u16	Val = model->Sensors[3].Val,threMin = model->thresModel->min[3],
			threMax = model->thresModel->max[3];
	if(Val <= threMin){
		if(threMin - Val > 3){//边界值
			model->alermModel[3].lowCnt = (!model->alermModel[3].lowCnt || model->alermModel[3].lowCnt <3)?model->alermModel[3].lowCnt + 1:model->alermModel[3].lowCnt - 1;
		}else{
			model->alermModel[3].lowCnt = (model->alermModel[3].lowCnt >0)?model->alermModel[3].lowCnt - 1:model->alermModel[3].lowCnt;
		}
		model->alermModel[3].highCnt = 0;
		
		if(model->alermModel[3].lowCnt >=3)model->alermModel[3].alermState = less;
	}else if(Val > threMax){
		if(Val - threMax > 3){
			model->alermModel[3].highCnt = (!model->alermModel[3].highCnt || model->alermModel[3].highCnt <3)?model->alermModel[3].highCnt + 1:model->alermModel[3].highCnt - 1;
		}else{
			model->alermModel[3].highCnt = (model->alermModel[3].highCnt >0)?model->alermModel[3].highCnt - 1:model->alermModel[3].highCnt;
		}
		
		model->alermModel[3].lowCnt = 0;
		if(model->alermModel[3].highCnt >=3)model->alermModel[3].alermState = over;
	}else{

				model->alermModel[3].highCnt = (model->alermModel[3].highCnt > 0)?model->alermModel[3].highCnt-1:model->alermModel[3].highCnt;
				model->alermModel[3].lowCnt = (model->alermModel[3].lowCnt > 0)?model->alermModel[3].lowCnt-1:model->alermModel[3].lowCnt;
		
	}


}













