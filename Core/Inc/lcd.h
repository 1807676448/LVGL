#ifndef __LCD_H
#define __LCD_H

#include "stm32h7xx_hal.h"
#include "spi.h"
#include "gpio.h"
#include "main.h"
#include "lcd.h"
#include "stdlib.h"
#include "dma.h"
#include <stdint.h>

#define AXI_SRAM_VAR __attribute__((section(".axi_sram")))

//========================================= 电源接线 ============================================//
//  LCD模块引脚   |   连接到STM32引脚   |   说明
//---------------|----------------------|------------------------------------------------
//  VCC           |   DC5V/3.3V          | 电源输入
//  GND           |   GND                | 电源地
//=============================================================================================//

//======================================= 液晶屏数据线接线 =======================================//
//  说明：本模块默认数据总线类型为SPI总线
//
//  LCD模块引脚   |   连接到STM32引脚   |   说明
//---------------|----------------------|-----------------------------------------                  -------
//  SDI(MOSI)     |   PA6                | 液晶屏SPI总线数据写信号
//  SDO(MISO)     |   PA7                | 液晶屏SPI总线数据读信号（无需读取时可不接）
//=============================================================================================//

//======================================= 液晶屏控制线接线 =======================================//
//  LCD模块引脚   |   连接到STM32引脚   |   说明
//---------------|----------------------|------------------------------------------------
//  LED           |   PC4                | 液晶屏背光控制信号（无需控制时可接5V/3.3V）
//  SCK           |   PA5                | 液晶屏SPI总线时钟信号
//  DC/RS         |   PC5                | 液晶屏数据/命令控制信号（高电平数据，低电平命令）
//  RST           |   PB1                | 液晶屏复位控制信号（低电平复位）
//  CS            |   PB0                | 液晶屏片选控制信号（低电平选中）
//=============================================================================================//

//========================================= 触摸屏接线 ==========================================//
//  LCD模块引脚   |   连接到STM32引脚   |   说明
//---------------|----------------------|------------------------------------------------
//  T_IRQ         |   PF13               | 触摸屏触摸中断信号（低电平表示有触摸）
//  T_DO          |   PF14               | 触摸屏SPI总线读信号
//  T_DIN         |   PF15               | 触摸屏SPI总线写信号
//  T_CS          |   PG0                | 触摸屏片选控制信号（低电平选中）
//  T_CLK         |   PG1                | 触摸屏SPI总线时钟信号
//=============================================================================================//

/**
 * @file lcd.h
 * @brief SPI接口LCD驱动头文件，适配ILI9488等常见彩屏
 *
 * 包含LCD引脚定义、参数结构体、常用颜色、主要操作函数声明等。
 * 支持横竖屏切换、DMA刷屏、像素/区域绘制等。
 */

// LCD参数结构体
typedef struct
{
    uint16_t width;   // LCD宽度
    uint16_t height;  // LCD高度
    uint16_t id;      // LCD ID
    uint8_t dir;      // 屏幕方向：0=竖屏，1=横屏
    uint16_t wramcmd; // 写GRAM指令
    uint16_t setxcmd; // 设置X坐标指令
    uint16_t setycmd; // 设置Y坐标指令
} _lcd_dev;
extern _lcd_dev lcddev; // 全局LCD参数

// LCD参数
#define USE_HORIZONTAL 1 // 屏幕旋转方向（0=竖屏，1=横屏，2=180度，3=270度）
#define LCD_W 480        // 屏幕物理宽度
#define LCD_H 320        // 屏幕物理高度

// TFTLCD部分外要调用的函数
extern uint16_t POINT_COLOR; // 当前画笔颜色
extern uint16_t BACK_COLOR;  // 背景颜色

//-----------------LCD端口定义----------------

#define LED 4 // 背光控制引脚
#define CS 0  // 片选引脚
#define RS 5  // 寄存器/数据选择引脚
#define RST 1 // 复位引脚

// QDtech全系列模块采用了三极管控制背光亮灭，用户也可以接PWM调节背光亮度
#define LCD_LED HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET) // LCD背光

#define LCD_CS HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET)
#define LCD_RS HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET)
#define LCD_RST HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET)

#define LCD_CS_SET HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET)  // 片选端口
#define LCD_RS_SET HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET)  // 数据/命令
#define LCD_RST_SET HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET) // 复位

#define LCD_CS_CLR HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET)  // 片选端口
#define LCD_RS_CLR HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET)  // 数据/命令
#define LCD_RST_CLR HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET) // 复位

// 常用颜色宏（RGB565）
#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE 0x001F
#define BRED 0XF81F
#define GRED 0XFFE0
#define GBLUE 0X07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define GREEN 0x07E0
#define CYAN 0x7FFF
#define YELLOW 0xFFE0
#define BROWN 0XBC40 // 棕色
#define BRRED 0XFC07 // 棕红色
#define GRAY 0X8430  // 灰色
// GUI颜色

#define DARKBLUE 0X01CF  // 深蓝色
#define LIGHTBLUE 0X7D7C // 浅蓝色
#define GRAYBLUE 0X5458  // 灰蓝色
// 以上三色为PANEL的颜色

#define LIGHTGREEN 0X841F // 浅绿色
#define LIGHTGRAY 0XEF5B  // 浅灰色(PANNEL)
#define LGRAY 0XC618      // 浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE 0XA651 // 浅灰蓝色(中间层颜色)
#define LBBLUE 0X2B12    // 浅棕蓝色(选择条目的反色)

/**
 * @brief LCD基本操作函数声明
 */
void LCD_Init(void);                              // 初始化LCD
void LCD_DisplayOn(void);                         // 打开显示
void LCD_DisplayOff(void);                        // 关闭显示
void LCD_Clear(uint16_t Color);                   // 全屏填充
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos); // 设置光标
void LCD_DrawPoint(uint16_t x, uint16_t y);       // 画点
void LCD_DrawPoint_Color(uint16_t x, uint16_t y, uint16_t color);
uint16_t LCD_ReadPoint(uint16_t x, uint16_t y);                                    // 读点
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);             // 画线
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);        // 画矩形
void LCD_SetWindows(uint16_t xStar, uint16_t yStar, uint16_t xEnd, uint16_t yEnd); // 设置窗口

uint16_t LCD_RD_DATA(void); // 读取LCD数据
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue);
void LCD_WR_DATA(uint8_t data);
uint16_t LCD_ReadReg(uint8_t LCD_Reg);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(uint16_t RGB_Code);
uint16_t LCD_ReadRAM(void);
uint16_t LCD_BGR2RGB(uint16_t c);
void LCD_SetParam(void);
void Lcd_WriteData_16Bit(uint16_t Data);
void LCD_direction(uint8_t direction);

#define LCD_DMA_BUFFER_SIZE (LCD_W * LCD_H * 3 / 8) // 使用一个较小的值以避免RAM溢出
#define BYTES_PER_PIXEL 2                           // RGB565格式为2字节
#define PIXELS_PER_DMA_BUFFER (LCD_DMA_BUFFER_SIZE / BYTES_PER_PIXEL)

extern volatile uint8_t spi_dma_tx_complete;




#define DMA_BUFFER_PIXEL_COUNT (LCD_W * LCD_H * 3 / 8)
#define OneStepSize (LCD_W * LCD_H / 8) // lvgl单次刷新像素点数
#define OnePointSize_Lvgl 2              // legl单个像素点占用uint8_t数目
#define OnePointSize_DMA 3               //  单个像素点DMA需要传输3个uint8_t数据
extern uint8_t dma_buffer[OneStepSize*OnePointSize_DMA];

#endif /* __LCD_H */