/**
 ******************************************************************************
 * @file    w25q64.h
 * @brief   W25Q64 SPI Flash 驱动头文件 (HAL库 + SPI2)
 * @note    参考 "11-2 硬件SPI读写W25Q64" 项目，适配 STM32H723 + HAL
 ******************************************************************************
 */
#ifndef __W25Q64_H
#define __W25Q64_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include <stdio.h>
/* ========================== W25Q64 指令定义 ================================ */
#define W25Q64_WRITE_ENABLE                         0x06
#define W25Q64_WRITE_DISABLE                        0x04
#define W25Q64_READ_STATUS_REGISTER_1               0x05
#define W25Q64_READ_STATUS_REGISTER_2               0x35
#define W25Q64_WRITE_STATUS_REGISTER                0x01
#define W25Q64_PAGE_PROGRAM                         0x02
#define W25Q64_QUAD_PAGE_PROGRAM                    0x32
#define W25Q64_BLOCK_ERASE_64KB                     0xD8
#define W25Q64_BLOCK_ERASE_32KB                     0x52
#define W25Q64_SECTOR_ERASE_4KB                     0x20
#define W25Q64_CHIP_ERASE                           0xC7
#define W25Q64_ERASE_SUSPEND                        0x75
#define W25Q64_ERASE_RESUME                         0x7A
#define W25Q64_POWER_DOWN                           0xB9
#define W25Q64_HIGH_PERFORMANCE_MODE                0xA3
#define W25Q64_CONTINUOUS_READ_MODE_RESET           0xFF
#define W25Q64_RELEASE_POWER_DOWN_HPM_DEVICE_ID     0xAB
#define W25Q64_MANUFACTURER_DEVICE_ID               0x90
#define W25Q64_READ_UNIQUE_ID                       0x4B
#define W25Q64_JEDEC_ID                             0x9F
#define W25Q64_READ_DATA                            0x03
#define W25Q64_FAST_READ                            0x0B
#define W25Q64_FAST_READ_DUAL_OUTPUT                0x3B
#define W25Q64_FAST_READ_DUAL_IO                    0xBB
#define W25Q64_FAST_READ_QUAD_OUTPUT                0x6B
#define W25Q64_FAST_READ_QUAD_IO                    0xEB
#define W25Q64_OCTAL_WORD_READ_QUAD_IO              0xE3

#define W25Q64_DUMMY_BYTE                           0xFF

/* ========================== W25Q64 芯片参数 ================================ */
#define W25Q64_PAGE_SIZE                            256U    /* 页大小: 256 字节 */
#define W25Q64_SECTOR_SIZE                          4096U   /* 扇区大小: 4KB */
#define W25Q64_BLOCK_SIZE                           65536U  /* 块大小: 64KB */
#define W25Q64_CAPACITY                             8388608U /* 总容量: 8MB */
#define W25Q64_SECTOR_COUNT                         2048U   /* 扇区数 */
#define W25Q64_BLOCK_COUNT                          128U    /* 块数 */

/* 状态寄存器位定义 */
#define W25Q64_SR1_BUSY                             0x01    /* 忙标志 */
#define W25Q64_SR1_WEL                              0x02    /* 写使能锁存 */
#define W25Q64_SR1_BP0                              0x04    /* 块保护位0 */
#define W25Q64_SR1_BP1                              0x08    /* 块保护位1 */
#define W25Q64_SR1_BP2                              0x10    /* 块保护位2 */
#define W25Q64_SR1_TB                               0x20    /* 顶部/底部块保护 */
#define W25Q64_SR1_SEC                              0x40    /* 扇区擦除保护 */
#define W25Q64_SR1_SRP0                             0x80    /* 状态寄存器保护位0 */

//调试标志位 1开0关
#define W25Q64_BURN_ENABLE  1


/* ========================== API 函数声明 =================================== */
void W25Q64_Init(void);
void W25Q64_ReadID(uint8_t *MID, uint16_t *DID);
void W25Q64_ReadUniqueID(uint8_t *UID);

uint8_t W25Q64_ReadStatusReg1(void);
uint8_t W25Q64_ReadStatusReg2(void);
void W25Q64_WriteStatusReg(uint8_t Reg1, uint8_t Reg2);

void W25Q64_WriteEnable(void);
void W25Q64_WriteDisable(void);
void W25Q64_WaitBusy(void);

void W25Q64_PageProgram(uint32_t Address, const uint8_t *DataArray, uint16_t Count);
void W25Q64_SectorErase(uint32_t Address);
void W25Q64_BlockErase_64K(uint32_t Address);
void W25Q64_BlockErase_32K(uint32_t Address);
void W25Q64_ChipErase(void);

/** 批量写入（自动处理跨页和扇区擦除，适合烧录字库等大数据） */
void W25Q64_Write(uint32_t Address, const uint8_t *DataArray, uint32_t Count);
/** 批量写入不擦除（调用前需确保目标区域已擦除，用于烧录协议数据块） */
void W25Q64_WriteNoErase(uint32_t Address, const uint8_t *DataArray, uint32_t Count);
/** 擦除指定地址范围的所有扇区 */
void W25Q64_EraseRange(uint32_t Address, uint32_t Count);

void W25Q64_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count);
void W25Q64_FastRead(uint32_t Address, uint8_t *DataArray, uint32_t Count);

void W25Q64_PowerDown(void);
void W25Q64_ReleasePowerDown(void);

/* 自检函数：擦除最后一个扇区 → 写入测试数据 → 读回校验 → 擦除还原 */
HAL_StatusTypeDef W25Q64_SelfTest(void);

#ifdef __cplusplus
}
#endif

#endif /* __W25Q64_H */
