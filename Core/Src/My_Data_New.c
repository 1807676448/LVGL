#include "My_Data_New.h"

static int parse_device_id_number(const cJSON *id)
{
    if (id == NULL)
        return -1;

    if (cJSON_IsNumber(id))
        return id->valueint;

    if (cJSON_IsString(id) && id->valuestring != NULL)
    {
        const char *p = id->valuestring;
        while (*p != '\0' && !isdigit((unsigned char)*p))
        {
            p++;
        }
        if (*p != '\0')
        {
            return atoi(p);
        }
    }
    return -1;
}

static bool g_time_synced = false;

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
        HAL_UART_Abort_IT(&huart2);
        HAL_UART_Receive_IT(&huart2, &uart2_rx_buf[0], 1);
    }
    else if (usart == &huart3)
    {
        memset(uart3_rx_buf, 0, sizeof(uart3_rx_buf));
        uart3_ins = 0;
        HAL_UART_Abort_IT(&huart3);
        HAL_UART_Receive_IT(&huart3, &uart3_rx_buf[0], 1);
    }
}

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
            in_string = !in_string;
            continue;
        }
        if (in_string)
            continue;

        if (ch == '{')
            depth++;
        else if (ch == '}')
            depth--;

        if (depth == 0)
        {
            *json_start = start;
            *json_end = p;
            return true;
        }
    }
    return false;
}

/*------<变量声明区>------*/
// 设备状态列表
device_status_t device_list[Device_Max] = {
    {1, "offline", false},
    {2, "offline", false},
    {3, "offline", false},
    {4, "offline", false}};

// 数据配置项列表
config_item_t config_items[MAX_CONFIG_ITEMS] = {0};
uint8_t config_count = 0;
extern uint64_t UNX_Now_Time; // 当前时间戳

/*------<基础解析函数>------*/
// 键名查找
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
// 数据存储
void N_My_JsonChange_Int(char *key, int num)
{
    if (key == NULL)
        return;
    int i = N_My_JsonQuery(key);
    if (i != -1)
    {
        if (config_items[i].value != NULL)
        {
            cJSON_Delete(config_items[i].value);
        }
        config_items[i].value = cJSON_CreateNumber((double)num);
    }
    else
    {
        // 修复：先检查容量，防止数组越界
        if (config_count >= MAX_CONFIG_ITEMS)
        {
            printf("error:config_items FULL");
            return;
        }
        // 安全复制
        strncpy(config_items[config_count].key, key, sizeof(config_items[0].key) - 1);
        config_items[config_count].key[sizeof(config_items[0].key) - 1] = '\0';

        config_items[config_count].value = cJSON_CreateNumber((double)num);
        config_items[config_count].type = 1;

        config_count++; // 最后增加计数
        printf("---------%d\r\n", config_count);
    }
}

void N_My_JsonChange_Double(char *key, double num)
{
    if (key == NULL)
        return;
    int i = N_My_JsonQuery(key);
    if (i != -1)
    {
        if (config_items[i].value != NULL)
        {
            cJSON_Delete(config_items[i].value);
        }
        config_items[i].value = cJSON_CreateNumber(num);
    }
    else
    {
        // 修复：先检查容量
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

void test(void)
{
    for (int i = 0; i < config_count; i++)
    {
        // 修复：索引不需要减1
        printf("[%d]%s : %d(%.2f) \r\n",
               i, config_items[i].key, (int)llround(cJSON_GetNumberValue(config_items[i].value)), cJSON_GetNumberValue(config_items[i].value));
    }
}

// Json解析
void N_My_JsonGet(char *Json_Data, UART_HandleTypeDef *usart)
{
    if (Json_Data == NULL || usart == NULL)
        return;

    char *Json_start = NULL;
    char *Json_end = NULL;
    if (!find_complete_json(Json_Data, &Json_start, &Json_end))
    {
        return; // 半包继续等待
    }

    size_t json_len = (size_t)(Json_end - Json_start + 1);
    if (json_len == 0 || json_len >= 500)
    {
        reset_uart_rx(usart);
        return;
    }

    char json_buf[500] = {0};
    memcpy(json_buf, Json_start, json_len);
    json_buf[json_len] = '\0';

    printf("Json_start:%s\n\r", json_buf);
    // 查找成功，进行解析
    cJSON *root = cJSON_Parse(json_buf);
    if (root != NULL)
    {
        // 进行特别字符串检测 // 示例: {"device_id": 1, "status": "active"}
        cJSON *id = cJSON_GetObjectItem(root, "device_id");
        cJSON *status = cJSON_GetObjectItem(root, "status");
        cJSON *timestamp = cJSON_GetObjectItem(root, "timestamp");
        cJSON *NowTime = cJSON_GetObjectItem(root, "NowTime");
        int device_id = parse_device_id_number(id);
        if (device_id > 0 && status != NULL && cJSON_IsString(status))
        {
            // 更新设备状态
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

            if (ts > 0 && ts < 1000000000000ULL)
            {
                ts *= 1000ULL;
            }

            if (ts >= 1600000000000ULL && ts <= 2200000000000ULL)
            {
                UNX_Now_Time = ts;
                g_time_synced = true;
                printf("Received timestamp(ms): %llu\r\n", (unsigned long long)ts);
            }
        }
        // 数据解析
        for (cJSON *item = root->child; item != NULL; item = item->next)
        {
            const char *key = item->string ? item->string : "(null)";
            if (strcmp(key, "timestamp") == 0 || strcmp(key, "NowTime") == 0)
            {
                continue;
            }
            // 只记录数字类型，其余类型只打印，不记录
            if (item->type == cJSON_Number)
            {
                double num = cJSON_GetNumberValue(item);
                int64_t num_int = (int64_t)llround(num);
                printf("%s : %lld (%.3f)\r\n", key, (long long)num_int, num);
                if (num == (double)num_int)
                {
                    update_screen1_item(key, num_int);
                    N_My_JsonChange_Int((char *)key, num_int);
                }
                else
                {
                    update_screen1_item(key, num);
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
// 通过UART发送JSON数据
void Send_JSON(const char *json_str, UART_HandleTypeDef *huart)
{
    if (huart == NULL || json_str == NULL)
        return;

    // 计算字符串长度
    size_t len = strlen(json_str);

    // 使用HAL_UART_Transmit发送数据
    HAL_UART_Transmit(huart, (uint8_t *)json_str, len, HAL_MAX_DELAY);
    HAL_UART_Transmit(huart, (uint8_t *)"\r\n", 2, HAL_MAX_DELAY);

    // 同步镜像到USART1，便于调试观察4G收发指令
    if (huart != &huart1)
    {
        HAL_UART_Transmit(&huart1, (uint8_t *)json_str, len, HAL_MAX_DELAY);
        HAL_UART_Transmit(&huart1, (uint8_t *)"\r\n", 2, HAL_MAX_DELAY);
    }
}

// 发送指定键对值的JSON数据
void Send_JSON_KeyValue(const char **key, int num, UART_HandleTypeDef *huart)
{
    // 合规性检查
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

        int j = N_My_JsonQuery((char *)key[i]); // 需强制转换或修改函数签名
        if (j != -1 && config_items[j].value != NULL)
        {
            // [关键修复] 创建嵌套对象 {"key": {"value": 123}} 以匹配 Python 后端
            cJSON *wrapper = cJSON_CreateObject();
            if (wrapper)
            {
                cJSON_AddItemToObject(wrapper, "value", cJSON_Duplicate(config_items[j].value, 1));
                cJSON_AddItemToObject(root, config_items[j].key, wrapper);
            }
        }
        else
        {
            // [关键修复] 数据缺失时发送默认对象 {"key": {"value": 0}} 防止 Python 报错
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
        char prefix[96];
        snprintf(prefix, sizeof(prefix),
                 "MQPUB,0,%d,{\"id\":\"1\",\"device_id\":\"%s\",\"params\":",
                 MQTT_TOPIC_UP_INDEX, MQTT_DEVICE_ID);
        size_t new_len = strlen(prefix) + strlen(json_str) + 1 + 1;
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

void MQTT_Subscribe_Downlink(UART_HandleTypeDef *huart)
{
    char sub_cmd[32];
    snprintf(sub_cmd, sizeof(sub_cmd), "MQSUB,0,%d", MQTT_TOPIC_DOWN_INDEX);
    Send_JSON(sub_cmd, huart);
}

void MQTT_Request_Time(UART_HandleTypeDef *huart)
{
    char cmd[128];
    snprintf(cmd, sizeof(cmd),
             "MQPUB,0,%d,{\"device_id\":\"%s\",\"command\":\"time\"}",
             MQTT_TOPIC_COMMAND_INDEX, MQTT_DEVICE_ID);
    Send_JSON(cmd, huart);
}

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

void TimeChange(void)
{
    if (g_time_synced && UNX_Now_Time > 0)
    {
        time_t t = (time_t)(UNX_Now_Time / 1000 + 28800); // 转为秒并调整为北京时间
        struct tm *lt = localtime(&t);
        char time_str[10];
        strftime(time_str, sizeof(time_str), "%H:%M:%S", lt);
        char date_str[12];
        strftime(date_str, sizeof(date_str), "%Y-%m-%d", lt);
        update_main_screen_info(time_str, date_str, NULL, "Hello, User!");
    }
}