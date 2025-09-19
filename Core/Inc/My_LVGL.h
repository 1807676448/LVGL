#ifndef __MY_LVGL_H
#define __MY_LVGL_H

#include "main.h"
#include "dma.h"
#include "memorymap.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "lcd.h"
#include "sys.h"
#include "..\Core\lvgl\src\lvgl.h"
#include "My_Debug.h"
#include "Touch.h"
#include "My_Data.h"

//控制函数
void my_flush_cb(lv_display_t *display, const lv_area_t *area, uint8_t *px_map);
void my_input_read(lv_indev_t * indev, lv_indev_data_t * data);
//显示函数
void My_Lvgl_Begin(void);
void setup_ui(void);

//接口函数声明
void update_main_screen_info(const char *new_time, const char *new_date, const char *new_weather, const char *new_welcome_msg);


//外部变量声明


#endif