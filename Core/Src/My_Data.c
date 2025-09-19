#include "My_Data.h"

// 基础解析函数
void clear_array(uint8_t *arr, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        arr[i] = 0;
    }
}
// 历遍全部节点并输出
static void print_object_items(cJSON *root)
{
    
    
    for (cJSON *item = root->child; item; item = item->next)
    {
        const char *key = item->string ? item->string : "(null)";
        if (item->type & cJSON_String)
        {
            printf("%s : %s\r\n", key, item->valuestring);
        }
        else if (item->type & cJSON_Number)
        {
            // 同时输出整数与浮点格式
            printf("%s : %d (%.3f)\r\n", key, item->valueint, item->valuedouble);
            update_screen1_item(key,item->valueint);
        }
        else if (item->type & cJSON_True)
        {
            printf("%s : true\r\n", key);
        }
        else if (item->type & cJSON_False)
        {
            printf("%s : false\r\n", key);
        }
        else if (item->type & cJSON_NULL)
        {
            printf("%s : null\r\n", key);
        }
        else if (item->type & cJSON_Object)
        {
            printf("%s : (object)\r\n", key);
            print_object_items(item); // 递归
        }
        else if (item->type & cJSON_Array)
        {
            printf("%s : (array)\r\n", key);
            int idx = 0;
            for (cJSON *sub = item->child; sub; sub = sub->next, idx++)
            {
                if (sub->type & cJSON_String)
                    printf("  [%d] %s\r\n", idx, sub->valuestring);
                else if (sub->type & cJSON_Number)
                    printf("  [%d] %d (%.3f)\r\n", idx, sub->valueint, sub->valuedouble);
                else
                    printf("  [%d] type=0x%x\r\n", idx, sub->type);
            }
        }
        else
        {
            printf("%s : (unsupported type=0x%x)\r\n", key, item->type);
        }
    }
}

void My_cJSON_Text(void)
{
    printf("Start text test!\n\r");
    const char *json_string = "{\"First\":\"Hello\", \"Second\":25, \"Third\":\"Beijing\"}";
    cJSON *root = cJSON_Parse(json_string);
    if (!root)
    {
        printf("Parse fail\r\n");
        return;
    }

    // 方式1：直接打印为 JSON 字符串（会动态分配内存）
    char *out = cJSON_PrintUnformatted(root);
    if (out)
    {
        printf("RAW:%s\r\n", out);
        free(out); // 使用 cJSON 自带的分配器时也用 free/cJSON_free
    }

    // 方式2：手动遍历输出键值
    if (root->type & cJSON_Object)
    {
        print_object_items(root);
    }

    cJSON_Delete(root); // 释放根节点
}
extern int16_t uart1_ins;
extern uint8_t uart1_rx_buf[500];
void My_cJSON_Get(char *json_data)
{
    cJSON *root = cJSON_Parse(json_data);
    if (!root)
    {
        // printf("Parse fail\r\n");
        return;
    }

    char *out = cJSON_PrintUnformatted(root);

    if (out)
    {
        printf("Runtime text test!\n\r");

        print_object_items(root);
        printf("RAW:%s\r\n", out);

        free(out); // 使用 cJSON 自带的分配器时也用 free/cJSON_free

        clear_array(json_data, 100);
        uart1_ins = 0;
        HAL_UART_Abort_IT(&huart1);
        HAL_UART_Receive_IT(&huart1, &uart1_rx_buf[0], 1);
    }
    cJSON_Delete(root); // 释放根节点
}

void My_cJSON_Change(const char* key, const int valueint){

}