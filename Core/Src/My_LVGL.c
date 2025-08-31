#include "My_LVGL.h"
#include "stm32h7xx_hal.h"

void my_flush_cb(lv_display_t *display, const lv_area_t *area, uint8_t *px_map)
{
    // 1. 计算�??要传输的总像素数
    int32_t w = lv_area_get_width(area);
    int32_t h = lv_area_get_height(area);
    int32_t total_pixels_to_flush = w * h;

    // 2. 设置LCD硬件的刷新窗�??
    LCD_SetWindows(area->x1, area->y1, area->x2, area->y2);

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
        uint16_t pixel;
        uint8_t *dma_ptr = dma_buffer;

        for (int32_t i = 0; i < chunk_pixel_count; i++)
        {
            *dma_ptr++ = (*color_p >> 8) & 0xF8; // R
            *dma_ptr++ = (*color_p >> 3) & 0xFC; // G
            *dma_ptr++ = (*color_p << 3);        // B
            color_p++;
        }
        // 等待上一次DMA传输完成

        while (!spi_dma_tx_complete)
            ;

        // 清除标志位，准备开始新的DMA传输
        spi_dma_tx_complete = 0;

        // ***** 关键步骤：在启动DMA前，清理D-Cache *****
        // 确保 dma_buffer 中的新数据被写入到主内存
        SCB_CleanDCache_by_Addr((uint32_t *)dma_buffer, chunk_pixel_count * 3);

        // 启动DMA传输
        HAL_SPI_Transmit_DMA(&hspi1, dma_buffer, chunk_pixel_count * 3);

        // 更新剩余像素计数和颜色指针
        total_pixels_to_flush -= chunk_pixel_count;
        color_p += chunk_pixel_count;
    }

    // 5. 等待�??后一块数据传输完�??
    while (!spi_dma_tx_complete)
        ;

    // 6. 传输完成，拉高片�??
    LCD_CS_SET;

    // 7. **非常重要**：�?�知LVGL刷新完成
    lv_display_flush_ready(display);
}

void my_input_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    if(TP_Scan(0)) {
        data->point.y = 320-(4096-tp_dev.x)*0.08;
        data->point.x = tp_dev.y*0.12;
        // My_Usart_Send_Num((int)data->point.x);
        // My_Usart_Send_Num((int)data->point.y);
        // LCD_DrawPoint((int)data->point.x,(int)data->point.y);
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
    // My_Usart_Send_Num(TP_Read_AD(0x90));
    // HAL_Delay(10);
    // My_Usart_Send_Num(TP_Read_AD(0XD0));
    // HAL_Delay(10);
    // My_Usart_Send("------------------------\n\r");
    // HAL_Delay(500);
}

void Three_Box_Move()
{
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
    lv_anim_set_duration(&a, 1500);
    lv_anim_set_playback_duration(&a, 1500);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
    lv_anim_start(&a); /* USER CODE END 2 */
}

void lv_example_get_started_1(void)
{
    /*Change the active screen's background color*/
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x003a57), LV_PART_MAIN);

    /*Create a white label, set its text and align it to the center*/
    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello world");
    lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

/**
 * Create a style transition on a button to act like a gum when clicked
 */
void lv_example_button_3(void)
{
    /*Properties to transition*/
    static lv_style_prop_t props[] = {
        LV_STYLE_TRANSFORM_WIDTH, LV_STYLE_TRANSFORM_HEIGHT, LV_STYLE_TEXT_LETTER_SPACE, 0};

    /*Transition descriptor when going back to the default state.
     *Add some delay to be sure the press transition is visible even if the press was very short*/
    static lv_style_transition_dsc_t transition_dsc_def;
    lv_style_transition_dsc_init(&transition_dsc_def, props, lv_anim_path_overshoot, 250, 100, NULL);

    /*Transition descriptor when going to pressed state.
     *No delay, go to presses state immediately*/
    static lv_style_transition_dsc_t transition_dsc_pr;
    lv_style_transition_dsc_init(&transition_dsc_pr, props, lv_anim_path_ease_in_out, 250, 0, NULL);

    /*Add only the new transition to he default state*/
    static lv_style_t style_def;
    lv_style_init(&style_def);
    lv_style_set_transition(&style_def, &transition_dsc_def);

    /*Add the transition and some transformation to the presses state.*/
    static lv_style_t style_pr;
    lv_style_init(&style_pr);
    lv_style_set_transform_width(&style_pr, 10);
    lv_style_set_transform_height(&style_pr, -10);
    lv_style_set_text_letter_space(&style_pr, 10);
    lv_style_set_transition(&style_pr, &transition_dsc_pr);

    lv_obj_t *btn1 = lv_button_create(lv_screen_active());
    lv_obj_align(btn1, LV_ALIGN_CENTER, 40, 80);
    lv_obj_add_style(btn1, &style_pr, LV_STATE_PRESSED);
    lv_obj_add_style(btn1, &style_def, 0);

    lv_obj_t *label = lv_label_create(btn1);
    lv_label_set_text(label, "Gum");
}
void create_eight_points(void)
{
    lv_obj_t * parent = lv_screen_active();
    // 定义8个点的坐标 (x, y)
    const lv_point_t points[] = {
        {50, 40},
        {200, 40},
        {50, 120},
        {200, 120},
        {50, 200},
        {200, 200},
        {50, 280},
        {200, 280}
    };

    // 循环创建8个点
    for (int i = 0; i < 8; i++)
    {
        // 创建一个对象作为点
        lv_obj_t * point_obj = lv_obj_create(parent);
        if (point_obj == NULL) {
            // 对象创建失败处理
            return;
        }

        // 移除所有样式，让它看起来像一个点而不是一个面板
        lv_obj_remove_style_all(point_obj);

        // 设置点的大小
        lv_obj_set_size(point_obj, 15, 15);

        // 设置点的位置
        lv_obj_set_pos(point_obj, points[i].x, points[i].y);

        // 设置点的样式，使其成为一个红色的圆点
        lv_obj_set_style_radius(point_obj, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(point_obj, lv_color_hex(0xFF0000), 0);
        lv_obj_set_style_bg_opa(point_obj, LV_OPA_COVER, 0);
    }
}