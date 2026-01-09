#include "My_LVGL.h"
#include "stm32h7xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <time.h> // 用于时间函数（如果需要）

/**
 * @brief LVGL显示刷新回调函数
 *
 * 此函数由LVGL调用，用于将帧缓冲区的内容刷新到实际的显示硬件（如LCD）。
 * 它处理颜色格式转换（假设从RGB565到RGB888/666）和通过DMA的分块传输。
 *
 * @param display 指向LVGL显示对象的指针
 * @param area    指向要刷新的屏幕区域的指针
 * @param px_map  指向包含像素数据的缓冲区的指针
 */
void my_flush_cb(lv_display_t *display, const lv_area_t *area, uint8_t *px_map)
{
    // 1. 计算要传输的总像素数
    int32_t w = lv_area_get_width(area);
    int32_t h = lv_area_get_height(area);
    int32_t total_pixels_to_flush = w * h;

    // 2. 设置LCD硬件的刷新窗口
    LCD_SetWindows(area->x1, area->y1, area->x2, area->y2);

    // 3. 准备开始传输
    LCD_CS_CLR; // 拉低片选信号
    LCD_RS_SET; // 设置数据/命令引脚为数据模式

    // 将LVGL的像素缓冲区指针 (px_map) 转换为16位颜色指针
    uint16_t *color_p = (uint16_t *)px_map;

    // 4. 分块进行传输，避免DMA缓冲区溢出
    while (total_pixels_to_flush > 0)
    {
        // 计算当前块能传输多少像素
        int32_t chunk_pixel_count = total_pixels_to_flush;
        if (chunk_pixel_count > OneStepSize)
        {
            chunk_pixel_count = OneStepSize; // 限制在单次传输最大值内
        }

        // **核心：进行颜色格式转换 (RGB565 -> RGB888/666)**
        // 假设 dma_buffer 是用于DMA传输的临时缓冲区
        uint16_t pixel;
        uint8_t *dma_ptr = dma_buffer; // 指向DMA缓冲区

        // 遍历当前块的所有像素，进行格式转换
        for (int32_t i = 0; i < chunk_pixel_count; i++)
        {
            // 从LVGL的RGB565格式中提取RGB分量，并扩展到8位
            *dma_ptr++ = (*color_p >> 8) & 0xF8; // 提取并扩展红色分量 (R)
            *dma_ptr++ = (*color_p >> 3) & 0xFC; // 提取并扩展绿色分量 (G)
            *dma_ptr++ = (*color_p << 3);        // 提取并扩展蓝色分量 (B)
            color_p++;                           // 移动到下一个像素
        }

        // 等待上一次DMA传输完成
        while (!spi_dma_tx_complete)
            ;

        // 清除标志位，准备开始新的DMA传输
        spi_dma_tx_complete = 0;

        // ***** 关键步骤：在启动DMA前，清理D-Cache *****
        // 确保 cpu cache 中的修改被写入到主内存，DMA才能访问到最新数据
        SCB_CleanDCache_by_Addr((uint32_t *)dma_buffer, chunk_pixel_count * 3);

        // 启动DMA传输，将转换后的数据发送到LCD
        HAL_SPI_Transmit_DMA(&hspi1, dma_buffer, chunk_pixel_count * 3);

        // 更新剩余像素计数
        total_pixels_to_flush -= chunk_pixel_count;
    }

    // 5. 等待最后一块数据传输完成
    while (!spi_dma_tx_complete)
        ;

    // 6. 传输完成，拉高片选信号，结束本次传输
    LCD_CS_SET;

    // 7. 通知LVGL刷新完成，使其可以继续渲染下一帧
    lv_display_flush_ready(display);
}

/**
 * @brief LVGL输入设备读取回调函数
 *
 * 此函数由LVGL定期调用，用于获取触摸屏等输入设备的状态。
 * 它读取触摸点坐标并转换为适合屏幕显示的坐标。
 *
 * @param indev 指向LVGL输入设备对象的指针
 * @param data  指向存储输入数据的结构体的指针
 */
void my_input_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    // 调用触摸屏扫描函数
    if (TP_Scan(0))
    {
        // 获取触摸点坐标并进行坐标变换和缩放
        // 注意：这里的变换系数 (0.08, 0.12) 需要根据实际触摸屏和LCD的尺寸及安装方向进行校准
        data->point.y = (tp_dev.x) * 0.08; // 可能需要交换x,y或调整系数
        data->point.x = tp_dev.y * 0.12;   // 可能需要交换x,y或调整系数
        data->state = LV_INDEV_STATE_PRESSED; // 设置状态为按下
    }
    else
    {
        data->state = LV_INDEV_STATE_RELEASED; // 设置状态为释放
    }
}

// 假设这些变量在其他地方声明或传递

// 声明事件处理函数原型
static void main_menu_event_handler(lv_event_t *e);    // 主菜单按钮事件处理
static void back_to_main_event_handler(lv_event_t *e); // 返回主菜单按钮事件处理

// 屏幕对象指针
static lv_obj_t *main_screen; // 主屏幕
static lv_obj_t *screen_1;    // 屏幕1
static lv_obj_t *screen_2;    // 屏幕2
static lv_obj_t *screen_3;    // 屏幕3
static lv_obj_t *screen_4;    // 屏幕4

// 指向动态信息标签的指针，用于后续更新
static lv_obj_t *time_label = NULL;    // 时间标签
static lv_obj_t *date_label = NULL;    // 日期标签
static lv_obj_t *weather_label = NULL; // 天气标签
static lv_obj_t *welcome_label = NULL; // 欢迎信息标签

// --- 动画处理函数 ---
/**
 * @brief Y轴平移动画执行回调
 *
 * 此函数被LVGL动画系统调用，用于设置对象在Y轴上的平移量。
 *
 * @param var 指向动画变量（这里是对象指针）的指针
 * @param v   当前动画值（平移的像素数）
 */
static void anim_y_handler(void *var, int32_t v)
{
    lv_obj_set_style_translate_y((lv_obj_t *)var, v, 0); // 设置Y轴平移样式
}

// --- 启动欢迎动画函数 ---
/**
 * @brief 为指定对象启动Y轴平移动画
 *
 * @param obj 要应用动画的对象（例如标签）
 */
static void start_welcome_animation(lv_obj_t *obj)
{
    if (!obj)
        return; // 检查对象是否有效

    lv_anim_t a;                                           // 定义动画结构体
    lv_anim_init(&a);                                      // 初始化动画结构体
    lv_anim_set_var(&a, obj);                              // 设置动画变量为对象本身
    lv_anim_set_values(&a, -5, 5);                         // 设置动画值范围：从-5像素到+5像素
    lv_anim_set_duration(&a, 1500);                        // 设置单程动画持续时间为1500毫秒
    lv_anim_set_playback_delay(&a, 0);                     // 设置回放前延迟为0
    lv_anim_set_playback_duration(&a, 1500);               // 设置回放持续时间（1500毫秒）
    lv_anim_set_repeat_delay(&a, 500);                     // 设置重复前延迟为500毫秒
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE); // 设置动画无限重复
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);     // 设置缓动路径（先慢后快再慢）
    lv_anim_set_exec_cb(&a, anim_y_handler);               // 设置动画执行回调函数
    lv_anim_start(&a);                                     // 启动动画
}

// --- 更新动态信息函数 ---
/**
 * @brief 更新主屏幕上的动态信息标签
 *
 * 此函数用于更新时间、日期、天气和欢迎信息标签的文本。
 * 如果传入的指针为NULL，则使用默认值。
 *
 * @param new_time        新的时间字符串
 * @param new_date        新的日期字符串
 * @param new_weather     新的天气字符串
 * @param new_welcome_msg 新的欢迎信息字符串
 */
void update_main_screen_info(const char *new_time, const char *new_date, const char *new_weather, const char *new_welcome_msg)
{
    // 更新时间标签
    if (time_label)
    {
        lv_label_set_text(time_label, new_time ? new_time : "00:00:00");
    }
    // 更新日期标签
    if (date_label)
    {
        lv_label_set_text(date_label, new_date ? new_date : "1970-01-01");
    }
    // 更新天气标签
    if (weather_label)
    {
        lv_label_set_text(weather_label, new_weather ? new_weather : "Sunny");
    }
    // 更新欢迎标签
    if (welcome_label)
    {
        lv_label_set_text(welcome_label, new_welcome_msg ? new_welcome_msg : "Welcome!");
        // 可选：如果标签文本改变后动画停止，可以重新启动动画
        // start_welcome_animation(welcome_label);
    }
}

// --- 创建主屏幕函数 ---
/**
 * @brief 创建主屏幕及其内容
 *
 * 此函数构建主屏幕的UI布局，包括左侧的导航按钮和右侧的动态信息显示区域。
 */
void create_main_screen(void)
{
    // 创建主屏幕对象
    main_screen = lv_obj_create(NULL);

    // --- 主内容容器 (水平布局) ---
    lv_obj_t *main_content_cont = lv_obj_create(main_screen);
    lv_obj_set_size(main_content_cont, LV_PCT(100), LV_PCT(100)); // 设置为全屏大小
    lv_obj_set_flex_flow(main_content_cont, LV_FLEX_FLOW_ROW);    // 设置为水平流式布局
    // 设置子元素对齐方式：主轴起始对齐，交叉轴居中对齐
    lv_obj_set_flex_align(main_content_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_border_width(main_content_cont, 0, 0); // 移除容器边框
    lv_obj_set_style_pad_all(main_content_cont, 0, 0);      // 移除容器内边距
    // 允许垂直滚动（虽然全屏不太可能溢出）
    lv_obj_set_scroll_dir(main_content_cont, LV_DIR_VER);

    // --- 左侧容器用于放置按钮 (占50%宽度) ---
    lv_obj_t *left_cont = lv_obj_create(main_content_cont);
    lv_obj_set_size(left_cont, LV_PCT(50), LV_PCT(100));  // 设置宽度为50%，高度为100%
    lv_obj_set_flex_flow(left_cont, LV_FLEX_FLOW_COLUMN); // 设置为垂直流式布局
    // 设置子元素居中对齐
    lv_obj_set_flex_align(left_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_border_width(left_cont, 0, 0); // 移除边框
    lv_obj_set_style_pad_all(left_cont, 10, 0);     // 添加内边距

    // --- 右侧容器用于显示信息 (占50%宽度) ---
    lv_obj_t *right_cont = lv_obj_create(main_content_cont);
    lv_obj_set_size(right_cont, LV_PCT(50), LV_PCT(100));  // 设置宽度为50%，高度为100%
    lv_obj_set_flex_flow(right_cont, LV_FLEX_FLOW_COLUMN); // 设置为垂直流式布局
    // 设置子元素居中对齐
    lv_obj_set_flex_align(right_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_border_width(right_cont, 0, 0); // 移除边框
    lv_obj_set_style_pad_all(right_cont, 10, 0);     // 添加内边距

    // --- 在左侧容器中创建按钮 ---
    lv_obj_t *buttons[4];                                                                   // 按钮对象数组
    const char *button_labels[] = {"转到屏幕 1", "转到屏幕 2", "转到屏幕 3", "转到屏幕 4"}; // 按钮标签

    for (int i = 0; i < 4; i++)
    {
        buttons[i] = lv_button_create(left_cont);      // 在左侧容器中创建按钮
        lv_obj_t *label = lv_label_create(buttons[i]); // 在按钮上创建标签
        lv_label_set_text(label, button_labels[i]);    // 设置标签文本
        lv_obj_center(label);                          // 将标签居中放置在按钮上
        // 为按钮添加点击事件回调，并传递屏幕ID (1-4)
        lv_obj_add_event_cb(buttons[i], main_menu_event_handler, LV_EVENT_CLICKED, (void *)(i + 1));
    }

    // --- 在右侧容器中创建信息标签 ---
    // 时间标签
    time_label = lv_label_create(right_cont);
    lv_label_set_text(time_label, "00:00:00"); // 初始文本
    // 可选：为时间标签设置字体样式
    // lv_obj_set_style_text_font(time_label, &lv_font_montserrat_20, 0);

    // 日期标签
    date_label = lv_label_create(right_cont);
    lv_label_set_text(date_label, "1970-01-01"); // 初始文本

    // 天气标签
    weather_label = lv_label_create(right_cont);
    lv_label_set_text(weather_label, "Sunny"); // 初始文本

    // 欢迎标签
    welcome_label = lv_label_create(right_cont);
    lv_label_set_text(welcome_label, "Welcome!"); // 初始文本
    // 为欢迎标签启动动画
    start_welcome_animation(welcome_label);

    // --- 示例：更新信息 (在其他地方数据变化时调用) ---
    // 模拟初始数据更新
    update_main_screen_info("14:30:00", "2023-10-27", "Partly Cloudy", "Hello, User!");
}

// 声明外部变量
// extern lv_obj_t *screen_1; // 声明屏幕1对象
extern void back_to_main_event_handler(lv_event_t *e); // 声明返回主菜单事件处理函数

static lv_obj_t *list_cont;

typedef struct
{
    const char *name;      // 名称(常量字符串)
    lv_obj_t *bar;         // 对应条形图对象
    lv_obj_t *value_label; // 显示数值的标签
} screen1_item_t;

static screen1_item_t screen1_items[7]; // 当前有7个项目
static uint8_t screen1_item_count = 0;

/**
 * @brief 根据名称更新屏幕1对应项目的数值
 * @param name  项目名称(如 "TDS","COD"...)
 * @param value 新值(0~100，可自行扩展范围)
 * @return 0 成功; 1 未找到; 2 对象未创建; 3 值超范围
 * @note 需在 LVGL 线程/上下文中调用(不要直接在中断里调用)。若在中断，可用 lv_async_call 封装。
 */
int update_screen1_item(const char *name, int value)
{
    if (!screen_1)
        return 2;
    if (value < 0 || value > 100)
        return 3;

    if(name && strcmp(name, "ZheXian1") == 0){
        add_data1_to_chart_screen_3(value);
    }
    else if(name && strcmp(name, "ZheXian2") == 0){
        add_data2_to_chart_screen_3(value);
    }

    for (uint8_t i = 0; i < screen1_item_count; i++)
    {
        if (screen1_items[i].name && strcmp(screen1_items[i].name, name) == 0)
        {
            if (screen1_items[i].bar)
            {
                lv_bar_set_value(screen1_items[i].bar, value, LV_ANIM_OFF);
            }
            if (screen1_items[i].value_label)
            {
                static char buf[12];
                snprintf(buf, sizeof(buf), "%d", value);
                lv_label_set_text(screen1_items[i].value_label, buf);
            }
            return 0;
        }
    }
    return 1;
}

/**
 * @brief 创建屏幕1及其内容
 *
 * 此函数构建屏幕1的UI，展示数据可视化示例（使用条形图）。
 */
void create_screen_1(void)
{
    // --- 基本屏幕设置 ---
    screen_1 = lv_obj_create(NULL); // 创建屏幕1对象

    // 添加标题标签
    lv_obj_t *title = lv_label_create(screen_1);
    lv_label_set_text(title, "数据可视化屏幕");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10); // 将标题对齐到顶部中央

    // 添加返回按钮
    lv_obj_t *back_btn = lv_button_create(screen_1);
    lv_obj_align(back_btn, LV_ALIGN_BOTTOM_MID, 0, -10); // 将按钮对齐到底部中央
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "返回主菜单");
    lv_obj_center(back_label);                                                         // 将标签居中放置在按钮上
    lv_obj_add_event_cb(back_btn, back_to_main_event_handler, LV_EVENT_CLICKED, NULL); // 添加点击事件回调

    // --- 数据可视化内容 ---
    // 示例数据数组 (值在0到100之间，用于条形图显示)
    int data_values[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    // 为数据项列表创建一个容器
    list_cont = lv_obj_create(screen_1);
    // 设置容器大小，宽度为屏幕90%，高度为屏幕高度减去80像素
    lv_obj_set_size(list_cont, LV_PCT(90), LV_VER_RES - 80);
    // 将容器放置在标题下方
    lv_obj_align_to(list_cont, title, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_set_flex_flow(list_cont, LV_FLEX_FLOW_COLUMN); // 设置为垂直流式布局
    // 设置子元素对齐方式
    lv_obj_set_flex_align(list_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(list_cont, LV_SCROLLBAR_MODE_AUTO); // 设置滚动条模式为自动

    // --- 条形图样式 ---
    // 条形图背景/主体部分的样式
    static lv_style_t bar_bg_style;
    lv_style_init(&bar_bg_style);                                     // 初始化样式
    lv_style_set_radius(&bar_bg_style, 4);                            // 设置圆角半径
    lv_style_set_bg_color(&bar_bg_style, lv_color_hex(0xDDDDDD));     // 设置背景色为浅灰色
    lv_style_set_bg_opa(&bar_bg_style, LV_OPA_COVER);                 // 设置背景不透明度为完全不透明
    lv_style_set_border_color(&bar_bg_style, lv_color_hex(0xAAAAAA)); // 设置边框颜色为灰色
    lv_style_set_border_width(&bar_bg_style, 1);                      // 设置边框宽度
    lv_style_set_pad_all(&bar_bg_style, 2);                           // 设置内部填充

    // 条形图指示器部分（填充部分）的样式
    static lv_style_t bar_indic_style;
    lv_style_init(&bar_indic_style);          // 初始化样式
    lv_style_set_radius(&bar_indic_style, 3); // 设置稍小的圆角半径
    // 可以更改此颜色以获得更高的对比度或代表不同的含义
    lv_style_set_bg_color(&bar_indic_style, lv_color_hex(0x007BFF)); // 设置填充色为鲜艳蓝色
    // lv_style_set_bg_color(&bar_indic_style, lv_color_hex(0x28A745)); // 或鲜艳绿色
    // lv_style_set_bg_color(&bar_indic_style, lv_color_hex(0xDC3545)); // 或鲜艳红色
    lv_style_set_bg_opa(&bar_indic_style, LV_OPA_COVER); // 设置填充不透明度为完全不透明

    const char *names[] = {"TDS", "COD", "TOC", "UV254", "pH", "Tem", "Hum"};
    screen1_item_count = 0;

    // 创建10个数据项
    for (int i = 0; i < 7; i++)
    {
        lv_obj_t *item_cont = lv_obj_create(list_cont);
        lv_obj_set_size(item_cont, LV_PCT(100), 40);
        lv_obj_set_flex_flow(item_cont, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(item_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_clear_flag(item_cont, LV_OBJ_FLAG_SCROLLABLE);

        // 名称标签
        lv_obj_t *label = lv_label_create(item_cont);
        lv_label_set_text(label, names[i]);
        lv_obj_set_width(label, 60);

        // 数值标签
        char value_text[8];
        snprintf(value_text, sizeof(value_text), "%d", data_values[i]);
        lv_obj_t *value_label = lv_label_create(item_cont);
        lv_label_set_text(value_label, value_text);
        lv_obj_set_width(value_label, 40);

        // 条形图
        lv_obj_t *bar = lv_bar_create(item_cont);
        lv_bar_set_range(bar, 0, 100);
        lv_bar_set_value(bar, data_values[i], LV_ANIM_OFF);
        lv_obj_set_flex_grow(bar, 1);
        lv_obj_add_style(bar, &bar_bg_style, LV_PART_MAIN);
        lv_obj_add_style(bar, &bar_indic_style, LV_PART_INDICATOR);
        lv_obj_set_height(bar, 20);

        // 保存映射
        screen1_items[i].name        = names[i];
        screen1_items[i].bar         = bar;
        screen1_items[i].value_label = value_label;
        screen1_item_count++;

        // 取消交互
        lv_obj_remove_event_cb(bar, NULL);
    }
}

// 声明外部变量
extern lv_obj_t *screen_2;                             // 声明屏幕2对象
extern void back_to_main_event_handler(lv_event_t *e); // 声明返回主菜单事件处理函数

// --- 事件处理函数原型声明 ---
static void button_1_event_handler(lv_event_t *e); // 按钮1事件处理
static void button_2_event_handler(lv_event_t *e); // 按钮2事件处理
static void button_3_event_handler(lv_event_t *e); // 按钮3事件处理
static void button_4_event_handler(lv_event_t *e); // 按钮4事件处理

static void switch_1_event_handler(lv_event_t *e); // 开关1事件处理
static void switch_2_event_handler(lv_event_t *e); // 开关2事件处理
static void switch_3_event_handler(lv_event_t *e); // 开关3事件处理
static void switch_4_event_handler(lv_event_t *e); // 开关4事件处理

// --- 事件处理函数定义 (空函数体) ---
/**
 * @brief 按钮1点击事件处理函数
 *
 * @param e 指向事件数据的指针
 */
static void button_1_event_handler(lv_event_t *e)
{
    // 在此处添加按钮1的事件处理逻辑
    // LV_UNUSED(e); // 如果'e'未被使用，可以取消注释以抑制编译器警告
    printf(" 按钮 1  被点击\n\r");
}

/**
 * @brief 按钮2点击事件处理函数
 *
 * @param e 指向事件数据的指针
 */
static void button_2_event_handler(lv_event_t *e)
{
    // 在此处添加按钮2的事件处理逻辑
}

/**
 * @brief 按钮3点击事件处理函数
 *
 * @param e 指向事件数据的指针
 */
static void button_3_event_handler(lv_event_t *e)
{
    // 在此处添加按钮3的事件处理逻辑
}

/**
 * @brief 按钮4点击事件处理函数
 *
 * @param e 指向事件数据的指针
 */
static void button_4_event_handler(lv_event_t *e)
{
    // 在此处添加按钮4的事件处理逻辑
}

/**
 * @brief 开关1值改变事件处理函数
 *
 * @param e 指向事件数据的指针
 */
static void switch_1_event_handler(lv_event_t *e)
{
    // 在此处添加开关1的事件处理逻辑
    // 可以使用以下代码获取开关状态：
    // lv_obj_t * sw = lv_event_get_target_obj(e);
    // bool is_on = lv_obj_has_state(sw, LV_STATE_CHECKED);
    // printf("开关 1 切换: %s\n", is_on ? "ON" : "OFF");
}

/**
 * @brief 开关2值改变事件处理函数
 *
 * @param e 指向事件数据的指针
 */
static void switch_2_event_handler(lv_event_t *e)
{
    // 在此处添加开关2的事件处理逻辑
}

/**
 * @brief 开关3值改变事件处理函数
 *
 * @param e 指向事件数据的指针
 */
static void switch_3_event_handler(lv_event_t *e)
{
    // 在此处添加开关3的事件处理逻辑
}

/**
 * @brief 开关4值改变事件处理函数
 *
 * @param e 指向事件数据的指针
 */
static void switch_4_event_handler(lv_event_t *e)
{
    // 在此处添加开关4的事件处理逻辑
}

// --- 创建屏幕2函数 ---
/**
 * @brief 创建屏幕2及其内容
 *
 * 此函数构建屏幕2的UI，展示按钮和开关控件。
 */
void create_screen_2(void)
{
    screen_2 = lv_obj_create(NULL); // 创建屏幕2对象

    // 添加标题标签
    lv_obj_t *title = lv_label_create(screen_2);
    lv_label_set_text(title, "按钮与开关");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5); // 将标题放置在顶部中央

    // 添加返回按钮
    lv_obj_t *back_btn = lv_button_create(screen_2);
    lv_obj_align(back_btn, LV_ALIGN_BOTTOM_MID, 0, -10); // 将按钮放置在底部中央
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "返回主菜单");
    lv_obj_center(back_label);                                                         // 将标签居中放置在按钮上
    lv_obj_add_event_cb(back_btn, back_to_main_event_handler, LV_EVENT_CLICKED, NULL); // 添加点击事件回调

    // --- 主内容容器 ---
    // 此容器包含左侧（按钮）和右侧（开关）两个部分
    lv_obj_t *main_cont = lv_obj_create(screen_2);
    lv_obj_set_scroll_dir(main_cont, LV_DIR_VER); // 设置垂直滚动方向
    // 设置容器大小：宽度为屏幕90%，高度为屏幕高度减去80像素
    lv_obj_set_size(main_cont, LV_PCT(90), LV_VER_RES - 80);
    // 将容器放置在标题下方
    lv_obj_align_to(main_cont, title, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    // 设置为水平流式布局，用于左右分区
    lv_obj_set_flex_flow(main_cont, LV_FLEX_FLOW_ROW);
    // 设置子元素对齐方式
    lv_obj_set_flex_align(main_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    // 可选：移除主容器的默认边框和内边距
    lv_obj_set_style_border_width(main_cont, 0, 0);
    lv_obj_set_style_pad_all(main_cont, 0, 0);

    // --- 左侧容器用于放置按钮 ---
    lv_obj_t *left_cont = lv_obj_create(main_cont);
    lv_obj_set_size(left_cont, LV_PCT(50), LV_PCT(100)); // 设置宽度为50%，高度为100%
    // 设置为垂直流式布局，用于按钮排列
    lv_obj_set_flex_flow(left_cont, LV_FLEX_FLOW_COLUMN);
    // 设置子元素对齐方式
    lv_obj_set_flex_align(left_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    // 可选：移除左侧容器的默认边框和内边距
    lv_obj_set_style_border_width(left_cont, 0, 0);
    lv_obj_set_style_pad_all(left_cont, 0, 0);

    // --- 右侧容器用于放置开关 ---
    lv_obj_t *right_cont = lv_obj_create(main_cont);
    lv_obj_set_size(right_cont, LV_PCT(50), LV_PCT(100)); // 设置宽度为50%，高度为100%
    // 设置为垂直流式布局，用于开关排列
    lv_obj_set_flex_flow(right_cont, LV_FLEX_FLOW_COLUMN);
    // 设置子元素对齐方式
    lv_obj_set_flex_align(right_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    // 可选：移除右侧容器的默认边框和内边距
    lv_obj_set_style_border_width(right_cont, 0, 0);
    lv_obj_set_style_pad_all(right_cont, 0, 0);

    // --- 在左侧容器中填充按钮 ---
    // 使用数组便于管理和未来样式设置
    lv_obj_t *buttons[4];
    for (int i = 0; i < 4; i++)
    {
        buttons[i] = lv_button_create(left_cont);      // 在左侧容器中创建按钮
        lv_obj_t *label = lv_label_create(buttons[i]); // 在按钮上创建标签
        char btn_text[20];
        snprintf(btn_text, sizeof(btn_text), "按钮 %d", i + 1); // 格式化按钮文本
        lv_label_set_text(label, btn_text);                     // 设置标签文本
        lv_obj_center(label);                                   // 将标签居中放置在按钮上
    }
    // 为按钮添加事件回调
    lv_obj_add_event_cb(buttons[0], button_1_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(buttons[1], button_2_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(buttons[2], button_3_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(buttons[3], button_4_event_handler, LV_EVENT_CLICKED, NULL);

    // --- 在右侧容器中填充开关和标签 ---
    // 数组用于保存开关指针，以便添加事件回调
    lv_obj_t *switches[4];
    // 开关标签文本数组（每个标签4个字符）
    const char *switch_labels[] = {"开关1", "开关2", "开关3", "开关4"};

    for (int i = 0; i < 4; i++)
    {
        // 为每个标签+开关对创建一个容器
        lv_obj_t *switch_item_cont = lv_obj_create(right_cont);
        // 设置容器大小：宽度为100%（相对于right_cont），高度自适应内容
        lv_obj_set_size(switch_item_cont, LV_PCT(100), LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(switch_item_cont, LV_FLEX_FLOW_ROW); // 设置为水平流式布局
        // 设置子元素对齐方式：起始对齐，垂直居中
        lv_obj_set_flex_align(switch_item_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        // 可选：移除边框
        lv_obj_set_style_border_width(switch_item_cont, 0, 0);
        // 可选：添加内边距
        lv_obj_set_style_pad_all(switch_item_cont, 2, 0);

        // 在项容器中创建标签
        lv_obj_t *label = lv_label_create(switch_item_cont);
        lv_label_set_text(label, switch_labels[i]); // 从数组中设置文本
        lv_obj_set_width(label, 60);                // 设置标签固定宽度（假设4个中文字符）

        // 在项容器中创建开关
        switches[i] = lv_switch_create(switch_item_cont);
        // 可选：在标签和开关之间添加一些间距
        // lv_obj_set_style_margin_left(switches[i], 5, 0);
    }
    // 为开关添加值改变事件回调
    lv_obj_add_event_cb(switches[0], switch_1_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(switches[1], switch_2_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(switches[2], switch_3_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(switches[3], switch_4_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
}

// 创建屏幕3的函数
// --- 全局/静态变量 ---
static lv_obj_t *screen_3 = NULL; // 屏幕3对象指针
static lv_obj_t *chart = NULL;    // 图表对象指针

static lv_chart_series_t *ser1 = NULL; // 图表系列1指针
static lv_chart_series_t *ser2 = NULL; // 图表系列2指针

static lv_timer_t *data_timer = NULL; // 用于更新图表数据的定时器指针

// --- 前向声明 ---
static void back_to_main_event_handler(lv_event_t *e);      // 返回主菜单事件处理
static void feed_chart_with_random_data(lv_timer_t *timer); // 定时器回调函数，用于生成随机数据

// --- 接口函数：向屏幕3的图表添加数据 ---
/**
 * @brief 向屏幕3上的图表添加新的数据点
 *
 * @param new_point1 系列1的新整数值
 * @param new_point2 系列2的新整数值
 */
void add_data1_to_chart_screen_3(int32_t new_point1)
{
    lv_chart_set_next_value(chart, ser1, new_point1); // 为系列1添加数据点
}
void add_data2_to_chart_screen_3(int32_t new_point2)
{
    lv_chart_set_next_value(chart, ser2, new_point2); // 为系列2添加数据点
}


// --- 创建屏幕3函数 ---
/**
 * @brief 创建屏幕3及其内容
 *
 * 此函数构建屏幕3的UI，展示一个动态更新的折线图。
 */
//示例代码{"ZheXian1":40,"ZheXian2":60}
void create_screen_3(void)
{
    // 初始化随机数种子

    screen_3 = lv_obj_create(NULL); // 创建屏幕3对象
    // lv_obj_set_size(screen_3, 480, 320); // 可选：显式设置屏幕大小（如果需要）

    // --- 标题标签 ---
    lv_obj_t *title = lv_label_create(screen_3);
    lv_label_set_text(title, "动态折线图");
    lv_obj_set_style_text_font(title, &lv_font_weiruan_16, 0); // 设置稍大的字体
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);              // 将标题对齐到顶部中央

    // --- 返回按钮 ---
    lv_obj_t *back_btn = lv_button_create(screen_3);
    lv_obj_set_width(back_btn, 180);                     // 设置按钮合理宽度
    lv_obj_align(back_btn, LV_ALIGN_BOTTOM_MID, 0, -10); // 将按钮对齐到底部中央
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "返回主菜单");
    lv_obj_center(back_label);                                                         // 将标签居中放置在按钮上
    lv_obj_add_event_cb(back_btn, back_to_main_event_handler, LV_EVENT_CLICKED, NULL); // 添加点击事件回调

    // --- 动态折线图 ---
    chart = lv_chart_create(screen_3);
    lv_obj_set_size(chart, 440, 220);             // 设置图表大小（根据需要调整）
    lv_obj_align(chart, LV_ALIGN_TOP_MID, 0, 40); // 将图表放置在标题下方

    // 配置图表
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE); // 设置图表类型为折线图
    lv_chart_set_point_count(chart, 50);          // 显示最多50个数据点

    // 添加Y轴并设置范围
    // lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 6, 2, true, 40); // Y轴刻度
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 100);   // 设置Y轴范围为0-100
    lv_chart_set_range(chart, LV_CHART_AXIS_SECONDARY_Y, 0, 100); // 设置Y轴范围为0-100

    // 添加X轴刻度 (可选，可以设置得不那么频繁)
    // lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 10, 5, 10, 1, true, 30);

    // 为图表创建数据系列
    ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y); // 添加绿色系列到主Y轴
    ser2 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_SECONDARY_Y); // 添加红色系列到次Y轴
    // 可选：设置线条宽度和点大小
    lv_obj_set_style_size(chart, 0, 0, LV_PART_INDICATOR); // 隐藏默认点大小
    lv_obj_set_style_line_width(chart, 3, LV_PART_ITEMS);  // 设置线条宽度

    // --- 示例：添加一些初始的虚拟数据 ---
    // 这是可选的，否则图表最初将是空的
    // for (int i = 0; i < 10; i++) {
    //     add_data_to_chart_screen_3(rand() % 101); // 最初添加10个随机点
    // }

    // --- 启动定时器以馈送随机数据 ---
    // 创建一个定时器，每300毫秒馈送一次随机数据
    // data_timer = lv_timer_create(feed_chart_with_random_data, 300, NULL);
    // if (!data_timer)
    // {
    //     // 如果需要，处理定时器创建错误
    //     LV_LOG_WARN("无法为屏幕3图表创建数据定时器。");
    // }
    // 定时器自动启动
}

// --- 定时器回调函数：生成并馈送随机数据 ---
/**
 * @brief 定时器回调函数，用于生成随机数据并更新图表
 *
 * @param timer 指向触发此回调的定时器的指针
 */
// static void feed_chart_with_random_data(lv_timer_t *timer)
// {
//     // 调用接口函数添加数据点
//     add_data_to_chart_screen_3((int32_t)lv_rand(0, 90), (int32_t)lv_rand(0, 90)); // 生成0-90之间的随机数
// }

// --- 外部/全局声明 ---
// 假设 screen_4 和 back_to_main_event_handler 在别处定义/声明

// --- 前向声明 ---

// --- 项目信息常量 (英文) ---
static const char *PROJECT_NAME = "智能环境监测系统";                // 项目名称
static const char *AUTHORS[] = {"张三", "李四", "王五"};             // 作者列表
static const int NUM_AUTHORS = sizeof(AUTHORS) / sizeof(AUTHORS[0]); // 作者数量
static const char *CREATION_TIME = "2024年10月27日";                 // 创建时间

// 创建屏幕4的函数
/**
 * @brief 创建屏幕4及其内容
 *
 * 此函数构建屏幕4的UI，展示关于项目的信息。
 */
void create_screen_4(void)
{
    // 创建屏幕对象
    screen_4 = lv_obj_create(NULL);
    // lv_obj_set_size(screen_4, 480, 320); // 可选：显式设置屏幕大小（如果需要）

    // --- 标题标签 ---
    lv_obj_t *title = lv_label_create(screen_4);
    lv_label_set_text(title, "关于");
    lv_obj_set_style_text_font(title, &lv_font_weiruan_16, 0); // 为标题设置稍大的字体
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);              // 将标题对齐到顶部中央

    // --- 信息容器 ---
    // 创建一个容器来容纳信息标签，以便更好地控制布局
    lv_obj_t *info_container = lv_obj_create(screen_4);
    // 设置容器大小：宽度为90%，高度为60%
    lv_obj_set_size(info_container, LV_PCT(90), LV_PCT(60));
    lv_obj_set_style_pad_all(info_container, 15, 0);           // 在容器内添加填充
    lv_obj_set_style_border_width(info_container, 1, 0);       // 可选：添加边框
    lv_obj_set_style_radius(info_container, 10, 0);            // 可选：圆角
    lv_obj_set_flex_flow(info_container, LV_FLEX_FLOW_COLUMN); // 将子元素垂直排列
    // 设置子元素对齐方式
    lv_obj_set_flex_align(info_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_align(info_container, LV_ALIGN_CENTER, 0, 0);       // 将容器居中放置在屏幕上
    lv_obj_set_style_border_width(info_container, 0, 0);       // 可选：移除默认边框
    lv_obj_set_style_bg_opa(info_container, LV_OPA_TRANSP, 0); // 使容器背景透明

    // --- 项目名称标签 ---
    lv_obj_t *project_name_label = lv_label_create(info_container);
    // 使用格式化字符串设置项目名称
    lv_label_set_text_fmt(project_name_label, "项目名称: %s", PROJECT_NAME);
    lv_obj_set_style_text_font(project_name_label, &lv_font_weiruan_16, 0); // 设置字体

    // --- 作者标签 ---
    lv_obj_t *authors_label = lv_label_create(info_container);
    // 动态构建作者字符串
    static char authors_text[150];  // 静态缓冲区，用于存放连接后的作者字符串
    authors_text[0] = '\0';         // 初始化为空字符串
    strcat(authors_text, "作者: "); // 添加前缀
    for (int i = 0; i < NUM_AUTHORS; ++i)
    {
        strcat(authors_text, AUTHORS[i]); // 添加作者名
        if (i < NUM_AUTHORS - 1)
        {
            strcat(authors_text, ", "); // 添加逗号分隔符
        }
    }
    lv_label_set_text(authors_label, authors_text);                    // 设置标签文本
    lv_obj_set_style_text_font(authors_label, &lv_font_weiruan_16, 0); // 设置字体

    // --- 创建时间标签 ---
    lv_obj_t *time_label = lv_label_create(info_container);
    // 使用格式化字符串设置创建时间
    lv_label_set_text_fmt(time_label, "创建时间: %s", CREATION_TIME);
    lv_obj_set_style_text_font(time_label, &lv_font_weiruan_16, 0); // 设置字体

    // --- 返回按钮 ---
    lv_obj_t *back_btn = lv_button_create(screen_4);
    lv_obj_set_style_pad_hor(back_btn, 20, 0);           // 为按钮添加水平内边距
    lv_obj_align(back_btn, LV_ALIGN_BOTTOM_MID, 0, -20); // 将按钮对齐到底部中央
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "返回主菜单");
    lv_obj_center(back_label);                                                         // 将标签居中放置在按钮内
    lv_obj_add_event_cb(back_btn, back_to_main_event_handler, LV_EVENT_CLICKED, NULL); // 添加点击事件回调
}

// 主菜单按钮的事件处理函数
/**
 * @brief 处理主菜单按钮的点击事件
 *
 * 根据用户数据（屏幕ID）加载相应的子屏幕。
 *
 * @param e 指向事件数据的指针
 */
static void main_menu_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e); // 获取事件代码
    lv_obj_t *obj = lv_event_get_target_obj(e);  // 获取触发事件的目标对象

    if (code == LV_EVENT_CLICKED) // 如果是点击事件
    {
        // 获取添加事件回调时传递的用户数据（屏幕ID）
        int screen_id = (int)(uintptr_t)lv_event_get_user_data(e);

        switch (screen_id)
        {
        case 1:
            lv_screen_load(screen_1); // 加载屏幕1
            break;
        case 2:
            lv_screen_load(screen_2); // 加载屏幕2
            break;
        case 3:
            lv_screen_load(screen_3); // 加载屏幕3
            break;
        case 4:
            lv_screen_load(screen_4); // 加载屏幕4
            break;
        default:
            break;
        }
    }
}

// 子屏幕上的"返回"按钮的事件处理函数
/**
 * @brief 处理子屏幕上"返回"按钮的点击事件
 *
 * 加载主菜单屏幕。
 *
 * @param e 指向事件数据的指针
 */
static void back_to_main_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e); // 获取事件代码
    lv_obj_t *obj = lv_event_get_target_obj(e);  // 获取触发事件的目标对象

    if (code == LV_EVENT_CLICKED) // 如果是点击事件
    {
        lv_screen_load(main_screen); // 加载主菜单屏幕
    }
}

// 主函数：设置UI
/**
 * @brief 初始化并设置整个用户界面
 *
 * 此函数负责创建所有屏幕并加载主屏幕。
 */
void setup_ui(void)
{
    // 创建所有屏幕
    create_main_screen();
    create_screen_1();
    create_screen_2();
    create_screen_3();
    create_screen_4();

    // 加载主屏幕以首先显示它
    lv_screen_load(main_screen);
}
