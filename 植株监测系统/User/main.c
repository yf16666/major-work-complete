#include "stm32f10x.h"                  // Device header

#include	"App.h"
#include "App_Controllers.h"
#include "SEGGER_RTT.h"

int main(void){
//	SEGGER_RTT_Init();
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4);
	App_Init();
//	u8 cnt = 0;
//	App_Run();
	while(1){
		
//		SEGGER_RTT_printf(0, "%sRTT Test cnt = %d\r\n%s",
//                          RTT_CTRL_TEXT_BRIGHT_GREEN,
//                          cnt++,
//                          RTT_CTRL_RESET);
		App_Run();
		

	}

}

