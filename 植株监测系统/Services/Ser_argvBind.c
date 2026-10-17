#include "Ser_argvBind.h"

#include "Ent_StoreCard.h"

//extern const char *thName[];

#include		"App_Views.h"
extern Usart  Usart1;

void getSdSTA(SD_Status SdSTA){
	switch(SdSTA){
		case	SD_Ok:
			Usart1.Print("[Ser_argvBind]成功！\r\n");
			break;
		case	SD_Busy:
			Usart1.Print("[Ser_argvBind]SD_Busy 忙碌中！\r\n");
			break;
		case	SD_timeOut:
			Usart1.Print("[Ser_argvBind]SD_TimeOut 超时\r\n");
			break;
		case SD_emptyPage:
			Usart1.Print("[Ser_argvBind]SD_emptyPage 页面空\r\n");
			break;
		case SD_nullPointer:
			Usart1.Print("[Ser_argvBind]SD_nullPointer 空指针\r\n");
			break;
		case	SD_Error:
			Usart1.Print("[Ser_argvBind]SD_Error\r\n");
			break;
		default:
			Usart1.Print("[Ser_argvBind]SD_状态不确定\r\n");
		
	}

}

/*
	先判断 AppModel->theesHold->max 或 min是否为空
	再判断要绑定最大/小
		1，检查每个阈值地址上的数据量，数据量为0的用Flash上的对应默认阈值进行替代，再写入
		2，若不为0，则按照数量(再-1）转为行偏移地址，读取最新的数据，放入对应的绑定数组中
	最后返回 成功状态
	
	推送数据则留给用户点击 对应地页面再进行获取model层的阈值数据进行下发
*/
//////const static u8 defaultMax[5] = {32,50,24,60,0};
//////const static u8 defaultMin[5] = {15,20,1,40,0};//最后调用下发时，先比对是否在默认阈值内，否则按默认阈值处理下发

SD_Status Ser_bindThre1Model(u8 maxOrmin,u8 *model){//再加入一个u8指针
	if(!model)return SD_nullPointer;
	//0x0t?000 -->t:最大最小对应的首地址，?:对应类型排列后的成员地址
	u32 	Addr = 0;
	if(maxOrmin == 1){
		Addr = sdAddr_userMaxThres;
	}else if(!maxOrmin){
		Addr = sdAddr_userMinThres;
	}else return SD_nullPointer;
	u16 thresSector = 0;

	SD_Status re = SD_Error;
	
	
		
		for(u8 i = 0;i<sd_ThresNum;i++){
			thresSector = i;

			re = Sd_readFinaldata(Addr + ((thresSector << 12) & 0xFFF000),&model[i]);
			if(re == SD_emptyPage){
					model[i] = (maxOrmin)?defaultMaxThres[i]:defaultMinThres[i];//把写入的空值;0xFF替换为默认对应值
					if(maxOrmin){
						Usart1.Print("[Ser_bindThre1Model]第%d个最大阈值暂无，已使用对应的默认阈值%d\r\n",i,model[i]);
					}else{
						Usart1.Print("[Ser_bindThre1Model]第%d个最小阈值暂无，已使用对应的默认阈值%d\r\n",i,model[i]);
					}
					
				//写入到相应W25Q的地址中
			}
			else if(re == SD_Ok){//不为空
					if(maxOrmin){
						Usart1.Print("[Ser_bindThre1Model]第%d个最大阈值：%d 已绑定\r\n",i,model[i]);
					}else{
						Usart1.Print("[Ser_bindThre1Model]第%d个最小阈值：%d 已绑定\r\n",i,model[i]);
					}
					
			}else{
				
					if(maxOrmin){
						Usart1.Print("[Ser_bindThre1Model]查找第%d个最大阈值时出现未识别的状态！！\r\n",i);
					}else{
						Usart1.Print("[Ser_bindThre1Model]查找第%d个最小阈值时出现未识别的状态！！\r\n",i);
					}
				
			}
		
	}
	Usart1.Print("[Ser_bindThre1Model]已完成阈值的绑定!\r\n");
	return re;
}



/*
	更新单个阈值至W25Q中对应的地址中
	 先判断model是否初始化！
	1，先获取对应地址的数据数量，得到最新的未写行的偏移地址
	2，判断行的偏移地址是否 >=  页最大存储数(0x100 / 所存储数据类型)
	3.1 是，则擦除当前地址对应的扇区（addr & 0xfff000)，再写入
	3.2 否，则直接写入
	4，最后将该更新到model层对应的数组的对应成员值
*/
SD_Status Ser_updateTheThres(u8 maxOrmin,u8 thresInd,u8 newThres){
		if(thresInd >= sd_ThresNum)return SD_Error;
	
		u32 Addr = 0;u16 sector = thresInd << 12;
				u8 Num = 0;
		if(maxOrmin == 1){
			Addr = sdAddr_userMaxThres + sector;//0x03?000
			Usart1.Print("[Ser_updateTheThres]更新用户最大阈值%d:0x%06X!\r\n",thresInd,Addr);
		}else if(!maxOrmin){
			Addr = sdAddr_userMinThres + sector;//0x04?000
			Usart1.Print("[Ser_updateTheThres]更新用户最小阈值%d:0x%06X!\r\n",thresInd,Addr);
		}else return SD_Error;
		
		Num = Sd_getPagedataNum(Addr);
		Usart1.Print("[Ser_updateTheThres]正在更新用户%d阈值(%d):0x%06X!，No.%d,Val:%d\r\n",maxOrmin,thresInd,Addr,Num,newThres);
			SD_Status re = SD_Error;
			if(Num >= sd_lineLength){//超过一页的数量，擦除，再写
							re = Sd_cleanUserSector(maxOrmin,sector);
				//共用一个，
//							u8 tempFinal = 0;
//							Sd_readFinaldata(Addr,&tempFinal);//临时保存最后一个阈值，防止擦除有误。！但是断电就全断了，临时保存的也没了！
								if(re == SD_Ok){//擦除成功，则写入新的
										re = Sd_writData(Addr & 0xFFF000,newThres);
										getSdSTA(re);//写入，然后播报播报写入的状态    写入的地址为：擦除后对应的首地址 + 扇区地址 + 首行地址
								}
			}else{//未超过，直接写
								re = Sd_writData(Addr + (Num & 0xFF),newThres);
							//共用同一个，最开始的地址与扇区偏移已确定，至此为行偏移
							getSdSTA(re);//写入的地址为：用户最大阈值首地址 + 对应扇区地址 + 行偏移地址（由Num得来）
							//通过函数getSdSTA()播报相应状态
			}
	return re;
		
}


SD_Status Ser_getNewLedSTA(u8 *ledSTA){
	if(!ledSTA){Usart1.Print("[Ser_getNewLedSTA]Null-Pointre!\r\n");return SD_nullPointer;}
	SD_Status re = SD_Error;
	re = Sd_readFinaldata(sdAddr_ledSTA,ledSTA);
	if(re == SD_Ok){//检查是否为0xFF说明没更新过
		if(*ledSTA == 0xFF){
			*ledSTA = 0;
			Usart1.Print("[Ser_getNewLedSTA]当前未更新过LED状态，已将当前LED状态置为：关!\r\n");
//			return SD_Ok;
		}
		Usart1.Print("[Ser_getNewLedSTA]已获取到上次更新的LED状态：%d√\r\n",*ledSTA);
	}else if(re == SD_Error){
		Usart1.Print("[Ser_getNewLedSTA]获取上次更新的LED状态失败×\r\n");
		
	}else{
		Usart1.Print("[Ser_getNewLedSTA]获取上次更新的LED状态出现其他未解析的状态,请稍后重试\r\n");
	}
	return re;
}

SD_Status Ser_updataLedSTA(u8 *newSTA){
	if(!newSTA){Usart1.Print("[Ser_updataLedSTA]Null-Pointre!\r\n");return SD_nullPointer;}
	SD_Status re = SD_Error;
	u16 Num = Sd_getPagedataNum(sdAddr_ledSTA);
	if(Num >= sd_lineLength){
		re = Sd_eraseTheSector(sdAddr_ledSTA);
		if(re == SD_Ok){
						re = Sd_writData(sdAddr_ledSTA,*newSTA);//擦除后再写入新LED状态
						getSdSTA(re);
						if(re == SD_Ok){
							Usart1.Print("[Ser_updataLedSTA]擦除历史LED状态并写入新状态：%d成功√\r\n",*newSTA);
						}else if(re == SD_Error){
							Usart1.Print("[Ser_updataLedSTA]擦除历史LED状态并写入新状态：%d失败×\r\n",*newSTA);
						}else{
							Usart1.Print("[Ser_updataLedSTA]擦除历史LED状态并写入新状态时出现未解析问题,请稍后重试\r\n");//Busy等
						}
		}else if(re == SD_Error){
			Usart1.Print("[Ser_updataLedSTA]擦除历史LED状态时出现错误×\r\n");
		}else{
			Usart1.Print("[Ser_updataLedSTA]擦除历史LED状态时出现未解析的情况,请稍后重试×\r\n");
		}
	}else{
			re = Sd_writData(sdAddr_ledSTA + (Num & 0xFF),*newSTA);
			if(re == SD_Ok){
							Usart1.Print("[Ser_updataLedSTA]更新LED新状态：%d成功√\r\n",*newSTA);
			}else if(re == SD_Error){
							Usart1.Print("[Ser_updataLedSTA]更新LED新状态：%d失败×\r\n",*newSTA);
			}else{
							Usart1.Print("[Ser_updataLedSTA]更新LED新状态时出现未解析问题,请稍后重试\r\n");//Busy等
			}
	}
	return re;
}


SD_Status Ser_cleanAlluserThres(u8 maxOrmin,u8 *modelArray){
	if(!modelArray){Usart1.Print("[Ser_cleanAlluserThres]Null-Pointer\r\n");return SD_nullPointer;}
	SD_Status re = SD_Error;

	for(u8 i=0;i<sd_ThresNum;i++){
		re = Sd_cleanUserSector(maxOrmin,i);
		if(re == SD_Ok){
			modelArray[i] = (maxOrmin)?defaultMaxThres[i]:defaultMinThres[i];//清空成功默认使用对应的阈值
			Usart1.Print("[Ser_cleanAlluserThres]清空用户%d阈值(%d)成功！\r\n",maxOrmin,i);
		}else if(re == SD_Error){
			Usart1.Print("[Ser_cleanAlluserThres]清空用户%d阈值(%d)失败！\r\n",maxOrmin,i);
		}else{
			Usart1.Print("[Ser_cleanAlluserThres]清空用户%d阈值(%d)状态未解析！\r\n",maxOrmin,i);
		}
	}
	return re;//按照最后一个状态为最终返回值
}

SD_Status Ser_checkHardFault(volatile u8 *status){
	if(!status)return SD_nullPointer;
	*status = Ent_checkHardWare();
	return SD_Ok;
}

