#include "My_Debug.h"

static uint32_t SystemTick;
static uint8_t ZhuangTai;

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

void My_Tick_Begin(void){
    SystemTick = HAL_GetTick();
}

uint32_t My_Tick_End(void){
    uint32_t a = HAL_GetTick()-SystemTick;
    My_Usart_Send_Num(a);
    My_Usart_Send("----------\n\r");
    return a;
}

uint32_t My_Tick_Get(void){
    uint32_t a = HAL_GetTick();
    if(ZhuangTai == 0){
        SystemTick = a;
        ZhuangTai = 1;
        return 0;
    }
    else{
        ZhuangTai = 0;
        My_Usart_Send_Num(a - SystemTick);
        return a - SystemTick;
    }
}