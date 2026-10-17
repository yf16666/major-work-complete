#include	"Dri_VOC.h"
#include	"string.h"

u8 Dri_readByte(RingBuf *rb,u8 *data){
	if(!rb  || !rb->cnt || !data)return 0;
	if(rb->rIndex >= rbSize)rb->rIndex = 0;
	*data = rb->data[rb->rIndex++];
	rb->cnt--;
	return 1;
}

u8 Dri_writeByte(RingBuf *rb,u8 ch){
	if(!rb || rb->cnt >= rbSize)return 0;
	if(rb->wIndex >= rbSize)rb->wIndex = 0;
	rb->data[rb->wIndex++] = ch;
	rb->cnt++;
	return 1;
}




static u8 calCkSum(u8 *data){//VOC协议最大成员数为：12
	u16 ck = 0;
	for(u8 i = 0;i < 11;i++){
		ck += data[i];
	}
	return (u8)((~ck) +1);
}

typedef enum{
	findHead,
	getData,
	calCK
}PRSTA;
#define debug 1

#if	debug
#include	"App_Models.h"
extern Usart Usart1;
#endif



static u8 data[12];

static u8 len=0;

static PRSTA pr=findHead;


void Dri_vocPraseData(RingBuf *rb,int16_t *temp,int16_t *humi,int16_t *CO2){

	u8 ch = 0;
	while(Dri_readByte(rb,&ch)){
		switch(pr){
			case findHead:
				if(ch == 0x2C){
					data[0] = ch;
					len = 1;
					pr = getData;
				}
			break;
			case	getData:
				data[len++] = ch;
				if(len == 11){
					pr = calCK;
				}
			break;
			case calCK:
				data[len] = ch;
				if(calCkSum(data) == data[11]){
					int16_t t = (int16_t)(data[7] << 8) | data[8];
					*temp = (int)(t  - 100);//注意传感器模块PCB的热量差
					*humi = (data[9] << 8) | data[10];
					*CO2 = 	(data[5] << 8) | data[6];
					#if debug
					Usart1.Print("[DeBug-Pra_VOC]\r\n\r\nt:%d,temp:%d,h:%d,CO2:%d\r\n\r\n",t,*temp,*humi,*CO2);
					#endif

				}else{
					*temp =0;*humi = 0;
				}
				memset(data,'\0',12);
				len = 0;pr = findHead;
			break;
		}
	
	}

}


























