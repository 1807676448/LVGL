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
#include "memorymap.h"
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

#define DMA_BUFFER_PIXEL_COUNT (LCD_W * LCD_H * 3 / 8)
#define AXI_SRAM_VAR __attribute__((section(".axi_sram"))) //buf内存位置优化,似乎没用

AXI_SRAM_VAR static uint8_t buf1[OneStepSize * OnePointSize_Lvgl] = {1}; // 第一帧缓冲区
AXI_SRAM_VAR static uint8_t buf2[OneStepSize * OnePointSize_Lvgl] = {1}; // 第二帧缓冲区

void my_flush_cb(lv_display_t *display, const lv_area_t *area, uint8_t *px_map)
{
  // 1. 计算�??要传输的总像素数
  int32_t w = lv_area_get_width(area);
  int32_t h = lv_area_get_height(area);
  int32_t total_pixels_to_flush = w * h;

  // 2. 设置LCD硬件的刷新窗�??
  LCD_SetWindows(area->x1, area->y1, area->x2, area->y2);

  //lvgl单次刷新窗口大小调试
  // My_Usart_Send_Num(area->x1);
  // My_Usart_Send_Num(area->y1);
  // My_Usart_Send_Num(area->x2);
  // My_Usrt_Send_Num(area->y2);
  // HAL_Delay(1000);

  // 3. 准备�??始传�??
  LCD_CS_CLR;
  LCD_RS_SET;

  // 将LVGL的像素缓冲区指针 (px_map) 转换�??16位颜色指�??
  uint16_t *color_p = (uint16_t *)px_map;

  // 4. 分块进行传输
  while (total_pixels_to_flush > 0)
  {
    // 计算当前块能传输多少像素
    int32_t chunk_pixel_count = total_pixels_to_flush;
    if (chunk_pixel_count > OneStepSize)
    {
      chunk_pixel_count = OneStepSize;
    }

    // **核心：进行颜色格式转�?? (�?? RGB565 -> RGB666)**
    for (int32_t i = 0; i < chunk_pixel_count; i++)
    {
      uint16_t color16 = color_p[i];
      dma_buffer[i * 3 + 0] = (color16 >> 8) & 0xF8; // R
      dma_buffer[i * 3 + 1] = (color16 >> 3) & 0xFC; // G
      dma_buffer[i * 3 + 2] = (color16 << 3);        // B
    }

    // 等待上一次DMA传输完成
    while (!spi_dma_tx_complete)
      ;

    // 清除标志位，准备�??始新的DMA传输
    spi_dma_tx_complete = 0;

    // 启动DMA传输
    HAL_SPI_Transmit_DMA(&hspi1, dma_buffer, chunk_pixel_count * 3);

    // 更新剩余像素计数和颜色指�??
    total_pixels_to_flush -= chunk_pixel_count;
    color_p += chunk_pixel_count;

  }

  // 5. 等待�??后一块数据传输完�??
  while (!spi_dma_tx_complete);

  // 6. 传输完成，拉高片�??
  LCD_CS_SET;

  // 7. **非常重要**：�?�知LVGL刷新完成
  lv_display_flush_ready(display);
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
  // MPU_Config();

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
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim6); // 使能定时器驱�??,提供LVGL时基
  LCD_Init();                    // 初始化LCD
  lv_init();                     // 初始化LVGL

  lv_display_t *display1 = lv_display_create(320, 480);

  lv_display_set_flush_cb(display1, my_flush_cb);
  lv_display_set_buffers(display1, buf1, buf2, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);

  /*--- 创建一个容器来容纳三个色块 ---*/
  // 1. 创建一个容器对象，它将作为三个色块的父对象
  lv_obj_t *cont = lv_obj_create(lv_screen_active());
  // 2. 移除容器的默认样式（如边框和内边距），使其完全透明
  lv_obj_remove_style_all(cont);
  // 3. 设置容器的大小以容纳三个 50x50 的色块
  lv_obj_set_size(cont, 50, 180);
  // 4. 设置容器的初始位置
  lv_obj_set_pos(cont, 10, 100);

  /*--- 在容器内创建三个不同颜色的色块 ---*/
  // 红色色块
  lv_obj_t *rect_red = lv_obj_create(cont);
  lv_obj_set_size(rect_red, 80, 50);
  lv_obj_set_pos(rect_red, 0, 0); // 相对于容器的位置
  lv_obj_set_style_bg_color(rect_red, lv_palette_main(LV_PALETTE_RED), 0);
  lv_obj_set_style_border_width(rect_red, 0, 0);

  // 绿色色块
  lv_obj_t *rect_green = lv_obj_create(cont);
  lv_obj_set_size(rect_green, 80, 50);
  lv_obj_set_pos(rect_green, 0, 60); // 紧挨着红色色块
  lv_obj_set_style_bg_color(rect_green, lv_palette_main(LV_PALETTE_GREEN), 0);
  lv_obj_set_style_border_width(rect_green, 0, 0);

  // 蓝色色块
  lv_obj_t *rect_blue = lv_obj_create(cont);
  lv_obj_set_size(rect_blue, 80, 50);
  lv_obj_set_pos(rect_blue, 0, 120); // 紧挨着绿色色块
  lv_obj_set_style_bg_color(rect_blue, lv_palette_main(LV_PALETTE_BLUE), 0);
  lv_obj_set_style_border_width(rect_blue, 0, 0);

  /*--- 为整个容器添加一个左右来回移动的动画 ---*/
  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_var(&a, cont); // 动画作用于容器对象
  lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
  lv_anim_set_values(&a, 10, 240); // 动画的X坐标范围
  lv_anim_set_duration(&a, 3000);
  lv_anim_set_playback_duration(&a, 3000);
  lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
  lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
  lv_anim_start(&a);  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    lv_timer_handler();
    HAL_Delay(5);
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
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  // �??查是否是TIM6定时器触发的中断
  if (htim->Instance == TIM6)
  {
    // 为LVGL提供心跳，参数是中断的周期（毫秒�??
    // 假设你的TIM6被配置为�??1ms中断�??�??
    lv_tick_inc(1);
  }
}
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  if (hspi->Instance == SPI1)
  {
    LCD_CS_SET;
    // PG_TOGGLE(7); // 等待完成时可以做点别的事，比如切换LED状�??
    spi_dma_tx_complete = 1;
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
