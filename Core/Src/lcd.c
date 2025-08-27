#include "spi.h" // 包含此文件以获取hspi1句柄
#include "lcd.h"
#include "sys.h"
#include "main.h" // 包含此文件以获取HAL_Delay

// DMA传输完成标志 (volatile关键字确保编译器不会优化掉对它的访问)
volatile uint8_t spi_dma_tx_complete = 1;

// 为DMA传输定义一个大的静态缓冲区，以减少DMA启动次数
// 大小应为3的倍数(适配ILI9488的18-bit模式)，且小于65535
uint8_t dma_buffer[OneStepSize*OnePointSize_DMA];

// 管理LCD重要参数
// 默认为竖屏
_lcd_dev lcddev;

// 画笔颜色,背景颜色
uint16_t POINT_COLOR = 0x0000, BACK_COLOR = 0xFFFF;
uint16_t DeviceCode;

/*****************************************************************************
 * @name       :void LCD_WR_REG(uint8_t data)
 * @date       :2018-08-09 (Ported to HAL)
 * @function   :向LCD屏幕写入8位命令
 * @parameters :data:要写入的命令值
 * @retvalue   :无
 ******************************************************************************/
void LCD_WR_REG(uint8_t data)
{
    LCD_CS_CLR;
    LCD_RS_CLR;
    HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
    LCD_CS_SET;
}

/*****************************************************************************
 * @name       :void LCD_WR_DATA(uint8_t data)
 * @date       :2018-08-09 (Ported to HAL)
 * @function   :向LCD屏幕写入8位数据
 * @parameters :data:要写入的数据值
 * @retvalue   :无
 ******************************************************************************/
void LCD_WR_DATA(uint8_t data)
{
    LCD_CS_CLR;
    LCD_RS_SET;
    HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
    LCD_CS_SET;
}

/*****************************************************************************
 * @name       :void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
 * @date       :2018-08-09 (Ported to HAL)
 * @function   :向寄存器写入数据
 * @parameters :LCD_Reg:寄存器地址
                LCD_RegValue:要写入的数据
 * @retvalue   :无
******************************************************************************/
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{
    LCD_WR_REG(LCD_Reg);
    LCD_WR_DATA(LCD_RegValue);
}

/*****************************************************************************
 * @name       :void LCD_WriteRAM_Prepare(void)
 * @date       :2018-08-09
 * @function   :准备写入GRAM
 * @parameters :无
 * @retvalue   :无
 ******************************************************************************/
void LCD_WriteRAM_Prepare(void)
{
    LCD_WR_REG(lcddev.wramcmd);
}

/*****************************************************************************
 * @name       :void Lcd_WriteData_16Bit(uint16_t Data)
 * @date       :2018-08-09 (Ported to HAL)
 * @function   :向LCD屏幕写入16位颜色数据 (18-bit format)
 * @parameters :Data:要写入的16位数据
 * @retvalue   :无
 ******************************************************************************/
void Lcd_WriteData_16Bit(uint16_t Data)
{
    uint8_t data_to_send[3];
    data_to_send[0] = (Data >> 8) & 0xF8; // RED
    data_to_send[1] = (Data >> 3) & 0xFC; // GREEN
    data_to_send[2] = Data << 3;          // BLUE

    LCD_CS_CLR;
    LCD_RS_SET;
    HAL_SPI_Transmit(&hspi1, data_to_send, 3, HAL_MAX_DELAY);
    LCD_CS_SET;
}

/*****************************************************************************
 * @name       :void LCD_DrawPoint(uint16_t x,uint16_t y)
 * @date       :2018-08-09
 * @function   :在指定位置绘制一个像素点
 * @parameters :x:像素点的X坐标, y:像素点的Y坐标
 * @retvalue   :无
 ******************************************************************************/
void LCD_DrawPoint(uint16_t x, uint16_t y)
{
    LCD_SetCursor(x, y);
    Lcd_WriteData_16Bit(POINT_COLOR);
}

void LCD_DrawPoint_Color(uint16_t x, uint16_t y, uint16_t color)
{
    LCD_SetCursor(x, y);
    Lcd_WriteData_16Bit(color);
}
// /*****************************************************************************
//  * @name       :static void LCD_SPI_DMA_Wait(uint32_t Timeout)
//  * @date       :2025-08-18
//  * @function   :等待SPI DMA传输完成，带超时处理
//  * @parameters :Timeout: 超时时间 (ms)
//  * @retvalue   :无
//  ******************************************************************************/
// static void LCD_SPI_DMA_Wait(uint32_t Timeout)
// {
//     uint32_t tickstart = HAL_GetTick();
//     while (spi_dma_tx_complete == 0)
//     {
//         if ((HAL_GetTick() - tickstart) > Timeout)
//         {
//             HAL_SPI_Abort(&hspi1);
//             spi_dma_tx_complete = 1;
//             break;
//         }
//     }
// }

/*****************************************************************************
 * @name       :void LCD_Clear(uint16_t Color)
 * @date       :2025-08-19 (Reverted to non-DMA)
 * @function   :全屏填充指定颜色 (使用逐点传输)
 * @parameters :Color:填充颜色值
 * @retvalue   :无
 ******************************************************************************/

void LCD_Clear(uint16_t Color)
{
    int32_t w = 320;
    int32_t h = 480;
    int32_t total_pixels_to_flush = w * h * 3;

    // 2. 设置LCD硬件的刷新窗口
    LCD_SetWindows(0, 0, w - 1, h - 1);

    // 3. 准备�?始传�?

    // 4. 分块进行传
    while (total_pixels_to_flush > 0)
    {
        // 计算当前块能传输多少像素
        int32_t chunk_pixel_count = total_pixels_to_flush;
        if (chunk_pixel_count > OneStepSize*OnePointSize_DMA)
        {
            chunk_pixel_count = OneStepSize*OnePointSize_DMA;
        }

        // **核心：进行颜色格式转�? (�? RGB565 -> RGB666)**
        for (int32_t i = 0; i < chunk_pixel_count / 3; i++)
        {
            uint16_t color16 = Color;
            
            dma_buffer[i * 3 + 0] = (color16 >> 8) & 0xF8; // R
            dma_buffer[i * 3 + 1] = (color16 >> 3) & 0xFC; // G
            dma_buffer[i * 3 + 2] = (color16 << 3);        // B
        }

        // 等待上一次DMA传输完成
        while (!spi_dma_tx_complete)
            ;

        // 清除标志位，准备�?始新的DMA传输
        spi_dma_tx_complete = 0;

        LCD_CS_CLR;
        LCD_RS_SET;

        // 启动DMA传输
        HAL_SPI_Transmit_DMA(&hspi1, dma_buffer, chunk_pixel_count);

        // 更新剩余像素计数和颜色指�?
        total_pixels_to_flush -= chunk_pixel_count;
    }
    while (!spi_dma_tx_complete)
        ;
}
/*****************************************************************************
 * @name       :void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
 * @date       :2025-08-17
 * @function   :SPI DMA发送完成回调函数
 * @parameters :hspi: SPI句柄指针
 * @retvalue   :无
 ******************************************************************************/
// void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
// {
//     if (hspi->Instance == SPI1)
//     {
//         LCD_CS_SET;
//         // PG_TOGGLE(7); // 等待完成时可以做点别的事，比如切换LED状态
//         spi_dma_tx_complete = 1;
//     }
// }

/*****************************************************************************
 * @name       :void LCD_RESET(void)
 * @date       :2018-08-09 (Ported to HAL)
 * @function   :复位LCD屏幕
 * @parameters :无
 * @retvalue   :无
 ******************************************************************************/
void LCD_RESET(void)
{
    LCD_RST_CLR;
    HAL_Delay(100);
    LCD_RST_SET;
    HAL_Delay(50);
}

/*****************************************************************************
 * @name       :void LCD_Init(void)
 * @date       :2018-08-09 (Ported to HAL)
 * @function   :初始化LCD屏幕
 * @parameters :无
 * @retvalue   :无
 ******************************************************************************/
void LCD_Init(void)
{
    LCD_RESET(); // LCD 复位
                 //************* ILI9488初始化 **********//
    LCD_WR_REG(0XF7);
    LCD_WR_DATA(0xA9);
    LCD_WR_DATA(0x51);
    LCD_WR_DATA(0x2C);
    LCD_WR_DATA(0x82);
    LCD_WR_REG(0xC0);
    LCD_WR_DATA(0x11);
    LCD_WR_DATA(0x09);
    LCD_WR_REG(0xC1);
    LCD_WR_DATA(0x41);
    LCD_WR_REG(0XC5);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x0A);
    LCD_WR_DATA(0x80);
    LCD_WR_REG(0xB1);
    LCD_WR_DATA(0xB0);
    LCD_WR_DATA(0x11);
    LCD_WR_REG(0xB4);
    LCD_WR_DATA(0x02);
    LCD_WR_REG(0xB6);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x42);
    LCD_WR_REG(0xB7);
    LCD_WR_DATA(0xc6);
    LCD_WR_REG(0xBE);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x04);
    LCD_WR_REG(0xE9);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0x36);
    LCD_WR_DATA((1 << 3) | (0 << 7) | (1 << 6) | (1 << 5));
    LCD_WR_REG(0x3A);
    LCD_WR_DATA(0x66);
    LCD_WR_REG(0xE0);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x07);
    LCD_WR_DATA(0x10);
    LCD_WR_DATA(0x09);
    LCD_WR_DATA(0x17);
    LCD_WR_DATA(0x0B);
    LCD_WR_DATA(0x41);
    LCD_WR_DATA(0x89);
    LCD_WR_DATA(0x4B);
    LCD_WR_DATA(0x0A);
    LCD_WR_DATA(0x0C);
    LCD_WR_DATA(0x0E);
    LCD_WR_DATA(0x18);
    LCD_WR_DATA(0x1B);
    LCD_WR_DATA(0x0F);
    LCD_WR_REG(0XE1);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x17);
    LCD_WR_DATA(0x1A);
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x0E);
    LCD_WR_DATA(0x06);
    LCD_WR_DATA(0x2F);
    LCD_WR_DATA(0x45);
    LCD_WR_DATA(0x43);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x0A);
    LCD_WR_DATA(0x09);
    LCD_WR_DATA(0x32);
    LCD_WR_DATA(0x36);
    LCD_WR_DATA(0x0F);
    LCD_WR_REG(0x11);
    HAL_Delay(120);
    LCD_WR_REG(0x29);

    LCD_direction(USE_HORIZONTAL); // 设置LCD显示方向
    LCD_LED;                       // 点亮背光
    LCD_Clear(WHITE);              // 清全屏白色
}

/*****************************************************************************
 * @name       :void LCD_SetWindows(uint16_t xStar, uint16_t yStar,uint16_t xEnd,uint16_t yEnd)
 * @date       :2018-08-09 (Ported to HAL)
 * @function   :设置LCD显示窗口
 * @parameters :xStar, yStar, xEnd, yEnd: 窗口坐标
 * @retvalue   :无
 ******************************************************************************/
void LCD_SetWindows(uint16_t xStar, uint16_t yStar, uint16_t xEnd, uint16_t yEnd)
{
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(xStar >> 8);
    LCD_WR_DATA(0x00FF & xStar);
    LCD_WR_DATA(xEnd >> 8);
    LCD_WR_DATA(0x00FF & xEnd);

    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(yStar >> 8);
    LCD_WR_DATA(0x00FF & yStar);
    LCD_WR_DATA(yEnd >> 8);
    LCD_WR_DATA(0x00FF & yEnd);

    LCD_WriteRAM_Prepare(); // 准备写入GRAM
}

/*****************************************************************************
 * @name       :void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
 * @date       :2018-08-09
 * @function   :设置光标位置
 * @parameters :Xpos, Ypos: 光标坐标
 * @retvalue   :无
 ******************************************************************************/
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
    LCD_SetWindows(Xpos, Ypos, Xpos, Ypos);
}

/*****************************************************************************
 * @name       :void LCD_direction(uint8_t direction)
 * @date       :2018-08-09 (Ported to HAL)
 * @function   :设置LCD屏幕的显示方向
 * @parameters :direction: 0-竖屏, 1-横屏, 2-180度, 3-270度
 * @retvalue   :无
 ******************************************************************************/
void LCD_direction(uint8_t direction)
{
    lcddev.setxcmd = 0x2A;
    lcddev.setycmd = 0x2B;
    lcddev.wramcmd = 0x2C;
    switch (direction)
    {
    case 0:
        lcddev.width = LCD_W;
        lcddev.height = LCD_H;
        LCD_WriteReg(0x36, (1 << 3) | (0 << 6) | (0 << 7)); // BGR=1,MY=0,MX=0,MV=0
        break;
    case 1:
        lcddev.width = LCD_H;
        lcddev.height = LCD_W;
        LCD_WriteReg(0x36, (1 << 3) | (0 << 7) | (1 << 6) | (1 << 5)); // BGR=1,MY=1,MX=0,MV=1
        break;
    case 2:
        lcddev.width = LCD_W;
        lcddev.height = LCD_H;
        LCD_WriteReg(0x36, (1 << 3) | (1 << 6) | (1 << 7)); // BGR=1,MY=0,MX=1,MV=0 -> Corrected for 180
        break;
    case 3:
        lcddev.width = LCD_H;
        lcddev.height = LCD_W;
        LCD_WriteReg(0x36, (1 << 3) | (1 << 7) | (1 << 5)); // BGR=1,MY=1,MX=1,MV=1 -> Corrected for 270
        break;
    default:
        break;
    }
}
