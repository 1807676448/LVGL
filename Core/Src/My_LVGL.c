#include "My_LVGL.h"
#include "stm32h7xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <time.h> // 用于时间函数（如果需要）

/* ===== 数据显示范围宏定义 ===== */
// 水质相关参数
#define TDS_MIN 0
#define TDS_MAX 2000
#define COD_MIN 0
#define COD_MAX 100
#define UV254_MIN 0
#define UV254_MAX 10
#define PH_MIN 0
#define PH_MAX 14

// 温湿度相关
#define TEM_MIN -40
#define TEM_MAX 85
#define HUM_MIN 0
#define HUM_MAX 100
#define AIR_TEMP_MIN -40
#define AIR_TEMP_MAX 85
#define AIR_HUM_MIN 0
#define AIR_HUM_MAX 100

// 气象相关
#define PRESSURE_MIN 900
#define PRESSURE_MAX 1100
#define ALTITUDE_MIN -500
#define ALTITUDE_MAX 5000
#define TUR_MIN 0
#define TUR_MAX 3000

/* ===== 数据显示格式宏定义 ===== */
#define DISPLAY_FORMAT_INT "%d"
#define DISPLAY_FORMAT_FLOAT_1 "%.1f"
#define DISPLAY_FORMAT_FLOAT_2 "%.2f"
#define DISPLAY_FORMAT_FLOAT_3 "%.3f"

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
        data->point.y = (tp_dev.x) * 0.08;    // 可能需要交换x,y或调整系数
        data->point.x = tp_dev.y * 0.12;      // 可能需要交换x,y或调整系数
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

static lv_style_t style_screen_bg;
static lv_style_t style_card;
static lv_style_t style_title;
static lv_style_t style_btn;
static lv_style_t style_btn_pressed;
static bool ui_style_initialized = false;

static void ui_init_theme_and_style(void)
{
    if (ui_style_initialized)
    {
        return;
    }

    lv_display_t *disp = lv_display_get_default();
    if (disp)
    {
        lv_theme_t *theme = lv_theme_default_init(disp,
                                                  lv_palette_main(LV_PALETTE_BLUE),
                                                  lv_palette_main(LV_PALETTE_GREY),
                                                  false,
                                                  LV_FONT_DEFAULT);
        lv_display_set_theme(disp, theme);
    }

    lv_style_init(&style_screen_bg);
    lv_style_set_bg_color(&style_screen_bg, lv_palette_lighten(LV_PALETTE_GREY, 5));
    lv_style_set_bg_opa(&style_screen_bg, LV_OPA_COVER);

    lv_style_init(&style_card);
    lv_style_set_radius(&style_card, 12);
    lv_style_set_bg_color(&style_card, lv_color_white());
    lv_style_set_bg_opa(&style_card, LV_OPA_COVER);
    lv_style_set_border_width(&style_card, 1);
    lv_style_set_border_color(&style_card, lv_palette_lighten(LV_PALETTE_GREY, 2));
    lv_style_set_pad_all(&style_card, 10);

    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, LV_FONT_DEFAULT);
    lv_style_set_text_color(&style_title, lv_palette_darken(LV_PALETTE_BLUE, 3));

    lv_style_init(&style_btn);
    lv_style_set_radius(&style_btn, 10);
    lv_style_set_bg_color(&style_btn, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_bg_opa(&style_btn, LV_OPA_COVER);
    lv_style_set_text_color(&style_btn, lv_color_white());
    lv_style_set_pad_ver(&style_btn, 10);
    lv_style_set_pad_hor(&style_btn, 18);

    lv_style_init(&style_btn_pressed);
    lv_style_set_bg_color(&style_btn_pressed, lv_palette_darken(LV_PALETTE_BLUE, 2));
    lv_style_set_bg_opa(&style_btn_pressed, LV_OPA_COVER);

    ui_style_initialized = true;
}

static void ui_apply_screen_style(lv_obj_t *screen)
{
    if (!screen)
    {
        return;
    }
    lv_obj_add_style(screen, &style_screen_bg, LV_PART_MAIN);
}

static void ui_apply_title_style(lv_obj_t *obj)
{
    if (!obj)
    {
        return;
    }
    lv_obj_add_style(obj, &style_title, LV_PART_MAIN);
}

static void ui_apply_button_style(lv_obj_t *btn)
{
    if (!btn)
    {
        return;
    }
    lv_obj_add_style(btn, &style_btn, LV_PART_MAIN);
    lv_obj_add_style(btn, &style_btn_pressed, LV_PART_MAIN | LV_STATE_PRESSED);
}

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
    ui_apply_screen_style(main_screen);

    // --- 主内容容器 (水平布局) ---
    lv_obj_t *main_content_cont = lv_obj_create(main_screen);
    lv_obj_set_size(main_content_cont, LV_PCT(100), LV_PCT(100)); // 设置为全屏大小
    lv_obj_set_flex_flow(main_content_cont, LV_FLEX_FLOW_ROW);    // 设置为水平流式布局
    // 设置子元素对齐方式：主轴起始对齐，交叉轴居中对齐
    lv_obj_set_flex_align(main_content_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_border_width(main_content_cont, 0, 0); // 移除容器边框
    lv_obj_set_style_pad_all(main_content_cont, 0, 0);      // 移除容器内边距
    lv_obj_set_style_bg_opa(main_content_cont, LV_OPA_TRANSP, 0);
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
    lv_obj_add_style(left_cont, &style_card, LV_PART_MAIN);
    lv_obj_set_style_pad_row(left_cont, 12, 0);

    // --- 右侧容器用于显示信息 (占50%宽度) ---
    lv_obj_t *right_cont = lv_obj_create(main_content_cont);
    lv_obj_set_size(right_cont, LV_PCT(50), LV_PCT(100));  // 设置宽度为50%，高度为100%
    lv_obj_set_flex_flow(right_cont, LV_FLEX_FLOW_COLUMN); // 设置为垂直流式布局
    // 设置子元素居中对齐
    lv_obj_set_flex_align(right_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_border_width(right_cont, 0, 0); // 移除边框
    lv_obj_set_style_pad_all(right_cont, 10, 0);     // 添加内边距
    lv_obj_add_style(right_cont, &style_card, LV_PART_MAIN);
    lv_obj_set_style_pad_row(right_cont, 10, 0);

    // --- 在左侧容器中创建按钮 ---
    lv_obj_t *buttons[4];                                                      // 按钮对象数组
    const char *button_labels[] = {"水质数据", "设备开关", "遥杆", "AI分析"}; // 按钮标签

    for (int i = 0; i < 4; i++)
    {
        buttons[i] = lv_button_create(left_cont);      // 在左侧容器中创建按钮
        lv_obj_set_size(buttons[i], 170, 46);
        ui_apply_button_style(buttons[i]);
        lv_obj_t *label = lv_label_create(buttons[i]); // 在按钮上创建标签
        lv_label_set_text(label, button_labels[i]);    // 设置标签文本
        lv_obj_set_style_text_font(label, LV_FONT_DEFAULT, 0);
        lv_obj_center(label);                          // 将标签居中放置在按钮上
        // 为按钮添加点击事件回调，并传递屏幕ID (1-4)
        lv_obj_add_event_cb(buttons[i], main_menu_event_handler, LV_EVENT_CLICKED, (void *)(i + 1));
    }

    // --- 在右侧容器中创建信息标签 ---
    // 时间标签
    time_label = lv_label_create(right_cont);
    lv_label_set_text(time_label, "00:00:00"); // 初始文本
    lv_obj_set_style_text_font(time_label, LV_FONT_DEFAULT, 0);
    lv_obj_set_style_text_color(time_label, lv_palette_darken(LV_PALETTE_BLUE, 2), 0);
    // 可选：为时间标签设置字体样式
    // lv_obj_set_style_text_font(time_label, &lv_font_montserrat_20, 0);

    // 日期标签
    date_label = lv_label_create(right_cont);
    lv_label_set_text(date_label, "1970-01-01"); // 初始文本
    lv_obj_set_style_text_font(date_label, LV_FONT_DEFAULT, 0);

    // 天气标签
    weather_label = lv_label_create(right_cont);
    lv_label_set_text(weather_label, "Sunny"); // 初始文本
    lv_obj_set_style_text_font(weather_label, LV_FONT_DEFAULT, 0);

    // 欢迎标签
    welcome_label = lv_label_create(right_cont);
    lv_label_set_text(welcome_label, "Welcome!"); // 初始文本
    lv_obj_set_style_text_font(welcome_label, LV_FONT_DEFAULT, 0);
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

/* ===== 数据项配置结构 ===== */
typedef struct
{
    const char *name;           // 项目名称
    int range_min;              // 显示范围最小值
    int range_max;              // 显示范围最大值
    const char *display_format; // 显示格式字符串
} DataItemConfig_t;

typedef struct
{
    const char *name;      // 名称(常量字符串)
    lv_obj_t *bar;         // 对应条形图对象
    lv_obj_t *value_label; // 显示数值的标签
} screen1_item_t;

#define SCREEN1_ITEM_TOTAL 10

/* ===== 各项数据的配置 ===== */
static const DataItemConfig_t item_configs[SCREEN1_ITEM_TOTAL] = {
    {"TDS", TDS_MIN, TDS_MAX, DISPLAY_FORMAT_FLOAT_2},
    {"COD", COD_MIN, COD_MAX, DISPLAY_FORMAT_FLOAT_2},

    {"UV254", UV254_MIN, UV254_MAX, DISPLAY_FORMAT_FLOAT_2},
    {"pH", PH_MIN, PH_MAX, DISPLAY_FORMAT_FLOAT_1},
    {"Tem", TEM_MIN, TEM_MAX, DISPLAY_FORMAT_FLOAT_1},
    {"Tur", TUR_MIN, TUR_MAX, DISPLAY_FORMAT_FLOAT_2},
    {"air_temp", AIR_TEMP_MIN, AIR_TEMP_MAX, DISPLAY_FORMAT_FLOAT_1},
    {"air_hum", AIR_HUM_MIN, AIR_HUM_MAX, DISPLAY_FORMAT_FLOAT_1},
    {"pressure", PRESSURE_MIN, PRESSURE_MAX, DISPLAY_FORMAT_FLOAT_1},
    {"altitude", ALTITUDE_MIN, ALTITUDE_MAX, DISPLAY_FORMAT_FLOAT_1},
};

static screen1_item_t screen1_items[SCREEN1_ITEM_TOTAL];
static uint8_t screen1_item_count = 0;

/**
 * @brief 根据名称更新屏幕1对应项目的数值
 * @param name  项目名称(如 "TDS","COD"...)
 * @param value 新值(整数或浮点数均可通过My_SendJson_QueryValue传入)
 * @return 0 成功; 1 未找到; 2 对象未创建; 3 值超范围
 * @note 需在 LVGL 线程/上下文中调用(不要直接在中断里调用)。若在中断，可用 lv_async_call 封装。
 */
int update_screen1_item(const char *name, double value)
{
    if (!screen_1)
    {
        printf("[LVGL] update_screen1_item: screen_1 is NULL!\r\n");
        return 2;
    }

    // 查找对应的配置和项目
    int config_idx = -1;
    for (int i = 0; i < SCREEN1_ITEM_TOTAL; i++)
    {
        if (strcmp(item_configs[i].name, name) == 0)
        {
            config_idx = i;
            break;
        }
    }

    if (config_idx == -1)
    {
        printf("[LVGL] update_screen1_item: name '%s' not found in config!\r\n", name ? name : "(null)");
        return 1; // 未找到配置
    }

    // 检查值是否超出范围
    if (value < item_configs[config_idx].range_min || value > item_configs[config_idx].range_max)
    {
        printf("[LVGL] update_screen1_item: '%s' value %.2f out of range [%d, %d]!\r\n",
               name, value, item_configs[config_idx].range_min, item_configs[config_idx].range_max);
        return 3;
    }

    for (uint8_t i = 0; i < screen1_item_count; i++)
    {
        if (screen1_items[i].name && strcmp(screen1_items[i].name, name) == 0)
        {
            if (screen1_items[i].bar)
            {
                lv_bar_set_value(screen1_items[i].bar, (int32_t)value, LV_ANIM_OFF);
            }
            if (screen1_items[i].value_label)
            {
                static char buf[32];
                snprintf(buf, sizeof(buf), item_configs[config_idx].display_format, value);
                lv_label_set_text(screen1_items[i].value_label, buf);
            }
            printf("[LVGL] update_screen1_item: '%s' = %.2f OK\r\n", name, value);
            return 0;
        }
    }
    printf("[LVGL] update_screen1_item: '%s' found in config but no matching screen item (screen1_item_count=%d)\r\n",
           name, screen1_item_count);
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
    ui_apply_screen_style(screen_1);

    // 添加标题标签
    lv_obj_t *title = lv_label_create(screen_1);
    lv_label_set_text(title, "水质检测数据");
    ui_apply_title_style(title);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10); // 将标题对齐到顶部中央

    // 添加返回按钮
    lv_obj_t *back_btn = lv_button_create(screen_1);
    ui_apply_button_style(back_btn);
    lv_obj_align(back_btn, LV_ALIGN_BOTTOM_MID, 0, -10); // 将按钮对齐到底部中央
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "返回");
    lv_obj_center(back_label);                                                         // 将标签居中放置在按钮上
    lv_obj_add_event_cb(back_btn, back_to_main_event_handler, LV_EVENT_CLICKED, NULL); // 添加点击事件回调

    // --- 数据可视化内容 ---
    // 示例数据数组 (值在0到100之间，用于条形图显示)
    int data_values[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    // 为数据项列表创建一个容器
    list_cont = lv_obj_create(screen_1);
    lv_obj_add_style(list_cont, &style_card, LV_PART_MAIN);
    // 设置容器大小，宽度为屏幕90%，高度为屏幕高度减去100像素
    lv_obj_set_size(list_cont, LV_PCT(90), LV_VER_RES - 100);
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

    /* 显示名称（中文）与内部键名（英文，用于JSON匹配）分离 */
    const char *names[SCREEN1_ITEM_TOTAL] = {"TDS", "COD", "UV254", "pH", "水温", "浊度", "气温", "空气湿度", "气压", "海拔"};
    screen1_item_count = 0;

    // 创建全部数据项
    for (int i = 0; i < SCREEN1_ITEM_TOTAL; i++)
    {
        lv_obj_t *item_cont = lv_obj_create(list_cont);
        lv_obj_set_size(item_cont, LV_PCT(100), 40);
        lv_obj_set_flex_flow(item_cont, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(item_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_clear_flag(item_cont, LV_OBJ_FLAG_SCROLLABLE);

        // 名称标签（中文显示）
        lv_obj_t *label = lv_label_create(item_cont);
        lv_label_set_text(label, names[i]);
        lv_obj_set_width(label, 78);

        // 数值标签（从My_Data查询初始值）
        char value_text[32];
        double value_double = 0.0;

        // if (My_SendJson_QueryValue(item_configs[i].name, value_text, sizeof(value_text)))
        // {
        //     // 查询成功，移除JSON引号并转换为数字
        //     if (value_text[0] == '"')
        //     {
        //         memmove(value_text, value_text + 1, strlen(value_text));
        //         char *last_quote = strchr(value_text, '"');
        //         if (last_quote)
        //             *last_quote = '\0';
        //     }
        //     value_double = atof(value_text);
        // }

        // 条形图：使用配置中的范围
        lv_obj_t *bar = lv_bar_create(item_cont);
        lv_bar_set_range(bar, item_configs[i].range_min, item_configs[i].range_max);

        // 设置初始值：若 0 在有效范围内则归零，否则取范围最小值（进度条"空"位置）
        int bar_value = item_configs[i].range_min;
        if (value_double >= item_configs[i].range_min && value_double <= item_configs[i].range_max)
        {
            bar_value = (int)value_double;
        }

        lv_bar_set_value(bar, bar_value, LV_ANIM_OFF);

        // 使用配置的格式显示数值（与 bar 实际值保持一致）
        snprintf(value_text, sizeof(value_text), item_configs[i].display_format, (double)bar_value);

        lv_obj_t *value_label = lv_label_create(item_cont);
        lv_label_set_text(value_label, value_text);
        lv_obj_set_width(value_label, 80);
        lv_obj_set_flex_grow(bar, 1);
        lv_obj_add_style(bar, &bar_bg_style, LV_PART_MAIN);
        lv_obj_add_style(bar, &bar_indic_style, LV_PART_INDICATOR);
        lv_obj_set_height(bar, 20);

        // 保存映射：内部键名使用英文（item_configs），与JSON数据键名一致
        screen1_items[i].name = item_configs[i].name;
        screen1_items[i].bar = bar;
        screen1_items[i].value_label = value_label;
        screen1_item_count++;

        // 取消交互
        lv_obj_remove_event_cb(bar, NULL);
    }
}

// 声明外部变量
extern lv_obj_t *screen_2;                             // 声明屏幕2对象
extern void back_to_main_event_handler(lv_event_t *e); // 声明返回主菜单事件处理函数

// --- 设备状态管理结构 ---
typedef struct
{
    char device_id[32];      // 设备ID
    uint32_t last_heartbeat; // 上次心跳时间(HAL_GetTick())
    bool is_online;          // 在线状态
    lv_obj_t *indicator_obj; // 指示器对象(用于改变颜色)
    lv_obj_t *label_obj;     // 标签对象(显示状态文本)
} DeviceStatus_t;

#define DEVICE_COUNT 4
#define DEVICE_TIMEOUT_MS 6000 // 6秒超时

// 初始化设备列表
static DeviceStatus_t devices[DEVICE_COUNT] = {
    {.device_id = "device_001", .is_online = false, .last_heartbeat = 0},
    {.device_id = "device_002", .is_online = false, .last_heartbeat = 0},
    {.device_id = "device_003", .is_online = false, .last_heartbeat = 0},
    {.device_id = "device_004", .is_online = false, .last_heartbeat = 0}};

static lv_timer_t *device_check_timer = NULL;

// --- 内部辅助函数 ---

/**
 * @brief 更新单个设备的UI显示状态
 */
void update_device_ui(int idx)
{
    if (idx < 0 || idx >= DEVICE_COUNT)
        return;
    DeviceStatus_t *dev = &devices[idx];
    if (!dev->indicator_obj || !dev->label_obj)
        return;

    if (dev->is_online)
    {
        // 活跃：橙色 (0xFFA500)
        lv_obj_set_style_bg_color(dev->indicator_obj, lv_color_hex(0xFFA500), 0);
        lv_label_set_text_fmt(dev->label_obj, "%s: On", dev->device_id);
    }
    else
    {
        // 下线：灰色 (0x808080)
        lv_obj_set_style_bg_color(dev->indicator_obj, lv_color_hex(0x808080), 0);
        lv_label_set_text_fmt(dev->label_obj, "%s: Off", dev->device_id);
    }
}

/**
 * @brief 定时器回调：检查设备超时
 */
static void device_check_timer_cb(lv_timer_t *t)
{
    uint32_t now = HAL_GetTick();
    for (int i = 0; i < DEVICE_COUNT; i++)
    {
        // 如果当前是在线，且距离上次心跳超过超时时间，则设为下线
        if (devices[i].is_online && (now - devices[i].last_heartbeat > DEVICE_TIMEOUT_MS))
        {
            devices[i].is_online = false;  
        }
        update_device_ui(i);
    }
}

// --- 设备监控屏幕的刷新回调(从 My_Data 整合设备状态) ---
/**
 * @brief 从 My_Data 模块获取设备状态并更新UI
 * 此回调定期调用以同步来自 My_Data 的设备检测数据到UI显示
 */
static void sync_device_status_from_data(lv_timer_t *t)
{
    // 遍历设备列表，从 My_Data 的 device_status_list 同步状态
    // 这允许 My_Data 作为数据源，UI 作为显示层

    for (int i = 0; i < DEVICE_COUNT; i++)
    {
        // 检查 My_Data 中是否有该索引的设备状态数据
        if (device_list[i].valid)
        {
            // 信息传递与状态改变
            devices[i].last_heartbeat = HAL_GetTick();
            devices[i].is_online = true;
            device_list[i].valid = false;
            update_device_ui(i);
            //Debug监控接口
            // printf("Device %d status synced from My_Data: On\r\n",i);
        }
    }
}

// --- 创建屏幕2函数 (设备状态监控) ---
/**
 * @brief 创建设备监控屏幕并整合 My_Data 的设备检测功能
 *
 * 此屏幕展示多个设备的在线/离线状态，显示最近的心跳时间等信息。
 * 与 My_Data 模块集成以获取设备检测数据。
 */
void create_screen_2(void)
{
    screen_2 = lv_obj_create(NULL); // 创建屏幕2对象
    ui_apply_screen_style(screen_2);

    // 添加标题标签
    lv_obj_t *title = lv_label_create(screen_2);
    lv_label_set_text(title, "设备监控");
    ui_apply_title_style(title);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);

    // 添加返回按钮
    lv_obj_t *back_btn = lv_button_create(screen_2);
    ui_apply_button_style(back_btn);
    lv_obj_align(back_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "返回");
    lv_obj_center(back_label);
    lv_obj_add_event_cb(back_btn, back_to_main_event_handler, LV_EVENT_CLICKED, NULL);

    // --- 主内容容器 ---
    lv_obj_t *main_cont = lv_obj_create(screen_2);
    lv_obj_add_style(main_cont, &style_card, LV_PART_MAIN);
    lv_obj_set_size(main_cont, LV_PCT(90), LV_VER_RES - 100);
    lv_obj_align_to(main_cont, title, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_set_flex_flow(main_cont, LV_FLEX_FLOW_ROW_WRAP); // 换行布局
    lv_obj_set_flex_align(main_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // 创建4个设备状态显示卡片
    for (int i = 0; i < DEVICE_COUNT; i++)
    {
        // 卡片容器
        lv_obj_t *card = lv_obj_create(main_cont);
        lv_obj_set_size(card, LV_PCT(40), 80);           // 宽40%，高80px
        lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN); // 垂直布局
        lv_obj_set_flex_align(card, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

        // 设置卡片样式
        lv_obj_set_style_radius(card, 10, 0);
        lv_obj_set_style_border_width(card, 1, 0);
        lv_obj_set_style_border_color(card, lv_palette_lighten(LV_PALETTE_GREY, 1), 0);
        lv_obj_set_style_pad_all(card, 5, 0);

        // 保存卡片对象作为指示器(背景变色)
        devices[i].indicator_obj = card;

        // 设备ID标签（头部）
        lv_obj_t *id_label = lv_label_create(card);
        lv_label_set_text_fmt(id_label, "设备: %s", devices[i].device_id);
        lv_obj_set_style_text_color(id_label, lv_color_hex(0xFFFFFF), 0);

        // 状态标签
        lv_obj_t *label = lv_label_create(card);
        devices[i].label_obj = label;
        lv_obj_set_style_text_color(label, lv_color_white(), 0); // 白色文字

        // 初始化显示
        update_device_ui(i);
    }

    // 创建并启动定时器，5秒检查一次设备超时 (避免重复创建)
    if (device_check_timer == NULL)
    {
        device_check_timer = lv_timer_create(device_check_timer_cb, DeviceHeartTime, NULL);
    }

    // 创建同步定时器，1秒从 My_Data 同步一次设备状态
    lv_timer_create(sync_device_status_from_data, 1000, NULL);
}

// 创建屏幕3的函数
// --- 全局/静态变量 ---
static lv_obj_t *joystick_base = NULL;
static lv_obj_t *joystick_knob = NULL;
static lv_obj_t *joystick_state_label = NULL;

#define JOYSTICK_MAX_DEFLECTION 34
#define JOYSTICK_DEADZONE 12

static joystick_state_t g_joystick_state = {
    .direction = JOYSTICK_DIR_CENTER,
    .x_percent = 0,
    .y_percent = 0,
    .active = false};

static joystick_direction_t joystick_calc_direction(int16_t dx, int16_t dy, bool active)
{
    if (!active)
    {
        return JOYSTICK_DIR_CENTER;
    }

    if (LV_ABS(dx) <= JOYSTICK_DEADZONE && LV_ABS(dy) <= JOYSTICK_DEADZONE)
    {
        return JOYSTICK_DIR_CENTER;
    }

    if (LV_ABS(dx) > LV_ABS(dy))
    {
        return (dx > 0) ? JOYSTICK_DIR_RIGHT : JOYSTICK_DIR_LEFT;
    }

    return (dy > 0) ? JOYSTICK_DIR_DOWN : JOYSTICK_DIR_UP;
}

const char *joystick_direction_to_str(joystick_direction_t dir)
{
    switch (dir)
    {
    case JOYSTICK_DIR_UP:
        return "UP";
    case JOYSTICK_DIR_DOWN:
        return "DOWN";
    case JOYSTICK_DIR_LEFT:
        return "LEFT";
    case JOYSTICK_DIR_RIGHT:
        return "RIGHT";
    case JOYSTICK_DIR_CENTER:
    default:
        return "CENTER";
    }
}

joystick_direction_t joystick_get_direction(void)
{
    return g_joystick_state.direction;
}

void joystick_get_state(joystick_state_t *out_state)
{
    if (!out_state)
    {
        return;
    }
    *out_state = g_joystick_state;
}

bool joystick_screen_is_active(void)
{
    return lv_screen_active() == screen_3;
}

static void joystick_update_state_label(void)
{
    if (!joystick_state_label)
    {
        return;
    }

    lv_label_set_text_fmt(joystick_state_label,
                          "方向:%s  X:%d%%  Y:%d%%",
                          joystick_direction_to_str(g_joystick_state.direction),
                          g_joystick_state.x_percent,
                          g_joystick_state.y_percent);
}

static void joystick_apply_vector(int16_t dx, int16_t dy, bool active)
{
    int16_t max_abs = LV_MAX(LV_ABS(dx), LV_ABS(dy));
    if (max_abs > JOYSTICK_MAX_DEFLECTION)
    {
        dx = (int16_t)((dx * JOYSTICK_MAX_DEFLECTION) / max_abs);
        dy = (int16_t)((dy * JOYSTICK_MAX_DEFLECTION) / max_abs);
    }

    if (!active)
    {
        dx = 0;
        dy = 0;
    }

    g_joystick_state.direction = joystick_calc_direction(dx, dy, active);
    g_joystick_state.x_percent = (int16_t)((dx * 100) / JOYSTICK_MAX_DEFLECTION);
    g_joystick_state.y_percent = (int16_t)((-dy * 100) / JOYSTICK_MAX_DEFLECTION);
    g_joystick_state.active = active;

    if (joystick_knob)
    {
        lv_obj_align(joystick_knob, LV_ALIGN_CENTER, dx, dy);
    }

    joystick_update_state_label();
}

static void joystick_base_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_PRESSED || code == LV_EVENT_PRESSING)
    {
        lv_indev_t *indev = lv_indev_active();
        if (!indev || !joystick_base)
        {
            return;
        }

        lv_point_t p;
        lv_area_t coords;
        lv_indev_get_point(indev, &p);
        lv_obj_get_coords(joystick_base, &coords);

        int16_t cx = (int16_t)((coords.x1 + coords.x2) / 2);
        int16_t cy = (int16_t)((coords.y1 + coords.y2) / 2);
        int16_t dx = (int16_t)(p.x - cx);
        int16_t dy = (int16_t)(p.y - cy);

        joystick_apply_vector(dx, dy, true);
        return;
    }

    if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST)
    {
        joystick_apply_vector(0, 0, false);
    }
}

static void joystick_key_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    joystick_direction_t dir = (joystick_direction_t)(uintptr_t)lv_event_get_user_data(e);

    if (code == LV_EVENT_PRESSED)
    {
        switch (dir)
        {
        case JOYSTICK_DIR_UP:
            joystick_apply_vector(0, -JOYSTICK_MAX_DEFLECTION, true);
            break;
        case JOYSTICK_DIR_DOWN:
            joystick_apply_vector(0, JOYSTICK_MAX_DEFLECTION, true);
            break;
        case JOYSTICK_DIR_LEFT:
            joystick_apply_vector(-JOYSTICK_MAX_DEFLECTION, 0, true);
            break;
        case JOYSTICK_DIR_RIGHT:
            joystick_apply_vector(JOYSTICK_MAX_DEFLECTION, 0, true);
            break;
        case JOYSTICK_DIR_CENTER:
        default:
            joystick_apply_vector(0, 0, false);
            break;
        }
        return;
    }

    if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST)
    {
        joystick_apply_vector(0, 0, false);
    }
}

// --- 创建屏幕3函数 ---
/**
 * @brief 创建屏幕3及其内容
 *
 * 此函数构建屏幕3的UI，展示虚拟遥杆与四方向按键。
 */
void create_screen_3(void)
{
    screen_3 = lv_obj_create(NULL); // 创建屏幕3对象
    ui_apply_screen_style(screen_3);
    lv_obj_clear_flag(screen_3, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(screen_3, LV_SCROLLBAR_MODE_OFF);

    lv_display_t *disp = lv_display_get_default();
    int32_t screen_w = 320;
    int32_t screen_h = 480;
    if (disp)
    {
        screen_w = lv_display_get_horizontal_resolution(disp);
        screen_h = lv_display_get_vertical_resolution(disp);
    }

    int32_t panel_w = (screen_w * 94) / 100;
    int32_t panel_h = screen_h - 170;
    if (panel_h > 300)
    {
        panel_h = 300;
    }
    if (panel_h < 170)
    {
        panel_h = 170;
    }

    int32_t joy_size = panel_h - 54;
    if (joy_size > 128)
    {
        joy_size = 128;
    }
    if (joy_size < 100)
    {
        joy_size = 100;
    }

    int32_t knob_size = joy_size / 3;
    if (knob_size > 46)
    {
        knob_size = 46;
    }
    if (knob_size < 36)
    {
        knob_size = 36;
    }

    int32_t dpad_size = joy_size - 8;
    if (dpad_size < 92)
    {
        dpad_size = 92;
    }

    int32_t dpad_btn_w = dpad_size / 3;
    int32_t dpad_btn_h = dpad_size / 4;
    if (dpad_btn_w > 44)
    {
        dpad_btn_w = 44;
    }
    if (dpad_btn_w < 34)
    {
        dpad_btn_w = 34;
    }
    if (dpad_btn_h > 34)
    {
        dpad_btn_h = 34;
    }
    if (dpad_btn_h < 28)
    {
        dpad_btn_h = 28;
    }

    // 左右控件间距收紧约15%（左右各向中心移动7.5%）
    int32_t inward = (panel_w * 15) / 200;
    if (inward < 10)
    {
        inward = 10;
    }

    // --- 标题标签 ---
    lv_obj_t *title = lv_label_create(screen_3);
    lv_label_set_text(title, "遥杆");
    ui_apply_title_style(title);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 8);

    // --- 返回按钮 ---
    lv_obj_t *back_btn = lv_button_create(screen_3);
    ui_apply_button_style(back_btn);
    lv_obj_set_width(back_btn, 140);
    lv_obj_align(back_btn, LV_ALIGN_BOTTOM_MID, 0, -12);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "返回");
    lv_obj_center(back_label);
    lv_obj_add_event_cb(back_btn, back_to_main_event_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t *panel = lv_obj_create(screen_3);
    lv_obj_add_style(panel, &style_card, LV_PART_MAIN);
    lv_obj_set_size(panel, panel_w, panel_h);
    lv_obj_align(panel, LV_ALIGN_TOP_MID, 0, 42);
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);

    joystick_base = lv_obj_create(panel);
    lv_obj_set_size(joystick_base, joy_size, joy_size);
    lv_obj_align(joystick_base, LV_ALIGN_LEFT_MID, 10 + inward, -4);
    lv_obj_set_style_radius(joystick_base, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(joystick_base, 3, 0);
    lv_obj_set_style_border_color(joystick_base, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_set_style_bg_color(joystick_base, lv_palette_lighten(LV_PALETTE_BLUE, 4), 0);
    lv_obj_set_style_bg_opa(joystick_base, LV_OPA_40, 0);
    lv_obj_set_style_pad_all(joystick_base, 0, 0);
    lv_obj_clear_flag(joystick_base, LV_OBJ_FLAG_SCROLLABLE);

    joystick_knob = lv_obj_create(joystick_base);
    lv_obj_set_size(joystick_knob, knob_size, knob_size);
    lv_obj_set_style_radius(joystick_knob, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(joystick_knob, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_set_style_bg_opa(joystick_knob, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(joystick_knob, 2, 0);
    lv_obj_set_style_border_color(joystick_knob, lv_color_white(), 0);
    lv_obj_clear_flag(joystick_knob, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(joystick_knob, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(joystick_knob, LV_ALIGN_CENTER, 0, 0);

    lv_obj_add_event_cb(joystick_base, joystick_base_event_handler, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(joystick_base, joystick_base_event_handler, LV_EVENT_PRESSING, NULL);
    lv_obj_add_event_cb(joystick_base, joystick_base_event_handler, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(joystick_base, joystick_base_event_handler, LV_EVENT_PRESS_LOST, NULL);

    lv_obj_t *dpad_cont = lv_obj_create(panel);
    lv_obj_set_size(dpad_cont, dpad_size, dpad_size);
    lv_obj_align(dpad_cont, LV_ALIGN_RIGHT_MID, -(10 + inward), -4);
    lv_obj_set_style_border_width(dpad_cont, 0, 0);
    lv_obj_set_style_bg_opa(dpad_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(dpad_cont, 0, 0);
    lv_obj_clear_flag(dpad_cont, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *up_btn = lv_button_create(dpad_cont);
    ui_apply_button_style(up_btn);
    lv_obj_set_size(up_btn, dpad_btn_w, dpad_btn_h);
    lv_obj_align(up_btn, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_t *up_label = lv_label_create(up_btn);
    lv_label_set_text(up_label, "上");
    lv_obj_center(up_label);

    lv_obj_t *down_btn = lv_button_create(dpad_cont);
    ui_apply_button_style(down_btn);
    lv_obj_set_size(down_btn, dpad_btn_w, dpad_btn_h);
    lv_obj_align(down_btn, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_t *down_label = lv_label_create(down_btn);
    lv_label_set_text(down_label, "下");
    lv_obj_center(down_label);

    lv_obj_t *left_btn = lv_button_create(dpad_cont);
    ui_apply_button_style(left_btn);
    lv_obj_set_size(left_btn, dpad_btn_w, dpad_btn_h);
    lv_obj_align(left_btn, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_t *left_label = lv_label_create(left_btn);
    lv_label_set_text(left_label, "左");
    lv_obj_center(left_label);

    lv_obj_t *right_btn = lv_button_create(dpad_cont);
    ui_apply_button_style(right_btn);
    lv_obj_set_size(right_btn, dpad_btn_w, dpad_btn_h);
    lv_obj_align(right_btn, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_t *right_label = lv_label_create(right_btn);
    lv_label_set_text(right_label, "右");
    lv_obj_center(right_label);

    lv_obj_add_event_cb(up_btn, joystick_key_event_handler, LV_EVENT_PRESSED, (void *)(uintptr_t)JOYSTICK_DIR_UP);
    lv_obj_add_event_cb(up_btn, joystick_key_event_handler, LV_EVENT_RELEASED, (void *)(uintptr_t)JOYSTICK_DIR_UP);
    lv_obj_add_event_cb(up_btn, joystick_key_event_handler, LV_EVENT_PRESS_LOST, (void *)(uintptr_t)JOYSTICK_DIR_UP);

    lv_obj_add_event_cb(down_btn, joystick_key_event_handler, LV_EVENT_PRESSED, (void *)(uintptr_t)JOYSTICK_DIR_DOWN);
    lv_obj_add_event_cb(down_btn, joystick_key_event_handler, LV_EVENT_RELEASED, (void *)(uintptr_t)JOYSTICK_DIR_DOWN);
    lv_obj_add_event_cb(down_btn, joystick_key_event_handler, LV_EVENT_PRESS_LOST, (void *)(uintptr_t)JOYSTICK_DIR_DOWN);

    lv_obj_add_event_cb(left_btn, joystick_key_event_handler, LV_EVENT_PRESSED, (void *)(uintptr_t)JOYSTICK_DIR_LEFT);
    lv_obj_add_event_cb(left_btn, joystick_key_event_handler, LV_EVENT_RELEASED, (void *)(uintptr_t)JOYSTICK_DIR_LEFT);
    lv_obj_add_event_cb(left_btn, joystick_key_event_handler, LV_EVENT_PRESS_LOST, (void *)(uintptr_t)JOYSTICK_DIR_LEFT);

    lv_obj_add_event_cb(right_btn, joystick_key_event_handler, LV_EVENT_PRESSED, (void *)(uintptr_t)JOYSTICK_DIR_RIGHT);
    lv_obj_add_event_cb(right_btn, joystick_key_event_handler, LV_EVENT_RELEASED, (void *)(uintptr_t)JOYSTICK_DIR_RIGHT);
    lv_obj_add_event_cb(right_btn, joystick_key_event_handler, LV_EVENT_PRESS_LOST, (void *)(uintptr_t)JOYSTICK_DIR_RIGHT);

    joystick_state_label = lv_label_create(screen_3);
    lv_obj_set_style_text_color(joystick_state_label, lv_palette_darken(LV_PALETTE_BLUE, 2), 0);
    lv_obj_align_to(joystick_state_label, panel, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);

    // 确保返回键不被其他对象遮挡
    lv_obj_move_foreground(back_btn);

    joystick_apply_vector(0, 0, false);
}

// --- 外部/全局声明 ---
// 假设 screen_4 和 back_to_main_event_handler 在别处定义/声明

// --- 前向声明 ---

// --- 项目信息常量 (英文) ---
static const char *PROJECT_NAME = "智能环境监测系统";                // 项目名称
static const char *AUTHORS[] = {"+++", "+++", "+++"};       // 作者列表
static const int NUM_AUTHORS = sizeof(AUTHORS) / sizeof(AUTHORS[0]); // 作者数量
static const char *CREATION_TIME = "2025年10月27日";                 // 创建时间

// 创建屏幕4的函数
/**
 * @brief 创建屏幕4及其内容
 *
 * 此函数构建屏幕4的UI，展示关于项目的信息。
 */
// ===== Screen 4 静态对象指针 =====
static lv_obj_t *s4_analysis_label = NULL;   // AI 分析文本标签
static lv_obj_t *s4_sample_label = NULL;     // 样本数标签
static lv_obj_t *s4_status_label = NULL;     // 状态图标标签（✅/❌/⏳）
static lv_obj_t *s4_time_label = NULL;       // 响应时间标签
static lv_obj_t *s4_request_btn = NULL;      // 请求分析按钮
static bool s4_waiting_response = false;     // 是否正在等待响应
static uint8_t s4_wait_seconds = 0;          // 等待响应秒数计数器
static bool s4_first_time_sync_done = false; // 首次时间同步后自动请求标记

/* 前向声明 */
static void s4_trigger_dshelp(void);

/**
 * @brief "请求AI分析" 按钮点击事件回调
 */
static void s4_dshelp_btn_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        // 防止重复触发
        if (s4_waiting_response) return;

        s4_trigger_dshelp();
    }
}

/**
 * @brief 对分析文本进行分行处理，在【标记前插入换行符
 * @param dst 输出缓冲区
 * @param dst_size 输出缓冲区大小
 * @param src 原始分析文本
 * @note 每个【章节】独占一行，方便 LVGL 标签显示
 */
static void format_analysis_text(char *dst, size_t dst_size, const char *src)
{
    if (!dst || !src || dst_size == 0) return;
    size_t di = 0;
    size_t si = 0;
    while (src[si] != '\0' && di < dst_size - 1)
    {
        /* 在非行首的 "【" 前插入换行符 */
        if (src[si] == '\xE3' && src[si + 1] == '\x80' && src[si + 2] == '\x90')
        {
            if (di > 0 && dst[di - 1] != '\n')
            {
                dst[di++] = '\n';
                if (di >= dst_size - 1) break;
            }
            dst[di++] = src[si++];
            dst[di++] = src[si++];
            dst[di++] = src[si++];
        }
        else
        {
            dst[di++] = src[si++];
        }
    }
    dst[di] = '\0';
}

/**
 * @brief 触发一次 dshelp AI 分析请求（共用逻辑）
 */
static void s4_trigger_dshelp(void)
{
    s4_waiting_response = true;
    s4_wait_seconds = 0;

    if (s4_status_label)
    {
        lv_label_set_text(s4_status_label, "...");  /* 等待中 */
    }
    if (s4_analysis_label)
    {
        lv_label_set_text(s4_analysis_label, "正在请求AI分析，请稍候...");
    }
    if (s4_sample_label)
    {
        lv_label_set_text(s4_sample_label, "样本数: -");
    }
    if (s4_request_btn)
    {
        lv_obj_add_state(s4_request_btn, LV_STATE_DISABLED);
    }

    extern UART_HandleTypeDef huart2;
    MQTT_Send_DsHelp(&huart2);
}

/**
 * @brief 更新 Screen 4 的 dshelp 分析结果显示
 *
 * 由 LVGL 定时器周期性调用（1秒），检查 g_dshelp_result.has_new_data 标志。
 * 有新数据时更新界面显示，无数据时仅更新时间。
 * 同时处理：20秒超时重发、首次时间同步后自动请求。
 */
void update_screen4_dshelp(void)
{
    if (!screen_4) return;

    /* ── 首次时间同步后自动发起 dshelp 请求 ── */
    if (!s4_first_time_sync_done && g_time_synced)
    {
        s4_first_time_sync_done = true;
        s4_trigger_dshelp();
    }

    /* ── 20秒超时未收到响应则重发 ── */
    if (s4_waiting_response)
    {
        s4_wait_seconds++;
        if (s4_wait_seconds >= 20)
        {
            printf("[dshelp] No response in 20s, re-sending request...\r\n");
            s4_trigger_dshelp();
            return; /* 重发后本轮不继续处理 */
        }
    }

    // 检查是否有新的 dshelp 结果
    if (g_dshelp_result.has_new_data)
    {
        g_dshelp_result.has_new_data = false;
        s4_waiting_response = false;
        s4_wait_seconds = 0;

        // 恢复按钮
        if (s4_request_btn)
        {
            lv_obj_remove_state(s4_request_btn, LV_STATE_DISABLED);
        }

        // 更新分析文本（自动分行处理）
        if (s4_analysis_label)
        {
            if (g_dshelp_result.analysis[0] != '\0')
            {
                char formatted[sizeof(g_dshelp_result.analysis) * 2];
                format_analysis_text(formatted, sizeof(formatted),
                                     g_dshelp_result.analysis);
                lv_label_set_text(s4_analysis_label, formatted);
            }
            else
            {
                lv_label_set_text(s4_analysis_label, "未能获取分析结果。");
            }
        }

        // 更新状态图标
        if (s4_status_label)
        {
            if (g_dshelp_result.ok)
            {
                lv_label_set_text(s4_status_label, "OK");
            }
            else
            {
                lv_label_set_text(s4_status_label, "ERR");
            }
        }

        // 更新样本数
        if (s4_sample_label)
        {
            lv_label_set_text_fmt(s4_sample_label, "样本数: %d",
                                  g_dshelp_result.sample_count);
        }
    }

    // 更新时间显示（无论是否有新数据）
    if (s4_time_label)
    {
        extern uint64_t UNX_Now_Time;
        if (UNX_Now_Time > 0)
        {
            time_t t = (time_t)(UNX_Now_Time / 1000 + 28800); // UTC+8
            struct tm *lt = localtime(&t);
            char time_str[10];
            strftime(time_str, sizeof(time_str), "%H:%M:%S", lt);
            lv_label_set_text(s4_time_label, time_str);
        }
    }
}

/**
 * @brief 创建 Screen 4 —— AI 水质分析界面
 *
 * 布局（480×320）：
 *   ┌─────────────────────────────┐
 *   │     💧 AI 水质分析          │  ← 标题行
 *   ├─────────────────────────────┤
 *   │  【水质概况】整体良好。      │
 *   │  【关键指标】pH7.2,TDS123   │  ← 分析文本区域（可滚动）
 *   │     COD45,水温22.5℃。      │
 *   │  【建议】继续日常监测即可。  │
 *   ├─────────────────────────────┤
 *   │ 样本数:5  │  ✅  │ 16:05   │  ← 底栏信息
 *   ├─────────────────────────────┤
 *   │ [ 请求AI分析 ]   [ 返回 ]   │  ← 按钮行
 *   └─────────────────────────────┘
 */
void create_screen_4(void)
{
    screen_4 = lv_obj_create(NULL);
    ui_apply_screen_style(screen_4);

    // ========== 标题 ==========
    lv_obj_t *title = lv_label_create(screen_4);
    lv_label_set_text(title, "AI \xE6\xB0\xB4\xE8\xB4\xA8\xE5\x88\x86\xE6\x9E\x90"); // "AI 水质分析"
    ui_apply_title_style(title);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 8);

    // ========== 分析文本卡片容器 ==========
    lv_obj_t *analysis_card = lv_obj_create(screen_4);
    lv_obj_add_style(analysis_card, &style_card, LV_PART_MAIN);
    lv_obj_set_size(analysis_card, LV_PCT(92), LV_VER_RES - 160);
    lv_obj_align_to(analysis_card, title, LV_ALIGN_OUT_BOTTOM_MID, 0, 8);
    lv_obj_set_style_pad_all(analysis_card, 12, 0);

    // 分析文本标签（支持自动换行和滚动）
    s4_analysis_label = lv_label_create(analysis_card);
    lv_label_set_text(s4_analysis_label, "点击下方按钮请求AI水质分析");
    lv_label_set_long_mode(s4_analysis_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(s4_analysis_label, LV_PCT(100));
    lv_obj_set_style_text_font(s4_analysis_label, LV_FONT_DEFAULT, 0);
    lv_obj_set_style_text_color(s4_analysis_label,
                                lv_palette_darken(LV_PALETTE_BLUE, 4), 0);

    // ========== 底栏信息行 ==========
    lv_obj_t *footer_cont = lv_obj_create(screen_4);
    lv_obj_set_size(footer_cont, LV_PCT(92), 32);
    lv_obj_align_to(footer_cont, analysis_card, LV_ALIGN_OUT_BOTTOM_MID, 0, 6);
    lv_obj_set_flex_flow(footer_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(footer_cont, LV_FLEX_ALIGN_SPACE_EVENLY,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_border_width(footer_cont, 0, 0);
    lv_obj_set_style_bg_opa(footer_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(footer_cont, 0, 0);
    lv_obj_clear_flag(footer_cont, LV_OBJ_FLAG_SCROLLABLE);

    // 样本数标签
    s4_sample_label = lv_label_create(footer_cont);
    lv_label_set_text(s4_sample_label, "样本数: -");
    lv_obj_set_style_text_font(s4_sample_label, LV_FONT_DEFAULT, 0);

    // 状态图标标签
    s4_status_label = lv_label_create(footer_cont);
    lv_label_set_text(s4_status_label, "-");
    lv_obj_set_style_text_font(s4_status_label, LV_FONT_DEFAULT, 0);

    // 时间标签
    s4_time_label = lv_label_create(footer_cont);
    lv_label_set_text(s4_time_label, "--:--:--");
    lv_obj_set_style_text_font(s4_time_label, LV_FONT_DEFAULT, 0);

    // ========== 按钮行 ==========
    lv_obj_t *btn_cont = lv_obj_create(screen_4);
    lv_obj_set_size(btn_cont, LV_PCT(92), 44);
    lv_obj_align_to(btn_cont, footer_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);
    lv_obj_set_flex_flow(btn_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_cont, LV_FLEX_ALIGN_SPACE_EVENLY,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_border_width(btn_cont, 0, 0);
    lv_obj_set_style_bg_opa(btn_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(btn_cont, 0, 0);
    lv_obj_clear_flag(btn_cont, LV_OBJ_FLAG_SCROLLABLE);

    // "请求AI分析" 按钮
    s4_request_btn = lv_button_create(btn_cont);
    ui_apply_button_style(s4_request_btn);
    lv_obj_set_size(s4_request_btn, 140, 38);
    lv_obj_t *req_label = lv_label_create(s4_request_btn);
    lv_label_set_text(req_label, "\xE8\xAF\xB7\xE6\xB1\x82"
                      "AI\xE5\x88\x86\xE6\x9E\x90"); // "请求AI分析"
    lv_obj_center(req_label);
    lv_obj_add_event_cb(s4_request_btn, s4_dshelp_btn_event_handler,
                        LV_EVENT_CLICKED, NULL);

    // "返回" 按钮
    lv_obj_t *back_btn = lv_button_create(btn_cont);
    ui_apply_button_style(back_btn);
    lv_obj_set_size(back_btn, 120, 38);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "\xE8\xBF\x94\xE5\x9B\x9E"); // "返回"
    lv_obj_center(back_label);
    lv_obj_add_event_cb(back_btn, back_to_main_event_handler,
                        LV_EVENT_CLICKED, NULL);

    // ========== 启动定时刷新（1 秒周期）==========
    lv_timer_create((lv_timer_cb_t)update_screen4_dshelp, 1000, NULL);
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
    ui_init_theme_and_style();

    // 创建所有屏幕
    create_main_screen();
    create_screen_1();
    create_screen_2();
    create_screen_3();
    create_screen_4();

    // 加载主屏幕以首先显示它
    lv_screen_load(main_screen);
}
