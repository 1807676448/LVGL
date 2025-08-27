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

void my_flush_cb(lv_display_t *display, const lv_area_t *area, uint8_t *px_map);
void Three_Box_Move(void);
void lv_example_get_started_1(void);

#endif