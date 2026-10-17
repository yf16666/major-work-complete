#include "Drivers_ADC.h"


#define debug 0
#if debug
	#define		Debug(x)	Usart1.Print(x)
#endif

static u16 fillterGetValue(u16 *Array,u8 arrayLength){
	//经过最后一次采样后，采样索引已变为 设定的adcSamolTimes = 25
	if(!Array || !arrayLength || arrayLength != adcSamplTimes)return 0xffff;//数组或长度有误
	#if debug
		Usart1.Print("[Debug-Fillter]arrayLength:%d\r\n",arrayLength);
	#endif
	
	u16 maxValue = Array[0];u16 minValue = Array[0];
	u16 currValue = 0;u32 sum = 0;
	for(u8 i =0;i<arrayLength;i++){
		currValue = Array[i];
		sum += currValue;
			if(currValue > maxValue)maxValue = currValue;//最大
			if(currValue < minValue)minValue = currValue;//最小
	}
	sum -= maxValue;sum -= minValue;
	
	#if debug
		Usart1.Print("[Debug-Fillter]maxValue:%u,minValue:%u,sum:%lu\r\n",maxValue,minValue,sum);
	#endif
	u32 result = (sum / (arrayLength - 2));
	if(result > 4095U){Usart1.Print("[Debug-Fillter]滤波总和溢出：%u\r\n",sum);}
	//对去掉两个极值后的采集数组进行求和
	return (u16)result;//返回平均值
}


u8 filterCycleAdcValue(u16 *sampVal,u8 *currSamlTimes,u16 *filterVal){
	if( *currSamlTimes > adcSamplTimes){
			#if debug 
				Usart1.Print("[Debug-filterCycleAdcValue]currSampltimes:%d,采集索引出错\r\n",model->Sensor->currSampltimes);
			#endif
		return 0;}//采集索引出错
		
		#if debug 
				Usart1.Print("[Debug-filterCycleAdcValue]currSampltimes:%d\r\n",model->Sensor->currSampltimes);
		#endif
		
	u16 groupFilterValue1 = fillterGetValue(sampVal,*currSamlTimes);
//	u16 groupFilterValue2 = fillterGetValue(model->Sensor->adcGroup2,model->Sensor->currSampltimes);
	if( groupFilterValue1 != 0xffff){//土壤湿度的数据可靠，且计算得到滤波值
//	if( groupFilterValue1 != 0xffff && groupFilterValue2 != 0xffff){//土壤湿度、光照采集的数据可靠，且计算得到两个滤波值
		*filterVal = groupFilterValue1;
//		model->Sensor->filterValue[1] = groupFilterValue2;
		//滤波值估算标定值
		#if debug 
			Usart1.Print("[Debug-fillterValue]groupFillterValue1:%d,groupFillterValue2:%d\r\n",groupFilterValue1,groupFilterValue2);
		#endif
		return 1;
	}
		
	#if debug 
			Usart1.Print("[Debug-fillterValue]groupFillterValue1/2 Is NULL×\r\n");
	#endif
	return 0;
}

u8 SensorStartSampl(AppModel *model,u16 *sampVal,u8 currSampTimes){//准备采样数据，存入待滤波数组

//		model->Sensors[3].STA = startSample;
		
		ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_55Cycles5);
		ADC_SoftwareStartConvCmd(ADC1,ENABLE);
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
		u16 result = ADC_GetConversionValue(ADC1);
		if(!result)return 0;
		sampVal[currSampTimes] = result;//最后一个采样索引为24
	
	#if 1//debug
	Usart1.Print("[Debug-AdcSampling]SamplTimes:%d,result:%d\r\n",currSampTimes,result);
	#endif
	
//	if(*currSamlTimes == 25){(model->Sensors[3].STA) = sampOk;return 1;}// 最后一个滤波值加入数组后，索引++，变为25
//	(model->Sensors[3].STA) = sensorIDLE;
	return 1;
}


