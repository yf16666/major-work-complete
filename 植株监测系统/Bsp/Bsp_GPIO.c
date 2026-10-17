#include	"Bsp_GPIO.h"


#define		gpioPort					GPIOB
#define		beepPin						GPIO_Pin_7		//低电平触发
#define		fansPin						GPIO_Pin_8		//风扇
#define		humidifierPin			GPIO_Pin_9		//加湿器
#define		motorPin					GPIO_Pin_10		//水泵


void Bsp_GPIO_Init(void){

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	
	GPIO_InitTypeDef	bspGPIO;
	bspGPIO.GPIO_Mode = GPIO_Mode_Out_PP;
	bspGPIO.GPIO_Speed = GPIO_Speed_50MHz;
	bspGPIO.GPIO_Pin = beepPin | fansPin | humidifierPin | motorPin;
	
	GPIO_Init(gpioPort,&bspGPIO);
	GPIO_WriteBit(gpioPort,beepPin,(BitAction)1);//低电平触发蜂鸣器
	
}




//void Bsp_toggleFans(void){
//	
//	if(!GPIO_ReadOutputDataBit(gpioPort,fansPin)){
//		GPIO_WriteBit(gpioPort,fansPin,(BitAction)1);
//	}else{
//		GPIO_WriteBit(gpioPort,fansPin,(BitAction)0);
//	}

//}

void Bsp_setFans(u8 status){
	
	if(!status){
		GPIO_WriteBit(gpioPort,fansPin,(BitAction)status);
	}else{
		GPIO_WriteBit(gpioPort,fansPin,(BitAction)status);
	}

}




void Bsp_toggleBeep(void){
	
	if(!GPIO_ReadOutputDataBit(gpioPort,beepPin)){
		GPIO_WriteBit(gpioPort,beepPin,(BitAction)1);
	}else{
		GPIO_WriteBit(gpioPort,beepPin,(BitAction)0);
	}

}

void Bsp_setBeep(u8 status){
	
	if(!status){
		GPIO_WriteBit(gpioPort,beepPin,(BitAction)status);
	}else{
		GPIO_WriteBit(gpioPort,beepPin,(BitAction)status);
	}

}

//void toggMotor(void){
//	
//	if(!GPIO_ReadOutputDataBit(gpioPort,motorPin)){
//		GPIO_WriteBit(gpioPort,motorPin,(BitAction)1);
//	}else{
//		GPIO_WriteBit(gpioPort,motorPin,(BitAction)0);
//	}

//}

void Bsp_setHumidifier(u8 status){
	
	if(!status){
		GPIO_WriteBit(gpioPort,humidifierPin,(BitAction)status);
	}else{
		GPIO_WriteBit(gpioPort,humidifierPin,(BitAction)status);
	}

}

void Bsp_setMotor(u8 status){
	
	if(!status){
		GPIO_WriteBit(gpioPort,motorPin,(BitAction)status);
	}else{
		GPIO_WriteBit(gpioPort,motorPin,(BitAction)status);
	}

}


