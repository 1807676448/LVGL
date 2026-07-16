# W25Q64 外接 SPI Flash 中文字库方案

## 概述

本文档描述如何为 STM32H723ZGT6 + LVGL 项目添加基于 W25Q64 外部 SPI Flash 的中文字库支持。

---

## 1. 硬件连接

### 1.1 完整引脚对应表

| W25Q64 引脚 | 引脚名称 | 功能 | 连接目标 | 说明 |
|------------|---------|------|---------|------|
| **1** | CS# | 片选（低有效） | STM32 PB14 (GPIO) | 软件控制 CS |
| **2** | DO (IO1) | MISO | STM32 PC2 (SPI2_MISO) | Flash → MCU |
| **3** | WP# (IO2) | 写保护（低有效） | **3.3V 或 10KΩ 上拉** | ⚠️ 必须高电平才能正常通信！ |
| **4** | GND | 电源地 | GND | |
| **5** | DI (IO0) | MOSI | STM32 PC1 (SPI2_MOSI) | MCU → Flash |
| **6** | CLK | SPI 时钟 | STM32 PB13 (SPI2_SCK) | |
| **7** | HOLD# (IO3) | 挂起/复位（低有效） | **3.3V 或 10KΩ 上拉** | ⚠️ 必须高电平才能正常通信！ |
| **8** | VCC | 电源正极 | **3.3V** | 加 0.1μF 去耦电容 |

### 1.2 ⚠️ 最常见故障原因

**JEDEC ID 全 0x00 / SPI TX timeout 的第一排查对象：**

| 引脚 | 故障现象 | 原因 |
|------|---------|------|
| **HOLD# (7)** | SPI 超时 / 全零 | 被拉低或悬空 → 芯片进入挂起状态，完全不响应 |
| **WP# (3)** | 全零（部分情况） | 被拉低 → 状态寄存器可能被锁定 |
| **VCC (8)** | 全零 | 未供电或电压 < 2.7V |
| **MOSI/MISO** | 全零 | 接反：MCU MOSI→Flash DO(2) 而非 DI(5) |

> **自查步骤**：用万用表直流电压档测量：
> 1. 引脚 8 (VCC) 对地 → 应为 3.3V ± 0.3V
> 2. 引脚 3 (WP#) 对地 → 应 ≥ 2.5V
> 3. 引脚 7 (HOLD#) 对地 → 应 ≥ 2.5V
> 4. 引脚 4 (GND) 对地 → 应为 0V（导通）

---

## 2. SPI2 当前配置 (CubeMX)

| 参数 | 值 | 说明 |
|------|-----|------|
| 模式 | Full Duplex Master | 全双工主机 |
| 数据宽度 | 8 Bit | |
| CPOL/CPHA | Low / 1 Edge | SPI Mode 0 |
| NSS | Soft | 软件控制 CS |
| 波特率分频 | 2 (91.67 Mbps) | PLL1_Q=183.33MHz / 2 |
| 时钟源 | PLL1_Q | |

> ⚠️ **CS 引脚初始状态问题**: CubeMX 自动生成的 `MX_GPIO_Init()` 将 PB14 初始化为 **低电平**（CS 有效），这会导致 SPI2 总线上电后 W25Q64 立即被选中。驱动程序初始化时会将其拉高修复。

> ⚠️ **SPI 速率与读命令**: 标准读命令 (0x03) 最大仅支持 50MHz。当前 SPI2 时钟为 91.67MHz，驱动已使用 **Fast Read (0x0B)** 命令（含 1 字节 Dummy，支持 104MHz）。若降低 SPI 速率至 50MHz 以下，可切换回标准读。写入和擦除命令不受此限制。

### SPI Mode 0 时序

```
CPOL=0, CPHA=0:
SCK 空闲为低电平，数据在第一个时钟沿（上升沿）采样，在第二个沿（下降沿）改变。
W25Q64 支持 Mode 0 和 Mode 3。
```

---

## 3. W25Q64 芯片参数

| 参数 | 值 |
|------|-----|
| 容量 | 64 Mbit = 8 MB |
| 页大小 | 256 字节 |
| 扇区大小 | 4 KB (4096 字节) |
| 块大小 | 64 KB (65536 字节) |
| 扇区数 | 2048 |
| 块数 | 128 |
| 最大 SPI 时钟 | 104 MHz (单线模式) |
| JEDEC ID | 0xEF4017 (厂商 0xEF, 内存类型 0x40, 容量 0x17) |
| 唯一 ID | 8 字节 (命令 0x4B) |

### 关键指令表

| 指令 | 代码 | 说明 |
|------|------|------|
| Write Enable | 0x06 | 写使能 |
| Write Disable | 0x04 | 写禁止 |
| Read Status Register-1 | 0x05 | 读状态寄存器1 (BUSY位=bit0) |
| Read Status Register-2 | 0x35 | 读状态寄存器2 |
| Read Data | 0x03 | 读数据 (最大 50MHz) |
| Fast Read | 0x0B | 快速读 (含 Dummy Byte) |
| Page Program | 0x02 | 页编程 (最多256字节) |
| Sector Erase (4KB) | 0x20 | 扇区擦除 |
| Block Erase (64KB) | 0xD8 | 块擦除 |
| Chip Erase | 0xC7 或 0x60 | 全片擦除 |
| Read JEDEC ID | 0x9F | 读制造商/设备 ID |
| Read Unique ID | 0x4B | 读8字节唯一 ID |
| Power Down | 0xB9 | 低功耗模式 |
| Release Power Down | 0xAB | 唤醒 |

---

## 4. Flash 存储分区规划

W25Q64 共 8MB，建议分区如下：

| 起始地址 | 大小 | 用途 |
|---------|------|------|
| 0x000000 | 128 KB | 预留 / 参数存储 |
| 0x020000 | ~6 MB | 中文字库位图数据 |
| 0x620000 | ~1.875 MB | 预留 (图片资源等) |

> 中文字库选择：推荐使用 LVGL 官方字体转换工具生成 GB2312 一级汉字（3755 字）的 16px 或 24px 抗锯齿字体，约占用 1~3 MB。

---

## 5. 实施步骤

### 第 1 步：硬件检查

1. 用万用表确认以下连接：
   - VCC ↔ 3.3V
   - GND ↔ GND
   - /WP ↔ 3.3V（通过 10KΩ 上拉）
   - /HOLD ↔ 3.3V（通过 10KΩ 上拉）
2. 确认 PB13、PC1、PC2_C、PB14 到 W25Q64 的连线无短路/断路

### 第 2 步：添加驱动文件

将以下文件加入工程：
- `Core/Inc/w25q64.h` — 驱动头文件
- `Core/Src/w25q64.c` — 驱动实现

### 第 3 步：编译烧录测试

1. 在 `main.c` 中包含 `#include "w25q64.h"`
2. 在 `main()` 初始化区（`lv_init()` 之前）调用 `W25Q64_Init()` 进行初始化
3. 调用 `W25Q64_SelfTest()` 进行全面自检
4. 观察串口输出确认芯片通信正常

### 第 4 步：中文字库集成（后续）

1. 使用 `lv_font_conv` 工具生成中文字库 C 数组
2. 编写 Flash 烧录工具将字库数据写入 W25Q64
3. 实现 LVGL 自定义字体回调函数，从 Flash 读取字形位图
4. 设置 LVGL 默认中文字体

---

## 6. 驱动 API 说明

### 初始化

```c
// 初始化 W25Q64，返回 true 表示成功
bool W25Q64_Init(void);
```

### 基本信息

```c
// 读取 JEDEC ID (3 字节)
void W25Q64_ReadJEDECID(uint8_t *id);

// 读取 8 字节唯一 ID
void W25Q64_ReadUniqueID(uint8_t *uid);

// 获取芯片容量 (字节)
uint32_t W25Q64_GetCapacity(void);
```

### 读取数据

```c
// 从指定地址读取数据
// addr: 24位地址 (0 ~ 0x7FFFFF)
// buf: 读取缓冲区
// len: 读取长度
void W25Q64_Read(uint32_t addr, uint8_t *buf, uint32_t len);
```

### 写入数据（含自动擦除管理）

```c
// 写入数据（自动处理跨页和擦除）
// 注意：写入前需确保目标区域已擦除！
void W25Q64_Write(uint32_t addr, const uint8_t *buf, uint32_t len);

// 页编程（单页内，不超过256字节，不跨页）
void W25Q64_PageProgram(uint32_t addr, const uint8_t *buf, uint16_t len);
```

### 擦除操作

```c
// 扇区擦除 (4KB)
void W25Q64_SectorErase(uint32_t addr);

// 块擦除 (64KB)
void W25Q64_BlockErase(uint32_t addr);

// 全片擦除 (耗时约 30~60 秒)
void W25Q64_ChipErase(void);
```

### 状态与自检

```c
// 读取状态寄存器1
uint8_t W25Q64_ReadStatus1(void);

// 等待芯片空闲 (BUSY 位清零)
void W25Q64_WaitBusy(void);

// 完整自检（读取ID、唯一ID、擦写读测试）
bool W25Q64_SelfTest(void);
```

---

## 7. 测试流程

运行 `W25Q64_SelfTest()` 后串口将输出：

```
=== W25Q64 Self Test Start ===
[1] JEDEC ID: EF 40 17       ← 必须匹配 EF 40 17
[2] Unique ID: XX XX XX XX XX XX XX XX
[3] Erase/Write/Read Test... ← 在最后一个扇区测试
[3] Test PASS!
=== W25Q64 Self Test PASS ===
```

### 常见故障排查

| 现象 | 可能原因 | 解决方法 |
|------|---------|---------|
| JEDEC ID 全 0x00 或 0xFF | SPI 通信失败 | 检查接线、CS 引脚电平 |
| JEDEC ID 错误 | 芯片型号不对 | 确认是否为 W25Q64 |
| 写入后读取不一致 | /WP 未接高电平 | 将 /WP 接 3.3V |
| 擦除超时 | 芯片 BUSY | 等待更长时间或降低 SPI 速率 |
| SPI2 初始化后 LCD 异常 | SPI2 干扰 SPI1 总线 | 确保 SPI2_CS 默认高电平 |

---

## 8. SPI2 速率调整建议

当前 SPI2 速率为 91.67 Mbps（PLL1_Q 2分频）。W25Q64 最大支持 104 MHz（单线），但建议：

| 场景 | 推荐分频 | 速率 | 
|------|---------|------|
| 开发调试 | /16 | ~11.5 Mbps |
| 稳定运行 | /8 | ~22.9 Mbps |
| 高性能 | /4 | ~45.8 Mbps |
| 极限性能 | /2 | ~91.7 Mbps |

修改方法：在 `Core/Src/spi.c` 的 `MX_SPI2_Init()` 中修改 `BaudRatePrescaler`。

---

## 9. 注意事项

1. **写前必须擦除**: Flash 只能将 1 写为 0，要写 1 必须先擦除（擦除后全为 0xFF）
2. **不能跨页写入**: Page Program 不能跨越 256 字节页边界
3. **CS 引脚初始电平**: CubeMX 将 PB14 初始化为低，需在驱动中立即拉高
4. **擦除耗时**: 扇区擦除约 45~400ms，块擦除约 150~1600ms，全片擦除约 30~60s
5. **D-Cache 一致性**: 如使用 DMA 读取 Flash 数据，需要注意 Cache 一致性
6. **电源滤波**: 建议在 W25Q64 的 VCC 引脚旁加 0.1μF 去耦电容

---

## 10. 参考资源

- W25Q64JV 数据手册: https://www.winbond.com/
- LVGL 字体转换工具: https://github.com/lvgl/lv_font_conv
- LVGL 外部字库实现参考: LVGL 文档 `lv_font.h` 中的 `get_glyph_bitmap_cb`

---

*文档版本: 1.0 | 最后更新: 2026-07-16*
