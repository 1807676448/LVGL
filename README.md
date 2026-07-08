# LVGL + STM32H723ZGT6 嵌入式人机交互终端

**技术参考文档**

---

| 项目 | 内容 |
|------|------|
| **MCU** | STM32H723ZGT6 — LQFP144, ARM Cortex-M7, 550MHz |
| **图形库** | LVGL v9.4.0-dev |
| **开发平台** | STM32CubeMX 6.15.0 + Keil MDK-ARM V5.32 (ArmClang AC6) |
| **HAL 版本** | STM32Cube FW_H7 V1.12.1 |
| **文档版本** | 2.0 |
| **最后更新** | 2025-08 |

---

## 目录

1. [项目概述](#1-项目概述)
2. [目录结构](#2-目录结构)
3. [硬件架构与引脚分配](#3-硬件架构与引脚分配)
4. [CubeMX 配置详情](#4-cubemx-配置详情)
5. [时钟配置](#5-时钟配置)
6. [外设初始化流程](#6-外设初始化流程)
7. [软件架构](#7-软件架构)
8. [LVGL 集成详解](#8-lvgl-集成详解)
9. [LCD 显示链路 (ILI9488)](#9-lcd-显示链路-ili9488)
10. [触摸屏驱动 (ADS7846)](#10-触摸屏驱动-ads7846)
11. [串口通信与数据协议](#11-串口通信与数据协议)
12. [JSON 数据处理与 MQTT 接口](#12-json-数据处理与-mqtt-接口)
13. [UI 系统设计](#13-ui-系统设计)
14. [中断管理](#14-中断管理)
15. [MPU 与 Cache 配置](#15-mpu-与-cache-配置)
16. [构建与烧录](#16-构建与烧录)
17. [配置入口与关键宏定义](#17-配置入口与关键宏定义)
18. [已知问题与优化方向](#18-已知问题与优化方向)
19. [变更记录](#19-变更记录)

---

## 1. 项目概述

本项目是基于 **STM32H723ZGT6** 微控制器的嵌入式人机交互终端系统，集成 **LVGL v9.4.0-dev** 图形库，通过 SPI 总线驱动 480×320 分辨率的 ILI9488 TFT LCD，并支持 ADS7846 电阻式触摸屏输入。系统具备 5 路 UART 串口通信能力，可通过 4G 模块（USART2）与云端 MQTT 服务器进行双向通信，支持 JSON 格式数据解析与封装，适用于 **水质监测、设备状态监控** 等物联网数据可视化场景。

### 技术亮点

- STM32CubeMX 生成 HAL 底层代码，业务逻辑与硬件抽象清晰分离
- LVGL 使用双缓冲 + SPI DMA 刷新机制，通过 D-Cache Clean 维护内存一致性
- ADS7846 触摸驱动采用 GPIO 模拟 SPI + 双重滤波算法（排序去极值 + 两次采样差值验证）
- JSON 解析支持半包/粘包处理，基于栈深度的完整 JSON 对象定位算法
- MQTT 指令封装支持订阅/发布/状态上报/时间同步等标准操作
- 多页 UI：主屏幕 / 水质数据条形图 / 设备状态监控 / 虚拟遥杆 / 关于页

### 核心功能模块

| 模块 | 功能 |
|------|------|
| **图形界面** | LVGL 驱动 ILI9488 480×320, RGB565→RGB666 转换, SPI1+DMA 刷新 |
| **触摸输入** | ADS7846 电阻触摸, GPIO 模拟 SPI, 双重滤波, 可变校准参数 |
| **数据通信** | 5 路 UART (USART1/2/3/6/10), 115200bps, 中断单字节循环拼包 |
| **JSON 解析** | 基于 cJSON, 提取设备状态/时间戳/数字键值对并存储 |
| **MQTT 协议** | 通过 USART2 的 AT 指令集实现订阅下行/数据上报/时间请求/状态上报 |
| **设备管理** | 4 设备在线状态监控, 心跳超时 (30s 默认) 自动标记离线 |
| **调试支持** | printf 重定向至 USART1, 串口收发镜像, 微秒级高精度延时 |

---

## 2. 目录结构

```
LVGL/
├── Core/
│   ├── Inc/                          # 头文件
│   │   ├── main.h                    # 主头文件, 引脚 GPIO_PIN 宏定义
│   │   ├── stm32h7xx_hal_conf.h      # HAL 模块使能配置
│   │   ├── stm32h7xx_it.h            # 中断服务声明
│   │   ├── gpio.h / spi.h / dma.h   # 外设句柄声明 (HAL)
│   │   ├── tim.h / usart.h           # 定时器/串口句柄声明
│   │   ├── memorymap.h              # 内存映射 (CubeMX)
│   │   ├── lcd.h                    # ILI9488 驱动, 颜色宏, 缓冲区宏
│   │   ├── Touch.h                  # ADS7846 驱动, 触摸结构体
│   │   ├── sys.h                    # GPIO 快速操作宏 (OUT/IN/TOGGLE)
│   │   ├── My_Debug.h              # 调试输出 + 微秒延时 + printf 重定向
│   │   ├── My_LVGL.h              # UI 构建, 刷新回调, 遥杆状态接口
│   │   ├── My_Data_New.h           # JSON/MQTT 模块接口, 结构体, 宏
│   │   └── cJSON.h                 # cJSON 库头文件
│   │
│   ├── Src/                          # 源文件
│   │   ├── main.c                    # 系统入口, 主循环, 中断回调
│   │   ├── stm32h7xx_hal_msp.c       # HAL MSP 初始化 (SPI/DMA/UART)
│   │   ├── stm32h7xx_it.c            # 中断向量处理 (DMA/SPI/UART/TIM)
│   │   ├── system_stm32h7xx.c        # SystemInit
│   │   ├── gpio.c / spi.c / dma.c   # 外设初始化 (CubeMX 生成)
│   │   ├── tim.c / usart.c           # 定时器/串口初始化
│   │   ├── lcd.c                    # ILI9488 驱动: 寄存器操作, 清屏, 窗口设置
│   │   ├── Touch.c                  # ADS7846 驱动: 模拟SPI, AD读取, 滤波
│   │   ├── My_Debug.c              # printf/fgetc 重定向, 微秒延时, 计时
│   │   ├── My_LVGL.c               # UI 4屏构建, flush_cb, 样式, 遥杆
│   │   ├── My_Data_New.c           # JSON解析, 键值存储, MQTT封装, 时间同步
│   │   └── cJSON.c                 # cJSON 实现
│   │
│   └── lvgl/                         # LVGL v9.4.0-dev
│       ├── lv_conf.h                # 项目级 LVGL 配置
│       └── src/                     # LVGL 核心源码
│
├── Drivers/                          # STM32Cube 固件包
│   ├── CMSIS/                        # CMSIS Core + Device (STM32H723xx)
│   └── STM32H7xx_HAL_Driver/         # HAL 驱动源码
│
├── MDK-ARM/                          # Keil 工程
│   ├── LVGL.uvprojx                  # Keil 项目
│   ├── LVGL.uvoptx                   # Keil 选项
│   └── startup_stm32h723xx.s         # 启动代码 (中断向量表)
│
├── Vscode/                           # VS Code + EIDE 配置与构建输出
├── LVGL.ioc                          # CubeMX 工程配置 (可双击打开)
├── .mxproject                        # CubeMX 元数据
└── README.md                         # 本文档
```

---

## 3. 硬件架构与引脚分配

### 3.1 系统框图

```
┌──────────────────── STM32H723ZGT6 (LQFP144) ────────────────────┐
│  Cortex-M7 @550MHz, I+D Cache                                   │
│  ┌──────────┐  AXI Bus 275MHz                                   │
│  │ DMA1     │                                                    │
│  │ S0→SPI1_TX → ILI9488 (480×320 LCD, SPI 45.83Mbps)           │
│  │ S3←SPI1_RX                                                    │
│  ├──────────┤                                                    │
│  │ APB Bus 137.5MHz                                              │
│  │ ├ SPI2   (预留硬件SPI, 91.67Mbps)                             │
│  │ ├ TIM6    LVGL 1ms 时基 (PSC=274, ARR=1000)                  │
│  │ ├ TIM7    微秒延时基准 (PSC=274, ARR=65535)                   │
│  │ ├ TIM13   周期任务调度 (PSC=274, ARR=1000)                    │
│  │ ├ USART1  调试输出 + 镜像 (printf重定向)                      │
│  │ ├ USART2  MQTT/4G 主通道 (AT指令交互)                         │
│  │ ├ USART3  遥杆控制下行                                        │
│  │ ├ USART6  预留扩展                                            │
│  │ └ USART10 预留扩展                                            │
│  ├──────────┤                                                    │
│  │ GPIO 模拟SPI (ADS7846 触摸)                                   │
│  │ PF13=T_CLK, PF14=T_CS, PF15=T_DIN, PG0=T_DO, PG1=T_IRQ      │
└──────────────────────────────────────────────────────────────────┘
```

### 3.2 完整引脚分配表

#### LCD 接口 (SPI1 + GPIO)

| STM32 引脚 | 信号 | LCD 引脚 | 方向 | 配置 |
|-----------|------|---------|------|------|
| PA5 | SPI1_SCK | SCK | O | AF5, 推挽, 无上下拉, 最高速 |
| PA6 | SPI1_MISO | SDO | I | AF5, 推挽, 无上下拉, 最高速 |
| PA7 | SPI1_MOSI | SDI | O | AF5, 推挽, 无上下拉, 最高速 |
| PB0 | GPIO_Out | CS | O | 推挽, 上拉, 最高速, 初始高, 标签 LCD_CS |
| PC5 | GPIO_Out | DC/RS | O | 推挽, 上拉, 最高速, 初始高, 标签 LCD_DC/RS |
| PB1 | GPIO_Out | RST | O | 推挽, 上拉, 最高速, 初始高, 标签 LCD_RST |
| PC4 | GPIO_Out | LED/BL | O | 推挽, 上拉, 最高速, 初始高, 标签 LCD_LED |

#### 触摸屏 (GPIO 模拟 SPI)

| STM32 引脚 | 信号 | 触摸引脚 | 方向 | 配置 |
|-----------|------|---------|------|------|
| PF13 | T_CLK | CLK | O | 推挽, 已锁定 |
| PF14 | T_CS | CS | O | 推挽, 初始高, 无上下拉, 标签 T_CS |
| PF15 | T_DIN | DIN | O | 推挽, 初始高, 无上下拉, 标签 T_DIN |
| PG0 | T_DO | DOUT | I | 浮空, 标签 T_DO |
| PG1 | T_IRQ | IRQ | I | 上拉, 标签 T_IRQ |

#### 串口接口

| STM32 引脚 | 信号 | 方向 | 用途 |
|-----------|------|------|------|
| PA9 | USART1_TX | O | 调试输出, printf 重定向 |
| PA10 | USART1_RX | I | 调试输入 |
| PA2 | USART2_TX | O | MQTT/4G 发送 |
| PA3 | USART2_RX | I | MQTT/4G 接收 |
| PB10 | USART3_TX | O | 遥杆控制输出 |
| PB11 | USART3_RX | I | 遥杆控制输入 |
| PC6 | USART6_TX | O | 预留 |
| PG9 | USART6_RX | I | 预留 |
| PE3 | USART10_TX | O | 预留 |
| PE2 | USART10_RX | I | 预留 |

#### 系统与时钟

| 引脚 | 信号 | 说明 |
|------|------|------|
| PH0/PH1 | HSE 25MHz | 外部高速晶振 |
| PC14/PC15 | LSE 32.768kHz | 外部低速晶振 |
| PA13/PA14 | SWDIO/SWCLK | 调试接口 |

---

## 4. CubeMX 配置详情

### 4.1 工程设置

| 配置项 | 值 |
|--------|-----|
| 项目名称 | LVGL |
| 目标芯片 | STM32H723ZGTx |
| 封装 | LQFP144 |
| CubeMX 版本 | 6.15.0 |
| 固件包版本 | STM32Cube FW_H7 V1.12.1 |
| 目标工具链 | MDK-ARM V5.32 |
| 堆大小 | 0x2000 (8KB) |
| 栈大小 | 0x4000 (16KB) |

### 4.2 CORTEX_M7 与 NVIC

| 中断源 | 抢占优先级 | 子优先级 | 用途 |
|--------|----------|---------|------|
| SysTick | 15 | 0 | HAL 时基 |
| DMA1_Stream0 | 0 | 0 | SPI1 TX 完成 |
| DMA1_Stream3 | 0 | 0 | SPI1 RX 完成 |
| SPI1 | 0 | 0 | SPI1 中断 |
| TIM6_DAC | 0 | 0 | LVGL 时基 |
| TIM7 | 0 | 0 | 微秒延时 |
| TIM8_UP_TIM13 | 2 | 0 | 周期业务调度 |
| USART1/2/3/6/10 | 0 | 0 | 串口接收 |
| HardFault/MemManage/BusFault/UsageFault | 0 | 0 | 异常处理 |

### 4.3 SPI 配置

| 参数 | SPI1 (LCD) | SPI2 (预留) |
|------|-----------|------------|
| 模式 | Full Duplex Master | Full Duplex Master |
| 数据宽度 | 8 Bit | 8 Bit |
| CPOL/CPHA | Low / 1 Edge | Low / 1 Edge |
| NSS | Soft | Soft |
| 波特率分频 | 4 (45.83Mbps) | 2 (91.67Mbps) |
| 时钟源 | PLL1_Q (183.33MHz) | PLL1_Q |

### 4.4 定时器配置

| 定时器 | Prescaler | ARR | 时钟源 | 用途 |
|--------|-----------|-----|--------|------|
| TIM6 | 274 | 1000 | APB1×2 (275MHz) | LVGL lv_tick_inc(1) |
| TIM7 | 274 | 65535 | APB1×2 | 微秒延时计数 |
| TIM13 | 274 | 1000 | APB1×2 | 周期业务调度 |

### 4.5 USART 与 DMA 配置

| USART | 波特率 | 用途 |
|-------|--------|------|
| USART1 | 115200 | 调试 + 镜像 |
| USART2 | 115200 | MQTT/4G |
| USART3 | 115200 | 遥杆 |
| USART6 | 115200 | 预留 |
| USART10 | 115200 | 预留 |

| DMA 流 | 方向 | 外设 | 模式 | 数据宽度 |
|--------|------|------|------|---------|
| DMA1_S0 | Mem→Periph | SPI1_TX | Normal | Byte |
| DMA1_S3 | Periph→Mem | SPI1_RX | Normal | Byte |

---

## 5. 时钟配置

### 5.1 时钟树

```
HSI (64MHz) ──► PLL1 (主PLL)
                 ├─ /M=4  → VCO Input  = 16MHz
                 ├─ ×N=34 → VCO Output = 544MHz
                 ├─ FRACN=3072 → 精确微调 VCO = 550MHz
                 ├─ /P=1 → PLL1_P = 550MHz  → SYSCLK (CPU)
                 ├─ /Q=3 → PLL1_Q = 183.33MHz → SPI123, USB, FDCAN, SDMMC
                 └─ /R=2 → PLL1_R = 275MHz → AHB, AXI, FMC, QSPI

总线分频:
  SYSCLK = 550MHz (DIV1)     CPU 内核频率
  HCLK   = 275MHz (DIV2)     AHB 总线
  APB1/2/3/4 = 137.5MHz (DIV2)   APB 外设总线

电源配置:
  LDO供电 | VOS0 (Scale 0, 最高性能) | FLASH_LATENCY_3
```

### 5.2 SystemClock_Config 流程

1. `HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY)` — LDO 供电模式
2. `__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0)` — Scale 0
3. 等待 `PWR_FLAG_VOSRDY` — VOS 就绪
4. 配置振荡器: HSI DIV1 → PLL1 (M=4, N=34, P=1, Q=3, R=2, FRACN=3072)
5. 配置系统时钟: SYSCLK=PLLCLK, HCLK_DIV2, APB_DIV2, FLASH_LATENCY_3

---

## 6. 外设初始化流程

### 6.1 main() 启动序列

```
MPU_Config()
  → SCB_EnableICache()
  → SCB_EnableDCache()
  → HAL_Init()                            // SysTick 配置
  → SystemClock_Config()                   // 时钟树配置
  → MX_GPIO_Init()                         // GPIO
  → MX_DMA_Init()                          // DMA1 Stream0/3
  → MX_SPI1_Init()                         // SPI1 (LCD 通道)
  → MX_TIM6_Init()                         // TIM6 (LVGL 时基)
  → MX_USART1_UART_Init()                  // USART1 (调试)
  → MX_TIM7_Init()                         // TIM7 (微秒延时)
  → MX_USART2_UART_Init()                  // USART2 (MQTT)
  → MX_TIM13_Init()                        // TIM13 (周期任务)
  → MX_SPI2_Init()                         // SPI2
  → MX_USART3_UART_Init()                  // USART3
  → MX_USART6_UART_Init() / MX_USART10_UART_Init()
  // ---------- 用户初始化 ----------
  → HAL_TIM_Base_Start_IT(&htim6)          // 启动 LVGL 时基
  → HAL_TIM_Base_Start_IT(&htim7)          // 启动微秒延时定时器
  → HAL_TIM_Base_Start_IT(&htim13)         // 启动周期任务定时器
  → HAL_UART_Receive_IT(&huart1/2/3, ..., 1) // 启动单字节中断接收
  → LCD_Init()                             // ILI9488 初始化
  → TP_Init()                              // 触摸初始化
  → MQTT_Subscribe_Downlink(&huart2)        // 发送 MQTT 订阅
  → MQTT_Request_Time(&huart2)              // 请求服务器时间
  → MQTT_Report_Status(&huart2, "online", 0) // 上报在线
  → lv_init()                              // LVGL 内核初始化
  → lv_display_create(480, 320)            // 创建显示
  → lv_display_set_flush_cb(my_flush_cb)   // 绑定刷新回调
  → lv_display_set_buffers(buf1, buf2, PARTIAL) // 设置双缓冲
  → lv_indev_create() + set_type(POINTER) + set_read_cb(my_input_read)
  → setup_ui()                             // 构建 UI
```

### 6.2 主循环

```c
while (1) {
    lv_timer_handler();              // LVGL 任务调度 (动画/重绘/定时器)
    HAL_Delay(5);                    // 5ms 循环周期
    TimeChange();                    // 时间显示更新 (UTC+8)
    N_My_JsonGet(uart1_rx_buf, &huart1);  // 解析 USART1
    N_My_JsonGet(uart2_rx_buf, &huart2);  // 解析 USART2
    N_My_JsonGet(uart3_rx_buf, &huart3);  // 解析 USART3
    if (joystick_screen_is_active())
        USART3_Test_Task();          // 遥杆控制帧发送
}
```

---

## 7. 软件架构

### 7.1 分层设计

```
┌──────────────────────────────────────────┐
│  应用层:  main.c — 系统初始化, 主循环      │
├──────────────────────────────────────────┤
│  表现层:  My_LVGL.c — UI 页面, flush_cb, │
│           样式主题, 动画, 遥杆逻辑         │
├──────────────────────────────────────────┤
│  协议层:  My_Data_New.c — JSON 解析,      │
│           MQTT 封装, 键值存储, 时间同步    │
├──────────────────────────────────────────┤
│  驱动层:  lcd.c, Touch.c, My_Debug.c     │
│           + CubeMX 生成的 spi/usart/tim   │
├──────────────────────────────────────────┤
│  HAL 层:  STM32H7xx_HAL_Driver            │
├──────────────────────────────────────────┤
│  CMSIS:   Core (Cortex-M7) + Device       │
└──────────────────────────────────────────┘
```

### 7.2 模块依赖图

```
main.c
 ├─ lcd.h ─────────► SPI1 + DMA + GPIO
 ├─ Touch.h ────────► GPIO 模拟 SPI
 ├─ My_LVGL.h ──────► LVGL + lcd + Touch + My_Data_New
 │   ├─ Core/lvgl/lvgl.h
 │   ├─ dma.h, memorymap.h, spi.h, tim.h
 │   └─ My_Debug.h
 ├─ My_Data_New.h ──► cJSON.h + My_Debug.h + My_LVGL.h
 └─ My_Debug.h ─────► usart.h, sys.h, tim.h
```

### 7.3 关键全局变量

| 变量 | 类型 | 位置 | 说明 |
|------|------|------|------|
| `buf1[38400], buf2[38400]` | `uint8_t[]` | main.c | LVGL 双帧缓冲 (AXI SRAM 段) |
| `dma_buffer[57600]` | `uint8_t[]` | lcd.c | SPI DMA 传输缓冲 (32 字节对齐) |
| `spi_dma_tx_complete` | `volatile uint8_t` | lcd.c | DMA 传输完成标志 |
| `uart1/2/3_rx_buf[500]` | `uint8_t[]` | main.c | 串口接收环形缓冲 |
| `uart1/2/3_ins` | `int16_t` | main.c | 串口缓冲区写入索引 |
| `UNX_Now_Time` | `uint64_t` | main.c | 当前 Unix 时间戳 (ms) |
| `tp_dev` | `_m_tp_dev` | Touch.c | 触摸设备全局状态 (坐标/校准) |
| `config_items[50]` | `config_item_t[]` | My_Data_New.c | JSON 键值对存储 |
| `device_list[4]` | `device_status_t[]` | My_Data_New.c | 设备在线状态列表 |

---

## 8. LVGL 集成详解

### 8.1 lv_conf.h 关键配置

| 配置项 | 值 | 说明 |
|--------|-----|------|
| `LV_COLOR_DEPTH` | 16 | RGB565 色深 |
| `LV_MEM_SIZE` | 128KB | 内部内存池 |
| `LV_DEF_REFR_PERIOD` | 33ms | 默认刷新周期 (~30FPS) |
| `LV_DPI_DEF` | 130 | 默认 DPI |
| `LV_USE_OS` | LV_OS_NONE | 裸机运行, 无 RTOS |
| `LV_DRAW_LAYER_SIMPLE_BUF_SIZE` | 24KB | 图层缓冲 |
| `LV_DRAW_THREAD_STACK_SIZE` | 8KB | 绘制线程栈 |
| `LV_USE_DRAW_SW` | 1 | 软件渲染 |
| `LV_DRAW_SW_COMPLEX` | 1 | 复杂渲染 (圆角/阴影) |
| `LV_DRAW_SW_CIRCLE_CACHE_SIZE` | 4 | 圆形缓存 |
| `LV_DRAW_SW_DRAW_UNIT_CNT` | 1 | 单绘制单元 |
| `LV_USE_STDLIB_MALLOC` | LV_STDLIB_BUILTIN | 内置内存管理 |

> **注意**: 未启用 DMA2D 硬件加速，所有渲染由软件完成。STM32H723 内置的 DMA2D 可在后续启用以提升性能。

### 8.2 初始化代码

```c
lv_init();
lv_display_t *disp = lv_display_create(480, 320);
lv_display_set_flush_cb(disp, my_flush_cb);
lv_display_set_buffers(disp, buf1, buf2, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);

lv_indev_t *indev = lv_indev_create();
lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
lv_indev_set_read_cb(indev, my_input_read);

setup_ui();
```

### 8.3 时基驱动

TIM6 中断回调中调用 `lv_tick_inc(1)`:

```c
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM6) {
        lv_tick_inc(1);       // LVGL 时间基准 1ms
        UNX_Now_Time += 1;    // 本地时间戳累加
    }
}
```

> 主循环中调用 `lv_timer_handler()` 驱动 LVGL 内部任务（动画/定时器/重绘）。循环周期 5ms。

---

## 9. LCD 显示链路 (ILI9488)

### 9.1 硬件参数

| 参数 | 值 |
|------|-----|
| 驱动 IC | ILI9488 |
| 分辨率 | 480 × 320 (横屏, USE_HORIZONTAL=1) |
| 接口 | 4 线 SPI (SCK, MOSI, CS, DC) |
| 像素格式 | ILI9488 接收 RGB666 (18-bit), LVGL 输出 RGB565 |
| SPI 速率 | 45.83 Mbits/s (SPI1, PLL1_Q 4 分频) |

### 9.2 RGB565 → RGB666 色彩转换

在 `my_flush_cb` 中逐像素转换:

```c
*dma_ptr++ = (*color_p >> 8) & 0xF8;   // R[7:3]
*dma_ptr++ = (*color_p >> 3) & 0xFC;   // G[7:2]
*dma_ptr++ = (*color_p << 3);          // B[7:3]
```

### 9.3 my_flush_cb 完整流程

```
1. 计算刷新区域 → LCD_SetWindows() 设置写入窗口
2. LCD_CS_CLR; LCD_RS_SET → 片选有效, 数据模式
3. 分块循环 (每块 ≤ OneStepSize = 19200 像素):
   a. RGB565→RGB666 转换到 dma_buffer
   b. while(!spi_dma_tx_complete); → 等待上次 DMA 完成
   c. spi_dma_tx_complete = 0;
   d. SCB_CleanDCache_by_Addr(dma_buffer, size);  ← 关键! 刷新 D-Cache
   e. HAL_SPI_Transmit_DMA(&hspi1, dma_buffer, size);
4. 等待最后一块 DMA 完成 → LCD_CS_SET
5. lv_display_flush_ready(display) → 通知 LVGL
```

### 9.4 缓冲区大小

| 缓冲 | 大小 | 位置 |
|------|------|------|
| `buf1/buf2` (LVGL 双缓冲) | 各 38.4KB (19200 × 2) | main.c, AXI SRAM 段 |
| `dma_buffer` (DMA 传输) | 57.6KB (19200 × 3) | lcd.c, 32 字节对齐 |

### 9.5 ILI9488 初始化序列 (LCD_Init)

| 寄存器 | 数据 | 说明 |
|--------|------|------|
| 0xF7 | A9,51,2C,82 | 电源配置 |
| 0xC0/C1 | 11,09 / 41 | 面板驱动/时序 |
| 0xB1/B4/B6/B7 | ... | 帧率/显示控制 |
| 0x36 | BGR=1,MY=1,MX=0,MV=1 | 内存访问 (横屏) |
| 0x3A | 0x66 | 像素格式 18-bit |
| 0xE0 | 15 字节 | 正 Gamma |
| 0xE1 | 15 字节 | 负 Gamma |
| 0x11 | (延时 120ms) | 退出睡眠 |
| 0x29 | - | 开显示 |

---

## 10. 触摸屏驱动 (ADS7846)

### 10.1 硬件参数

| 参数 | 值 |
|------|-----|
| 触控 IC | ADS7846 (兼容 XPT2046) |
| 通信接口 | GPIO 模拟 SPI (PF13=CLK, PF15=DIN, PG0=DO, PF14=CS) |
| 检测引脚 | PG1 (T_IRQ), 低电平有效 |

### 10.2 双重滤波算法

**第一级 — 排序去极值 (`TP_Read_XOY`)**:
1. 连续读取 5 次 AD 值 → 冒泡排序
2. 丢弃最大值和最小值各 1 个
3. 取中间 3 个值的平均值

**第二级 — 双重验证 (`TP_Read_XY2`)**:
1. 连续两次调用 `TP_Read_XY` 获取坐标
2. 检查两次 X/Y 差值均在 ERR_RANGE(50) 以内
3. 取两次平均值

### 10.3 LVGL 输入回调

```c
void my_input_read(lv_indev_t *indev, lv_indev_data_t *data) {
    if (TP_Scan(0)) {
        data->point.y = tp_dev.x * 0.08;    // 坐标缩放 + X/Y 交换
        data->point.x = tp_dev.y * 0.12;
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}
```

> 坐标变换系数 (0.08, 0.12) 与 X/Y 轴交换需根据实际屏幕安装方向校准。

### 10.4 ADS7846 读取流程

```
1. TCS=0 (片选有效)
2. TP_Write_Byte(CMD) — 发送命令 (CMD_RDX=0xD0 读X, CMD_RDY=0x90 读Y)
3. 延时 8μs 等待 AD 转换完成
4. 额外时钟脉冲清除 BUSY 标志
5. 循环读取 16 位 AD 值 (MSB first, 上升沿采样)
6. 右移 4 位 (丢弃低 4 位无效数据)
7. TCS=1 (释放片选)
```

---

## 11. 串口通信与数据协议

### 11.1 UART 角色分配

| UART | 用途 | 接收方式 |
|------|------|---------|
| USART1 | 调试输出 + 镜像 (printf 重定向, MQTT 指令镜像) | 中断单字节 |
| USART2 | MQTT/4G 通信主通道 | 中断单字节 |
| USART3 | 遥杆控制 (接收状态 + 发送控制帧) | 中断单字节 |
| USART6/10 | 预留扩展 | 中断单字节 |

### 11.2 中断接收机制

单字节中断循环拼包, 500 字节环形缓冲:

```c
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        if (uart2_ins > 500-1) uart2_ins = 0;  // 环形缓冲
        uart2_ins++;
        HAL_UART_Receive_IT(&huart2, &uart2_rx_buf[uart2_ins], 1);
    }
    // USART1, USART3 同理
}
```

### 11.3 遥杆控制帧格式 (USART3_TX)

遥杆屏幕激活时, 每 250ms 发送 6 字节控制帧:

```
帧结构: 0xAA | Up(%) | Down(%) | Left(%) | Right(%) | 0x55
各方向值: 0~100, 基于遥杆偏移百分比计算
```

---

## 12. JSON 数据处理与 MQTT 接口

### 12.1 数据结构

```c
// 设备状态 (最多 4 设备)
typedef struct {
    int id;
    char status[16];      // "online"/"offline"/"active"
    bool valid;
} device_status_t;

// 配置项 (最多 50 项)
typedef struct {
    char key[32];
    cJSON *value;          // cJSON 节点指针
    uint8_t type;          // 类型标识
} config_item_t;
```

### 12.2 JSON 解析算法 — find_complete_json

从原始字节流中提取第一个完整 JSON 对象:

1. `strchr(raw, '{')` — 查找起始花括号
2. 深度计数 + 状态机遍历:
   - `\\` — 转义字符跳过
   - `"` — 切换字符串内/外状态
   - 非字符串内: `{` → depth++, `}` → depth--
3. `depth == 0` → 找到匹配的 `}`, 返回 [start, end]

### 12.3 N_My_JsonGet 完整流程

```
原始数据 → find_complete_json() → 提取完整 JSON
  → cJSON_Parse() 解析
  → 提取 "device_id" + "status" → 更新 device_list[]
  → 提取 "timestamp" 或 "NowTime":
      - 若 < 10^12: ×1000 转为毫秒
      - 验证范围 (2020~2039)
      - 同步 UNX_Now_Time
  → 遍历所有子节点 (数字类型):
      - update_screen1_item(key, value) → 更新 UI 条形图
      - N_My_JsonChange_Int/Double(key, value) → 存储键值对
  → cJSON_Delete(root)
  → reset_uart_rx(huart) → 重置接收缓冲区
```

### 12.4 MQTT 指令封装

**主题配置宏**:
```c
#define MQTT_DEVICE_ID           "device_002"
#define MQTT_TOPIC_UP_INDEX      0   // 数据上报
#define MQTT_TOPIC_STATUS_INDEX  1   // 状态上报
#define MQTT_TOPIC_COMMAND_INDEX 2   // 命令下发
#define MQTT_TOPIC_DOWN_INDEX    0   // 下行订阅
```

**指令格式**:

| 功能 | AT 指令格式 |
|------|-----------|
| 订阅下行 | `MQSUB,0,<topic>` |
| 请求时间 | `MQPUB,0,<cmd_idx>,{"device_id":"device_002","command":"time"}` |
| 状态上报 | `MQPUB,0,<status_idx>,{"device_id":"device_002","status":"online","runtime_seconds":...}` |
| 数据上报 | `MQPUB,0,<up_idx>,{"id":"device_002","device_id":"device_002","params":{<key>:{<value>}}}` |

> 所有 MQTT 指令通过 `Send_JSON()` 发送, 并在末尾添加 `\r\n`。非 USART1 的发送会自动镜像到 USART1 以便调试。

### 12.5 周期性任务 (TIM13 回调)

| 任务 | 触发条件 | 说明 |
|------|---------|------|
| 数据上报 | `times % SendData_Time == 0` | 默认每 3000 次中断 (6s) 发送 |
| 时间请求 | `times % 10000 == 123` | 每约 20s 请求服务器时间 |
| 状态上报 | `times % StatusReport_Time == 0` | 默认每 5000 次中断 (10s) 上报 |

### 12.6 时间同步

`TimeChange()` 函数将 `UNX_Now_Time` (毫秒级 Unix 时间戳) 转换为北京时间 (UTC+8) 后更新主屏幕显示:
```c
void TimeChange(void) {
    time_t t = (time_t)(UNX_Now_Time / 1000 + 28800); // UTC+8
    struct tm *lt = localtime(&t);
    update_main_screen_info(time_str, date_str, NULL, "Hello, User!");
}
```

---

## 13. UI 系统设计

### 13.1 屏幕架构

```
setup_ui()
├── ui_init_theme_and_style()    // 初始化全局样式 (主题/卡片/按钮/标题)
├── create_main_screen()        // 主屏幕: 导航按钮 + 信息显示
├── create_screen_1()           // 水质数据: 11 项条形图
├── create_screen_2()           // 设备监控: 4 设备状态卡片
├── create_screen_3()           // 遥杆: 虚拟摇杆控件
├── create_screen_4()           // 关于页
└── lv_screen_load(main_screen) // 加载主屏幕
```

### 13.2 主屏幕 (main_screen)

- **左侧 (50%)**: 4 个导航按钮 (水质数据 / 设备开关 / 遥杆 / About)
- **右侧 (50%)**: 动态信息标签 (时间 / 日期 / 天气 / 欢迎语)
- **欢迎语动画**: Y 轴 ±5px 往复平移, 1500ms 周期, 无限循环

### 13.3 水质数据页 (screen_1)

- 11 项数据条形图: TDS, COD, TOC, UV254, pH, Tem, Tur, air_temp, air_hum, pressure, altitude
- 每项含: 名称标签 (70px) + 数值标签 (80px) + 条形图 (自适应)
- 条形图使用 `item_configs` 中的 `range_min/range_max` 作为显示范围
- `update_screen1_item(name, value)` 接口按名称更新单项值

### 13.4 设备监控页 (screen_2)

- 4 张设备状态卡片 (2×2 网格)
- 在线状态指示: 橙色 `0xFFA500` = 在线, 灰色 `0x808080` = 离线
- 心跳超时检测: 30s 无心跳自动标记离线 (DeviceHeartTime)
- 与 `My_Data_New` 的设备状态列表实时同步 (每秒)

### 13.5 遥杆控制页 (screen_3)

- 虚拟遥杆控件: 触屏拖拽 + 键盘方向键双输入
- 最大偏移 34px, 死区 12px
- 状态输出: 方向 + X/Y 百分比 (-100~100)
- 遥杆激活时 `USART3_Test_Task()` 每 250ms 发送控制帧

### 13.6 样式主题

| 样式 | 用途 |
|------|------|
| `style_screen_bg` | 屏幕背景: 浅灰色 |
| `style_card` | 卡片容器: 白色, 圆角 12px, 边框 |
| `style_title` | 标题文字: 深蓝色 |
| `style_btn` | 按钮: 蓝色背景, 白色文字, 圆角 10px |
| `style_btn_pressed` | 按钮按下: 深蓝色 |

---

## 14. 中断管理

### 14.1 中断向量表 (stm32h7xx_it.c)

| 中断服务函数 | 处理内容 |
|------------|---------|
| `DMA1_Stream0_IRQHandler` | `HAL_DMA_IRQHandler(&hdma_spi1_tx)` → 触发 TxCpltCallback |
| `DMA1_Stream3_IRQHandler` | `HAL_DMA_IRQHandler(&hdma_spi1_rx)` |
| `SPI1_IRQHandler` | `HAL_SPI_IRQHandler(&hspi1)` |
| `USART1/2/3/6/10_IRQHandler` | 各自 `HAL_UART_IRQHandler` → 触发 RxCpltCallback |
| `TIM6_DAC_IRQHandler` | `HAL_TIM_IRQHandler(&htim6)` → 触发 PeriodElapsedCallback |
| `TIM7_IRQHandler` | `HAL_TIM_IRQHandler(&htim7)` |
| `TIM8_UP_TIM13_IRQHandler` | `HAL_TIM_IRQHandler(&htim13)` |
| `SysTick_Handler` | `HAL_IncTick()` |
| HardFault/MemManage/BusFault/UsageFault | while(1) 死循环 |

### 14.2 中断回调汇总 (main.c)

| 回调函数 | 触发源 | 逻辑 |
|---------|-------|------|
| `HAL_TIM_PeriodElapsedCallback` | TIM6 | `lv_tick_inc(1)`, `UNX_Now_Time++` |
| 同上 | TIM13 | 周期计数 → 数据上报/时间请求/状态上报 |
| `HAL_SPI_TxCpltCallback` | SPI1 | `LCD_CS_SET`, `spi_dma_tx_complete=1` |
| `HAL_UART_RxCpltCallback` | USART1/2/3 | 环形缓冲区写入, 重新启动单字节接收 |

---

## 15. MPU 与 Cache 配置

### 15.1 MPU 配置

```c
void MPU_Config(void) {
    HAL_MPU_Disable();
    // Region 0: 整个 4GB 地址空间
    MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress      = 0x0;
    MPU_InitStruct.Size             = MPU_REGION_SIZE_4GB;
    MPU_InitStruct.SubRegionDisable = 0x87;          // 部分子区域禁用
    MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
    MPU_InitStruct.IsCacheable      = MPU_ACCESS_NOT_CACHEABLE;
    MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
    HAL_MPU_ConfigRegion(&MPU_InitStruct);
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}
```

### 15.2 Cache 注意事项

> **关键**: STM32H7 的 Cortex-M7 内核具有独立的 I-Cache 和 D-Cache。当使用 DMA 从内存读取数据到外设时, **必须**在启动 DMA 前执行 `SCB_CleanDCache_by_Addr()` 以确保 CPU Cache 中的数据已写入主内存, 否则 DMA 可能读取到陈旧数据。

本项目中 D-Cache 清理位于 `my_flush_cb` 的每次 DMA 传输前:
```c
SCB_CleanDCache_by_Addr((uint32_t *)dma_buffer, chunk_pixel_count * 3);
HAL_SPI_Transmit_DMA(&hspi1, dma_buffer, chunk_pixel_count * 3);
```

---

## 16. 构建与烧录

### 16.1 前置环境

1. 安装 **Keil MDK-ARM** V5.32 及以上 (含 ArmClang AC6 编译器)
2. 安装 VS Code 插件 **EIDE** (Embedded IDE)
3. 打开工作区: `Vscode/LVGL.code-workspace` (如果存在)

### 16.2 VS Code 任务 (EIDE)

| 任务 | 说明 |
|------|------|
| `build` | 增量编译 |
| `rebuild` | 全量重编 |
| `flash` | 烧录到设备 |
| `build and flash` | 一键编译并烧录 |
| `clean` | 清理构建产物 |

### 16.3 Keil 工程

- 工程文件: `MDK-ARM/LVGL.uvprojx`
- 编译器: ArmClang AC6
- 优化级别: -O2 (ProjectManager.CompilerOptimize=6)

### 16.4 构建产物

- `Vscode/build/LVGL/LVGL.axf` — ELF/AXF 可执行文件
- `Vscode/build/LVGL/LVGL.hex` — HEX 烧录文件
- `Vscode/build/LVGL/LVGL.map` — 链接映射文件

---

## 17. 配置入口与关键宏定义

### 17.1 业务参数 (main.c)

```c
#define SendData_Time      3000   // TIM13 中断次数: 数据上报周期 (6s)
#define StatusReport_Time  5000   // TIM13 中断次数: 状态上报周期 (10s)
```

### 17.2 MQTT 配置 (My_Data_New.h)

```c
#define MQTT_DEVICE_ID           "device_002"   // 当前设备 ID
#define MQTT_TOPIC_UP_INDEX      0              // 数据上报主题索引
#define MQTT_TOPIC_STATUS_INDEX  1              // 状态上报主题索引
#define MQTT_TOPIC_COMMAND_INDEX 2              // 命令下发主题索引
#define MQTT_TOPIC_DOWN_INDEX    0              // 下行订阅主题索引
#define Device_Max               4              // 最大设备数
#define MAX_CONFIG_ITEMS         50             // 最大配置项数
```

### 17.3 设备管理 (My_LVGL.h)

```c
#define DeviceHeartTime    30000   // 设备心跳超时 30 秒
```

### 17.4 LCD 缓冲 (lcd.h)

```c
#define LCD_W 480
#define LCD_H 320
#define USE_HORIZONTAL 1
#define OneStepSize       (LCD_W * LCD_H / 8)    // 19200 像素/块
#define OnePointSize_Lvgl 2                       // RGB565 字节/像素
#define OnePointSize_DMA  3                       // RGB666 字节/像素
```

### 17.5 数据显示范围 (My_LVGL.c)

| 项目 | 最小值 | 最大值 | 格式 |
|------|--------|--------|------|
| TDS | 0 | 1000 | %.2f |
| COD | 0 | 100 | %.2f |
| TOC | 0 | 100 | %.2f |
| UV254 | 0 | 100 | %.2f |
| pH | 0 | 14 | %.1f |
| Tem | -40 | 85 | %.1f |
| Tur | 0 | 3000 | %.2f |
| air_temp | -40 | 85 | %.1f |
| air_hum | 0 | 100 | %.1f |
| pressure | 900 | 1100 | %.1f |
| altitude | -500 | 5000 | %.1f |

---

## 18. 已知问题与优化方向

### 18.1 已完成

- LVGL 移植完成并稳定刷新
- 调试接口已接入 (printf 重定向, 时序测量)
- 早期刷新色块残留问题已通过降低 SPI 频率规避
- 多路串口中断接收正常工作
- JSON 解析支持粘包/半包场景

### 18.2 待优化

| 问题 | 建议 |
|------|------|
| **DMA2D 加速** | STM32H723 内置 DMA2D, 可启用 LVGL 的 `LV_USE_DRAW_DMA2D` 加速填充/混合/颜色转换 |
| **触摸校准** | `my_input_read` 中坐标缩放系数 (0.08, 0.12) 需根据实机重新标定, 建议增加 EEPROM 存储校准参数 |
| **TIM6 时基精度** | 当前 TIM6 实际为 2ms (500Hz), `lv_tick_inc(1)` 理论应该 1ms。建议将 ARR 改为 `500 - 1` 或 PSC 改为 `137 - 1` |
| **串口异常处理** | 增加粘包统计、超时检测、帧错误计数、缓冲区溢出告警 |
| **Heap/Stack 优化** | 8KB Heap / 16KB Stack 可能偏小, 建议根据实际 map 文件分析后调整 |
| **AXI SRAM 放置** | `AXI_SRAM_VAR` 宏的实际效果取决于链接脚本, 建议在 `.sct` 文件中确认 `.axi_sram` 段定义 |
| **内存优化** | `dma_buffer` 57.6KB + `buf1/buf2` 76.8KB = 134.4KB RAM, 可考虑减小 OneStepSize 或使用单缓冲 |

---

## 19. 变更记录

| 日期 | 变更内容 |
|------|---------|
| 2025-08-24 | 创建项目并完成初版移植 |
| 2025-08-25 | 添加 Debug 接口 (printf 重定向, 微秒延时, 计时) |
| 2025-08-27 | 优化 my_flush_cb 计数逻辑, 规避数组越界风险 |
| 2025-08-30 | 集成 cJSON 库, 实现 JSON 解析与 MQTT 封装 |
| 2025-09-05 | 完成 4 页 UI 系统 (主屏/数据/设备/遥杆) |
| 2025-09-10 | 添加设备心跳监控与状态同步机制 |
| 2025-09-15 | 文档 v2.0 全面更新 — 覆盖 CubeMX 配置、引脚分配、时钟树、外设初始化、软件架构 |

---

## 20. 心跳上报接入指南

> 本节整合了原 `docs/heartbeat-guide.md` 的内容，作为新增外部设备接入时的唯一说明入口。

### 20.1 目标与数据流

当前系统的心跳/设备状态管理分为三层：

```
┌──────────────────────────────────────────────────────────┐
│  第3层: UI 显示 (My_LVGL.c)                              │
│  ─ DeviceStatus_t devices[4]  ← 设备在线状态 + UI 对象   │
│  ─ lv_timer: sync_device_status_from_data (1秒)          │
│  ─ lv_timer: device_check_timer_cb (超时检查)            │
├──────────────────────────────────────────────────────────┤
│  第2层: 数据层 (My_Data_New.c)                           │
│  ─ device_status_t device_list[4]  ← JSON 解析后的状态   │
│  ─ N_My_JsonGet() 解析 UART 数据 → 更新 device_list[]    │
├──────────────────────────────────────────────────────────┤
│  第1层: 物理层 (UART + 4G模块)                           │
│  ─ USART1/2/3 接收外部设备发来的 JSON 心跳包             │
│  ─ USART2 通过 4G 模块走 MQTT 协议与云端通信             │
└──────────────────────────────────────────────────────────┘
```

**心跳数据流：** 外部设备 → UART → N_My_JsonGet() → device_list[].valid=true → sync_device_status_from_data() → devices[].is_online=true, last_heartbeat=now → device_check_timer_cb() → 超时自动标记 offline。

### 20.2 需要修改的文件

| 文件 | 修改内容 |
|------|---------|
| `Core/Inc/My_Data_New.h` | 增加 `Device_Max` 宏（如需扩容）；声明新函数 |
| `Core/Inc/My_LVGL.h` | 增加设备数量/时间常量；声明新函数 |
| `Core/Src/My_Data_New.c` | 扩展 `device_list[]`；实现心跳上报函数；实现设备上线通知函数 |
| `Core/Src/My_LVGL.c` | 扩展 `devices[]` 数组；增加 UI 卡片创建；更新定时器逻辑 |
| `Core/Src/main.c` | 初始化时调用设备上线通知；主循环中处理心跳上报定时 |

### 20.3 关键常量与扩容规则

```c
#define Device_Max 4
#define DeviceHeartTime 60000
#define DEVICE_COUNT Device_Max
```

如果要增加设备数量，只需要同步修改 `Device_Max`、`DEVICE_COUNT`，并在 `My_Data_New.c` 与 `My_LVGL.c` 中扩展对应数组初始化即可。

### 20.4 上报函数建议

在 `My_Data_New.c` 中增加两类接口：

```c
void MQTT_Report_Device_Heartbeat(UART_HandleTypeDef *huart,
                                   const char *device_id,
                                   const char *status);
void UART_Report_Device_Heartbeat(UART_HandleTypeDef *huart,
                                   const char *device_id,
                                   const char *status);
```

前者通过 `huart2` 走 MQTT/4G 上报云端，后者用于本地 UART 广播。两者都建议统一发送 `{"device_id":"device_003","status":"online"}` 这类 JSON。

### 20.5 UI 与超时检查

`My_LVGL.c` 中的设备监控页建议保持“数据层同步 + 定时超时检测”的结构：

- `create_screen_2()` 通过循环创建设备卡片，数量由 `DEVICE_COUNT` 决定
- `sync_device_status_from_data()` 每秒把 `device_list[]` 同步到 UI
- `device_check_timer_cb()` 负责超时离线判断，建议检查周期短于心跳超时的一半

如果设备数量增加到 6 个以上，建议把卡片布局改成可滚动容器或调小单卡宽度。

### 20.6 接入步骤

1. 在 `My_Data_New.h` 中确认或扩展 `Device_Max`。
2. 在 `My_Data_New.c` 与 `My_LVGL.c` 中同步扩展设备数组。
3. 在 `My_Data_New.c` 中补充心跳上报函数。
4. 在 `main.c` 中确认相关 UART 都已经调用 `N_My_JsonGet()`。
5. 用 `{"device_id":"device_005","status":"online"}` 这类 JSON 验证设备是否能被识别并在 `screen_2` 上显示在线。

### 20.7 示例：添加 `device_005`

```c
#define Device_Max 5

device_status_t device_list[Device_Max] = {
    {1, "offline", false},
    {2, "offline", false},
    {3, "offline", false},
    {4, "offline", false},
    {5, "offline", false},
};

static DeviceStatus_t devices[DEVICE_COUNT] = {
    {.device_id = "device_001", .is_online = false, .last_heartbeat = 0},
    {.device_id = "device_002", .is_online = false, .last_heartbeat = 0},
    {.device_id = "device_003", .is_online = false, .last_heartbeat = 0},
    {.device_id = "device_004", .is_online = false, .last_heartbeat = 0},
    {.device_id = "device_005", .is_online = false, .last_heartbeat = 0},
};
```

---

> **技术支持**: 如需扩展其他说明，建议直接继续补充到本 README 的对应章节中，避免再拆分为独立文档。
