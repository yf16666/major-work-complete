#include "M_Usart.h"

#define Cerat_X_Print(X)\
    static void Usart##X##_Print(char *fmt, ...) {\
        char Buff[USART_Buf_Len + 1];\
        size_t i = 0;\
        va_list arg_ptr;\
        va_start(arg_ptr, fmt);\
        int len = vsprintf(Buff, fmt, arg_ptr);\
        if (len > USART_Buf_Len) {\
            len = USART_Buf_Len;\
        }\
        while((i < USART_Buf_Len) && (i < len)){\
            while((USART##X->SR & 0x40) == 0){\
                /* 等待发送数据寄存器空闲 */\
            }\
            USART##X->DR = (u8)Buff[i++];\
        }\
        va_end(arg_ptr);\
    }


Cerat_X_Print(1)
Cerat_X_Print(2) 
Cerat_X_Print(3)

void Usartx_Init(Usart* Usartx,u8 idle_status){
	if(Usartx->USARTx == NULL) {
        return;  //未指定初始化的串口
	}
	GPIO_TypeDef *Uartx;
	uint16_t	Tx_Pin;
	uint16_t	Rx_Pin;
	uint8_t Usartx_Channel;
	if(Usartx->USARTx == USART1){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA,ENABLE);
		Uartx = GPIOA;
		Tx_Pin = GPIO_Pin_9;
		Rx_Pin = GPIO_Pin_10;
		Usartx_Channel = USART1_IRQn;
		Usartx->Print = Usart1_Print;
	}else if(Usartx->USARTx == USART2){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
		Uartx = GPIOA;
		Tx_Pin = GPIO_Pin_2;
		Rx_Pin = GPIO_Pin_3;
		Usartx_Channel = USART2_IRQn;
		Usartx->Print = Usart2_Print;
	}else if(Usartx->USARTx == USART3){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
		Uartx = GPIOB;
		Tx_Pin = GPIO_Pin_10;
		Rx_Pin = GPIO_Pin_11;
		Usartx_Channel = USART3_IRQn;
		Usartx->Print = Usart3_Print;
	}

	GPIO_InitTypeDef Usartx_Pin;
	Usartx_Pin.GPIO_Mode = GPIO_Mode_AF_PP;
	Usartx_Pin.GPIO_Pin = Tx_Pin;
	Usartx_Pin.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Uartx,&Usartx_Pin);
	
	Usartx_Pin.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	Usartx_Pin.GPIO_Pin = Rx_Pin;
	Usartx_Pin.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Uartx,&Usartx_Pin);
	
	USART_InitTypeDef Usartx_ART;
	Usartx_ART.USART_BaudRate = Usartx->Baud;//
	Usartx_ART.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	Usartx_ART.USART_Parity = USART_Parity_No;
	Usartx_ART.USART_StopBits = USART_StopBits_1;
	Usartx_ART.USART_WordLength = USART_WordLength_8b;
	Usartx_ART.USART_Mode = USART_Mode_Rx |USART_Mode_Tx;
	USART_Init(Usartx->USARTx,&Usartx_ART);

	NVIC_InitTypeDef Usart_NVIC;
	Usart_NVIC.NVIC_IRQChannel = Usartx_Channel;
	Usart_NVIC.NVIC_IRQChannelPreemptionPriority = Usartx->PreemptionPriority;//抢占
	Usart_NVIC.NVIC_IRQChannelSubPriority = Usartx->SubPriority;//响应
	Usart_NVIC.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&Usart_NVIC);

	if(idle_status){
		USART_ITConfig(Usartx->USARTx, USART_IT_IDLE, ENABLE);
		USART_ITConfig(Usartx->USARTx,USART_IT_RXNE,ENABLE);
		USART_Cmd(Usartx->USARTx,ENABLE);
	}else{
		USART_ITConfig(Usartx->USARTx,USART_IT_RXNE,ENABLE);
//	USART_ClearFlag(USART1,USART_FLAG_TXE | USART_FLAG_RXNE);
		USART_Cmd(Usartx->USARTx,ENABLE);
	}

}

/*
void USART1_IRQhandler(void){
	
	if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE) == SET){
		
		
	}
	
}


void USART2_IRQhandler(void){
	
	if(USART_GetFlagStatus(USART2,USART_FLAG_RXNE) == SET){
		
	}
}
void USART3_IRQhandler(void){
	
	if(USART_GetFlagStatus(USART3,USART_FLAG_RXNE) == SET){
		
	}
	
}
*/




