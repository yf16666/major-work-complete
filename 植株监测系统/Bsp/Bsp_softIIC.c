#include	"Bsp_softIIC.h"

#define	IICPort	GPIOB
#define	SCLPin	GPIO_Pin_12
#define	SDAPin	GPIO_Pin_13
//
#define	SCL_Set(x)	ctlI2CPin(SCLPin,x)
#define	SDA_Set(x)	ctlI2CPin(SDAPin,x)
#define	iicTime			10
void Bsp_softIIC_Init(void){

	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef	I2c;
	
	I2c.GPIO_Mode = GPIO_Mode_Out_OD;
	I2c.GPIO_Pin = SCLPin | SDAPin;
	I2c.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IICPort,&I2c);
	
	GPIO_SetBits(IICPort,SCLPin | SDAPin);

}

static void ctlI2CPin(u16 pin,u8 b){
	GPIO_WriteBit(IICPort,pin,(BitAction)b);
	Delay_us(iicTime);
}

void BspIIC_start(void){
	SCL_Set(1);
	SDA_Set(1);
	
	SDA_Set(0);
	SCL_Set(0);
	
}

void BspIIC_senAck(u8 ack){
	SCL_Set(0);
	if(ack)SDA_Set(1);else SDA_Set(0);
	SCL_Set(1);
	SCL_Set(0);
}

u8 BspIIC_waitAck(void){
	SCL_Set(0);
	SDA_Set(1);
	SCL_Set(1);
	Delay_us(iicTime);
	u8 ack = GPIO_ReadInputDataBit(IICPort,SDAPin);
	SCL_Set(0);
	return ack;
}

void BspIIC_senByte(u8 byte){//高位先行
	SCL_Set(0);
	for(u8 i = 0;i < 8;i++){
		SCL_Set(0);
		if((byte << i) &0x80)SDA_Set(1);else SDA_Set(0);
		SCL_Set(1);
	}
	SCL_Set(0);
}

u8 BspIIC_receByte(void){
	u8 byte = 0;
	SCL_Set(0);
	SDA_Set(1);
	for(u8 i =0;i<8;i++){
		SCL_Set(1);
		byte = (GPIO_ReadInputDataBit(IICPort,SDAPin))? byte | (1 << (7-i)): byte | (0 << i);
		SCL_Set(0);
	}
	return byte;
}

void BspIIC_Stop(void){
	SDA_Set(0);
	SCL_Set(1);
	SDA_Set(1);
}
















