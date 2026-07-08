#ifndef __MY_DATA_NEW_H
#define __MY_DATA_NEW_H

#include "main.h"

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>

#include "My_Debug.h"
#include "My_LVGL.h"
#include "cJSON.h"

//宏定义区
#define Device_Max 4
#define MAX_CONFIG_ITEMS 50
#define MQTT_DEVICE_ID "device_002"
#define MQTT_TOPIC_UP_INDEX 0
#define MQTT_TOPIC_STATUS_INDEX 1
#define MQTT_TOPIC_COMMAND_INDEX 2
#define MQTT_TOPIC_DOWN_INDEX 0
//外部变量引用区
extern volatile uint8_t uart1_rx_buf[500]; // 接收缓冲区
extern volatile int16_t uart1_ins;
extern volatile uint8_t uart2_rx_buf[500]; // 接收缓冲区
extern volatile int16_t uart2_ins;
extern volatile uint8_t uart3_rx_buf[500]; // 接收缓冲区
extern volatile int16_t uart3_ins;

// 配置设备结构体（存储设备状态）
typedef struct {
    int id;              // 设备ID
    char status[16];     // 设备状态字符串 (如 "online", "offline", "active" 等)
    bool valid;          // 该项是否有效（已初始化）
} device_status_t;
// 配置项结构体（存储 JSON 键值对及类型信息）
typedef struct {
    char key[32];        // 配置项键名（最大31个字符，留1位存字符串结束符'\0'）
    cJSON *value;        // 配置项值（cJSON 节点指针，存储具体数据）
    uint8_t type;        // 配置项类型：0-未使用，1-整数，2-浮点数，3-字符串，4-布尔值
} config_item_t;

extern device_status_t device_list[Device_Max];


//函数声明区

void N_My_JsonGet(char *Json_Data,UART_HandleTypeDef *usart);
void Send_JSON(const char *json_str, UART_HandleTypeDef *huart);
void Send_JSON_KeyValue(const char **key, int num, UART_HandleTypeDef *huart);
void MQTT_Subscribe_Downlink(UART_HandleTypeDef *huart);
void MQTT_Request_Time(UART_HandleTypeDef *huart);
void MQTT_Report_Status(UART_HandleTypeDef *huart, const char *status, uint32_t runtime_seconds);
int N_My_JsonQuery(char *key);
void TimeChange(void);

void test(void);


#endif  // __MY_DATA_NEW_H