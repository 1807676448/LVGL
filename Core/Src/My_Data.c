// #include "My_Data.h"
// #include <math.h>

// // 基础清除函数
// void clear_array(uint8_t *arr, size_t len)
// {
//     for (size_t i = 0; i < len; i++)
//     {
//         arr[i] = 0;
//     }
// }

// static config_item_t config_items[MAX_CONFIG_ITEMS];
// static uint8_t config_count = 0;

// // 设备状态列表（已在 My_Data.h 中声明为 extern，这里定义全局实例）
// device_status_t device_status_list[MAX_DEVICE_STATUS];

// static bool is_special_key(const char *key)
// {
//     return (key && (strcmp(key, "device_id") == 0 || strcmp(key, "status") == 0));
// }

// static int find_device_status_index(int id)
// {
//     for (int i = 0; i < MAX_DEVICE_STATUS; i++)
//     {
//         if (device_status_list[i].valid && device_status_list[i].id == id)
//         {
//             return i;
//         }
//     }
//     return -1;
// }

// static void update_device_status(int id, const char *status)
// {
//     int idx = find_device_status_index(id);
//     if (idx == -1)
//     {
//         for (int i = 0; i < MAX_DEVICE_STATUS; i++)
//         {
//             if (!device_status_list[i].valid)
//             {
//                 idx = i;
//                 device_status_list[i].valid = true;
//                 device_status_list[i].id = id;
//                 break;
//             }
//         }
//     }

//     if (idx != -1)
//     {
//         strncpy(device_status_list[idx].status, status, sizeof(device_status_list[idx].status) - 1);
//         device_status_list[idx].status[sizeof(device_status_list[idx].status) - 1] = '\0';
//         printf("device %d status -> %s\r\n", id, device_status_list[idx].status);
//     }
//     else
//     {
//         printf("device status table full, id=%d dropped\r\n", id);
//     }
// }

// static void handle_device_status(cJSON *root)
// {
//     if (!root)
//     {
//         return;
//     }

//     cJSON *id = cJSON_GetObjectItem(root, "device_id");
//     cJSON *status = cJSON_GetObjectItem(root, "status");
//     if (id && status && cJSON_IsNumber(id) && cJSON_IsString(status))
//     {
//         update_device_status(id->valueint, status->valuestring);
//     }
// }

// // 查找配置项索引
// static int find_config_index(const char *key)
// {
//     for (int i = 0; i < config_count; i++)
//     {
//         if (strcmp(config_items[i].key, key) == 0)
//         {
//             return i;
//         }
//     }
//     return -1;
// }

// /*--数据存储处理区--*/
// // 存储整数配置项
// void My_SendJson_Change_Int(const char *key, int value)
// {
//     int index = find_config_index(key);

//     if (index == -1)
//     {
//         // 新配置项
//         if (config_count >= MAX_CONFIG_ITEMS)
//         {
//             printf("error-My_SendJson_Change_Int!\r\n");
//             return;
//         }
//         index = config_count++;
//         strncpy(config_items[index].key, key, sizeof(config_items[index].key) - 1);
//         config_items[index].key[sizeof(config_items[index].key) - 1] = '\0';
//     }

//     // 释放旧值
//     if (config_items[index].value)
//     {
//         cJSON_Delete(config_items[index].value);
//     }

//     // 创建新值
//     config_items[index].value = cJSON_CreateNumber(value);
//     config_items[index].type = 1;
// }
// // 存储浮点数配置项
// void My_SendJson_Change_Double(const char *key, double value)
// {
//     int index = find_config_index(key);

//     if (index == -1)
//     {
//         if (config_count >= MAX_CONFIG_ITEMS)
//         {
//             printf("error-My_SendJson_Change_Double!\r\n");
//             return;
//         }
//         index = config_count++;
//         strncpy(config_items[index].key, key, sizeof(config_items[index].key) - 1);
//         config_items[index].key[sizeof(config_items[index].key) - 1] = '\0';
//     }

//     if (config_items[index].value)
//     {
//         cJSON_Delete(config_items[index].value);
//     }

//     config_items[index].value = cJSON_CreateNumber(value);
//     config_items[index].type = 2;
// }
// // 存储字符串配置项
// void My_SendJson_Change_String(const char *key, const char *value)
// {
//     int index = find_config_index(key);

//     if (index == -1)
//     {
//         if (config_count >= MAX_CONFIG_ITEMS)
//         {
//             printf("error-My_SendJson_Change_String!\r\n");
//             return;
//         }
//         index = config_count++;
//         strncpy(config_items[index].key, key, sizeof(config_items[index].key) - 1);
//         config_items[index].key[sizeof(config_items[index].key) - 1] = '\0';
//     }

//     if (config_items[index].value)
//     {
//         cJSON_Delete(config_items[index].value);
//     }

//     config_items[index].value = cJSON_CreateString(value);
//     config_items[index].type = 3;
// }
// // 存储布尔值配置项
// void My_SendJson_Change_Bool(const char *key, bool value)
// {
//     int index = find_config_index(key);

//     if (index == -1)
//     {
//         if (config_count >= MAX_CONFIG_ITEMS)
//         {
//             printf("error-My_SendJson_Change_Bool!\r\n");
//             return;
//         }
//         index = config_count++;
//         strncpy(config_items[index].key, key, sizeof(config_items[index].key) - 1);
//         config_items[index].key[sizeof(config_items[index].key) - 1] = '\0';
//     }

//     if (config_items[index].value)
//     {
//         cJSON_Delete(config_items[index].value);
//     }

//     config_items[index].value = cJSON_CreateBool(value);
//     config_items[index].type = 4;
// }

// // 通用存储函数（自行判断类型）
// void My_SendJson_Change(const char *key, void *value, uint8_t value_type)
// {
//     switch (value_type)
//     {
//     case 1: // 整数
//         My_SendJson_Change_Int(key, *(int *)value);
//         break;
//     case 2: // 浮点数
//         My_SendJson_Change_Double(key, *(double *)value);
//         break;
//     case 3: // 字符串
//         My_SendJson_Change_String(key, (char *)value);
//         break;
//     case 4: // 布尔值
//         My_SendJson_Change_Bool(key, *(bool *)value);
//         break;
//     default:
//         printf("Unsupported value type: %d\r\n", value_type);
//         break;
//     }
// }

// // 删除配置项
// void My_SendJson_Remove(const char *key)
// {
//     int index = find_config_index(key);
//     if (index == -1)
//         return;

//     // 释放内存
//     if (config_items[index].value)
//     {
//         cJSON_Delete(config_items[index].value);
//     }

//     // 移动后续项目
//     for (int i = index; i < config_count - 1; i++)
//     {
//         config_items[i] = config_items[i + 1];
//     }

//     config_count--;
//     config_items[config_count].type = 0;
//     config_items[config_count].key[0] = '\0';
//     config_items[config_count].value = NULL;
// }

// // 获取配置项数量
// uint8_t My_SendJson_GetCount(void)
// {
//     return config_count;
// }

// // 生成并发送JSON数据
// void My_SendJson_Send(void)
// {
//     if (config_count == 0)
//     {
//         printf("No configuration items to send\r\n");
//         return;
//     }

//     // 创建根JSON对象
//     cJSON *root = cJSON_CreateObject();
//     if (!root)
//     {
//         printf("Create JSON object failed\r\n");
//         return;
//     }

//     // 添加所有配置项到JSON对象
//     for (int i = 0; i < config_count; i++)
//     {
//         if (config_items[i].value)
//         {
//             cJSON_AddItemToObject(root, config_items[i].key,
//                                   cJSON_Duplicate(config_items[i].value, 1));
//         }
//     }

//     // 序列化JSON
//     char *json_str = cJSON_PrintUnformatted(root);
//     if (json_str)
//     {
//         printf("Sending JSON: %s\r\n", json_str);

//         // 这里添加实际的发送代码，例如通过UART、网络等
//         // HAL_UART_Transmit(&huart1, (uint8_t*)json_str, strlen(json_str), HAL_MAX_DELAY);

//         free(json_str);
//     }

//     cJSON_Delete(root);
// }

// void My_SendJson_SendAll(UART_HandleTypeDef *huart)
// {
//     if (config_count == 0)
//     {
//         printf("No configuration items to send\r\n");
//         return;
//     }

//     cJSON *root = cJSON_CreateObject();
//     if (!root)
//     {
//         printf("Create JSON object failed\r\n");
//         return;
//     }

//     for (int i = 0; i < config_count; i++)
//     {
//         if (config_items[i].value)
//         {
//             cJSON_AddItemToObject(root, config_items[i].key,
//                                   cJSON_Duplicate(config_items[i].value, 1));
//         }
//     }

//     char *json_str = cJSON_PrintUnformatted(root);
//     if (json_str)
//     {
//         Send_JSON_Over_UART(json_str, huart);
//         free(json_str);
//     }

//     cJSON_Delete(root);
// }

// void My_SendJson_SendKeys(const char *keys[], size_t key_count, UART_HandleTypeDef *huart)
// {
//     if (!keys || key_count == 0)
//     {
//         printf("No keys provided\r\n");
//         return;
//     }

//     cJSON *root = cJSON_CreateObject();
//     if (!root)
//     {
//         printf("Create JSON object failed\r\n");
//         return;
//     }

//     size_t added = 0;
//     for (size_t i = 0; i < key_count; i++)
//     {
//         int idx = find_config_index(keys[i]);
//         if (idx >= 0 && config_items[idx].value)
//         {
//             cJSON_AddItemToObject(root, config_items[idx].key,
//                                   cJSON_Duplicate(config_items[idx].value, 1));
//             added++;
//         }
//     }

//     if (added == 0)
//     {
//         printf("No matching keys to send\r\n");
//         cJSON_Delete(root);
//         return;
//     }

//     char *json_str = cJSON_PrintUnformatted(root);
//     if (json_str)
//     {
//         Send_JSON_Over_UART(json_str, huart);
//         free(json_str);
//     }

//     cJSON_Delete(root);
// }

// bool My_SendJson_QueryValue(const char *key, char *out, size_t out_len)
// {
//     if (!key || !out || out_len == 0)
//     {
//         return false;
//     }

//     int idx = find_config_index(key);
//     if (idx == -1 || !config_items[idx].value)
//     {
//         return false;
//     }

//     char *json_str = cJSON_PrintUnformatted(config_items[idx].value);
//     if (!json_str)
//     {
//         return false;
//     }

//     strncpy(out, json_str, out_len - 1);
//     out[out_len - 1] = '\0';
//     free(json_str);
//     return true;
// }

// // 打印所有存储的配置项
// extern uint64_t UNX_Now_Time;
// void My_SendJson_PrintAll(void)
// {
//     printf("Stored configuration items (%d):\r\n", config_count);
//     for (int i = 0; i < config_count; i++)
//     {
//         printf("  [%d] %s: ", i, config_items[i].key);

//         cJSON *value = config_items[i].value;
//         if (!value)
//         {
//             printf("(null)");
//         }
//         else
//         {
//             switch (config_items[i].type)
//             {
//             case 1: // 整数
//                 printf("%lld", (long long)llround(value->valuedouble));
//                 break;
//             case 2: // 浮点
//                 if (strcmp(config_items[i].key, "NowTime") == 0 || strcmp(config_items[i].key, "timestamp") == 0)
//                 {
//                     UNX_Now_Time = (uint64_t)llround(value->valuedouble);
//                     printf("%lld", (long long)llround(value->valuedouble));
//                 }
//                 else
//                 {
//                     printf("%.6f", value->valuedouble);
//                 }
//                 break;
//             case 3: // 字符串
//                 printf("%s", value->valuestring ? value->valuestring : "(null)");
//                 break;
//             case 4: // 布尔
//                 printf("%s", cJSON_IsTrue(value) ? "true" : "false");
//                 break;
//             default:
//             {
//                 char *str = cJSON_PrintUnformatted(value);
//                 if (str)
//                 {
//                     printf("%s", str);
//                     free(str);
//                 }
//                 else
//                 {
//                     printf("(unknown)");
//                 }
//                 break;
//             }
//             }
//         }
//         printf(" (type:%d)\r\n", config_items[i].type);
//     }
// }

// // 清空所有配置项
// void My_SendJson_ClearAll(void)
// {
//     for (int i = 0; i < config_count; i++)
//     {
//         if (config_items[i].value)
//         {
//             cJSON_Delete(config_items[i].value);
//         }
//     }
//     config_count = 0;
//     printf("All configuration items cleared\r\n");
// }

// // 历遍全部节点并输出
// static void print_object_items(cJSON *root)
// {
//     for (cJSON *item = root->child; item; item = item->next)
//     {
//         const char *key = item->string ? item->string : "(null)";

//         if (is_special_key(key))
//         {
//             continue;
//         }

//         switch (item->type)
//         {
//         case cJSON_String:
//             printf("%s : %s\r\n", key, item->valuestring);
//             break;

//         case cJSON_Number:
//         {
//             // 同时输出整数与浮点格式
//             double raw = cJSON_GetNumberValue(item);
//             int64_t as_int = (int64_t)llround(raw);
//             printf("%s : %lld (%.3f)\r\n", key, (long long)as_int, raw);
//             if ((float)(item->valueint) == (item->valuedouble))
//             {
//                 My_SendJson_Change(key, &(item->valueint), 1);
//                 update_screen1_item(key, (double)item->valueint);
//                 // My_SendJson_PrintAll();
//             }
//             else
//             {
//                 My_SendJson_Change(key, &(item->valuedouble), 2);
//                 update_screen1_item(key, item->valuedouble);
//                 // My_SendJson_PrintAll();
//             }
//             break;
//         }

//         case cJSON_True:
//             printf("%s : true\r\n", key);
//             break;

//         case cJSON_False:
//             printf("%s : false\r\n", key);
//             break;

//         case cJSON_NULL:
//             printf("%s : null\r\n", key);
//             break;

//         case cJSON_Object:
//             printf("%s : (object)\r\n", key);
//             print_object_items(item); // 递归
//             break;

//         case cJSON_Array:
//             printf("%s : (array)\r\n", key);
//             int idx = 0;
//             for (cJSON *sub = item->child; sub; sub = sub->next, idx++)
//             {
//                 switch (sub->type)
//                 {
//                 case cJSON_String:
//                     printf("  [%d] %s\r\n", idx, sub->valuestring);
//                     break;

//                 case cJSON_Number:
//                     printf("  [%d] %d (%.3f)\r\n", idx, sub->valueint, sub->valuedouble);
//                     break;

//                 default:
//                     printf("  [%d] type=0x%x\r\n", idx, sub->type);
//                     break;
//                 }
//             }
//             break;

//         default:
//             printf("%s : (unsupported type=0x%x)\r\n", key, item->type);
//             break;
//         }
//     }
// }

// void My_cJSON_Text(void)
// {
//     printf("Start text test!\n\r");
//     const char *json_string = "{\"TDS\": 0,\"COD\": 0,\"TOC\": 0,\"UV254\": 0,\"pH\": 0,\"Tem\": 0,\"Hum\": 0,\"NowTime\": 0}";
//     cJSON *root = cJSON_Parse(json_string);
//     if (!root)
//     {
//         printf("Parse fail\r\n");
//         return;
//     }

//     // 方式1：直接打印为 JSON 字符串（会动态分配内存）
//     char *out = cJSON_PrintUnformatted(root);
//     if (out)
//     {
//         printf("RAW:%s\r\n", out);
//         free(out); // 使用 cJSON 自带的分配器时也用 free/cJSON_free
//     }

//     // 方式2：手动遍历输出键值
//     if (root->type & cJSON_Object)
//     {
//         print_object_items(root);
//     }

//     cJSON_Delete(root); // 释放根节点
// }

// void My_cJSON_Get(char *json_data, UART_HandleTypeDef *huart)
// {
//     // 如果数据以 +MQRECV 开头，找到 JSON 部分的起始位置
//     char *json_start = strchr(json_data, '{');
//     if (!json_start)
//     {
//         cJSON *root_raw = cJSON_Parse(json_data);
//         if (!root_raw)
//         {
//             if (uart1_ins > 150 || uart2_ins > 150)
//             {
//                 uart1_ins = 0;
//                 uart2_ins = 0;
//             }
//             return;
//         }
//         else
//         {
//             json_start = json_data; // 刚好是完整 JSON
//             cJSON_Delete(root_raw);
//         }
//     }

//     cJSON *root = cJSON_Parse(json_start);
//     if (!root)
//     {
//         if (uart1_ins > 150 || uart2_ins > 150)
//         {
//             uart1_ins = 0;
//             uart2_ins = 0;
//         }
//         return;
//     }

//     handle_device_status(root);

//     char *out = cJSON_PrintUnformatted(root);

//     if (out && huart == &huart1)
//     {
//         printf("Usart1 Begin!\n\r");

//         print_object_items(root);
//         // printf("RAW:%s\r\n", out);
//         My_SendJson_PrintAll();

//         free(out); // 使用 cJSON 自带的分配器时也用 free/cJSON_free

//         clear_array(json_data, 100);
//         uart1_ins = 0;
//         HAL_UART_Abort_IT(&huart1);
//         HAL_UART_Receive_IT(&huart1, &uart1_rx_buf[0], 1);
//     }
//     if (out && huart == &huart2)
//     {
//         printf("Usart2 Begin!\n\r");

//         print_object_items(root);
//         // printf("RAW:%s\r\n", out);
//         My_SendJson_PrintAll();

//         free(out); // 使用 cJSON 自带的分配器时也用 free/cJSON_free

//         clear_array(json_data, 100);
//         uart2_ins = 0;
//         HAL_UART_Abort_IT(&huart2);
//         HAL_UART_Receive_IT(&huart2, &uart2_rx_buf[0], 1);
//     }
//     if (out && huart == &huart3)
//     {
//         printf("Usart3 Begin!\n\r");

//         print_object_items(root);
//         // printf("RAW:%s\r\n", out);
//         My_SendJson_PrintAll();

//         free(out); // 使用 cJSON 自带的分配器时也用 free/cJSON_free

//         clear_array(json_data, 100);
//         uart3_ins = 0;
//         HAL_UART_Abort_IT(&huart3);
//         HAL_UART_Receive_IT(&huart3, &uart3_rx_buf[0], 1);
//     }

//     cJSON_Delete(root); // 释放根节点
// }

// // 从配置项数组中发送JSON数据
// void ConfigData_SendJSON(UART_HandleTypeDef *huart)
// {
//     if (config_count == 0)
//     {
//         printf("No configuration items to send\r\n");
//         return;
//     }

//     // 创建根对象
//     cJSON *root = cJSON_CreateObject();
//     if (!root)
//     {
//         printf("ConfigData_SendJSON-error");
//         return;
//     }

//     // 添加id字段
//     cJSON_AddStringToObject(root, "id", "1");

//     // 创建params对象
//     cJSON *params = cJSON_CreateObject();
//     if (!params)
//     {
//         cJSON_Delete(root);
//         return;
//     }
//     cJSON_AddItemToObject(root, "params", params);

//     // 遍历配置项数组，添加到params中
//     for (int i = 0; i < config_count; i++)
//     {
//         if (config_items[i].type != 0 && config_items[i].value != NULL)
//         {
//             // 为每个配置项创建一个对象，包含value字段
//             cJSON *item_obj = cJSON_CreateObject();
//             if (item_obj)
//             {
//                 // 根据类型将值添加到value字段
//                 switch (config_items[i].type)
//                 {
//                 case 1: // 整数
//                 case 2: // 浮点数
//                     cJSON_AddItemToObject(item_obj, "value",
//                                           cJSON_Duplicate(config_items[i].value, 1));
//                     break;

//                 case 3: // 字符串
//                     cJSON_AddStringToObject(item_obj, "value",
//                                             config_items[i].value->valuestring);
//                     break;

//                 case 4: // 布尔值
//                     cJSON_AddBoolToObject(item_obj, "value",
//                                           cJSON_IsTrue(config_items[i].value));
//                     break;

//                 default:
//                     // 不支持的类型，跳过
//                     cJSON_Delete(item_obj);
//                     continue;
//                 }

//                 // 将对象添加到params中，使用配置项的key作为字段名
//                 cJSON_AddItemToObject(params, config_items[i].key, item_obj);
//             }
//         }
//     }

//     // 序列化为JSON字符串
//     char *json_str = cJSON_PrintUnformatted(root);
//     if (json_str)
//     {
//         // 拼接前缀 "MQPUB,0,0,"
//         const char *prefix = "MQPUB,0,0, ";
//         size_t new_len = strlen(prefix) + strlen(json_str) + 1;
//         char *full_str = (char *)malloc(new_len);
//         if (full_str)
//         {
//             strcpy(full_str, prefix);
//             strcat(full_str, json_str);
//             // 发送拼接后的数据
//             Send_JSON_Over_UART(full_str, huart);
//             free(full_str);
//         }
//         else
//         {
//             printf("Memory allocation failed for full_str\r\n");
//             // 内存不足时，至少尝试发送原始JSON（或报错）
//             // Send_JSON_Over_UART(json_str, huart);
//         }

//         free(json_str);
//     }

//     // 清理内存
//     cJSON_Delete(root);
// }

// // 通过UART发送JSON数据
// void Send_JSON_Over_UART(const char *json_str, UART_HandleTypeDef *huart)
// {
//     if (huart == NULL || json_str == NULL)
//     {
//         printf("Send_JSON_Over_UART-error\r\n");
//         return;
//     }

//     // 计算字符串长度
//     size_t len = strlen(json_str);

//     // 使用HAL_UART_Transmit发送数据
//     HAL_UART_Transmit(huart, (uint8_t *)json_str, len, HAL_MAX_DELAY);

//     // 可选：发送换行符以便于接收端识别结束
//     HAL_UART_Transmit(huart, (uint8_t *)"\r\n", 2, HAL_MAX_DELAY);
// }

// void My_cJSON_Change(const char *key, const int valueint)
// {
// }

// /* ======================== 设备状态管理导出函数实现 ======================== */

// /**
//  * @brief 获取指定ID的设备状态
//  */
// device_status_t* Get_Device_Status(int id)
// {
//     for (int i = 0; i < MAX_DEVICE_STATUS; i++)
//     {
//         if (device_status_list[i].valid && device_status_list[i].id == id)
//         {
//             return &device_status_list[i];
//         }
//     }
//     return NULL;
// }

// /**
//  * @brief 获取所有有效的设备状态数量
//  */
// int Get_Valid_Device_Count(void)
// {
//     int count = 0;
//     for (int i = 0; i < MAX_DEVICE_STATUS; i++)
//     {
//         if (device_status_list[i].valid)
//         {
//             count++;
//         }
//     }
//     return count;
// }

// /**
//  * @brief 查询设备是否在线（基于设备ID）
//  */
// bool Is_Device_Online(int id)
// {
//     device_status_t *dev = Get_Device_Status(id);
//     if (!dev) return false;
    
//     // 判断在线状态：如果 status 不是 "offline" 或 "0"，则认为在线
//     if (strcmp(dev->status, "offline") == 0 || strcmp(dev->status, "0") == 0)
//     {
//         return false;
//     }
//     return true;
// }

// /**
//  * @brief 清空所有设备状态（重置设备列表）
//  */
// void Clear_All_Device_Status(void)
// {
//     for (int i = 0; i < MAX_DEVICE_STATUS; i++)
//     {
//         device_status_list[i].valid = false;
//         device_status_list[i].id = 0;
//         device_status_list[i].status[0] = '\0';
//     }
// }