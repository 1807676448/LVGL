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

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define AXI_SRAM_VAR __attribute__((section(".axi_sram"))) // buf内存位置优化,似乎没用
#define SendData_Time 3000                                 // 每30s发送一次水质数据
#define StatusReport_Time 5000                            // 每60s发送一次在线状态

AXI_SRAM_VAR static uint8_t buf1[OneStepSize * OnePointSize_Lvgl] = {1}; // 第一帧缓冲区
AXI_SRAM_VAR static uint8_t buf2[OneStepSize * OnePointSize_Lvgl] = {1}; // 第二帧缓冲区

void my_flush_cb(lv_display_t *display, const lv_area_t *area, uint8_t *px_map);

uint8_t uart1_rx_buf[500]; // 接收缓冲区
int16_t uart1_ins;
uint8_t uart2_rx_buf[500]; // 接收缓冲区
int16_t uart2_ins;
uint8_t uart3_rx_buf[500]; // 接收缓冲区
int16_t uart3_ins;

uint64_t UNX_Now_Time = 0; // 当前时间戳
static const char *keys[] = {"TDS", "COD", "TOC", "UV254", "pH", "Tem", "Tur", "air_temp", "air_hum", "pressure", "altitude"};

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

  HAL_UART_Receive_IT(&huart1, uart1_rx_buf, 1);
  HAL_UART_Receive_IT(&huart2, uart2_rx_buf, 1);
  HAL_UART_Receive_IT(&huart3, uart3_rx_buf, 1);

  // My_cJSON_Text();

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

    N_My_JsonGet((char *)uart1_rx_buf, &huart1);
    N_My_JsonGet((char *)uart2_rx_buf, &huart2);
    N_My_JsonGet((char *)uart3_rx_buf, &huart3);

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
    if (times % SendData_Time == 0)
    {
      printf("Send water quality data\r\n");
      // Send_JSON_KeyValue(keys, 11, &huart1);
      Send_JSON_KeyValue(keys, 11, &huart2);
    }
    if (times % 10000 == 123)
    {
      printf("Request time stamp\r\n\r\n");
      MQTT_Request_Time(&huart2);
    }
    if (times % StatusReport_Time == 0)
    {
      MQTT_Report_Status(&huart2, "online", (uint32_t)(HAL_GetTick() / 1000U));
    }
  }
}
// SPI发送完成回调函数
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  if (hspi->Instance == SPI1)
  {
    LCD_CS_SET;
    spi_dma_tx_complete = 1;
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    if (uart1_ins > 500 - 1)
    {
      uart1_ins = 0;
    }
    uart1_ins++;
    // 重新启动接收
    HAL_UART_Receive_IT(&huart1, &uart1_rx_buf[uart1_ins], 1); // 大小为1才会仅中断
  }
  if (huart->Instance == USART2)
  {
    // USART2收到的数据同步转发一份到USART1
    // HAL_UART_Transmit(&huart1, &uart2_rx_buf[uart2_ins], 1, 10);

    if (uart2_ins > 500 - 1)
    {
      uart2_ins = 0;
    }
    uart2_ins++;
    // 重新启动接收
    HAL_UART_Receive_IT(&huart2, &uart2_rx_buf[uart2_ins], 1); // 大小为1才会仅中断
  }
  if (huart->Instance == USART3)
  {
    if (uart3_ins > 500 - 1)
    {
      uart3_ins = 0;
    }
    uart3_ins++;
    // 重新启动接收
    HAL_UART_Receive_IT(&huart3, &uart3_rx_buf[uart3_ins], 1); // 大小为1才会仅中断
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
