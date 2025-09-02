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
    if (TP_Scan(0))
    {
        data->point.y = (tp_dev.x) * 0.08;
        data->point.x = tp_dev.y * 0.12;
        // My_Usart_Send_Num((int)data->point.x);
        // My_Usart_Send_Num((int)data->point.y);
        data->state = LV_INDEV_STATE_PRESSED;
    }
    else
    {
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
    lv_anim_set_duration(&a, 100);
    lv_anim_set_playback_duration(&a, 100);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
    lv_anim_start(&a); /* USER CODE END 2 */
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

// 全局内容区与当前页面
static lv_obj_t *g_panel_content = NULL;
static lv_obj_t *g_current_page = NULL;

// 用于事件回调的上下文（仅保存目标页索引）
typedef struct {
    uint8_t idx;      // 点击后要显示的页面索引
} nav_ctx_t;

// 各页面的创建（按需创建）
static lv_obj_t *create_page1(lv_obj_t *parent)
{
    lv_obj_t *page = lv_obj_create(parent);
    lv_obj_set_size(page, lv_obj_get_width(parent), lv_obj_get_height(parent));
    lv_obj_set_pos(page, 0, 0);
    lv_obj_set_style_pad_all(page, 12, 0);
    lv_obj_set_style_border_width(page, 0, 0);
    /* Make the page non-scrollable / non-draggable */
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_CHAIN | LV_OBJ_FLAG_SCROLL_ONE);

    lv_obj_t *label = lv_label_create(page);
    lv_label_set_text(label, "Page1");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);

    /* 两个并排的仪表盘，使用居中对齐并左右偏移，避免重叠 */
    const int ARC_SIZE = 140;
    const int ARC_X_OFFSET = 90; // 水平偏移量，确保间距

    // 温度仪表（左）
    lv_obj_t *arc_t = lv_arc_create(page);
    lv_obj_set_size(arc_t, ARC_SIZE, ARC_SIZE);
    lv_arc_set_range(arc_t, 0, 100);
    lv_arc_set_value(arc_t, 26);
    lv_obj_clear_flag(arc_t, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(arc_t, LV_ALIGN_CENTER, -ARC_X_OFFSET, -6);

    lv_obj_t *title_t = lv_label_create(page);
    lv_label_set_text(title_t, "Temperature");
    lv_obj_align_to(title_t, arc_t, LV_ALIGN_OUT_TOP_MID, 0, -6);

    lv_obj_t *val_t = lv_label_create(page);
    lv_label_set_text(val_t, "26 C");
    lv_obj_align_to(val_t, arc_t, LV_ALIGN_OUT_BOTTOM_MID, 0, 6);

    // 湿度仪表（右）
    lv_obj_t *arc_h = lv_arc_create(page);
    lv_obj_set_size(arc_h, ARC_SIZE, ARC_SIZE);
    lv_arc_set_range(arc_h, 0, 100);
    lv_arc_set_value(arc_h, 55);
    lv_obj_clear_flag(arc_h, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(arc_h, LV_ALIGN_CENTER, ARC_X_OFFSET, -6);

    lv_obj_t *title_h = lv_label_create(page);
    lv_label_set_text(title_h, "Humidity");
    lv_obj_align_to(title_h, arc_h, LV_ALIGN_OUT_TOP_MID, 0, -6);

    lv_obj_t *val_h = lv_label_create(page);
    lv_label_set_text(val_h, "55 %");
    lv_obj_align_to(val_h, arc_h, LV_ALIGN_OUT_BOTTOM_MID, 0, 6);

    return page;
}

static lv_obj_t *create_page2(lv_obj_t *parent)
{
    lv_obj_t *page = lv_obj_create(parent);
    lv_obj_set_size(page, lv_obj_get_width(parent), lv_obj_get_height(parent));
    lv_obj_set_pos(page, 0, 0);
    lv_obj_set_style_pad_all(page, 12, 0);
    lv_obj_set_style_border_width(page, 0, 0);
    /* Make the page non-scrollable / non-draggable */
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_CHAIN | LV_OBJ_FLAG_SCROLL_ONE);

    lv_obj_t *label = lv_label_create(page);
    lv_label_set_text(label, "Page2");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t *slider = lv_slider_create(page);
    lv_obj_set_width(slider, 200);
    lv_obj_align(slider, LV_ALIGN_CENTER, 0, 0);

    return page;
}

static lv_obj_t *create_page3(lv_obj_t *parent)
{
    lv_obj_t *page = lv_obj_create(parent);
    lv_obj_set_size(page, lv_obj_get_width(parent), lv_obj_get_height(parent));
    lv_obj_set_pos(page, 0, 0);
    lv_obj_set_style_pad_all(page, 12, 0);
    lv_obj_set_style_border_width(page, 0, 0);
    /* Make the page non-scrollable / non-draggable */
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_CHAIN | LV_OBJ_FLAG_SCROLL_ONE);

    lv_obj_t *label = lv_label_create(page);
    lv_label_set_text(label, "Page3");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t *sw = lv_switch_create(page);
    lv_obj_align(sw, LV_ALIGN_CENTER, 0, 0);

    return page;
}

static lv_obj_t *create_page4(lv_obj_t *parent)
{
    lv_obj_t *page = lv_obj_create(parent);
    lv_obj_set_size(page, lv_obj_get_width(parent), lv_obj_get_height(parent));
    lv_obj_set_pos(page, 0, 0);
    lv_obj_set_style_pad_all(page, 12, 0);
    lv_obj_set_style_border_width(page, 0, 0);
    /* Make the page non-scrollable / non-draggable */
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_CHAIN | LV_OBJ_FLAG_SCROLL_ONE);

    lv_obj_t *label = lv_label_create(page);
    lv_label_set_text(label, "Page4");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t *dd = lv_dropdown_create(page);
    lv_dropdown_set_options(dd, "A\nB\nC\nD");
    lv_obj_align(dd, LV_ALIGN_CENTER, 0, 0);

    return page;
}

static void switch_to_page(uint8_t idx)
{
    if(g_panel_content == NULL) return;
    if(g_current_page) {
        lv_obj_delete(g_current_page);
        g_current_page = NULL;
    }

    switch(idx) {
        case 0: g_current_page = create_page1(g_panel_content); break;
        case 1: g_current_page = create_page2(g_panel_content); break;
        case 2: g_current_page = create_page3(g_panel_content); break;
        case 3: g_current_page = create_page4(g_panel_content); break;
        default: g_current_page = create_page1(g_panel_content); break;
    }

    // 切换后刷新界面
    lv_obj_t *scr = lv_screen_active();
    lv_obj_update_layout(scr);
    lv_obj_invalidate(scr);
}

static void nav_btn_event_cb(lv_event_t *e)
{
    nav_ctx_t *ctx = (nav_ctx_t *)lv_event_get_user_data(e);
    if (ctx == NULL) return;
    switch_to_page(ctx->idx);
}

void Ji_Ben_Jie_Mian(void)
{
    // 假设显示分辨率为 480x320，创建一个根容器铺满屏幕
    lv_obj_t *root = lv_obj_create(lv_screen_active());
    lv_obj_set_size(root, 480, 320);
    lv_obj_set_pos(root, 0, 0);
    lv_obj_set_style_pad_all(root, 0, 0);
    lv_obj_set_style_border_width(root, 0, 0);

    // 左侧导航栏（固定宽度 120），右侧内容区（360x320）
    const int NAV_W = 120;
    const int SCR_W = 480;
    const int SCR_H = 320;

    // 左侧面板
    lv_obj_t *panel_nav = lv_obj_create(root);
    lv_obj_set_size(panel_nav, NAV_W, SCR_H);
    lv_obj_set_pos(panel_nav, 0, 0);
    lv_obj_set_style_pad_all(panel_nav, 8, 0);
    lv_obj_set_style_border_width(panel_nav, 0, 0);
    lv_obj_set_style_bg_color(panel_nav, lv_palette_lighten(LV_PALETTE_GREY, 2), 0);

    // 右侧内容区
    lv_obj_t *panel_content = lv_obj_create(root);
    lv_obj_set_size(panel_content, SCR_W - NAV_W, SCR_H);
    lv_obj_set_pos(panel_content, NAV_W, 0);
    lv_obj_set_style_pad_all(panel_content, 12, 0);
    lv_obj_set_style_border_width(panel_content, 0, 0);
    /* Ensure content panel is not scrollable/draggable */
    lv_obj_clear_flag(panel_content, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_CHAIN | LV_OBJ_FLAG_SCROLL_ONE);
    g_panel_content = panel_content;
    // 默认显示 Page1
    g_current_page = create_page1(panel_content);

    // 为每个按钮准备上下文，并创建按钮
    static nav_ctx_t ctxs[4];
    for (int i = 0; i < 4; i++)
    {
        ctxs[i].idx = (uint8_t)i;
    }

    // 按钮工厂：创建导航按钮并绑定事件
    int y = 8;
    const int step = 70;
    const char *names[4] = {"Page1", "Page2", "Page3", "Page4"};
    for (int i = 0; i < 4; i++)
    {
        lv_obj_t *btn = lv_button_create(panel_nav);
        lv_obj_set_size(btn, NAV_W - 24, 48);
        lv_obj_set_pos(btn, 8, y + step * i);
        lv_obj_t *lb = lv_label_create(btn);
        lv_label_set_text(lb, names[i]);
        lv_obj_center(lb);
        lv_obj_add_event_cb(btn, nav_btn_event_cb, LV_EVENT_CLICKED, &ctxs[i]);
    }
}