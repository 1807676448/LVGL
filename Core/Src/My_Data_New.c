/**
 * @file My_Data_New.c
 * @brief JSON数据处理模块，负责解析来自UART的JSON数据、管理设备状态、存储配置项，
 *        以及通过UART发送JSON格式的MQTT消息。
 *
 * 该模块主要功能：
 * - 从UART接收并解析JSON数据，提取设备状态、时间戳和数字类型数据。
 * - 维护设备状态列表和设备配置项（键值对）列表。
 * - 提供接口查询、更新和发送配置项数据。
 * - 封装MQTT指令（订阅、请求时间、上报状态）并通过UART发送。
 * - 同步时间戳并更新本地时间显示。
 *
 * 依赖：cJSON库、HAL库UART驱动、自定义全局变量（如UART缓冲区、MQTT配置等）。
 */

#include "My_Data_New.h"

/**
 * @brief 从cJSON对象中解析设备ID号。
 * @param id 指向cJSON对象的指针，可以是数字或字符串类型。
 * @return 成功返回设备ID（整数），失败返回-1。
 * @note 若为字符串，则跳过前导非数字字符，提取第一个数字序列。
 */
static int parse_device_id_number(const cJSON *id)
{
    if (id == NULL)
        return -1;

    if (cJSON_IsNumber(id))
        return id->valueint;

    if (cJSON_IsString(id) && id->valuestring != NULL)
    {
        const char *p = id->valuestring;
        // 跳过前导非数字字符
        while (*p != '\0' && !isdigit((unsigned char)*p))
        {
            p++;
        }
        if (*p != '\0')
        {
            return atoi(p); // 将数字部分转换为整数
        }
    }
    return -1;
}

/** 全局时间同步标志，表示是否已从服务器获取到有效时间戳 */
static bool g_time_synced = false;

/**
 * @brief 重置指定UART的接收状态，清空接收缓冲区并重新启动单字节接收。
 * @param usart 指向UART句柄的指针，支持huart1、huart2、huart3。
 * @note 用于在解析完成或出错后重新准备接收下一帧数据。
 */
static void reset_uart_rx(UART_HandleTypeDef *usart)
{
    if (usart == &huart1)
    {
        memset(uart1_rx_buf, 0, sizeof(uart1_rx_buf));
        uart1_ins = 0;
        HAL_UART_Abort_IT(&huart1);
        HAL_UART_Receive_IT(&huart1, &uart1_rx_buf[0], 1);
    }
    else if (usart == &huart2)
    {
        memset(uart2_rx_buf, 0, sizeof(uart2_rx_buf));
        uart2_ins = 0;
        // STM32H7 D-Cache 一致性：memset 写入了 Cache，Clean+Invalidate 后 DMA 才能安全写入
        SCB_CleanInvalidateDCache_by_Addr((uint32_t *)uart2_rx_buf, sizeof(uart2_rx_buf));
        HAL_UART_AbortReceive(&huart2);
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, uart2_rx_buf, sizeof(uart2_rx_buf));
    }
    else if (usart == &huart3)
    {
        memset(uart3_rx_buf, 0, sizeof(uart3_rx_buf));
        uart3_ins = 0;
        // STM32H7 D-Cache 一致性：Clean+Invalidate 后 DMA 才能安全写入
        SCB_CleanInvalidateDCache_by_Addr((uint32_t *)uart3_rx_buf, sizeof(uart3_rx_buf));
        HAL_UART_AbortReceive(&huart3);
        HAL_UARTEx_ReceiveToIdle_DMA(&huart3, uart3_rx_buf, sizeof(uart3_rx_buf));
    }
}

/**
 * @brief 在原始字符串中查找第一个完整的JSON对象（Json解析器实现）。
 * @param raw 原始字符串（可能包含多段数据）。
 * @param json_start 输出参数，指向JSON开始的指针。
 * @param json_end 输出参数，指向JSON结束的指针。
 * @return 找到完整JSON返回true，否则false。
 * @note 使用栈深度和字符串转义状态来精确匹配花括号。
 */
static bool find_complete_json(char *raw, char **json_start, char **json_end)
{
    if (raw == NULL || json_start == NULL || json_end == NULL)
        return false;

    char *start = strchr(raw, '{');
    if (start == NULL)
        return false;

    int depth = 0;
    bool in_string = false;
    bool escaped = false;
    for (char *p = start; *p != '\0'; p++)
    {
        char ch = *p;
        //该部分程序的目的是识别json的深度，找出与起始花括号相对应的末尾花括号，因此“”内的内容，以及被转义的字符都不应该影响深度计数,这部分被直接略过了。
        if (escaped)
        {
            escaped = false;
            continue;
        }
        if (ch == '\\')
        {
            escaped = true;
            continue;
        }
        if (ch == '"')
        {
            in_string = !in_string; // 切换字符串内/外状态
            continue;
        }
        if (in_string)
            continue;
        // 只有在非字符串，非转移字符状态下才处理花括号
        if (ch == '{')
            depth++;
        else if (ch == '}')
            depth--;

        if (depth == 0) // 找到匹配的结束括号
        {
            *json_start = start;
            *json_end = p;
            return true;
        }
    }
    return false;
}

/*------<变量声明区>------*/
// 设备状态列表，最多Device_Max个设备，每个设备包含ID、状态字符串和有效标志
device_status_t device_list[Device_Max] = {
    {1, "offline", false},
    {2, "offline", false},
    {3, "offline", false},
    {4, "offline", false}};

// 数据配置项列表，存储从JSON中解析出的键值对（仅数字类型）
config_item_t config_items[MAX_CONFIG_ITEMS] = {0};
uint8_t config_count = 0;                // 当前已存储的配置项数量
extern uint64_t UNX_Now_Time;             // 当前时间戳（毫秒级Unix时间戳）

/*------<基础解析函数>------*/
/**
 * @brief 根据键名在配置项列表中查找索引。
 * @param key 要查找的键名。
 * @return 找到返回索引，否则返回-1。
 */
int N_My_JsonQuery(char *key)
{
    if (key == NULL)
        return -1;
    for (int i = 0; i < config_count; i++)
    {
        if (strcmp(config_items[i].key, key) == 0)
            return i;
    }
    return -1;
}

/**
 * @brief 存储或更新一个整数类型的配置项。
 * @param key 键名。
 * @param num 整数值。
 * @note 若键已存在，则更新其cJSON值；若不存在且未达上限，则新增一项。
 */
void N_My_JsonChange_Int(char *key, int num)
{
    if (key == NULL)
        return;
    int i = N_My_JsonQuery(key);
    if (i != -1)
    {
        // 更新现有项
        if (config_items[i].value != NULL)
        {
            cJSON_Delete(config_items[i].value);
        }
        config_items[i].value = cJSON_CreateNumber((double)num);
    }
    else
    {
        // 新增项，检查容量
        if (config_count >= MAX_CONFIG_ITEMS)
        {
            printf("error:config_items FULL");
            return;
        }
        // 安全复制键名
        strncpy(config_items[config_count].key, key, sizeof(config_items[0].key) - 1);
        config_items[config_count].key[sizeof(config_items[0].key) - 1] = '\0';

        config_items[config_count].value = cJSON_CreateNumber((double)num);
        config_items[config_count].type = 1; // 1表示数字类型（但未使用）

        config_count++; // 增加计数
        printf("---------%d\r\n", config_count);
    }
}

/**
 * @brief 存储或更新一个双精度浮点类型的配置项。
 * @param key 键名。
 * @param num 双精度浮点值。
 * @note 若键已存在，则更新其cJSON值；若不存在且未达上限，则新增一项。
 */
void N_My_JsonChange_Double(char *key, double num)
{
    if (key == NULL)
        return;
    int i = N_My_JsonQuery(key);
    if (i != -1)
    {
        // 更新现有项
        if (config_items[i].value != NULL)
        {
            cJSON_Delete(config_items[i].value);
        }
        config_items[i].value = cJSON_CreateNumber(num);
    }
    else
    {
        // 新增项，检查容量
        if (config_count >= MAX_CONFIG_ITEMS)
        {
            printf("error:config_items FULL");
            return;
        }

        strncpy(config_items[config_count].key, key, sizeof(config_items[0].key) - 1);
        config_items[config_count].key[sizeof(config_items[0].key) - 1] = '\0';

        config_items[config_count].value = cJSON_CreateNumber(num);
        config_items[config_count].type = 1;

        config_count++;
        printf("---------%d\r\n", config_count);
    }
}

/**
 * @brief 测试函数：打印当前所有配置项的键和数值（四舍五入取整及原始值）。
 */
void test(void)
{
    for (int i = 0; i < config_count; i++)
    {
        printf("[%d]%s : %d(%.2f) \r\n",
               i, config_items[i].key, (int)llround(cJSON_GetNumberValue(config_items[i].value)), cJSON_GetNumberValue(config_items[i].value));
    }
}

/**
 * @brief 解析JSON数据的主函数，从传入字符串中提取完整的JSON对象并处理。
 * @param Json_Data 原始接收字符串（可能包含多个JSON或部分数据）。
 * @param usart 对应的UART句柄，用于在解析完成后重置接收。
 * @note 功能包括：
 *       - 查找完整JSON对象并提取。
 *       - 解析设备ID和状态，更新device_list。
 *       - 提取时间戳（timestamp或NowTime）并同步系统时间。
 *       - 提取所有数字类型的键值对，存储到config_items并更新屏幕显示。
 */
void N_My_JsonGet(char *Json_Data, UART_HandleTypeDef *usart)
{
    if (Json_Data == NULL || usart == NULL)
        return;

    char *Json_start = NULL;
    char *Json_end = NULL;
    if (!find_complete_json(Json_Data, &Json_start, &Json_end))
    {
        return; // 未找到完整JSON，可能是半包数据，等待下一次接收
    }

    size_t json_len = (size_t)(Json_end - Json_start + 1);
    if (json_len == 0 || json_len >= 500) // 限制最大长度，防止溢出
    {
        reset_uart_rx(usart);
        return;
    }

    char json_buf[500] = {0};
    memcpy(json_buf, Json_start, json_len);
    json_buf[json_len] = '\0';

    printf("Json_start:%s\n\r", json_buf);
    // 解析JSON
    cJSON *root = cJSON_Parse(json_buf);
    if (root != NULL)
    {
        // 提取设备ID和状态，更新设备列表
        cJSON *id = cJSON_GetObjectItem(root, "device_id");
        cJSON *status = cJSON_GetObjectItem(root, "status");
        cJSON *timestamp = cJSON_GetObjectItem(root, "timestamp");
        cJSON *NowTime = cJSON_GetObjectItem(root, "NowTime");
        int device_id = parse_device_id_number(id);
        if (device_id > 0 && status != NULL && cJSON_IsString(status))
        {
            // 更新对应设备的状态
            for (int i = 0; i < Device_Max; i++)
            {
                if (device_id == device_list[i].id)
                {
                    device_list[i].valid = true;
                    strncpy(device_list[i].status, status->valuestring, sizeof(device_list[i].status) - 1);
                }
            }
            // printf("Device ID: %d, Status: %s, Is_valid: %d\r\n", device_id, status->valuestring, device_list[device_id - 1].valid);
        }

        // 提取时间戳（优先使用timestamp，否则使用NowTime）
        cJSON *ts_item = NULL;
        if (timestamp != NULL)
        {
            ts_item = timestamp;
        }
        else if (NowTime != NULL)
        {
            ts_item = NowTime;
        }
        if (ts_item != NULL)
        {
            uint64_t ts = 0;
            if (cJSON_IsNumber(ts_item))
            {
                ts = (uint64_t)llround(cJSON_GetNumberValue(ts_item));
            }
            else if (cJSON_IsString(ts_item) && ts_item->valuestring != NULL)
            {
                ts = strtoull(ts_item->valuestring, NULL, 10);
            }

            // 若时间戳小于1e12，可能是秒级，转换为毫秒级
            if (ts > 0 && ts < 1000000000000ULL)
            {
                ts *= 1000ULL;
            }

            // 验证时间戳是否在合理范围（2020年~2039年左右）
            if (ts >= 1600000000000ULL && ts <= 2200000000000ULL)
            {
                UNX_Now_Time = ts;
                g_time_synced = true;
                printf("Received timestamp(ms): %llu\r\n", (unsigned long long)ts);
            }
        }

        // 遍历所有子节点，提取数字类型的键值对
        for (cJSON *item = root->child; item != NULL; item = item->next)
        {
            const char *key = item->string ? item->string : "(null)";
            // 跳过已经处理过的时间戳字段
            if (strcmp(key, "timestamp") == 0 || strcmp(key, "NowTime") == 0)
            {
                continue;
            }
            // 仅处理数字类型
            if (item->type == cJSON_Number)
            {
                double num = cJSON_GetNumberValue(item);
                int64_t num_int = (int64_t)llround(num);
                printf("%s : %lld (%.3f)\r\n", key, (long long)num_int, num);
                // 判断数值是否为整数（与四舍五入后的整数相等）
                if (num == (double)num_int)
                {
                    update_screen1_item(key, num_int); // 更新屏幕显示（整数版）
                    N_My_JsonChange_Int((char *)key, num_int);
                }
                else
                {
                    update_screen1_item(key, num);     // 更新屏幕显示（浮点版）
                    N_My_JsonChange_Double((char *)key, num);
                }
            }
        }
    }
    else
    {
        printf("Error In Json Parse(N_My_JsonGet_1)\n\r");
        reset_uart_rx(usart);
        return;
    }

    cJSON_Delete(root); // 释放根节点
    reset_uart_rx(usart);
}

/*------<JSON发送函数>------*/
/**
 * @brief 通过UART发送JSON字符串，并在末尾添加换行符。
 * @param json_str 要发送的JSON字符串。
 * @param huart 目标UART句柄。
 * @note 如果目标UART不是huart1，还会将相同内容镜像发送到huart1以便调试。
 */
void Send_JSON(const char *json_str, UART_HandleTypeDef *huart)
{
    if (huart == NULL || json_str == NULL)
        return;

    size_t len = strlen(json_str);

    HAL_UART_Transmit(huart, (uint8_t *)json_str, len, HAL_MAX_DELAY);
    HAL_UART_Transmit(huart, (uint8_t *)"\r\n", 2, HAL_MAX_DELAY);

    // 镜像到USART1便于调试观察4G收发指令
    if (huart != &huart1)
    {
        HAL_UART_Transmit(&huart1, (uint8_t *)json_str, len, HAL_MAX_DELAY);
        HAL_UART_Transmit(&huart1, (uint8_t *)"\r\n", 2, HAL_MAX_DELAY);
    }
}

/**
 * @brief 根据指定的键列表，构造并发送JSON格式的MQTT发布消息。
 * @param key 键名指针数组。
 * @param num 键的数量。
 * @param huart 目标UART句柄。
 * @note 构造的消息格式为：MQPUB,0,<topic_index>,{"id":"1","device_id":"<DEVICE_ID>","params":{...}}
 *       每个键对应一个包装对象：{"key": {"value": 数值}}，若数据缺失则发送默认值0。
 */
void Send_JSON_KeyValue(const char **key, int num, UART_HandleTypeDef *huart)
{
    if (key == NULL || huart == NULL || num <= 0)
        return;

    cJSON *root = cJSON_CreateObject();
    if (!root)
    {
        printf("Create JSON object failed\r\n");
        return;
    }

    // 遍历指定的键列表
    for (int i = 0; i < num; i++)
    {
        if (key[i] == NULL)
            continue;

        int j = N_My_JsonQuery((char *)key[i]); // 查找配置项
        if (j != -1 && config_items[j].value != NULL)
        {
            // 数据存在：创建包装对象 {"value": 实际值}
            cJSON *wrapper = cJSON_CreateObject();
            if (wrapper)
            {
                cJSON_AddItemToObject(wrapper, "value", cJSON_Duplicate(config_items[j].value, 1));
                cJSON_AddItemToObject(root, config_items[j].key, wrapper);
            }
        }
        else
        {
            // 数据缺失：创建包装对象 {"value": 0} 避免后端解析错误
            cJSON *wrapper = cJSON_CreateObject();
            if (wrapper)
            {
                cJSON_AddNumberToObject(wrapper, "value", 0);
                cJSON_AddItemToObject(root, key[i], wrapper);
            }
        }
    }

    char *json_str = cJSON_PrintUnformatted(root);
    if (json_str)
    {
        // 构造完整的MQTT发布命令
        char prefix[96];//只有传递的内容为非函数参数的数组时，sizeof才会返回整个数组的大小，否则会退化为指针大小，因此这里直接定义一个足够大的数组来存储前缀字符串。
        snprintf(prefix, sizeof(prefix),
                 "MQPUB,0,%d,{\"id\":\"device_002\",\"device_id\":\"%s\",\"params\":",
                 MQTT_TOPIC_UP_INDEX, MQTT_DEVICE_ID);
        size_t new_len = strlen(prefix) + strlen(json_str) + 1 + 1; // +1 for '}', +1 for '\0'
        char *full_str = (char *)malloc(new_len);
        if (full_str)
        {
            strcpy(full_str, prefix);
            strcat(full_str, json_str);
            strcat(full_str, "}");
            Send_JSON(full_str, huart);
            free(full_str);
        }
        else
        {
            printf("Memory allocation failed\r\n");
        }
        free(json_str);
    }
    cJSON_Delete(root);
}

/**
 * @brief 发送MQTT订阅下行主题的命令。
 * @param huart 目标UART句柄。
 * @note 命令格式：MQSUB,0,<MQTT_TOPIC_DOWN_INDEX>
 */
void MQTT_Subscribe_Downlink(UART_HandleTypeDef *huart)
{
    char sub_cmd[32];
    snprintf(sub_cmd, sizeof(sub_cmd), "MQSUB,0,%d", MQTT_TOPIC_DOWN_INDEX);
    Send_JSON(sub_cmd, huart);
}

/**
 * @brief 发送请求服务器时间的MQTT命令。
 * @param huart 目标UART句柄。
 * @note 命令格式：MQPUB,0,<MQTT_TOPIC_COMMAND_INDEX>,{"device_id":"<DEVICE_ID>","command":"time"}
 */
void MQTT_Request_Time(UART_HandleTypeDef *huart)
{
    char cmd[128];
    snprintf(cmd, sizeof(cmd),
             "MQPUB,0,%d,{\"device_id\":\"%s\",\"command\":\"time\"}",
             MQTT_TOPIC_COMMAND_INDEX, MQTT_DEVICE_ID);
    Send_JSON(cmd, huart);
}

/**
 * @brief 上报设备状态和运行时长。
 * @param huart 目标UART句柄。
 * @param status 状态字符串（如"online"），若为NULL则默认使用"online"。
 * @param runtime_seconds 运行时长（秒）。
 * @note 命令格式：MQPUB,0,<MQTT_TOPIC_STATUS_INDEX>,{"device_id":"<DEVICE_ID>","status":"...","runtime_seconds":...}
 */
void MQTT_Report_Status(UART_HandleTypeDef *huart, const char *status, uint32_t runtime_seconds)
{
    if (huart == NULL)
        return;

    if (status == NULL || status[0] == '\0')
    {
        status = "online";
    }

    char cmd[160];
    snprintf(cmd, sizeof(cmd),
             "MQPUB,0,%d,{\"device_id\":\"%s\",\"status\":\"%s\",\"runtime_seconds\":%lu}",
             MQTT_TOPIC_STATUS_INDEX, MQTT_DEVICE_ID, status, (unsigned long)runtime_seconds);
    Send_JSON(cmd, huart);
}

/**
 * @brief 如果时间已同步，则将UNX_Now_Time转换为北京时间并更新主屏幕的时间日期显示。
 * @note 调用前需确保UNX_Now_Time为有效毫秒级Unix时间戳。
 */
void TimeChange(void)
{
    if (g_time_synced && UNX_Now_Time > 0)
    {
        time_t t = (time_t)(UNX_Now_Time / 1000 + 28800); // 毫秒转秒，并加8小时（28800秒）调整为北京时间
        struct tm *lt = localtime(&t);
        char time_str[10];
        strftime(time_str, sizeof(time_str), "%H:%M:%S", lt);
        char date_str[12];
        strftime(date_str, sizeof(date_str), "%Y-%m-%d", lt);
        update_main_screen_info(time_str, date_str, NULL, "Hello, User!");
    }
}