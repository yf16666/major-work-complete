#include "Bsp_ADC.h"



void Bsp_ADC_Init(void){
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	//开启ADC1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//开启GPIOA的时钟
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//72MHZ-->ADC最大12MHZ

	GPIO_InitTypeDef ADC_GPIO;
	ADC_GPIO.GPIO_Mode = GPIO_Mode_AIN;
	ADC_GPIO.GPIO_Speed = GPIO_Speed_50MHz;
	ADC_GPIO.GPIO_Pin = GPIO_Pin_0; //| GPIO_Pin_1;
	GPIO_Init(GPIOA,&ADC_GPIO);
	
	ADC_InitTypeDef Adc;
	ADC_StructInit(&Adc);//初始化ADC结构体，防止出现随机值
	Adc.ADC_Mode = ADC_Mode_Independent;//模式选择：独立
	Adc.ADC_DataAlign = ADC_DataAlign_Right;//数据右对齐-->结果放在16位数据寄存器的低12位
	Adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//不需要外部触发通道
	Adc.ADC_ScanConvMode = DISABLE;//禁用扫描模式；
	Adc.ADC_ContinuousConvMode = DISABLE;//禁用连续转换模式
	Adc.ADC_NbrOfChannel = 1;//ADC通道数量-->对应使用的ADC通道
	
	
	ADC_Init(ADC1,&Adc);
	ADC_ITConfig(ADC1,ADC_IT_EOC,ENABLE);
	
	ADC_Cmd(ADC1,ENABLE);
	//复位ADC校准寄存器
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1) == SET){
		//可在此进行初始化日志输出；
	};
	//让单片机ADC模块内部电路自动进行ADC校准
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1) == SET){
		//可在此进行初始化日志输出；
	};
	
//	NVIC_InitTypeDef adcNVIC;
//	adcNVIC.NVIC_IRQChannel = ADC1_2_IRQn;
//	adcNVIC.NVIC_IRQChannelPreemptionPriority = 4;
//	adcNVIC.NVIC_IRQChannelSubPriority = 0;
//	adcNVIC.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&adcNVIC);
	
}

//手动更改每次的扫描顺序，若固定的话就将规则组顺序配置直接写在初始化中即可




