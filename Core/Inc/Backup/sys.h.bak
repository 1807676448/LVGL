#ifndef __SYS_H
#define __SYS_H

#include "stm32h7xx_hal.h"

/*
 * 注意: STM32H7 (Cortex-M7) 内核不支持位带操作。
 * 以下宏定义使用STM32 HAL库函数进行封装，以提供一种简洁的GPIO操作方式。
 * HAL库的 HAL_GPIO_WritePin 函数内部通过BSRR寄存器实现，本身就是原子操作。
 */

// --- GPIO 输出宏 ---
// 用法:
// PA_OUT(5, 1); // 将PA5设置为高电平
// PB_OUT(12, 0); // 将PB12设置为低电平
#define PA_OUT(pin, val)  HAL_GPIO_WritePin(GPIOA, (1U << pin), (val) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define PB_OUT(pin, val)  HAL_GPIO_WritePin(GPIOB, (1U << pin), (val) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define PC_OUT(pin, val)  HAL_GPIO_WritePin(GPIOC, (1U << pin), (val) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define PD_OUT(pin, val)  HAL_GPIO_WritePin(GPIOD, (1U << pin), (val) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define PE_OUT(pin, val)  HAL_GPIO_WritePin(GPIOE, (1U << pin), (val) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define PF_OUT(pin, val)  HAL_GPIO_WritePin(GPIOF, (1U << pin), (val) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define PG_OUT(pin, val)  HAL_GPIO_WritePin(GPIOG, (1U << pin), (val) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define PH_OUT(pin, val)  HAL_GPIO_WritePin(GPIOH, (1U << pin), (val) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define PI_OUT(pin, val)  HAL_GPIO_WritePin(GPIOI, (1U << pin), (val) ? GPIO_PIN_SET : GPIO_PIN_RESET)

// --- GPIO 输入宏 ---
// 用法:
// uint8_t status = PA_IN(0); // 读取PA0的电平 (返回 1 或 0)
#define PA_IN(pin)        (HAL_GPIO_ReadPin(GPIOA, (1U << pin)) == GPIO_PIN_SET ? 1 : 0)
#define PB_IN(pin)        (HAL_GPIO_ReadPin(GPIOB, (1U << pin)) == GPIO_PIN_SET ? 1 : 0)
#define PC_IN(pin)        (HAL_GPIO_ReadPin(GPIOC, (1U << pin)) == GPIO_PIN_SET ? 1 : 0)
#define PD_IN(pin)        (HAL_GPIO_ReadPin(GPIOD, (1U << pin)) == GPIO_PIN_SET ? 1 : 0)
#define PE_IN(pin)        (HAL_GPIO_ReadPin(GPIOE, (1U << pin)) == GPIO_PIN_SET ? 1 : 0)
#define PF_IN(pin)        (HAL_GPIO_ReadPin(GPIOF, (1U << pin)) == GPIO_PIN_SET ? 1 : 0)
#define PG_IN(pin)        (HAL_GPIO_ReadPin(GPIOG, (1U << pin)) == GPIO_PIN_SET ? 1 : 0)
#define PH_IN(pin)        (HAL_GPIO_ReadPin(GPIOH, (1U << pin)) == GPIO_PIN_SET ? 1 : 0)
#define PI_IN(pin)        (HAL_GPIO_ReadPin(GPIOI, (1U << pin)) == GPIO_PIN_SET ? 1 : 0)

// --- GPIO 翻转宏 ---
// 用法:
// PA_TOGGLE(5); // 翻转PA5的电平
#define PA_TOGGLE(pin)    HAL_GPIO_TogglePin(GPIOA, (1U << pin))
#define PB_TOGGLE(pin)    HAL_GPIO_TogglePin(GPIOB, (1U << pin))
#define PC_TOGGLE(pin)    HAL_GPIO_TogglePin(GPIOC, (1U << pin))
#define PD_TOGGLE(pin)    HAL_GPIO_TogglePin(GPIOD, (1U << pin))
#define PE_TOGGLE(pin)    HAL_GPIO_TogglePin(GPIOE, (1U << pin))
#define PF_TOGGLE(pin)    HAL_GPIO_TogglePin(GPIOF, (1U << pin))
#define PG_TOGGLE(pin)    HAL_GPIO_TogglePin(GPIOG, (1U << pin))
#define PH_TOGGLE(pin)    HAL_GPIO_TogglePin(GPIOH, (1U << pin))
#define PI_TOGGLE(pin)    HAL_GPIO_TogglePin(GPIOI, (1U << pin))


// --- 低级CPU控制宏 (CMSIS Core API) ---

// 执行WFI (Wait For Interrupt) 指令
#define WFI_SET()       __WFI()

// 关闭所有可屏蔽中断
#define INTX_DISABLE()  __disable_irq()

// 开启所有可屏蔽中断
#define INTX_ENABLE()   __enable_irq()

// 设置主堆栈指针 (Main Stack Pointer)
#define MSR_MSP(addr)   __set_MSP(addr)


#endif
