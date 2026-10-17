#include	"Ser_sensorFilter.h"

#define		Alfa_temp				0.6
#define		Alfa_lumin			0.6
#define		Alfa_soil				0.3

#define	debug	1
void	 Ser_tempFil(AppModel *model){//室内温度的滤波
	if(!model)return;
	model->alermModel[0].filterVal = Alfa_temp * (model->Sensors[0].Val / 10) + (1 - Alfa_temp) * model->alermModel[0].filterVal;
	#if	debug
		Usart1.Print("[Debug-Ser_tempFil]filterVal:%d\r\n",model->alermModel[0].filterVal);
	#endif
	
}

void	 Ser_humiFil(AppModel *model){//室内湿度的滤波
	if(!model)return;
	model->alermModel[1].filterVal = Alfa_temp * (model->Sensors[1].Val / 10 )+ (1 - Alfa_temp) * model->alermModel[1].filterVal;
	#if	debug
		Usart1.Print("[Debug-Ser_humiFil]filterVal:%d\r\n",model->alermModel[1].filterVal);
	#endif
	
}

void	 Ser_luminFil(AppModel *model){//光照强度的滤波
	if(!model)return;

	model->alermModel[2].filterVal = Alfa_lumin * model->Sensors[2].Val + (1 - Alfa_lumin) * model->alermModel[2].filterVal;
	#if	debug
		Usart1.Print("[Debug-Ser_luminFil]filterVal:%d\r\n",model->alermModel[2].filterVal);
	#endif
	
	
}

void	Ser_soilFil(AppModel *model){//土壤湿度值的滤波
	if(!model)return;
	model->alermModel[3].filterVal = Alfa_soil * (model->Sensors[3].Val) + (1 - Alfa_soil) * model->alermModel[3].filterVal;
	#if	debug
		Usart1.Print("[Debug-Ser_soilFil]filterVal:%d\r\n",model->alermModel[3].filterVal);
	#endif
	
}















