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

void my_flush_cb(lv_display_t *display, const lv_area_t *area, uint8_t *px_map);
void my_input_read(lv_indev_t * indev, lv_indev_data_t * data);
void Three_Box_Move(void);
void lv_example_get_started_1(void);
void lv_example_button_3(void);
void create_eight_points(void);

// 基本界面：左侧4个页面选项按钮，右侧内容区 480x320
void Ji_Ben_Jie_Mian(void);



#endif