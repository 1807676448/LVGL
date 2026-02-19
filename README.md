# LVGL + STM32H723 工程文档

本项目是基于 **STM32H723ZGT6** 与 **LVGL v9.4.0-dev** 的嵌入式图形界面工程，当前实现了：

- 480x320 显示驱动与 LVGL 刷新链路（SPI1 + DMA）。
- 触摸输入接入（SPI2）。
- 多路 UART 数据接收与 JSON 解析。
- MQTT 指令封装（订阅下行、请求时间、状态上报、数据上报）。
- UI 页面展示与动态数据刷新。

---

## 1. 工程概览

### 1.1 硬件与软件基线

- MCU：`STM32H723ZGT6`（LQFP144）
- 框架：`STM32 HAL` + `LVGL`
- LVGL 版本：`9.4.0-dev`
- 开发方式：STM32CubeMX 生成底层，业务代码在 `Core/Src` 与 `Core/Inc`
- 编译链：`ArmClang AC6`（Keil MDK 工具链）
- VS Code 工程：通过 EIDE 任务编译/下载

### 1.2 已启用核心外设

- `SPI1`：LCD 数据通道，开启 DMA（`DMA1_Stream0/3`）
- `SPI2`：触摸相关通信
- `TIM6`：LVGL tick 时基（在中断中 `lv_tick_inc(1)`）
- `TIM13`：周期性业务任务（数据上报/在线状态/时间请求）
- `USART1/2/3/6/10`：串口通信（当前均为 115200）

---

## 2. 目录结构（整理后建议认知）

> 工程存在双层 `Vscode/` 与 `LVGL/` 目录并存，实际开发建议以 `LVGL/` 作为源码根目录。

```text
LVGL/
├─ Core/
│  ├─ Inc/                # 头文件（驱动、业务、UI接口）
│  ├─ Src/                # 源码实现
│  └─ lvgl/               # LVGL 源码与配置
├─ Drivers/               # STM32 HAL/CMSIS
├─ MDK-ARM/               # Keil 工程
├─ Vscode/                # VS Code/EIDE 配置与构建输出
├─ LVGL.ioc               # CubeMX 工程配置
└─ README.md              # 当前文档
```

### 2.1 关键源码模块

- `Core/Src/main.c`：系统启动、外设初始化、LVGL 初始化、主循环。
- `Core/Src/My_LVGL.c`：UI 构建、页面逻辑、显示刷新回调 `my_flush_cb`、触摸读取回调。
- `Core/Src/My_Data_New.c`：JSON 解析、配置项缓存、设备状态管理、MQTT 命令封装、时间同步。
- `Core/Src/lcd.c` / `Core/Src/Touch.c`：屏幕与触摸底层驱动。
- `Core/Src/usart.c` / `spi.c` / `tim.c`：外设初始化。

---

## 3. 系统运行流程

### 3.1 上电后主流程

1. 执行 `MPU_Config`、I-Cache、D-Cache、时钟初始化。
2. 初始化 GPIO/DMA/SPI/TIM/UART。
3. 启动 `TIM6/TIM7/TIM13` 中断。
4. 启动 UART 单字节中断接收（USART1/2/3）。
5. 初始化 LCD 与触摸。
6. 发送 MQTT 初始命令：订阅下行、请求时间、上报在线状态。
7. 初始化 LVGL：创建 display、绑定 `flush_cb`、注册输入设备、`setup_ui()`。
8. 进入主循环：
  - `lv_timer_handler()`
  - `TimeChange()`
  - `N_My_JsonGet()` 解析各串口缓存数据

### 3.2 定时任务逻辑（TIM13）

- 周期触发后维护 `times` 计数。
- 按周期执行：
  - 数据上报：`Send_JSON_KeyValue(...)`
  - 时间请求：`MQTT_Request_Time(...)`
  - 在线状态上报：`MQTT_Report_Status(...)`

---

## 4. 显示与触摸链路说明

### 4.1 显示刷新（LVGL -> LCD）

- LVGL 调用 `my_flush_cb` 下发局部刷新区域。
- 像素从 `RGB565` 转换后写入 DMA 缓冲。
- 使用 `HAL_SPI_Transmit_DMA` 分块发送，避免一次传输过大。
- DMA 发送前执行 D-Cache 清理，确保内存一致性。
- 发送结束调用 `lv_display_flush_ready()` 通知 LVGL。

### 4.2 触摸输入

- LVGL 输入设备回调为 `my_input_read`。
- 内部调用 `TP_Scan(0)` 获取触摸状态与坐标。
- 当前已包含坐标缩放/映射，后续如更换屏幕需重新标定系数。

---

## 5. 通信与数据协议

### 5.1 UART 角色（当前代码约定）

- `USART1`：调试输出与镜像串口。
- `USART2`：MQTT/4G 指令交互主通道。
- `USART3`：预留数据输入。

> 所有接收采用中断单字节拼包，解析函数会从缓冲区中提取第一个完整 JSON 对象。

### 5.2 JSON 解析策略

- 使用括号深度 + 字符串转义状态定位完整 JSON。
- 解析字段：
  - `device_id` + `status`：更新设备状态表。
  - `timestamp`/`NowTime`：同步本地时间戳。
  - 其他数值字段：写入配置项并刷新 UI。

### 5.3 MQTT 指令接口

- 订阅下行：`MQSUB,0,<topic>`
- 请求时间：`MQPUB,0,<topic>,{"device_id":"...","command":"time"}`
- 状态上报：`MQPUB,0,<topic>,{"device_id":"...","status":"online",...}`
- 数据上报：`Send_JSON_KeyValue` 自动按键值列表打包 `params`

---

## 6. 构建与烧录（VS Code）

### 6.1 前置环境

1. 安装 `Keil MDK`（含 `ArmClang AC6`，并确认路径可用）。
2. 安装 VS Code 插件 `EIDE`。
3. 打开工作区 `Vscode/LVGL.code-workspace`。

### 6.2 常用任务

工作区已提供以下任务：

- `build`：编译
- `flash`：下载到设备
- `build and flash`：一键编译并下载
- `rebuild`：全量重编
- `clean`：清理构建产物

### 6.3 构建产物

默认位于：

- `Vscode/build/LVGL/LVGL.axf`
- `Vscode/build/LVGL/LVGL.map*`
- `Vscode/build/LVGL/LVGL.s19`

---

## 7. 配置入口与常改项

### 7.1 业务参数

- 发送周期：`main.c` 中 `SendData_Time` / `StatusReport_Time`
- MQTT 设备与主题：`My_Data_New.h` 中 `MQTT_DEVICE_ID` 与 topic 索引

### 7.2 LVGL 相关

- LVGL 配置：`Core/lvgl/lv_conf.h`
- UI 入口：`setup_ui()`（`My_LVGL.c`）
- 刷新缓冲：`main.c` 中 `buf1/buf2` 与 `lv_display_set_buffers`

### 7.3 硬件映射

主要控制引脚（见 `main.h`）：

- LCD：`LCD_CS` / `LCD_RST` / `LCD_DC_RS` / `LCD_LED`
- Touch：`T_CS` / `T_DIN` / `T_IRQ`

---

## 8. 工程整理规范（建议执行）

为降低维护成本，建议后续按以下规范持续整理：

1. **代码分层固定化**
  - 驱动层：`lcd.c`, `Touch.c`, `spi.c`, `usart.c`
  - 协议层：`My_Data_New.c`
  - 表现层：`My_LVGL.c`
  - 启动层：`main.c`

2. **文档与代码一致性**
  - 每次修改 topic、串口用途、上报字段后同步更新本 README。

3. **清理历史冗余文件**
  - `My_Data.h` 当前为历史注释内容，建议后续归档或删除（确认无引用后再执行）。
  - 构建中间产物不纳入版本管理（建议完善 `.gitignore`）。

4. **接口命名统一**
  - 保持 `N_My_*`、`MQTT_*`、`update_*` 语义清晰，避免同义重复接口。

---

## 9. 当前状态与已知问题

### 9.1 已完成

- LVGL 移植完成并稳定刷新。
- Debug 接口已接入。
- 早期刷新色块残留问题已通过降低 SPI 频率规避。

### 9.2 待优化

- 引入 DMA2D 加速部分绘制路径。
- 触摸坐标标定参数进一步拟合实机。
- 增加串口异常包、粘包、超时场景的统计与告警。

---

## 10. 变更记录（节选）

- 2025-08-24：创建项目并完成初版移植。
- 2025-08-25：添加 Debug 接口。
- 2025-08-27：优化 `my_flush_cb` 计数逻辑，规避数组填充阶段内存溢出风险。

---

如需新增：

- 《串口协议字段手册》
- 《UI 页面与控件索引》
- 《故障定位手册（黑屏/花屏/卡顿/串口异常）》

可在本 README 基础上扩展为 `docs/` 多文档体系。