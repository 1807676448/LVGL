/**
 ******************************************************************************
 * @file    w25q64.c
 * @brief   W25Q64 SPI Flash 驱动 (HAL库 + SPI2)
 * @note    基于 "11-2 硬件SPI读写W25Q64" 项目逻辑，适配 STM32H723 + HAL
 *
 *          硬件连接:
 *            PB13  --> SPI2_SCK
 *            PC1   --> SPI2_MOSI
 *            PC2   --> SPI2_MISO
 *            PB14  --> CS (软件控制)
 *
 *          SPI 配置: Mode 0 (CPOL=0, CPHA=1Edge), MSB, 8-bit, NSS=Soft
 ******************************************************************************
 */
#include "w25q64.h"

/* ========================== 超时设置 ======================================= */
#define W25Q64_SPI_TIMEOUT 5000U    /* SPI 传输超时 (ms) */
#define W25Q64_BUSY_TIMEOUT 200000U /* 忙等待超时 (循环次数) */

/* ========================== CS 宏 ========================================== */
#define W25Q64_CS_LOW() HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET)
#define W25Q64_CS_HIGH() HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET)

/* ========================== 内部辅助函数 ==================================== */

/**
 * @brief  SPI 全双工交换数据（发送并同时接收）
 * @param  txData: 发送数据缓冲区
 * @param  rxData: 接收数据缓冲区 (可与 txData 相同)
 * @param  size:   数据长度
 * @retval HAL_StatusTypeDef
 */
static HAL_StatusTypeDef W25Q64_SPI_Exchange(uint8_t *txBuf, uint8_t *rxBuf, uint16_t size)
{
    HAL_StatusTypeDef ret;
    __disable_irq();
    ret = HAL_SPI_TransmitReceive(&hspi2, txBuf, rxBuf, size, W25Q64_SPI_TIMEOUT);
    __enable_irq();
    return ret;
}

/**
 * @brief  SPI 仅发送数据（接收数据丢弃）
 * @param  txBuf: 发送数据缓冲区
 * @param  size:  数据长度
 * @retval HAL_StatusTypeDef
 */
static HAL_StatusTypeDef W25Q64_SPI_Transmit(uint8_t *txBuf, uint16_t size)
{
    HAL_StatusTypeDef ret;
    __disable_irq();
    ret = HAL_SPI_Transmit(&hspi2, txBuf, size, W25Q64_SPI_TIMEOUT);
    __enable_irq();
    return ret;
}

/* ========================== 初始化和基础操作 ================================ */

/**
 * @brief  W25Q64 初始化（确保 CS 高电平，器件未选中）
 */
void W25Q64_Init(void)
{
    /* 确保 SPI2 时钟已开启 (已在 MX_SPI2_Init 中完成) */
    /* CS 初始化为高电平，先拉低再拉高确保芯片复位 */
    W25Q64_CS_HIGH();
    HAL_Delay(10); /* 等待芯片上电稳定 */

    // 开机自检程序，调试用
    if (W25Q64_BURN_ENABLE)
    {
        uint8_t MID;
        uint16_t DID;

        /* 多次尝试读取 ID (SPI Flash 上电后可能需要稳定时间) */
        for (int retry = 0; retry < 5; retry++)
        {
            W25Q64_ReadID(&MID, &DID);
            if (MID == 0xEF && DID == 0x4017)
                break;
            HAL_Delay(10);
        }

        printf("[W25Q64] JEDEC ID: MID=0x%02X, DID=0x%04X", MID, DID);
        if (MID == 0x00 && DID == 0x0000)
            printf(" (全0 - 检查WP/HOLD是否上拉到3.3V, 或SPI接线)\r\n");
        else if (MID == 0xFF && DID == 0xFFFF)
            printf(" (全F - 检查CS/MOSI/MISO/SCK接线, 或芯片未供电)\r\n");
        else if (MID == 0xEF && DID == 0x4017)
            printf(" -> W25Q64 识别成功!\r\n");
        else
            printf(" (未知芯片!)\r\n");

        if (MID == 0xEF && DID == 0x4017)
        {
            printf("[W25Q64] 芯片识别成功!\r\n");
            printf("[W25Q64] === 烧录模式 ===\r\n");
            printf("[W25Q64] 请运行: python tools\\burn_font_to_flash.py\r\n");
        }
        else
        {
            printf("[W25Q64] 芯片识别失败! 请检查硬件连接 (WP/HOLD 上拉到3.3V?)\r\n");
        }
    }
}

/**
 * @brief  读取 JEDEC ID (制造商ID + 设备ID)
 */
void W25Q64_ReadID(uint8_t *MID, uint16_t *DID)
{
    uint8_t txBuf[4] = {W25Q64_JEDEC_ID, W25Q64_DUMMY_BYTE, W25Q64_DUMMY_BYTE, W25Q64_DUMMY_BYTE};
    uint8_t rxBuf[4] = {0};

    /* 确保 CS 起始为高 */
    W25Q64_CS_HIGH();
    /* 短暂延时确保芯片识别 CS 释放 */
    for (volatile int d = 0; d < 10; d++)
    {
        __NOP();
    }

    W25Q64_CS_LOW();
    /* 再延时确保 CS 建立时间 */
    for (volatile int d = 0; d < 10; d++)
    {
        __NOP();
    }

    W25Q64_SPI_Exchange(txBuf, rxBuf, 4);

    W25Q64_CS_HIGH();

    *MID = rxBuf[1];
    *DID = ((uint16_t)rxBuf[2] << 8) | rxBuf[3];
}

/**
 * @brief  读取8字节唯一ID
 * @param  UID: 输出8字节唯一ID缓冲区
 */
void W25Q64_ReadUniqueID(uint8_t *UID)
{
    uint8_t txBuf[13] = {0}; /* 1 cmd + 4 dummy + 8 UID = 13 */
    uint8_t rxBuf[13] = {0};

    txBuf[0] = W25Q64_READ_UNIQUE_ID;
    for (int i = 1; i < 13; i++)
        txBuf[i] = W25Q64_DUMMY_BYTE;

    W25Q64_CS_LOW();
    W25Q64_SPI_Exchange(txBuf, rxBuf, 13);
    W25Q64_CS_HIGH();

    /* UID 在 rxBuf[5] ~ rxBuf[12]，忽略前4字节 dummy */
    for (int i = 0; i < 8; i++)
    {
        UID[i] = rxBuf[5 + i];
    }
}

/* ========================== 状态寄存器 ===================================== */

/**
 * @brief  读取状态寄存器1
 * @retval 状态寄存器1的值
 */
uint8_t W25Q64_ReadStatusReg1(void)
{
    uint8_t txBuf[2] = {W25Q64_READ_STATUS_REGISTER_1, W25Q64_DUMMY_BYTE};
    uint8_t rxBuf[2] = {0};

    W25Q64_CS_LOW();
    W25Q64_SPI_Exchange(txBuf, rxBuf, 2);
    W25Q64_CS_HIGH();

    return rxBuf[1];
}

/**
 * @brief  读取状态寄存器2
 * @retval 状态寄存器2的值
 */
uint8_t W25Q64_ReadStatusReg2(void)
{
    uint8_t txBuf[2] = {W25Q64_READ_STATUS_REGISTER_2, W25Q64_DUMMY_BYTE};
    uint8_t rxBuf[2] = {0};

    W25Q64_CS_LOW();
    W25Q64_SPI_Exchange(txBuf, rxBuf, 2);
    W25Q64_CS_HIGH();

    return rxBuf[1];
}

/**
 * @brief  写状态寄存器 (Reg1 + Reg2)
 * @param  Reg1: 状态寄存器1 的值
 * @param  Reg2: 状态寄存器2 的值
 */
void W25Q64_WriteStatusReg(uint8_t Reg1, uint8_t Reg2)
{
    uint8_t txBuf[3] = {W25Q64_WRITE_STATUS_REGISTER, Reg1, Reg2};

    W25Q64_WriteEnable();
    W25Q64_CS_LOW();
    W25Q64_SPI_Transmit(txBuf, 3);
    W25Q64_CS_HIGH();
    W25Q64_WaitBusy();
}

/* ========================== 写使能/禁止 ==================================== */

/**
 * @brief  写使能 (Write Enable)
 */
void W25Q64_WriteEnable(void)
{
    uint8_t cmd = W25Q64_WRITE_ENABLE;

    W25Q64_CS_LOW();
    W25Q64_SPI_Transmit(&cmd, 1);
    W25Q64_CS_HIGH();
}

/**
 * @brief  写禁止 (Write Disable)
 */
void W25Q64_WriteDisable(void)
{
    uint8_t cmd = W25Q64_WRITE_DISABLE;

    W25Q64_CS_LOW();
    W25Q64_SPI_Transmit(&cmd, 1);
    W25Q64_CS_HIGH();
}

/* ========================== 忙等待 ========================================= */

/**
 * @brief  等待 Flash 忙标志清除 (BUSY=0)
 * @note   含超时保护，避免死等
 */
void W25Q64_WaitBusy(void)
{
    uint32_t timeout = W25Q64_BUSY_TIMEOUT;

    while ((W25Q64_ReadStatusReg1() & W25Q64_SR1_BUSY) != 0)
    {
        if (--timeout == 0)
        {
            break; /* 超时退出，避免死等 */
        }
    }
}

/* ========================== 擦除操作 ======================================= */

/**
 * @brief  扇区擦除 (4KB)
 * @param  Address: 扇区地址 (24位, 0x000000 ~ 0x7FFFFF)
 */
void W25Q64_SectorErase(uint32_t Address)
{
    uint8_t txBuf[4];

    txBuf[0] = W25Q64_SECTOR_ERASE_4KB;
    txBuf[1] = (uint8_t)(Address >> 16);
    txBuf[2] = (uint8_t)(Address >> 8);
    txBuf[3] = (uint8_t)(Address);

    W25Q64_WriteEnable();
    W25Q64_CS_LOW();
    W25Q64_SPI_Transmit(txBuf, 4);
    W25Q64_CS_HIGH();
    W25Q64_WaitBusy();
}

/**
 * @brief  64KB 块擦除
 * @param  Address: 块地址 (24位)
 */
void W25Q64_BlockErase_64K(uint32_t Address)
{
    uint8_t txBuf[4];

    txBuf[0] = W25Q64_BLOCK_ERASE_64KB;
    txBuf[1] = (uint8_t)(Address >> 16);
    txBuf[2] = (uint8_t)(Address >> 8);
    txBuf[3] = (uint8_t)(Address);

    W25Q64_WriteEnable();
    W25Q64_CS_LOW();
    W25Q64_SPI_Transmit(txBuf, 4);
    W25Q64_CS_HIGH();
    W25Q64_WaitBusy();
}

/**
 * @brief  32KB 块擦除
 * @param  Address: 块地址 (24位)
 */
void W25Q64_BlockErase_32K(uint32_t Address)
{
    uint8_t txBuf[4];

    txBuf[0] = W25Q64_BLOCK_ERASE_32KB;
    txBuf[1] = (uint8_t)(Address >> 16);
    txBuf[2] = (uint8_t)(Address >> 8);
    txBuf[3] = (uint8_t)(Address);

    W25Q64_WriteEnable();
    W25Q64_CS_LOW();
    W25Q64_SPI_Transmit(txBuf, 4);
    W25Q64_CS_HIGH();
    W25Q64_WaitBusy();
}

/**
 * @brief  全片擦除 (Chip Erase)
 * @note   耗时约 40~80 秒，请勿频繁调用
 */
void W25Q64_ChipErase(void)
{
    uint8_t cmd = W25Q64_CHIP_ERASE;

    W25Q64_WriteEnable();
    W25Q64_CS_LOW();
    W25Q64_SPI_Transmit(&cmd, 1);
    W25Q64_CS_HIGH();
    W25Q64_WaitBusy();
}

/* ========================== 编程操作 ======================================= */

/**
 * @brief  页编程 (最多 256 字节)
 * @param  Address:   目标地址 (24位)
 * @param  DataArray: 数据源缓冲区
 * @param  Count:     写入字节数 (≤ 256, 且不能跨页)
 * @note   写入前需确保目标地址已擦除 (0xFF)，否则写入结果不正确
 */
void W25Q64_PageProgram(uint32_t Address, const uint8_t *DataArray, uint16_t Count)
{
    uint8_t header[4];

    if (Count == 0 || Count > W25Q64_PAGE_SIZE)
        return;

    header[0] = W25Q64_PAGE_PROGRAM;
    header[1] = (uint8_t)(Address >> 16);
    header[2] = (uint8_t)(Address >> 8);
    header[3] = (uint8_t)(Address);

    W25Q64_WriteEnable();
    W25Q64_CS_LOW();
    /* 先发送命令 + 地址 */
    W25Q64_SPI_Transmit(header, 4);
    /* 再发送数据 */
    W25Q64_SPI_Transmit((uint8_t *)DataArray, Count);
    W25Q64_CS_HIGH();
    W25Q64_WaitBusy();
}

/**
 * @brief  擦除指定地址范围覆盖的所有扇区 (4KB)
 * @param  Address: 起始地址
 * @param  Count:   字节数
 */
void W25Q64_EraseRange(uint32_t Address, uint32_t Count)
{
    uint32_t startSector = Address / W25Q64_SECTOR_SIZE;
    uint32_t endSector = (Address + Count - 1) / W25Q64_SECTOR_SIZE;

    for (uint32_t sec = startSector; sec <= endSector; sec++)
    {
        W25Q64_SectorErase(sec * W25Q64_SECTOR_SIZE);
    }
}

/**
 * @brief  批量写入（不擦除，调用前需确保目标区域已擦除）
 * @param  Address:   目标起始地址 (24位)
 * @param  DataArray: 数据源
 * @param  Count:     写入字节数
 * @note   烧录字库时使用，INFO 帧已提前擦除整个区域
 */
void W25Q64_WriteNoErase(uint32_t Address, const uint8_t *DataArray, uint32_t Count)
{
    uint32_t remaining = Count;
    uint32_t addr = Address;
    const uint8_t *pData = DataArray;

    if (Count == 0) return;

    while (remaining > 0)
    {
        uint32_t pageOffset = addr & 0xFF;
        uint32_t spaceInPage = W25Q64_PAGE_SIZE - pageOffset;
        uint32_t chunk = (remaining < spaceInPage) ? remaining : spaceInPage;

        W25Q64_PageProgram(addr, pData, (uint16_t)chunk);

        addr += chunk;
        pData += chunk;
        remaining -= chunk;
    }
}

/* ========================== 读取操作 ======================================= */

/**
 * @brief  标准读取数据 (最大 50MHz SCLK)
 * @param  Address:   源地址 (24位)
 * @param  DataArray: 接收缓冲区
 * @param  Count:     读取字节数
 * @note   SPI 时钟 > 50MHz 时请使用 W25Q64_FastRead()
 */
void W25Q64_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count)
{
    uint8_t header[4];
    uint8_t dummy_tx[256];  /* 栈上 dummy 发送缓冲 */

    header[0] = W25Q64_READ_DATA;
    header[1] = (uint8_t)(Address >> 16);
    header[2] = (uint8_t)(Address >> 8);
    header[3] = (uint8_t)(Address);

    W25Q64_CS_LOW();
    __disable_irq();
    /* 发送命令 + 地址 (4字节) */
    HAL_SPI_Transmit(&hspi2, header, 4, W25Q64_SPI_TIMEOUT);

    /* 批量读取数据, 每次最多256字节 */
    {
        uint32_t remaining = Count;
        uint32_t offset = 0;
        while (remaining > 0) {
            uint32_t chunk = (remaining > 256) ? 256 : remaining;
            HAL_SPI_TransmitReceive(&hspi2, dummy_tx, &DataArray[offset], (uint16_t)chunk, W25Q64_SPI_TIMEOUT);
            remaining -= chunk;
            offset += chunk;
        }
    }
    __enable_irq();
    W25Q64_CS_HIGH();
}

/**
 * @brief  快速读取数据 (支持 104MHz SCLK, 含 1 字节 Dummy)
 * @param  Address:   源地址 (24位)
 * @param  DataArray: 接收缓冲区
 * @param  Count:     读取字节数
 * @note   适用 SPI 时钟 > 50MHz 的场景
 */
void W25Q64_FastRead(uint32_t Address, uint8_t *DataArray, uint32_t Count)
{
    uint8_t header[5];
    uint32_t i;

    header[0] = W25Q64_FAST_READ;
    header[1] = (uint8_t)(Address >> 16);
    header[2] = (uint8_t)(Address >> 8);
    header[3] = (uint8_t)(Address);
    header[4] = W25Q64_DUMMY_BYTE; /* Fast Read 需要一个 Dummy Byte */

    W25Q64_CS_LOW();
    /* 发送命令 + 地址 + dummy */
    W25Q64_SPI_Transmit(header, 5);
    /* 接收数据 */
    for (i = 0; i < Count; i++)
    {
        uint8_t dummy = W25Q64_DUMMY_BYTE;
        uint8_t rxByte = 0;
        W25Q64_SPI_Exchange(&dummy, &rxByte, 1);
        DataArray[i] = rxByte;
    }
    W25Q64_CS_HIGH();
}

/* ========================== 电源管理 ======================================= */

/**
 * @brief  进入掉电模式 (Power Down)
 */
void W25Q64_PowerDown(void)
{
    uint8_t cmd = W25Q64_POWER_DOWN;

    W25Q64_CS_LOW();
    W25Q64_SPI_Transmit(&cmd, 1);
    W25Q64_CS_HIGH();
}

/**
 * @brief  退出掉电模式 (Release Power Down)
 */
void W25Q64_ReleasePowerDown(void)
{
    uint8_t cmd = W25Q64_RELEASE_POWER_DOWN_HPM_DEVICE_ID;

    W25Q64_CS_LOW();
    W25Q64_SPI_Transmit(&cmd, 1);
    W25Q64_CS_HIGH();
    /* 等待 tRES1 ≥ 3μs */
    for (volatile uint32_t i = 0; i < 100; i++)
    {
        __NOP();
    }
}

/* ========================== 自检函数 ======================================= */

/**
 * @brief  W25Q64 自检
 *         1. 读取 JEDEC ID 验证芯片
 *         2. 擦除最后一个扇区 (0x7FF000)
 *         3. 写入测试数据
 *         4. 读回校验
 *         5. 擦除还原
 * @retval HAL_OK 自检通过, HAL_ERROR 自检失败
 */
HAL_StatusTypeDef W25Q64_SelfTest(void)
{
    uint8_t MID;
    uint16_t DID;
    uint8_t writeBuf[256];
    uint8_t readBuf[256];
    uint32_t testAddr = W25Q64_CAPACITY - W25Q64_SECTOR_SIZE; /* 最后一个扇区 */

    /* 1. 读 ID 验证 */
    W25Q64_ReadID(&MID, &DID);
    if (MID != 0xEF || DID != 0x4017)
    {
        return HAL_ERROR; /* ID 不匹配 */
    }

    /* 2. 擦除扇区 */
    W25Q64_SectorErase(testAddr);

    /* 3. 填充测试数据并写入 */
    for (uint32_t i = 0; i < W25Q64_PAGE_SIZE; i++)
    {
        writeBuf[i] = (uint8_t)(i & 0xFF);
    }
    W25Q64_PageProgram(testAddr, writeBuf, W25Q64_PAGE_SIZE);

    /* 4. 读回校验 */
    W25Q64_ReadData(testAddr, readBuf, W25Q64_PAGE_SIZE);
    for (uint32_t i = 0; i < W25Q64_PAGE_SIZE; i++)
    {
        if (readBuf[i] != (uint8_t)(i & 0xFF))
        {
            return HAL_ERROR; /* 数据不匹配 */
        }
    }

    /* 5. 擦除还原 */
    W25Q64_SectorErase(testAddr);

    return HAL_OK;
}
