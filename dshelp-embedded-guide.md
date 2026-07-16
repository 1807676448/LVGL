# dshelp 命令 — 嵌入式设备适配指南

> 本文档供嵌入式设备开发使用，描述如何通过 MQTT 触发 DeepSeek AI 水质分析并接收显示结果。
>
> **本项目架构**：STM32H723 通过 UART2 连接 4G/WiFi 透传模块，使用自定义 AT 指令（`MQPUB`/`MQSUB`）与模块交互，由模块完成 MQTT 的 Broker 连接、Topic 映射和消息收发。STM32 端不直接运行 MQTT 协议栈。

---

## 1. 协议概述

设备通过 UART 发送 MQTT 指令，经 4G/WiFi 模块转发至 MQTT Broker，服务器调用 DeepSeek AI 分析最近 5 次水质数据，将精简结果下发至设备。

```
STM32 ──UART(AT)──▶ 4G/WiFi模块 ──MQTT──▶ devices/{id}/command  { "command": "dshelp" }
STM32 ◀──UART(AT)── 4G/WiFi模块 ◀──MQTT── devices/{id}/down     { "type": "dshelp", "analysis": "【水质概况】..." }
```

---

## 2. 发送命令

### 2.1 通信接口

| 参数 | 值 |
|------|-----|
| 物理接口 | UART2（连接 4G/WiFi 透传模块） |
| 波特率 | 按模块实际配置（本项目使用 DMA + IDLE 中断接收） |
| AT 指令格式 | `MQPUB,<qos>,<topic_index>,<json_payload>` |
| QoS | `0`（本项目模块内部保证送达） |
| 模块端配置 | 由模块固件维护 Broker 地址、用户名/密码、Client ID 等 |

### 2.2 Topic 索引说明

本项目使用 **Topic 索引号** 而非完整 Topic 字符串，由 4G/WiFi 模块内部完成索引→Topic 的映射：

| 索引 | 常量名 | 对应 MQTT Topic |
|------|--------|----------------|
| `0` | `MQTT_TOPIC_UP_INDEX` | `devices/{device_id}/up`（数据上报） |
| `1` | `MQTT_TOPIC_STATUS_INDEX` | `devices/{device_id}/status`（状态上报） |
| `2` | `MQTT_TOPIC_COMMAND_INDEX` | `devices/{device_id}/command`（命令下发） |

下行订阅使用 `MQSUB,<qos>,<topic_index>` 格式，订阅主题：

| 索引 | 常量名 | 对应 MQTT Topic |
|------|--------|----------------|
| `0` | `MQTT_TOPIC_DOWN_INDEX` | `devices/{device_id}/down`（下行数据） |

> 设备 ID 通过宏 `MQTT_DEVICE_ID` 定义，当前为 `"device_002"`

### 2.3 消息体（JSON）

```json
{
  "device_id": "device_002",
  "command": "dshelp"
}
```

| 字段 | 类型 | 必填 | 说明 |
|------|------|------|------|
| `device_id` | string | 是 | 设备唯一标识（与 `MQTT_DEVICE_ID` 一致） |
| `command` | string | 是 | 固定值 `"dshelp"`，不区分大小写 |

### 2.4 C 语言发送示例（STM32 — 基于 UART AT 指令）

```c
/**
 * @brief 发送 dshelp 命令请求 AI 水质分析
 * @param huart 连接 4G/WiFi 模块的 UART 句柄（本项目为 &huart2）
 *
 * AT 指令格式：MQPUB,0,2,{"device_id":"device_002","command":"dshelp"}
 *   0 = QoS
 *   2 = MQTT_TOPIC_COMMAND_INDEX（命令 Topic 索引）
 */
void MQTT_Send_DsHelp(UART_HandleTypeDef *huart)
{
    if (huart == NULL) return;

    char cmd[128];
    snprintf(cmd, sizeof(cmd),
             "MQPUB,0,%d,{\"device_id\":\"%s\",\"command\":\"dshelp\"}",
             MQTT_TOPIC_COMMAND_INDEX, MQTT_DEVICE_ID);

    /* Send_JSON 会追加 \r\n 换行，并镜像输出到 USART1 便于调试 */
    Send_JSON(cmd, huart);
}
```

> **关键宏定义**（位于 `My_Data_New.h`）：
> ```c
> #define MQTT_DEVICE_ID "device_002"
> #define MQTT_TOPIC_COMMAND_INDEX 2
> ```

### 2.5 Python（MicroPython / K230）发送示例

> 以下示例适用于 K230 等直接运行 MQTT 客户端的平台，与本项目 STM32 的 AT 透传方式不同。

```python
import ujson as json
from umqtt.simple import MQTTClient

def send_dshelp(device_id):
    client = MQTTClient(device_id, "106.15.53.24", port=1883)
    client.connect()
    topic = f"devices/{device_id}/command"
    payload = json.dumps({"device_id": device_id, "command": "dshelp"})
    client.publish(topic, payload, qos=1)
    client.disconnect()
```

---

## 3. 接收下行响应

### 3.1 订阅下行主题

设备启动时通过 `MQSUB` AT 指令订阅下行 Topic，之后 4G/WiFi 模块收到的 MQTT 消息会透传到 UART2。

### 3.2 订阅命令（启动时调用一次）

```c
/**
 * @brief 订阅下行主题
 * @param huart 连接 4G/WiFi 模块的 UART 句柄
 *
 * AT 指令格式：MQSUB,0,0
 *   0 = QoS
 *   0 = MQTT_TOPIC_DOWN_INDEX（下行 Topic 索引）
 */
void MQTT_Subscribe_Downlink(UART_HandleTypeDef *huart)
{
    char sub_cmd[32];
    snprintf(sub_cmd, sizeof(sub_cmd), "MQSUB,0,%d", MQTT_TOPIC_DOWN_INDEX);
    Send_JSON(sub_cmd, huart);
}
```

### 3.3 UART 接收与 JSON 解析

4G/WiFi 模块将 MQTT 下行消息原样通过 UART2 发送给 STM32。本项目使用 **DMA + IDLE 中断** 接收不定长数据，主循环中调用 `N_My_JsonGet()` 解析完整 JSON 帧：

```c
// 主循环中持续解析 UART2 接收缓冲区的 JSON 数据
N_My_JsonGet((char *)uart2_rx_buf, &huart2);
```

> 接收缓冲区 `uart2_rx_buf` 为 32 字节对齐（适配 STM32H7 D-Cache Line），大小 500 字节。

---

## 4. 下行响应格式

### 4.1 成功响应

```json
{
  "type": "dshelp",
  "ok": true,
  "timestamp": 1752652800000,
  "analysis": "【水质概况】整体良好。【关键指标】pH7.2,TDS123,COD45。【建议】继续监测。",
  "sample_count": 5,
  "device_id": "device_002"
}
```

### 4.2 字段说明

| 字段 | 类型 | 说明 |
|------|------|------|
| `type` | string | **固定 `"dshelp"`**，用于区分其他下行消息（如 `"time"` 校时响应） |
| `ok` | boolean | `true` 分析成功，`false` 分析失败 |
| `timestamp` | number | 服务器 Unix 毫秒时间戳 |
| `analysis` | string | **核心字段**，≤80 汉字的三段式分析文本，可直接显示 |
| `sample_count` | number | 参与分析的数据条数 |
| `device_id` | string | 设备 ID |

### 4.3 失败响应（示例）

```json
{
  "type": "dshelp",
  "ok": false,
  "timestamp": 1752652800000,
  "analysis": "暂无水质数据，请先上报数据。",
  "sample_count": 0,
  "device_id": "device_002"
}
```

常见失败原因：
| `analysis` 内容 | 原因 |
|-----------------|------|
| `暂无水质数据，请先上报数据。` | 数据库中没有该设备的水质记录 |
| `未配置AI密钥，请联系管理员。` | 服务器未配置 DeepSeek API Key |
| `AI服务暂时不可用，请稍后重试。` | 网络超时或 DeepSeek API 异常 |
| `AI分析异常，请稍后重试。` | 其它未知错误 |

---

## 5. 解析与显示

### 5.1 解析逻辑

下行 JSON 在 `N_My_JsonGet()` 中统一解析。需要在该函数中增加对 `type: "dshelp"` 消息的处理。

```c
// 在 N_My_JsonGet() 中增加 dshelp 类型判断
// 位置：解析完 root 对象后，在遍历数字键值对之前

void N_My_JsonGet(char *Json_Data, UART_HandleTypeDef *usart)
{
    // ... 现有代码：find_complete_json(), cJSON_Parse(), 设备状态解析 ...

    cJSON *root = cJSON_Parse(json_buf);
    if (root != NULL)
    {
        // ── 新增：检查下行消息类型 ──
        cJSON *msg_type = cJSON_GetObjectItem(root, "type");
        if (msg_type && cJSON_IsString(msg_type))
        {
            if (strcmp(msg_type->valuestring, "dshelp") == 0)
            {
                // 这是 AI 水质分析结果
                cJSON *analysis = cJSON_GetObjectItem(root, "analysis");
                cJSON *ok       = cJSON_GetObjectItem(root, "ok");
                cJSON *sc       = cJSON_GetObjectItem(root, "sample_count");

                if (analysis && analysis->valuestring) {
                    printf("[dshelp] analysis: %s\r\n", analysis->valuestring);
                    // TODO: 调用 LVGL 显示函数将 analysis 文本渲染到屏幕
                    // lcd_show_analysis(analysis->valuestring);
                }
                if (sc) {
                    printf("[dshelp] sample_count: %d\r\n", sc->valueint);
                }
                if (ok && cJSON_IsFalse(ok)) {
                    printf("[dshelp] AI analysis failed\r\n");
                }

                cJSON_Delete(root);
                reset_uart_rx(usart);
                return;  // dshelp 消息不包含传感器数据，直接返回
            }
            else if (strcmp(msg_type->valuestring, "time") == 0)
            {
                // 校时响应：timestamp 已在下方统一处理
            }
        }

        // ... 现有代码：解析 timestamp / NowTime、遍历数字键值对 ...
    }
}
```

### 5.2 显示屏布局建议

```
┌─────────────────────────────┐
│     💧 AI 水质分析          │  ← 标题行
│─────────────────────────────│
│                             │
│  【水质概况】整体良好。      │  ← analysis 第1段
│  【关键指标】pH7.2,TDS123   │  ← analysis 第2段
│     COD45,水温22.5℃。      │
│  【建议】继续日常监测即可。  │  ← analysis 第3段
│                             │
│─────────────────────────────│
│  样本数:5  │  16:05  ✅     │  ← 底栏：样本数 + 时间 + 状态
└─────────────────────────────┘
```

- `analysis` 文本格式固定为 `【水质概况】...【关键指标】...【建议】...`
- 可直接按 `【` 分段显示，或整体渲染
- 文本已限制在 80 汉字以内，适配 128×64 / 240×135 等常见嵌入式屏幕

---

## 6. 完整交互流程

```
┌──────────────────────────────────────────────────────────────┐
│ 1. 设备启动                                                   │
│    ├─ 初始化 UART2（DMA + IDLE 中断接收）                     │
│    ├─ MQTT_Subscribe_Downlink(&huart2)  ← 订阅下行 Topic     │
│    ├─ MQTT_Request_Time(&huart2)        ← 请求校时            │
│    └─ MQTT_Report_Status(&huart2, "online", 0) ← 上报在线    │
│                                                              │
│ 2. 用户触发（按键 / 触摸屏按钮）                               │
│    └─ MQTT_Send_DsHelp(&huart2)                              │
│       → UART 发送: MQPUB,0,2,{"device_id":"...",             │
│                     "command":"dshelp"}                      │
│                                                              │
│ 3. 等待响应（建议超时 15 秒）                                  │
│    ├─ UART2 收到下行 JSON → N_My_JsonGet() 解析              │
│    ├─ type="dshelp" → 提取 analysis 显示到 LCD               │
│    ├─ ok=false      → 显示 analysis 中的错误提示              │
│    └─ 超时未收到     → 显示"AI分析超时，请重试"               │
└──────────────────────────────────────────────────────────────┘
```

> 当前项目中，`MQTT_Subscribe_Downlink()`、`MQTT_Request_Time()`、`MQTT_Report_Status()` 已在 `main.c` 启动流程中调用。需要新增 `MQTT_Send_DsHelp()` 函数，并在 dshelp 触发逻辑（触摸按钮/按键）中调用。

---

## 7. 注意事项

| 事项 | 说明 |
|------|------|
| **command 不区分大小写** | `dshelp`、`DSHELP`、`DsHelp` 均可 |
| **必须先上报数据** | 至少 5 条水质数据才能获得有效分析，否则返回 `sample_count:0` |
| **分析耗时** | 通常 2~5 秒，取决于网络和 DeepSeek API 响应速度 |
| **AT 指令 QoS** | 本项目 MQPUB/MQSUB 使用 QoS 0，由 4G/WiFi 模块保证送达 |
| **重复触发保护** | 建议加防抖：收到响应或超时前，忽略重复按键 |
| **下行消息类型区分** | 务必检查 `type` 字段，不要混淆 `dshelp`、`time` 和普通数据下发 |
| **字符编码** | JSON 使用 UTF-8，`analysis` 仅含常用汉字+数字+中文标点 |
| **D-Cache 一致性** | STM32H7 在 UART DMA 接收后需调用 `SCB_CleanInvalidateDCache_by_Addr()` 确保数据一致性 |
| **UART 缓冲区对齐** | 接收缓冲区需 32 字节对齐（D-Cache Line 大小） |

---

## 8. 调试建议

### 8.1 先用 PC 验证链路

```bash
# 终端1：监听下行
mosquitto_sub -h 服务器IP -t "devices/device_002/down" -q 1 -v

# 终端2：发送命令
mosquitto_pub -h 服务器IP -t "devices/device_002/command" \
  -m '{"device_id":"device_002","command":"dshelp"}' -q 1
```

### 8.2 检查 STM32 端 UART 日志

STM32 的 `Send_JSON()` 会将所有 AT 指令镜像输出到 USART1（调试串口），可通过串口助手观察：

```
[USART1 镜像] MQSUB,0,0
[USART1 镜像] MQPUB,0,2,{"device_id":"device_002","command":"time"}
[USART1 镜像] MQPUB,0,1,{"device_id":"device_002","status":"online","runtime_seconds":0}
[USART1 镜像] MQPUB,0,2,{"device_id":"device_002","command":"dshelp"}
```

UART2 接收的下行 JSON 也会在 `N_My_JsonGet()` 中通过 `printf` 输出到 USART1：

```
Json_start:{"type":"dshelp","ok":true,"timestamp":1752652800000,"analysis":"【水质概况】...","sample_count":5,"device_id":"device_002"}
[dshelp] analysis: 【水质概况】整体良好。【关键指标】pH7.2,TDS123,COD45。【建议】继续监测。
[dshelp] sample_count: 5
```

### 8.3 检查 4G/WiFi 模块状态

确认模块已正确配置 MQTT Broker 地址并成功联网。模块通常有自己的状态指示灯或 AT 查询指令（如 `AT+CSQ` 查询信号强度）。
