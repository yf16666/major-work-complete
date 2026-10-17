#include "Hmi_Protocol.h"


static void clearScreenData(Usart *screen){
	memset(screen->status->Data,'\0',255);
	screen->status->dataLen = 0;
	screen->status->receOk = 0;
}


static u8 ParseScreenProtocol(const u8 *data,const u8 dataLen,HMI_Event *hmiEvent){
    if(dataLen < 3)return 0;

    u8 index = 0xff;
    for(u8 i = 0;i<dataLen;i++){
        if(data[i] == 0xAA ||
           data[i] == 0xAC ||
           data[i] == 0xAD ||
           data[i] == 0xAE ||
           data[i] == 0xAF)
        {
            index = i;
            break;
        }
    }
		
    if(index == 0xff)return 0;
		if(data[index] == 0xAC && data[index + 2] == 0xFF){
					switch(data[index+1]){
						case 0x00:
							hmiEvent->evtype = ev_hmiStart;
							hmiEvent->currPage = unknowStatus;
							break;
							case 0x01:
								hmiEvent->currPage = home; 
									break;
							case 0x02:
									hmiEvent->currPage = liveData;
									break;
							case 0x03:
									hmiEvent->currPage = thresSetting;
									break;
							case 0x04:
									hmiEvent->currPage = setting;
									break;
							default:
									hmiEvent->currPage = home;
									break;
					}
					return 1;
    }else if(data[index] == 0xAA && data[index + 2] == 0xFF){
					switch(data[index+1]){
							case 0x02:
									hmiEvent->currPage = liveData;//更新实时传感器状态到当前页面
									hmiEvent->evtype = ev_IDLE;
									break;
							case 0x03:
									hmiEvent->currPage = thresSetting;
									hmiEvent->evtype = ev_readAllthres;
									break;
							case 0x04:
									hmiEvent->currPage = setting;
									hmiEvent->evtype = ev_readLedSTA;
									break;
							case 0x01:
							default://（重启）上电
								 hmiEvent->currPage = home; 
									hmiEvent->evtype = ev_none;//上电初始化
									break;
					}
					return 1;
    }else if(data[index] == 0xAD && data[index + 3] == 0xDA){//恢复默认值
							switch(data[index+1]){
								case 0x01:
									hmiEvent->evtype = ev_recoverMaxThres;
										hmiEvent->thresValue = 0x01;
									break;
								case 0x02:
									hmiEvent->evtype = ev_recoverMinThres;
										hmiEvent->thresValue = 0x02;
									break;
								case 0x00:
									hmiEvent->evtype = ev_recoverAllThres;
										hmiEvent->thresValue = 0x00;
									break;
							}
						return 1;
    }else if(data[index] == 0xAD && data[index + 4] == 0xFF){//更改单个阈值
							if(data[index+1] > 3 || data[index+2] > 1)return 0;
			
							hmiEvent->thresNo = data[index + 1];//阈值名称：温湿度、光强、土壤、生长指数：01234
							hmiEvent->thresType = data[index + 2];//阈值类型：00最小、01最大
							hmiEvent->thresValue = data[index + 3];
						hmiEvent->evtype = ev_changeSingleThres;
						return 1;
    }else if(data[index] == 0xAF && data[index + 2] == 0xFA){//注意，不记录用户翻转的状态
							hmiEvent->evtype = ev_switchLed;
							hmiEvent->thresValue = data[index+1];//0x02->关、0x03->开
							return 1;
		}
		return 0;
//		if(data[index] == 0xAE && data[index + 2] == 0xEA){//注意，不记录用户翻转的状态
//							//hmiEvent->evtype = 恢复默认设置;
//						return 1;
//		}
		
}




extern Usart Usart1;

void	Ser_screenDataParse(Usart *screen,HMI_Event *event){
	if(!screen || !event){Usart1.Print("[App-Error]screen 或 event模型初始化失败！\r\n");return;}
	if(screen->status->receOk){
		ParseScreenProtocol(screen->status->Data,screen->status->dataLen,event);
		clearScreenData(screen);
	}


}






