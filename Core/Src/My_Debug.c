#include "My_Debug.h"

void My_Usart_Send(char *str)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
}

void My_Usart_Send_Num(int num) {
    char buffer[20]; // 足够存储32位整数的字符串表示
    snprintf(buffer, sizeof(buffer), "%d", num);
    My_Usart_Send(buffer);
    My_Usart_Send("\n\r");
}
