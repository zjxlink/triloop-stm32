#include <stdio.h>
#include <string.h>

#include <stm32f4xx_ll_usart.h>

#include "tool.h"

void My_USART_Send(USART_TypeDef *uart, uint8_t *ptr, int len)
{
    while(len--)
    {
        LL_USART_TransmitData8(uart, *ptr++);
        while(!LL_USART_IsActiveFlag_TXE(uart));
    }
}