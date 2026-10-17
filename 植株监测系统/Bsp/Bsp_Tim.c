#include "Bsp_Tim.h"

//PB6			LEDÒý½Å-->PWM²¨¿ØÖÆ

void Bsp_Tim_Init(u16 Psc,u16 Arr){

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

	TIM_TimeBaseInitTypeDef timer;
	
	timer.TIM_ClockDivision = TIM_CKD_DIV1;
	timer.TIM_CounterMode = TIM_CounterMode_Up;
	timer.TIM_Period = Arr - 1;
	timer.TIM_Prescaler = Psc - 1;
	timer.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3,&timer);
	
	
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
	NVIC_InitTypeDef timerNvic;
	timerNvic.NVIC_IRQChannel = TIM3_IRQn;
	timerNvic.NVIC_IRQChannelPreemptionPriority = 0;
	timerNvic.NVIC_IRQChannelSubPriority = 0;
	timerNvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&timerNvic);
	
	TIM_Cmd(TIM3,ENABLE);
}

#define		ledPin			GPIO_Pin_6

void Bsp_ledPWM_Init(void){//TIM4

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);//PWM:PB6-PB10
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef	bspGPIO;
	bspGPIO.GPIO_Mode = GPIO_Mode_AF_PP;
	bspGPIO.GPIO_Speed = GPIO_Speed_50MHz;
	bspGPIO.GPIO_Pin = ledPin;
	GPIO_Init(GPIOB,&bspGPIO);

	TIM_TimeBaseInitTypeDef ledPwm;
	ledPwm.TIM_ClockDivision = TIM_CKD_DIV1;
	ledPwm.TIM_CounterMode = TIM_CounterMode_Up;
	ledPwm.TIM_Period = 100 -1;
	ledPwm.TIM_Prescaler = 720 - 1;
	ledPwm.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM4,&ledPwm);
	
	TIM_OCInitTypeDef ledPwmOc;
	TIM_OCStructInit(&ledPwmOc);
	
	ledPwmOc.TIM_OCMode = TIM_OCMode_PWM1;
	ledPwmOc.TIM_OCPolarity = TIM_OCPolarity_High;
	ledPwmOc.TIM_OutputState = TIM_OutputState_Enable;
	ledPwmOc.TIM_Pulse = 0;
	TIM_OC1Init(TIM4,&ledPwmOc);
	
	TIM_Cmd(TIM4,ENABLE);
	TIM_SetCompare1(TIM4,0);
}


void Drivers_ledSetCompare(u16 ledCompare){
	if(ledCompare > 100)ledCompare = 100;
	TIM_SetCompare1(TIM4,ledCompare);
}




