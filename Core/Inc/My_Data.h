#ifndef MY_JSON_UTILS_H
#define MY_JSON_UTILS_H

// My_Data.c 顶部的头文件区域
#include <stdlib.h>   // 新增：用于 free 函数声明
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "cJSON.h"
#include "My_Data.h"
#include "stm32h7xx_hal.h"
#include "main.h"           // 新增：后续解决 huart1 需用到（main.h 通常包含 huart1 声明）
#include "My_LVGL.h"
#include "My_Debug.h"

/* --------------------------- 宏定义声明 --------------------------- */
// 配置项数组最大容量（外部可能需要知晓该限制，故对外声明）
#define MAX_CONFIG_ITEMS 50


/* --------------------------- 结构体类型声明 --------------------------- */
// 配置项结构体（存储 JSON 键值对及类型信息，外部可能需访问配置项属性）
typedef struct {
    char key[32];        // 配置项键名（最大31个字符，留1位存字符串结束符'\0'）
    cJSON *value;        // 配置项值（cJSON 节点指针，存储具体数据）
    uint8_t type;        // 配置项类型：0-未使用，1-整数，2-浮点数，3-字符串，4-布尔值
} config_item_t;


/* --------------------------- 外部变量声明 --------------------------- */
// UART1 接收状态标志（外部文件需访问该标志判断接收状态）
extern int16_t uart1_ins;

// UART1 接收缓冲区（外部文件需访问该缓冲区获取接收的 JSON 数据）
extern uint8_t uart1_rx_buf[500];


/* --------------------------- 基础工具函数声明 --------------------------- */
/**
 * @brief 清空指定长度的字节数组（将数组所有元素置0）
 * @param arr: 待清空的数组指针（uint8_t 类型）
 * @param len: 数组长度（size_t 类型，确保与数组实际长度匹配）
 */
void clear_array(uint8_t *arr, size_t len);


/* --------------------------- JSON 配置项操作函数声明 --------------------------- */
/**
 * @brief 存储整数类型配置项（键存在则更新值，不存在则新增配置项）
 * @param key: 配置项键名（字符串指针，需确保非NULL）
 * @param value: 整数类型值（int 类型）
 */
void My_SendJson_Change_Int(const char *key, int value);

/**
 * @brief 存储浮点数类型配置项（键存在则更新值，不存在则新增配置项）
 * @param key: 配置项键名（字符串指针，需确保非NULL）
 * @param value: 浮点数类型值（double 类型，支持高精度小数）
 */
void My_SendJson_Change_Double(const char *key, double value);

/**
 * @brief 存储字符串类型配置项（键存在则更新值，不存在则新增配置项）
 * @param key: 配置项键名（字符串指针，需确保非NULL）
 * @param value: 字符串类型值（字符串指针，需确保非NULL）
 */
void My_SendJson_Change_String(const char *key, const char *value);

/**
 * @brief 存储布尔类型配置项（键存在则更新值，不存在则新增配置项）
 * @param key: 配置项键名（字符串指针，需确保非NULL）
 * @param value: 布尔类型值（bool 类型，true/false）
 */
void My_SendJson_Change_Bool(const char *key, bool value);

/**
 * @brief 通用配置项存储函数（根据类型自动调用对应类型的存储函数）
 * @param key: 配置项键名（字符串指针，需确保非NULL）
 * @param value: 数据指针（需根据 value_type 转换为对应类型，确保指针非NULL）
 * @param value_type: 数据类型（1-整数，2-浮点数，3-字符串，4-布尔值）
 */
void My_SendJson_Change(const char *key, void *value, uint8_t value_type);

/**
 * @brief 删除指定键名的配置项（键不存在则无操作，删除后自动整理数组）
 * @param key: 待删除配置项的键名（字符串指针，需确保非NULL）
 */
void My_SendJson_Remove(const char *key);

/**
 * @brief 获取当前存储的配置项总数
 * @return 配置项数量（uint8_t 类型，最大不超过 MAX_CONFIG_ITEMS）
 */
uint8_t My_SendJson_GetCount(void);

/**
 * @brief 生成所有配置项的 JSON 字符串并发送（通过 UART 发送，需自行实现发送逻辑）
 * @note 内部会创建 JSON 根对象，序列化后发送，发送完成自动释放内存
 */
void My_SendJson_Send(void);

/**
 * @brief 打印所有存储的配置项（调试用，输出键名、值、类型到控制台）
 */
void My_SendJson_PrintAll(void);

/**
 * @brief 清空所有配置项（释放所有 cJSON 节点内存，重置配置项计数）
 */
void My_SendJson_ClearAll(void);


/* --------------------------- JSON 解析/测试函数声明 --------------------------- */
/**
 * @brief JSON 文本解析测试（固定 JSON 字符串解析，用于验证解析逻辑）
 * @note 内部会解析预设的 JSON 字符串，打印原始内容并遍历输出键值对
 */
void My_cJSON_Text(void);

/**
 * @brief 解析外部传入的 JSON 数据（从 UART 接收缓冲区获取数据，解析后更新配置项）
 * @param json_data: 待解析的 JSON 字符串指针（需确保非NULL且格式正确）
 * @note 解析完成后会重置 UART 接收状态，重新开启 UART 中断接收
 */
void My_cJSON_Get(char *json_data, UART_HandleTypeDef *huart);

/**
 * @brief （待实现）更新指定键名的整数类型配置项
 * @param key: 配置项键名（字符串指针，需确保非NULL）
 * @param valueint: 待更新的整数类型值（int 类型）
 * @note 原代码中该函数为空实现，需根据需求补充逻辑
 */
void My_cJSON_Change(const char* key, const int valueint);

static void print_object_items(cJSON *root);

void ConfigData_SendJSON(UART_HandleTypeDef *huart);
void Send_JSON_Over_UART(const char *json_str, UART_HandleTypeDef *huart);


#endif  // MY_JSON_UTILS_H