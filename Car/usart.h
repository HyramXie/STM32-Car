#ifndef __USART_H__
#define __USART_H__

#include "stm32f10x.h"

void usart1_init(void);
void usart1_sendstring(char *str);
#endif