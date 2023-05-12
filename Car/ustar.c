#include "usart.h"
#include <stdio.h>
// usart1中断服务函数，当串口接收到数据时会运行这个函数

char usart_recv[100];		// 全局变量，保存STM32串口接收到的字符串（数据）
int usart_index;			// 全局变量，usart_recv数据的下标
int usart_flag;				// 全局变量，当接收完数据后usart_flag = 1，否则usart_flag = 0

// usart1中断服务函数，当串口接收到数据时会运行这个函数，快进快出
void USART1_IRQHandler(void)
{
	if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET) 					// 判断是否真的接收到数据了
	{
			usart_recv[usart_index] = USART_ReceiveData(USART1);			// 读取接收到的数据
			
			if (usart_recv[usart_index] == '\n')
			{		// 接收完字符串了
					usart_index = 0;
					usart_flag = 1;
			}
			else
			{
				usart_index++;
			}
			
	}
}


void usart1_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 		// 开启USART1外设时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 		// 开启A组外设时钟
	
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9;								// 引脚号
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;					// 开关速度
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;						// 推挽复用输出
	GPIO_Init(GPIOA, &GPIO_InitStruct);			

	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_10;								// 引脚号
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;					// 开关速度
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN_FLOATING;			// 浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStruct);			
	
	
	USART_InitStruct.USART_BaudRate   = 115200;			// 波特率
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;  // 8位有效数据
	USART_InitStruct.USART_StopBits   = USART_StopBits_1;		// 1位停止位
	USART_InitStruct.USART_Parity     = USART_Parity_No; 			// 无校验位
	USART_InitStruct.USART_Mode 			= USART_Mode_Rx | USART_Mode_Tx;	// 收发模式
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		// 无硬件控制流
	USART_Init(USART1, &USART_InitStruct);
	
	
	NVIC_InitStruct.NVIC_IRQChannel	= USART1_IRQn;   				// 中断源						
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;	// 主优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;					// 子优先级
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;						// 使能
	NVIC_Init(&NVIC_InitStruct);					// 中断优先级配置

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);		// 开启接收数据的中断，当串口接收到数据时会运行USART1_IRQHandler这个函数
	USART_Cmd(USART1, ENABLE);	// 串口1使能
}

void usart1_sendstring(char *str)
{
	while(*str != '\0')
	{
		USART_SendData(USART1, *str);					// 将读到的数据发送回去
		str++;
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		// 等待发送数据寄存器为空
	}
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);		// 等待发送完
}

// 重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到串口 */
		USART_SendData(USART1, (uint8_t) ch);

		/* 等待发送完毕 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}


