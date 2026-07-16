/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "sys.h"
#include "..\Core\lvgl\src\lvgl.h"
#include "My_Debug.h"
#include "My_LVGL.h"
#include "My_Data_New.h"
#include "w25q64.h"
#include <stdio.h>
#include <time.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */
static void USART3_Test_Task(void);
static void My_TimerTask_Handler(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define AXI_SRAM_VAR __attribute__((section(".axi_sram"))) // buf内存位置优化,似乎没用
#define SendData_Time 3000                                 // 每30s发送一次水质数据
#define StatusReport_Time 5000                             // 每60s发送一次在线状态

AXI_SRAM_VAR static uint8_t buf1[OneStepSize * OnePointSize_Lvgl] = {1}; // 第一帧缓冲区
AXI_SRAM_VAR static uint8_t buf2[OneStepSize * OnePointSize_Lvgl] = {1}; // 第二帧缓冲区

void my_flush_cb(lv_display_t *display, const lv_area_t *area, uint8_t *px_map);

/* ===== UART 接收缓冲区：必须 32 字节对齐（STM32H7 D-Cache Line = 32B）===== */
/* 不对齐会导致 SCB_InvalidateDCache 覆盖相邻变量的 Cache Line，造成数据随机损坏 */
__attribute__((aligned(32))) volatile uint8_t uart1_rx_buf[500];
volatile int16_t uart1_ins;
__attribute__((aligned(32))) volatile uint8_t uart2_rx_buf[500];
volatile int16_t uart2_ins;
__attribute__((aligned(32))) volatile uint8_t uart3_rx_buf[500];
volatile int16_t uart3_ins;
/* USART3 RX DMA 累积接收标志：ISR 置位，主循环处理 */
static volatile bool uart3_rx_updated = false;

/* ===== 定时器触发标志（替代 ISR 中的阻塞操作）===== */
static volatile bool flag_send_water_data = false;
static volatile bool flag_request_time = false;
static volatile bool flag_report_status = false;

uint64_t UNX_Now_Time = 0; // 当前时间戳
static const char *keys[] = {"TDS", "COD", "UV254", "pH", "Tem", "Tur", "air_temp", "air_hum", "pressure", "altitude"};

/* ===== W25Q64 烧录模式 ===== */
static volatile bool g_w25q64_burn_mode = false;

/* ISR 直接处理协议, Flash 写入也在 ISR 中完成 */
static uint8_t  burn_buf[2060];   /* 数据缓冲 */
static uint16_t burn_idx;
static uint8_t  burn_st;          /* 0=cmd, 1=info, 2=data */
static uint32_t burn_addr;
static uint32_t burn_total;
static uint16_t burn_dlen;
static uint16_t burn_dcnt;
static uint32_t burn_daddr;
static uint8_t  burn_dcrc;

static void burn_tx(uint8_t b) {
    while ((USART1->ISR & USART_ISR_TXE_TXFNF) == 0);
    USART1->TDR = b;
}

/* ISR 每收到一字节调用 */
static void burn_isr_feed(uint8_t b)
{
    switch (burn_st) {
    case 0:
        if (b == 0x55) { burn_tx(0xAA); }
        else if (b == 0xA0) { burn_st = 1; burn_idx = 0; }
        else if (b == 0xA5) { burn_st = 2; burn_idx = 0; burn_buf[0] = 0xA5; }
        else if (b == 0xA1) {
            uint8_t vb[256]; W25Q64_ReadData(burn_addr, vb, 256);
            uint8_t ok = 0; for (int i=0;i<256;i++) if(vb[i]!=0xFF){ok=1;break;}
            burn_tx(ok?0x06:0x15); g_w25q64_burn_mode = false;
        }
        break;
    case 1: /* INFO: PC发 [0xA0][addr_4B][size_4B][crc_1B] */
        burn_buf[burn_idx++] = b;
        if (burn_idx >= 9) {
            burn_st = 0; burn_idx = 0;
            /* CRC = XOR of addr+size (前8字节), 与 Python 一致 */
            uint8_t crc = 0; for (int i=0;i<8;i++) crc ^= burn_buf[i];
            if (crc == burn_buf[8]) {
                burn_addr  = ((uint32_t)burn_buf[0]<<24)|((uint32_t)burn_buf[1]<<16)|((uint32_t)burn_buf[2]<<8)|burn_buf[3];
                burn_total = ((uint32_t)burn_buf[4]<<24)|((uint32_t)burn_buf[5]<<16)|((uint32_t)burn_buf[6]<<8)|burn_buf[7];
                W25Q64_EraseRange(burn_addr, burn_total);
                burn_tx(0x06);
            } else { burn_tx(0x15); }
        }
        break;
    case 2: /* DATA: PC发 [0xA5][addr_4B][len_2B][data_N][crc_1B], 0xA5 已被状态0消费 */
        /* burn_buf 布局: [0..3]=addr, [4..5]=len, [6..6+N-1]=data */
        burn_buf[burn_idx++] = b;
        if (burn_idx == 4) {
            /* 收到4字节地址 */
        } else if (burn_idx == 6) {
            /* 收到2字节长度 */
            burn_daddr = ((uint32_t)burn_buf[0]<<24)|((uint32_t)burn_buf[1]<<16)|((uint32_t)burn_buf[2]<<8)|burn_buf[3];
            burn_dlen  = ((uint16_t)burn_buf[4]<<8) | burn_buf[5];
            burn_dcnt  = 0;
        } else if (burn_idx >= 7) {
            /* 收到数据字节 (burn_idx==7 是第一个数据字节) */
            uint16_t data_idx = burn_idx - 7;
            if (data_idx < burn_dlen && data_idx < 2048) {
                burn_buf[6 + data_idx] = b;  /* 从 burn_buf[6] 开始存数据 */
            }
            /* 判断是否收完: 地址4B + 长度2B + 数据N字节 + CRC1B */
            if (burn_idx >= (uint16_t)(7 + burn_dlen)) {
                /* 收到 CRC 字节 */
                burn_st = 0; burn_idx = 0;
                /* CRC = XOR of data bytes only (与Python一致) */
                uint8_t crc = 0;
                for (uint16_t i = 0; i < burn_dlen; i++) crc ^= burn_buf[6 + i];
                if (crc == b) {
                    W25Q64_WriteNoErase(burn_daddr, &burn_buf[6], burn_dlen);
                    burn_tx(0x06);
                } else { burn_tx(0x15); }
            }
        }
        break;
    }
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_TIM6_Init();
  MX_USART1_UART_Init();
  MX_TIM7_Init();
  MX_USART2_UART_Init();
  MX_TIM13_Init();
  MX_SPI2_Init();
  MX_USART3_UART_Init();
  MX_USART6_UART_Init();
  MX_USART10_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim6); // 使能定时器驱动,提供LVGL时基
  HAL_TIM_Base_Start_IT(&htim7);
  HAL_TIM_Base_Start_IT(&htim13);

  HAL_UART_Receive_IT(&huart1, (uint8_t *)uart1_rx_buf, 1);
  HAL_UARTEx_ReceiveToIdle_DMA(&huart2, (uint8_t *)uart2_rx_buf, sizeof(uart2_rx_buf));
  HAL_UARTEx_ReceiveToIdle_DMA(&huart3, (uint8_t *)uart3_rx_buf, sizeof(uart3_rx_buf));

  // My_cJSON_Text();

  W25Q64_Init();
  {
      uint8_t MID; uint16_t DID;
      for (int retry = 0; retry < 5; retry++) {
          W25Q64_ReadID(&MID, &DID);
          if (MID == 0xEF && DID == 0x4017) break;
          HAL_Delay(10);
      }
      printf("[W25Q64] MID=0x%02X DID=0x%04X\r\n", MID, DID);
      if (MID == 0xEF && DID == 0x4017) {
#if 0  /* 1=烧录模式, 0=跳过 */
          printf("[W25Q64] 烧录模式, 请运行 burn_font_to_flash.py\r\n");
          g_w25q64_burn_mode = true;
          while (g_w25q64_burn_mode) { HAL_Delay(100); }
          printf("[W25Q64] 烧录完成!\r\n");
#endif

          /* === 验证 Flash 数据: 打印前256字节和几个字形的位图 === */
          printf("\r\n========== W25Q64 Flash Dump ==========\r\n");
          {
              uint8_t dump[256];
              W25Q64_ReadData(0x000000, dump, 256);
              printf("[Flash 0x000000~0x0000FF]:\r\n");
              for (int i = 0; i < 256; i += 16) {
                  printf("  %04X: ", i);
                  for (int j = 0; j < 16 && (i+j) < 256; j++) {
                      printf("%02X ", dump[i+j]);
                  }
                  printf("\r\n");
              }
              /* 对比: 显示开头的几个 ASCII 可识别模式 */
              printf("[Flash 前32字节 raw]: ");
              for (int i = 0; i < 32; i++) printf("%02X ", dump[i]);
              printf("\r\n");
          }
          printf("========== Flash Dump End ==========\r\n\r\n");
      }
  }

  LCD_Init(); // 初始化LCD
  TP_Init();

  MQTT_Subscribe_Downlink(&huart2);
  MQTT_Request_Time(&huart2);
  MQTT_Report_Status(&huart2, "online", 0);

  lv_init(); // 初始化LVGL

  lv_display_t *display1 = lv_display_create(480, 320);

  lv_display_set_flush_cb(display1, my_flush_cb);
  lv_display_set_buffers(display1, buf1, buf2, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);

  /* Create and set up at least one display before you register any input devices. */
  lv_indev_t *indev = lv_indev_create();           /* Create input device connected to Default Display. */
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); /* Touch pad is a pointer-like device. */
  lv_indev_set_read_cb(indev, my_input_read);      /* Set driver function. */

  setup_ui();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  static char buffer[50];
  extern config_item_t config_items[MAX_CONFIG_ITEMS];

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    lv_timer_handler();
    HAL_Delay(5);

    TimeChange();

    /* 处理定时器触发的周期性任务（替代 ISR 中的阻塞操作）*/
    My_TimerTask_Handler();

      N_My_JsonGet((char *)uart1_rx_buf, &huart1);
      N_My_JsonGet((char *)uart2_rx_buf, &huart2);

    /* USART3 累积接收：若上次 IDLE 后数据不完整，从断点继续 DMA */
    
    if (uart3_rx_updated)
    {
      uart3_rx_updated = false;
      /* 检查是否已收到完整 JSON（以 '}' 结尾） */
      bool complete = (uart3_ins > 0 && uart3_rx_buf[uart3_ins - 1] == '}');
      if (!complete && uart3_ins < (int16_t)(sizeof(uart3_rx_buf) - 10))
      {
        /* 数据不完整且还有空间：从断点继续 DMA 接收 */
        uint16_t remaining = sizeof(uart3_rx_buf) - uart3_ins;
        SCB_CleanInvalidateDCache_by_Addr((uint32_t *)&uart3_rx_buf[uart3_ins], remaining);
        HAL_UARTEx_ReceiveToIdle_DMA(&huart3, (uint8_t *)&uart3_rx_buf[uart3_ins], remaining);
      }
    }

    N_My_JsonGet((char *)uart3_rx_buf, &huart3);

    /* 遥杆遥控模式：进入/退出时向共享 USART3 总线的 Water1 发送静默/恢复指令 */
    {
      static bool was_active = false;
      bool is_active = joystick_screen_is_active();

      if (is_active && !was_active)
      {
        /* 遥控启动 → 令 Water1 停止 USART3 数据发送（重复3次防丢） */
        uint8_t mute_cmd[] = {0xDD, 0xDD, 0xDD};
        for (int i = 0; i < 3; i++)
        {
          HAL_UART_Transmit(&huart3, mute_cmd, 3, 100);
        }
        printf("[REMOTE] Water1 muted (x3)\r\n");
      }
      else if (!is_active && was_active)
      {
        /* 遥控退出 → 令 Water1 恢复 USART3 数据发送（重复3次防丢） */
        uint8_t unmute_cmd[] = {0xEE, 0xEE, 0xEE};
        for (int i = 0; i < 3; i++)
        {
          HAL_UART_Transmit(&huart3, unmute_cmd, 3, 100);
        }
        printf("[REMOTE] Water1 unmuted (x3)\r\n");
      }

      was_active = is_active;

      if (is_active)
      {
        USART3_Test_Task();
      }
    }

    // HAL_Delay(500);
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
   */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
   */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
  {
  }

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = 64;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 34;
  RCC_OscInitStruct.PLL.PLLP = 1;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 3072;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/* USART3 DMA 发送忙标志，防止重复启动 DMA */
static volatile bool usart3_tx_dma_busy = false;
/* USART3 TX DMA 发送缓冲区（32 字节对齐，确保 SCB_CleanDCache 安全）*/
__attribute__((aligned(32))) static uint8_t joystick_tx_buf[6] = {0xAAU, 0, 0, 0, 0, 0x55U};

static void USART3_Test_Task(void)
{
  static uint32_t last_tick = 0U;
  static uint8_t last_up = 0xFFU; /* 上次成功发送的值，0xFF 哨兵确保首帧必发 */
  static uint8_t last_down = 0xFFU;
  static uint8_t last_left = 0xFFU;
  static uint8_t last_right = 0xFFU;
  joystick_state_t state;
  uint8_t up = 0U;
  uint8_t down = 0U;
  uint8_t left = 0U;
  uint8_t right = 0U;

  if ((HAL_GetTick() - last_tick) < 250)
  {
    return;
  }

  last_tick = HAL_GetTick();

  /* 上一次 DMA 发送未完成则跳过本次 */
  if (usart3_tx_dma_busy)
  {
    return;
  }

  joystick_get_state(&state);

  if (state.active)
  {
    if (state.y_percent > 0)
    {
      up = (state.y_percent >= 100) ? 100U : (uint8_t)state.y_percent;
    }
    else
    {
      int16_t v = (int16_t)(-state.y_percent);
      down = (v >= 100) ? 100U : (uint8_t)v;
    }

    if (state.x_percent > 0)
    {
      right = (state.x_percent >= 100) ? 100U : (uint8_t)state.x_percent;
    }
    else
    {
      int16_t v = (int16_t)(-state.x_percent);
      left = (v >= 100) ? 100U : (uint8_t)v;
    }
  }

  /* 遥杆指令无变化则跳过发送，为共享 USART3 的其他设备让出带宽 */
  if (up == last_up && down == last_down && left == last_left && right == last_right)
  {
    return;
  }

  /* CPU 写入发送缓冲区 */
  joystick_tx_buf[0] = 0xAAU;
  joystick_tx_buf[1] = up;
  joystick_tx_buf[2] = down;
  joystick_tx_buf[3] = left;
  joystick_tx_buf[4] = right;
  joystick_tx_buf[5] = 0x55U;

  /* STM32H7 D-Cache 一致性：CPU 写入在 Cache 中，Clean 后 DMA 才能读到 */
  SCB_CleanDCache_by_Addr((uint32_t *)joystick_tx_buf, sizeof(joystick_tx_buf));

  /* 非阻塞 DMA 发送：先置忙标志再启动，防止 DMA 瞬间完成后中断清零被覆盖 */
  usart3_tx_dma_busy = true;
  if (HAL_UART_Transmit_DMA(&huart3, joystick_tx_buf, sizeof(joystick_tx_buf)) != HAL_OK)
  {
    usart3_tx_dma_busy = false; // 启动失败，回退标志
    /* DMA 启动失败不更新 last_*，下次循环会重试发送 */
  }
  else
  {
    /* 发送成功，记录本次指令值供下次比较 */
    last_up = up;
    last_down = down;
    last_left = left;
    last_right = right;
  }
}

static uint64_t times = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  // 检查是否是TIM6定时器触发的中断
  if (htim->Instance == TIM6)
  {
    lv_tick_inc(1); // 1ms触发一次,时序错误会导致lvgl卡顿
    UNX_Now_Time += 1;
  }
  if (htim->Instance == TIM13)
  {
    times++;
    /* ⚠️ 严禁在 ISR 中调用 printf / cJSON / HAL_UART_Transmit 等阻塞/非重入函数！
     *   仅设置标志位，实际工作由主循环中的 My_TimerTask_Handler() 完成。
     */
    if (times % SendData_Time == 0)
    {
      flag_send_water_data = true;
    }
    if (times % 10000 == 123)
    {
      flag_request_time = true;
    }
    if (times % StatusReport_Time == 0)
    {
      flag_report_status = true;
    }
  }
}
// SPI发送完成回调函数

/**
 * @brief 处理定时器触发的周期性任务（在主循环中调用，非 ISR 上下文）
 * @note 替代原来在 TIM13 ISR 中直接调用的 printf / cJSON / HAL_UART_Transmit
 */
static void My_TimerTask_Handler(void)
{
  if (flag_send_water_data)
  {
    flag_send_water_data = false;
    printf("Send water quality data\r\n");
    Send_JSON_KeyValue(keys, 10, &huart2);
  }
  if (flag_request_time)
  {
    flag_request_time = false;
    printf("Request time stamp\r\n\r\n");
    MQTT_Request_Time(&huart2);
  }
  if (flag_report_status)
  {
    flag_report_status = false;
    MQTT_Report_Status(&huart2, "online", (uint32_t)(HAL_GetTick() / 1000U));
  }
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  if (hspi->Instance == SPI1)
  {
    LCD_CS_SET;
    spi_dma_tx_complete = 1;
  }
}

/**
 * @brief UART DMA 发送完成回调
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART3)
  {
    usart3_tx_dma_busy = false; // 清除忙标志，允许下次发送
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    /* 烧录模式: 字节存入环形缓冲, 主循环处理 */
    if (g_w25q64_burn_mode)
    {
      burn_isr_feed((uint8_t)(uart1_rx_buf[0]));
      HAL_UART_Receive_IT(&huart1, (uint8_t *)uart1_rx_buf, 1);
      return;
    }

    /* 正常模式: 缓冲区累积 */
    if (uart1_ins >= (int16_t)(sizeof(uart1_rx_buf) - 1))
    {
      uart1_ins = -1;
    }
    uart1_ins++;
    uart1_rx_buf[uart1_ins] = '\0';
    HAL_UART_Receive_IT(&huart1, (uint8_t *)&uart1_rx_buf[uart1_ins], 1);
  }
}

/**
 * @brief USART2 DMA+IDLE 接收完成回调
 * @param huart UART 句柄
 * @param Size 接收到的字节数
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if (huart->Instance == USART2)
  {
    /* STM32H7 D-Cache 一致性：仅 Invalidate 实际接收范围（缓冲区已 32B 对齐，安全）*/
    if (Size > 0)
    {
      SCB_InvalidateDCache_by_Addr((uint32_t *)uart2_rx_buf, (int32_t)Size);
    }

    if (Size > 0 && Size < sizeof(uart2_rx_buf))
    {
      uart2_rx_buf[Size] = '\0'; // 添加字符串终止符
      uart2_ins = (int16_t)Size; // 记录接收长度
    }
    else if (Size >= sizeof(uart2_rx_buf))
    {
      // 缓冲区满，最后一个字节留给 '\0'
      uart2_rx_buf[sizeof(uart2_rx_buf) - 1] = '\0';
      uart2_ins = sizeof(uart2_rx_buf) - 1;
    }
  }
  else if (huart->Instance == USART3)
  {
    /* STM32H7 D-Cache 一致性：仅 Invalidate 实际接收范围 */
    if (Size > 0)
    {
      SCB_InvalidateDCache_by_Addr((uint32_t *)uart3_rx_buf, (int32_t)Size);
    }

    if (Size > 0)
    {
      /* 累积模式：在已有数据后追加，不覆盖 */
      int16_t current_pos = uart3_ins; /* 原子快照，防止主循环竞态 */
      int16_t new_pos = current_pos + (int16_t)Size;
      if (new_pos < (int16_t)sizeof(uart3_rx_buf))
      {
        uart3_rx_buf[new_pos] = '\0';
        /* 使用 __DSB 确保写入完成后再更新索引 */
        __DSB();
        uart3_ins = new_pos;
      }
      else
      {
        /* 缓冲区满 */
        uart3_rx_buf[sizeof(uart3_rx_buf) - 1] = '\0';
        __DSB();
        uart3_ins = (int16_t)(sizeof(uart3_rx_buf) - 1);
      }
      uart3_rx_updated = true; // 通知主循环有新数据
    }
  }
}

/* USER CODE END 4 */

/* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
   */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
    My_Usart_Send("Error occurred");
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
