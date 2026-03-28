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
#include "My_Data_New.h"

#define DeviceHeartTime 30000 //设备心跳时间30秒

//控制函数
void my_flush_cb(lv_display_t *display, const lv_area_t *area, uint8_t *px_map);
void my_input_read(lv_indev_t * indev, lv_indev_data_t * data);
//显示函数
void My_Lvgl_Begin(void);
void setup_ui(void);

//接口函数声明
void update_main_screen_info(const char *new_time, const char *new_date, const char *new_weather, const char *new_welcome_msg);
int update_screen1_item(const char *name, double value);

typedef enum
{
	JOYSTICK_DIR_CENTER = 0,
	JOYSTICK_DIR_UP,
	JOYSTICK_DIR_DOWN,
	JOYSTICK_DIR_LEFT,
	JOYSTICK_DIR_RIGHT
} joystick_direction_t;

typedef struct
{
	joystick_direction_t direction;
	int16_t x_percent; // -100~100，右为正
	int16_t y_percent; // -100~100，上为正
	bool active;
} joystick_state_t;

joystick_direction_t joystick_get_direction(void);
void joystick_get_state(joystick_state_t *out_state);
const char *joystick_direction_to_str(joystick_direction_t dir);
bool joystick_screen_is_active(void);

// 解析串口JSON并更新设备状态
// JSON格式示例: {"device_id": "device_001", "status": "active"} //或 1
void Parse_Device_Status_JSON(const char *json);

void update_device_ui(int idx);

//外部变量声明

#endif