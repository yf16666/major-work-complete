#include	"Dri_GY30.h"

#define BHAddWrite     0x46      //写地址
#define BHAddRead      0x47      //读地址
#define BHPowDown      0x00      //关闭模块
#define BHPowOn        0x01      //打开模块等待指令 
#define BHReset        0x07      //重置数据寄存器仅在BHpowOn模式下有效
#define BHModeH1       0x10      //高分辨率 模式1 单位 11X 测量时间 120ms 
#define BHModeH2       0x11      //高分辨率 模式2 单位 11X 测量时间 120ms 
#define BHModeL        0x13      //低分辨率 单位4lx 测量时间16ms  
#define BHSigModeH     0x20      //一次测量高分辨率模式1，然后转到powerdown模式
#define BHSigModeH2    0x21      //同上类似
#define BHSigModeL     0x23      //同上类似  




void Dri_GY30_Init(void){
	Bsp_softIIC_Init();
}

u16 Dri_GY30_getCurrVal(void){
	BspIIC_start();
	BspIIC_senByte(BHAddWrite);
	if(BspIIC_waitAck())return 0xffff;
	BspIIC_senByte(BHPowOn);
	if(BspIIC_waitAck())return 0xffff;
	BspIIC_Stop();
	
	BspIIC_start();
	BspIIC_senByte(BHAddWrite);
	if(BspIIC_waitAck())return 0xffff;
	BspIIC_senByte(BHModeH1);
	if(BspIIC_waitAck())return 0xffff;
	BspIIC_Stop();
	
	
	Delay_ms(150);
	u16 data;
	
	BspIIC_start();
	BspIIC_senByte(BHAddRead);
	if(BspIIC_waitAck())return 0xffff;
	data = BspIIC_receByte() << 8;
	BspIIC_senAck(0);
	data |= BspIIC_receByte();
	BspIIC_senAck(1);
	BspIIC_Stop();
	
	return (u16)((float)data / 1.2f);
}





















