#ifndef __TOOL_H
#define __TOOL_H

#include <stdint.h>
#include "stm32f407xx.h"

extern void My_USART_Send(USART_TypeDef *uart, uint8_t *ptr, int len);

#endif